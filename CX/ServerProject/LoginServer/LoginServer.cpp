#include "LoginServer.h"
#include "INIParser.h"
#include "RakNetTypes.h"
#include "CSVParser.h"
#include "LogManage.h"
#pragma comment(lib, "ws2_32.lib")
LoginServer::LoginServer(void)
{
}


LoginServer::~LoginServer(void)
{
	Stop();
}

bool LoginServer::Start()
{	
//服务器配置
	INIParser ini;
	if( ! ini.open("ServerConfig.ini") )	
		return false;
	std::string value = ini.read( "Server" , "LoginServerPort" );
	m_Port = atoi(value.c_str());

	m_GateServerIP = ini.read( "Server" , "GateServerIP" );
	value = ini.read( "Server" , "GateServerPort" );
	m_GateServerPort = atoi(value.c_str());	
	m_GateAddress.FromStringExplicitPort(m_GateServerIP.c_str(), m_GateServerPort);
//日志设置名字
    gLog.SetName("LoginServer");
//账号数据库配置
	m_AccountDBName = ini.read( "DB" , "AccountDBName" );
	m_AccountDBIP = ini.read( "DB" , "AccountDBIP" );
	value = ini.read( "DB" , "AccountDBPort" );
	m_AccountDBPort = atoi(value.c_str());
	m_AccountDBAccount = ini.read( "DB" , "AccountDBAccount" );
	m_AccountDBPassword = ini.read( "DB" , "AccountDBPassword" );
	if (!(m_AccountDBName.size()>0 && m_AccountDBIP.size() > 0 && m_AccountDBAccount.size() > 0 && m_AccountDBPassword.size() > 0))
	{
		gLog.Error("~~~DBConfig\n");		
		return false;
	}

//读csv游戏服务器配置
	CSVParser csv;
	csv.LoadFile("Config.csv");
	csv.GetLine();	//忽略第一行
	GameServerInfo info;
	while (csv.GetLine())
	{		
		info.ServerID = csv.GetInt();	//服务器ID		
		info.ServerName = csv.GetStr();		//服务器名
		info.ServerState = csv.GetInt();		//服务器状态
		char* ip = csv.GetStr();
		info.ServerAddress.FromStringExplicitPort(ip, csv.GetInt());			//服务器地址
		info.isStart = false;				//服务器状态
		m_GameServerList[info.ServerID] = info;
	}	

//初始化网络
	m_Net = RakNet::RakPeerInterface::GetInstance();
	m_Net->SetIncomingPassword(SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));	//设置接入密码
	m_Net->SetOccasionalPing(true);
	m_Net->SetUnreliableTimeout(LINK_TIMEOUT_TIME);	//设置超时时间
	if (m_Net->Startup(LOGINSERVER_CONNECTIONS_MAX, &RakNet::SocketDescriptor(m_Port,0), 1) != RakNet::RAKNET_STARTED)
	{		
		return false;
	}	
	m_Net->SetMaximumIncomingConnections(LOGINSERVER_CONNECTIONS_MAX);	//最大接入数量，需小于最大连接数量
	
//连接到网关
	m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));

//连接到账号数据库
	if(m_Mysql.Init(m_AccountDBIP.c_str(),m_AccountDBAccount.c_str(),m_AccountDBPassword.c_str(),m_AccountDBName.c_str(),m_AccountDBPort))
	{
		gLog.Info("OK~~~Link DB\n");		
	}
	else
	{
		gLog.Error("~~~Link DB\n");
		return false;
	}

//运行主循环接收消息
	m_StopFlag = false;
	Run();

	return true;
}

