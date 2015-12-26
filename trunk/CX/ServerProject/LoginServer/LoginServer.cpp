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
//����������
	INIParser ini;
	if( ! ini.open("ServerConfig.ini") )	
		return false;
	std::string value = ini.read( "Server" , "LoginServerPort" );
	m_Port = atoi(value.c_str());

	m_GateServerIP = ini.read( "Server" , "GateServerIP" );
	value = ini.read( "Server" , "GateServerPort" );
	m_GateServerPort = atoi(value.c_str());	
	m_GateAddress.FromStringExplicitPort(m_GateServerIP.c_str(), m_GateServerPort);
//��־��������
    gLog.SetName("LoginServer");
//�˺����ݿ�����
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

//��csv��Ϸ����������
	CSVParser csv;
	csv.LoadFile("Config.csv");
	csv.GetLine();	//���Ե�һ��
	GameServerInfo info;
	while (csv.GetLine())
	{		
		info.ServerID = csv.GetInt();	//������ID		
		info.ServerName = csv.GetStr();		//��������
		info.ServerState = csv.GetInt();		//������״̬
		char* ip = csv.GetStr();
		info.ServerAddress.FromStringExplicitPort(ip, csv.GetInt());			//��������ַ
		info.isStart = false;				//������״̬
		m_GameServerList[info.ServerID] = info;
	}	

//��ʼ������
	m_Net = RakNet::RakPeerInterface::GetInstance();
	m_Net->SetIncomingPassword(SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));	//���ý�������
	m_Net->SetOccasionalPing(true);
	m_Net->SetUnreliableTimeout(LINK_TIMEOUT_TIME);	//���ó�ʱʱ��
	if (m_Net->Startup(LOGINSERVER_CONNECTIONS_MAX, &RakNet::SocketDescriptor(m_Port,0), 1) != RakNet::RAKNET_STARTED)
	{		
		return false;
	}	
	m_Net->SetMaximumIncomingConnections(LOGINSERVER_CONNECTIONS_MAX);	//��������������С�������������
	
//���ӵ�����
	m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));

//���ӵ��˺����ݿ�
	if(m_Mysql.Init(m_AccountDBIP.c_str(),m_AccountDBAccount.c_str(),m_AccountDBPassword.c_str(),m_AccountDBName.c_str(),m_AccountDBPort))
	{
		gLog.Info("OK~~~Link DB\n");		
	}
	else
	{
		gLog.Error("~~~Link DB\n");
		return false;
	}

//������ѭ��������Ϣ
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
			case ID_CONNECTION_REQUEST_ACCEPTED:	//���ӳɹ�
				{
					if (packet->systemAddress == m_GateAddress)
					{
						gLog.Info("OK~~~Link GateServer\n");		
					}					
				}
				break;
			case ID_NEW_INCOMING_CONNECTION:	//�յ��µ�����				
				AddServerLink(packet->systemAddress);
				break;			
			case ID_ALREADY_CONNECTED:	//�Է�˵�����ظ�������
			case ID_CONNECTION_BANNED: // ����ֹ����
			case ID_CONNECTION_ATTEMPT_FAILED:	//���������޷����
			case ID_NO_FREE_INCOMING_CONNECTIONS:	//�Է������������������µ�����
				{
					if (packet->systemAddress == m_GateAddress)
					{
						m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));	//��������
					}
					gLog.Error("~~~SYSMsgID = %d\n", packet->data[0]);		
				}
				break;
			case ID_DISCONNECTION_NOTIFICATION:	//�������Ѿ��ر�	
			case ID_CONNECTION_LOST:		//�Է����Ӷ�ʧ
				gLog.Warning("ID_CONNECTION_LOST %s\n", packet->systemAddress.ToString(true));
				if (packet->systemAddress == m_GateAddress)
				{
					//���ӵ�����
					m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));
				}
				else
				{
					DelServerLink(packet->systemAddress);
				}
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:  //RAKNET_PROTOCOL_VERSION��Զ��ϵͳ�ϵİ汾ֵ��ƥ��
				gLog.Warning("ID_INCOMPATIBLE_PROTOCOL_VERSION = %d\n", packet->data[2]);
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // �ڿͻ���/������������,һ���ͻ����Ѿ��Ͽ�������
				gLog.Warning("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
				break;
			case ID_REMOTE_CONNECTION_LOST: // �ڿͻ���/�������������ͻ��˱��ȶϿ�������
				gLog.Warning("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // �ڿͻ���/�������������µĿͻ��˽���
				gLog.Warning("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;			
			case ID_INVALID_PASSWORD:	//��Ч������
				gLog.Error("ID_INVALID_PASSWORD\n");
				break;
			case ID_CONNECTED_PING:		//�յ�һ��δ���ӵ�Ping
			case ID_UNCONNECTED_PING:	//�յ�һ�������ӵ�Ping
				gLog.Info("Ping from %s\n", packet->systemAddress.ToString(true));
				break;
			case ID_C2L:		//�յ��Զ���İ�
				OnRecvClient(packet);	
				break;
			default:
				break;
			}
		}
		//��ʱ��
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
	//!��ε�½������
	LoginPlayerInfo* info = GetLoginPlayer(accountid);
	if (info)
	{		
		if (info->state == 2)
		{ 
			//֪ͨ��Ӧ�������������
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
	//�˺ų��ȼ��
	if (strlen(temp->Name) < MIN_NAME_SIZE|| strlen(temp->Name) > MAX_NAME_SIZE)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Regiser_NameLen);
		SendToClient(msg->OBJID, &ttt);
		return;
	}
	//���볤�ȼ��
	if (strlen(temp->Password) < MIN_PASS_SIZE || strlen(temp->Password) > MAX_PASS_SIZE)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Regiser_PasswordLen);
		SendToClient(msg->OBJID, &ttt);
		return;
	}
	//�������ּ��



	//�鿴�˺����Ƿ����
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
	

	//����ע����Ϣ	
	sprintf(strBuf, "INSERT INTO account (accountName, pwd) VALUES('%s', '%s')", temp->Name, temp->Password);	
	if ((m_Mysql.DbExecute(strBuf, eMysqlInsert)) > 0)
	{
		//����ע��ɹ�
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
	//�鿴�˺����Ƿ����
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
		//��½�ɹ�
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
		//�˺��������
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
	//�ж�����Ƿ��½״̬
	LoginPlayerInfo* player = GetLoginPlayerByLinkID(msg->OBJID);
	if (!player || player->state != 1)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Login_ReLogin);
		SendToClient(temp->OBJID, &ttt);
		return;
	}

	//��ѯ�������Ƿ���
	GameServerInfo* info = GetServerInfo(temp->ServerID);
	if (!info || !info->isStart)
	{
		MsgNotify ttt(ID_L2C, Notify_S2C_Login_ServerNotOpen);
		SendToClient(temp->OBJID, &ttt);
		return;
	}

	//�������״̬
	player->state = 2;
	player->lastServerId = temp->ServerID;

	//֪ͨ��Ϸ��������ҽ���
	MsgPlayer_Enter_GameServer send(player->accountId, msg->OBJID);
	SendToGame(temp->ServerID, &send);
}