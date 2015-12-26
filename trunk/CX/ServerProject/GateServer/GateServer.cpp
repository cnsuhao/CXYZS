#include "GateServer.h"
#include "INIParser.h"
#include "windows.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "LogManage.h"
#pragma comment(lib, "ws2_32.lib")

GateServer *g_GateServer = NULL;

GateServer::GateServer(void)
{
}


GateServer::~GateServer(void)
{
	Stop();
}

bool GateServer::Start()
{	
//服务器配置	
	INIParser ini;
	if( ! ini.open("ServerConfig.ini") )	
		return false;
	std::string value = ini.read( "Server" , "GateServerPort" );
	m_Port = atoi(value.c_str());
	m_LoginServerIP = ini.read( "Server" , "LoginServerIP" );
	value = ini.read( "Server" , "LoginServerPort" );
	m_LoginServerPort = atoi(value.c_str());	
	m_LoginAddress.FromStringExplicitPort(m_LoginServerIP.c_str(), m_LoginServerPort);
//日志设置名字
    gLog.SetName("GateServer");
//初始化网络
	m_Net = RakNet::RakPeerInterface::GetInstance();
	m_Net->SetIncomingPassword(GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));	//设置接入密码
	m_Net->SetOccasionalPing(true);
	m_Net->SetUnreliableTimeout(LINK_TIMEOUT_TIME);	//设置超时时间
	if (m_Net->Startup(GATESERVER_CONNECTIONS_MAX, &RakNet::SocketDescriptor(m_Port,0), 1) != RakNet::RAKNET_STARTED)
	{
        gLog.Error("RakNet startup failture.");
		return false;
	}	
	m_Net->SetMaximumIncomingConnections(GATESERVER_CONNECTIONS_MAX);	//最大接入数量，需小于最大连接数量

//运行主循环接收消息
	m_StopFlag = false;
	g_TimerManage = new TimerManage;
	Run();

	return true;
}

void GateServer::Run()
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
				}
				break;
			case ID_NEW_INCOMING_CONNECTION:	//收到新的连接
				AddLink(packet->systemAddress);
				break;							
			case ID_ALREADY_CONNECTED:	//对方说你是重复的连接
			case ID_CONNECTION_BANNED: // 被禁止连接
			case ID_CONNECTION_ATTEMPT_FAILED:	//连接请求无法完成
			case ID_NO_FREE_INCOMING_CONNECTIONS:	//对方连接已满，不接受新的连接
				{
					gLog.Error("~~~SYSMsgID = %d\n", packet->data[0]);		
				}
				break;
			case ID_DISCONNECTION_NOTIFICATION:	//连接已经关闭
			case ID_CONNECTION_LOST:			//对方连接丢失
				DelLink(packet->systemAddress);
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
				gLog.Error("~~~PASSWORD\n");
				break;
			case ID_CONNECTED_PING:		//收到一个未连接的Ping
			case ID_UNCONNECTED_PING:	//收到一个已连接的Ping
				gLog.Info("Ping from %s\n", packet->systemAddress.ToString(true));
				break;
			case ID_C2L:
				{
					unsigned long linkid = RakNet::SystemAddress::ToInteger(packet->systemAddress);
					MsgBase* msg = (MsgBase*)(packet->data);
					msg->OBJID = linkid;
					SendToLogin(msg);
				}
				break;
			case ID_L2C:
				{
					MsgBase* msg = (MsgBase*)(packet->data);
					SendToClientByLinkID(msg);
				}
				break;
			case ID_C2G:
				{					
					unsigned long linkid = RakNet::SystemAddress::ToInteger(packet->systemAddress);
					MsgBase* msg = (MsgBase*)(packet->data);
					Link_Info* info = GetLinkByLinkID(linkid);
					if (info && info->AccountId > 0)
					{
						msg->OBJID = info->AccountId;
						SendToGame(info->lastServerId, msg);
					}
				}
				break;
			case ID_G2C:
				{
					MsgBase* msg = (MsgBase*)(packet->data);					
					SendToClientByAccountID(msg);
				}
				break;
			case ID_D2C:
				{
					MsgBase* msg = (MsgBase*)(packet->data);					
					SendToClientByAccountID(msg);
				}
				break;
			case ID_GAME2GATE:
				{
					OnRecvGame(packet);
				}
				break;			
			default:
				break;
			}
		}
		//定时器
		//m_TimerManage.Run();
		
		g_TimerManage->Run();		

		Sleep(1);
	}
}

bool GateServer::Stop()
{
	m_Net->Shutdown(300);
	RakNet::RakPeerInterface::DestroyInstance(m_Net);
	m_StopFlag = true;
	SAFE_DELETE(g_TimerManage);
	return true;
}

void GateServer::OnRecvGame(RakNet::Packet* p)
{	
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg->MsgID < Msg_MAX)
	{
		switch (msg->MsgID)
		{
		case Msg_GameServer_Register:
			OnGameServerRegister(p);
			break;
		case Msg_Player_Enter_GameServer:
			OnPlayerEnterGameServer(p);
			break;
		default:
			break;
		}
	}
	else
	{
		gLog.Warning("eorr Packet id = %d", msg->MsgID);
	}
}

bool GateServer::Send(RakNet::SystemAddress &address, MsgBase *msg)
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

bool GateServer::SendToClientByLinkID(MsgBase *msg)
{
	Link_Info* link = GetLinkByLinkID((unsigned long)msg->OBJID);
	if (link)
	{
		Send(link->Address, msg);
	}
	return true;
}

bool GateServer::SendToClientByAccountID(MsgBase *msg)
{
	Link_Info* link = GetLinkByAccountId(msg->OBJID);
	if (link)
	{
		Send(link->Address, msg);
	}
	return true;
}

bool GateServer::SendToGame(uint16 &serverid, MsgBase *msg)
{	
	Link_Info* info = GetLinkByServerId(serverid);
	if (info)
	{
		return Send(info->Address, msg);
	}
	return true;
}

bool GateServer::SendToLogin(MsgBase *msg)
{
	Send(m_LoginAddress, msg);
	return true;
}


void GateServer::OnGameServerRegister(RakNet::Packet* msg)
{
	MsgGameServer_Register* info = (MsgGameServer_Register*) msg->data;
	GameServerRegister(RakNet::SystemAddress::ToInteger(msg->systemAddress), info->ServerID, info->ServerName);
}

void GateServer::OnPlayerEnterGameServer(RakNet::Packet* msg)
{
	MsgPlayer_Enter_GameServer* info = (MsgPlayer_Enter_GameServer*) msg->data;
	AddClient((uint16)info->OBJID, info->LinkID, info->AccountID);
}