void LoginServer::Run()
{
	RakNet::Packet *packet = NULL;
	int i;
	while( !m_StopFlag )
	{
		for(i = 0; i < FRAME_GET_MSG_MAX && (packet = m_Net->Receive()); i++)
		{
			//!test
			if (packet->data[0] > ID_USER_PACKET_ENUM)
			{
				gLog.MsgIO(false,packet->data[0],packet->data[1],packet->length);
			}
			switch(packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:	//连接成功
				{
					if (packet->systemAddress == m_GateAddress)
					{
						gLog.Info("OK~~~Link GateServer\n");		
					}					
				}
				break;
			case ID_NEW_INCOMING_CONNECTION:	//收到新的连接				
				AddServerLink(packet->systemAddress);
				break;			
			case ID_ALREADY_CONNECTED:	//对方说你是重复的连接
			case ID_CONNECTION_BANNED: // 被禁止连接
			case ID_CONNECTION_ATTEMPT_FAILED:	//连接请求无法完成
			case ID_NO_FREE_INCOMING_CONNECTIONS:	//对方连接已满，不接受新的连接
				{
					if (packet->systemAddress == m_GateAddress)
					{
						m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));	//重连网关
					}
					gLog.Error("~~~SYSMsgID = %d\n", packet->data[0]);		
				}
				break;
			case ID_DISCONNECTION_NOTIFICATION:	//服务器已经关闭	
			case ID_CONNECTION_LOST:		//对方连接丢失
				gLog.Warning("ID_CONNECTION_LOST %s\n", packet->systemAddress.ToString(true));
				if (packet->systemAddress == m_GateAddress)
				{
					//连接到网关
					m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));
				}
				else
				{
					DelServerLink(packet->systemAddress);
				}
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:  //RAKNET_PROTOCOL_VERSION与远端系统上的版本值不匹配
				gLog.Warning("ID_INCOMPATIBLE_PROTOCOL_VERSION = %d\n", packet->data[2]);
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // 在客户端/服务器环境中,一个客户端已经断开了连接
				gLog.Warning("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
				break;
			case ID_REMOTE_CONNECTION_LOST: // 在客户端/服务器环境，客户端被迫断开了连接
				gLog.Warning("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // 在客户端/服务器环境，新的客户端接入
				gLog.Warning("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;			
			case ID_INVALID_PASSWORD:	//无效的密码
				gLog.Error("ID_INVALID_PASSWORD\n");
				break;
			case ID_CONNECTED_PING:		//收到一个未连接的Ping
			case ID_UNCONNECTED_PING:	//收到一个已连接的Ping
				gLog.Info("Ping from %s\n", packet->systemAddress.ToString(true));
				break;
			case ID_C2L:		//收到自定义的包
				OnRecvClient(packet);	
				break;
			default:
				break;
			}
		}
		//定时器
		m_TimerManage.Run();
				

		Sleep(1);
	}
}

bool LoginServer::Stop()
{
	m_Net->Shutdown(300);
	RakNet::RakPeerInterface::DestroyInstance(m_Net);
	//m_Mysql.Free();
	m_StopFlag = true;
	return true;
}

void LoginServer::OnRecvClient(RakNet::Packet* p)
{		
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg && msg->MsgID < Msg_MAX)
	{
		switch (msg->MsgID)
		{
		case Msg_Notify:
			{		
				MsgNotify* temp =  (MsgNotify*)(p->data);
				gLog.Info("%d\n", temp->Type);
				switch (temp->Type)
				{
				case Notify_C2S_Login_ServerList:	
					{						
						MsgServerList *send = (MsgServerList *)m_SendBuff;
						send->SysID = ID_L2C;
						send->MsgID = Msg_ServerList;
						send->ServerNum = m_GameServerList.size();
						ServerListIt *temp = &(send->ServerListInfo);
						std::map<uint16, GameServerInfo>::iterator it = m_GameServerList.begin();
						for (; it != m_GameServerList.end(); ++it)
						{							
							temp->ServerID =  it->second.ServerID;	
							memset(temp->ServerName,0, MAX_NAME_SIZE);
							memcpy(temp->ServerName, it->second.ServerName.c_str(), strlen(it->second.ServerName.c_str()));
							temp->ServerState =  it->second.ServerState;
							temp->isStart = it->second.isStart;
							temp++;
						}
						send->Len = sizeof(MsgServerList) + (send->ServerNum - 1) * sizeof(ServerListIt);
						SendToClient(msg->OBJID, send);
					}
					break;
				default:
					break;
				}
			}
			break;
		case Msg_Regiser_Account:
			Regiser(msg);
			break;
		case Msg_Account_Login:
			Login(msg);
			break;
		case Msg_SelServer:
			SelServer(msg);
			break;
		default:
			gLog.Warning("eorr Packet id = %d", msg->MsgID);
			break;
		}
	}	
}

void LoginServer::OnRecvGame(RakNet::Packet* p)
{		
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg && msg->MsgID < Msg_MAX)
	{
		switch (msg->MsgID)
		{
		case 0:
			
		default:
			gLog.Warning("eorr Packet id = %d", msg->MsgID);
			break;
		}
	}	
}

