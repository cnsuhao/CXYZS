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
//����������	
	INIParser ini;
	if( ! ini.open("ServerConfig.ini") )	
		return false;
	std::string value = ini.read( "Server" , "GateServerPort" );
	m_Port = atoi(value.c_str());
	m_LoginServerIP = ini.read( "Server" , "LoginServerIP" );
	value = ini.read( "Server" , "LoginServerPort" );
	m_LoginServerPort = atoi(value.c_str());	
	m_LoginAddress.FromStringExplicitPort(m_LoginServerIP.c_str(), m_LoginServerPort);
//��־��������
    gLog.SetName("GateServer");
//��ʼ������
	m_Net = RakNet::RakPeerInterface::GetInstance();
	m_Net->SetIncomingPassword(GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));	//���ý�������
	m_Net->SetOccasionalPing(true);
	m_Net->SetUnreliableTimeout(LINK_TIMEOUT_TIME);	//���ó�ʱʱ��
	if (m_Net->Startup(GATESERVER_CONNECTIONS_MAX, &RakNet::SocketDescriptor(m_Port,0), 1) != RakNet::RAKNET_STARTED)
	{
        gLog.Error("RakNet startup failture.");
		return false;
	}	
	m_Net->SetMaximumIncomingConnections(GATESERVER_CONNECTIONS_MAX);	//��������������С�������������

//������ѭ��������Ϣ
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
			case ID_CONNECTION_REQUEST_ACCEPTED:	//���ӳɹ�
				{
				}
				break;
			case ID_NEW_INCOMING_CONNECTION:	//�յ��µ�����
				AddLink(packet->systemAddress);
				break;							
			case ID_ALREADY_CONNECTED:	//�Է�˵�����ظ�������
			case ID_CONNECTION_BANNED: // ����ֹ����
			case ID_CONNECTION_ATTEMPT_FAILED:	//���������޷����
			case ID_NO_FREE_INCOMING_CONNECTIONS:	//�Է������������������µ�����
				{
					gLog.Error("~~~SYSMsgID = %d\n", packet->data[0]);		
				}
				break;
			case ID_DISCONNECTION_NOTIFICATION:	//�����Ѿ��ر�
			case ID_CONNECTION_LOST:			//�Է����Ӷ�ʧ
				DelLink(packet->systemAddress);
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
				gLog.Error("~~~PASSWORD\n");
				break;
			case ID_CONNECTED_PING:		//�յ�һ��δ���ӵ�Ping
			case ID_UNCONNECTED_PING:	//�յ�һ�������ӵ�Ping
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
		//��ʱ��
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