bool LoginServer::Send(RakNet::SystemAddress &address, MsgBase *msg)
{
	if (msg->Len < sizeof(MsgBase))
	{
		printf("Eorr~~~Send Packet id = %d  %d", msg->SysID, msg->MsgID);
		return false;
	}
	//!test
	gLog.MsgIO(true,msg->SysID,msg->MsgID,msg->Len);
	
	int i = m_Net->Send((const char*)msg, msg->Len, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, address, false);
	return i > 0;
}

bool LoginServer::SendToClient(uint64 &objid, MsgBase *msg)
{	
	msg->OBJID = objid;
	msg->SysID = ID_L2C;
	return Send(m_GateAddress, msg);
}

bool LoginServer::SendToGame(uint16 &serverid, MsgBase *msg)
{	
	GameServerInfo* info = GetServerInfo(serverid);
	if (info)
	{
		msg->SysID = ID_L2G;
		return Send(info->ServerAddress, msg);
	}
	return false;
}

void LoginServer::AddServerLink(RakNet::SystemAddress &address)
{
	std::map<uint16, GameServerInfo>::iterator it = m_GameServerList.begin();
	for (; it != m_GameServerList.end(); ++it)
	{
		if (it->second.ServerAddress == address)
		{
			it->second.isStart = true;
			return;
		}
	}
}

void LoginServer::DelServerLink(RakNet::SystemAddress &address)
{
	std::map<uint16, GameServerInfo>::iterator it = m_GameServerList.begin();
	for (; it != m_GameServerList.end(); ++it)
	{
		if (it->second.ServerAddress == address)
		{
			it->second.isStart = false;
			return;
		}
	}
}

GameServerInfo* LoginServer::GetServerInfo(uint16 id)
{
	std::map<uint16, GameServerInfo>::iterator it = m_GameServerList.find(id);
	if (it != m_GameServerList.end())
	{
		return &it->second;
	}
	return NULL;
}

void LoginServer::AddLoginPlayer(uint64 linkid, uint64 accountid, char* accountname, uint16 lastserverid)
{
	//!多次登陆有问题
	LoginPlayerInfo* info = GetLoginPlayer(accountid);
	if (info)
	{		
		if (info->state == 2)
		{ 
			//通知对应服务器玩家下线
			MsgPlayer_Leave_GameServer send(accountid);
			SendToGame(info->lastServerId, &send);
		}
		info->LinkID = linkid;
		info->state = 1;
		info->lastServerId = lastserverid;
	}
	else
	{
		info = new LoginPlayerInfo;
		info->accountId = accountid;
		info->LinkID = linkid;
		strncpy(info->accountName, accountname, MAX_NAME_SIZE);
		info->lastServerId = lastserverid;
		info->state = 1;
		m_LoginPlayerListByID[accountid] = info;
	}
}

void LoginServer::DelLoginPlayer(uint64 accountid)
{
	std::map<uint64, LoginPlayerInfo*>::iterator it = m_LoginPlayerListByID.find(accountid);
	if (it != m_LoginPlayerListByID.end())
	{
		if (it->second)
		{
			SAFE_DELETE(it->second);
			m_LoginPlayerListByID.erase(it);
		}
	}	
}

LoginPlayerInfo* LoginServer::GetLoginPlayer(uint64 accountid)
{	
	std::map<uint64, LoginPlayerInfo*>::iterator it = m_LoginPlayerListByID.find(accountid);
	if (it != m_LoginPlayerListByID.end())
	{
		return it->second;
	}	
	return NULL;
}

LoginPlayerInfo* LoginServer::GetLoginPlayerByLinkID(uint64 linkid)
{	
	std::map<uint64, LoginPlayerInfo*>::iterator it = m_LoginPlayerListByID.begin();
	for (; it != m_LoginPlayerListByID.end(); ++it)
	{
		if (linkid == it->second->LinkID)
		{
			return it->second;
		}
	}	
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void LoginServer::Regiser(MsgBase* msg)
{
	MsgRegiser_Login* temp = (MsgRegiser_Login*)(msg);
	if(!temp)
	{
		return;
	}
	//账号长度检测
	if (strlen(temp->Name) < MIN_NAME_SIZE|| strlen(temp->Name) > MAX_NAME_SIZE)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Regiser_NameLen);
		SendToClient(msg->OBJID, &ttt);
		return;
	}
	//密码长度检测
	if (strlen(temp->Password) < MIN_PASS_SIZE || strlen(temp->Password) > MAX_PASS_SIZE)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Regiser_PasswordLen);
		SendToClient(msg->OBJID, &ttt);
		return;
	}
	//！屏蔽字检测



	//查看账号名是否存在
	char strBuf[512] = {0};
	sprintf(strBuf, "SELECT 'accountName' FROM account WHERE accountName = '%s'", temp->Name);
	//m_Mysql.ExecSql(strBuf, strlen(strBuf));
	if (m_Mysql.DbQuery(strBuf).size()> 0)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Regiser_NameRepeat);
		SendToClient(msg->OBJID, &ttt);
		return;
	}
	/*mysqlCol tt[2];
	tt[0].name = "accountName";
	tt[0].mysqlType = MYSQL_INT8;
	tt[0].dataSize = 128;
	tt[1].name = 0;
	tt[1].mysqlType = 0;
	tt[1].dataSize = 0;
	if (m_Mysql.SelectDataByCol("account", tt, "accountName = 'name112'", 0) > 0)
	{
	MsgNotify ttt(msg->OBJID, ID_L2C, Notify_S2C_Regiser_NameRepeat);
	SendToClient(&ttt, ttt.Len);
	return;
	}*/
	

	//保存注册信息	
	sprintf(strBuf, "INSERT INTO account (accountName, pwd) VALUES('%s', '%s')", temp->Name, temp->Password);	
	if ((m_Mysql.DbExecute(strBuf, eMysqlInsert)) > 0)
	{
		//返回注册成功
		MsgNotify ttt(ID_L2C, Notify_S2C_Regiser_OK);
		SendToClient(msg->OBJID, &ttt);
	}
}

void LoginServer::Login(MsgBase* msg)
{
	MsgRegiser_Login* temp = (MsgRegiser_Login*)(msg);
	if(!temp)
	{
		return;
	}
	//查看账号名是否存在
	char strBuf[512] = {0};
	sprintf(strBuf, "SELECT `accountId`,`lastServerId` FROM account WHERE accountName = '%s' and pwd = '%s'", temp->Name, temp->Password);
// 	
// 	m_Mysql.ExecSql(strBuf, strlen(strBuf));	
// 	m_Mysql.UseResult();
	struct t_VisitorCheck
	{
		unsigned __int64 accountId;
		unsigned __int16 lastServerId;
	};
	t_VisitorCheck record;
	StoreQueryResult res;
	if((res=m_Mysql.DbQuery(strBuf)).size()>0)//m_Mysql.FullSelectData(SQL_account, (unsigned char *)(&record)))
	{
		//登陆成功
		//m_Mysql.ResetResult();
		StoreQueryResult::const_iterator it;
		for(it=res.begin();it!=res.end();++it)
		{
			if(!(*it)["accountId"].is_null())
				record.accountId=(*it)["accountId"];
			if(!(*it)["lastServerId"].is_null())
				record.lastServerId=(*it)["lastServerId"];
		}
		AddLoginPlayer(temp->OBJID, record.accountId, temp->Name, record.lastServerId);

		MsgNotify ttt(ID_L2C, Notify_S2C_Login_OK);
		SendToClient(msg->OBJID, &ttt);
	}
	else
	{
		//账号密码错误
		MsgNotify ttt(ID_L2C, Notify_S2C_Login_NamePassError);
		SendToClient(msg->OBJID, &ttt);
		return;
	}
}

void LoginServer::SelServer(MsgBase* msg)
{
	MsgSelServer* temp = (MsgSelServer*)(msg);
	if(!temp)
	{
		return;
	}
	//判断玩家是否登陆状态
	LoginPlayerInfo* player = GetLoginPlayerByLinkID(msg->OBJID);
	if (!player || player->state != 1)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Login_ReLogin);
		SendToClient(temp->OBJID, &ttt);
		return;
	}

	//查询服务器是否开启
	GameServerInfo* info = GetServerInfo(temp->ServerID);
	if (!info || !info->isStart)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Login_ServerNotOpen);
		SendToClient(temp->OBJID, &ttt);
		return;
	}

	//设置玩家状态
	player->state = 2;
	player->lastServerId = temp->ServerID;

	//通知游戏服务器玩家进入
	MsgPlayer_Enter_GameServer send(player->accountId, msg->OBJID);
	SendToGame(temp->ServerID, &send);
}