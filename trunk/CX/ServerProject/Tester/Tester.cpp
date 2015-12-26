// Tester.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <string>
#include "MsgInfo.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include <stdio.h>
#include "stdafx.h"
#include <vector>
#include <stdarg.h>
#include <stdlib.h> 
#include <malloc.h>

std::string tt;
unsigned char 				m_packetIdentifier;
RakNet::RakPeerInterface* m_peer;
const char* m_ServerIP = "127.0.0.1";
#define CONNECTION_PASSWORD	 "~Game@9494~"
int curPort = 11000;

RakNet::SystemAddress m_peerSystemAddress(m_ServerIP, curPort);

int Send(char* buf, int len)
{
	return m_peer->Send(buf, (const int) len, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_peerSystemAddress, false);
}

//����ע����Ϣ  //����Ѿ��������
void SendRegister()
{	
	C2SLoginRegister Msg;
	strcpy(Msg.name,"name222");
	strcpy(Msg.password,"111111");
	Msg.tourist_id = 0;
	int sendLen = Send((char*)&Msg, sizeof(Msg));
}
//���͵�½��Ϣ  //����Ѿ��������
void SendLogin()
{	
	C2SLogin_Login Msg;
	strcpy(Msg.name,"name222");
	strcpy(Msg.password,"111111");
	Msg.tourist_id = 0;
	int sendLen = Send((char*)&Msg, sizeof(Msg));
}

//������ϢC->S
void SendConnect()
{
	SendLogin();    //��½
	//SendRegister();           //����ע����Ϣ                    //����Ѿ��������
	//SendGetUserInfo();        //ȡ���û���Ϣ                    //����Ѿ��������
	//SendInfoMod();            //�����û���Ϣ                    //����Ѿ��������
	//SendLoginBaseInfo();      //��ҵ�½ȡ���û��Ļ�����Ϣ      //����Ѿ��������   //if(!pPreUser)������ڵ����⣿������������û�а취����
	//SendAccountCheck();		//�˺ŵ���֤					  //����Ѿ��������
	//SendVisitorCheck();
}
//
//������ϢS->C
void OnRecvPrintf(unsigned char* p)
{
	//RecvGetRandRoleName(p);   //���ȡ����ҵ�����              //����Ѿ��������
	//RecvRegister(p);          //����ע����Ϣ                    //����Ѿ��������
	//RecvGetUserInfo(p);       //ȡ���û���Ϣ                    //����Ѿ��������
	//RecvInfoMod(p);           //�����û���Ϣ                    //����Ѿ��������
	//RecvLoginBaseInfo(p);     //��ҵ�½ȡ���û��Ļ�����Ϣ      //����Ѿ��������   
	//RecvAccountCheck(p);		//ȡ���˺ŵ���֤				//����Ѿ��������
	//RecvVisitorCheck(p);
} 
//
void OnRecv(RakNet::Packet* pData)
{
	MsgBase* msg = (MsgBase*)(pData->data);
	if (msg && msg->SysID == ID_L2C)
	{
		switch (msg->MsgID)
		{
		case S2C_Login_ServerList:
			{
				S2CLoginServerList * temp = (S2CLoginServerList*)msg;
				if (temp)
				{
					ServerListIt* ttt= &(temp->serverListInfo);
					for (int i =0; i < temp->serverNum; i++)
					{
						ServerListIt it;
						memcpy(&it, ttt ,sizeof(ServerListIt));
						ttt++;
					}
				}
			}
			break;
		case Msg_Notify:
			{
				MsgNotify* temp =  (MsgNotify*)(pData->data);
				printf("%d\n", temp->Type);
				switch (temp->Type)
				{
				case Notify_S2C_Login_OK:	//��½�ɹ�
					{
						//��ѯ�������б�
						MsgNotify ttt(ID_C2L, Notify_C2S_Login_ServerList);
						Send((char*)&ttt, sizeof(ttt));
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
}


unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char) p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char) p->data[0];
}

void OnConnect()
{//����
	//C2S_VerCheck msg;
	//msg.mainVer = 1;
	//msg.subVer = 1;
	//int sendLen = Send((char*)(&msg), sizeof(msg));
	//printf("Send return %d.....\n", sendLen);
	SendConnect();


}

int _tmain(int argc, _TCHAR* argv[])
{
	//����
	m_peer=RakNet::RakPeerInterface::GetInstance();
	m_peer->AllowConnectionResponseIPMigration(false);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.socketFamily=AF_INET;
	m_peer->Startup(1,&socketDescriptor, 1);
	m_peer->SetOccasionalPing(true);

	RakNet::ConnectionAttemptResult car = m_peer->Connect(m_ServerIP, curPort, CONNECTION_PASSWORD, (int)strlen(CONNECTION_PASSWORD));
	//RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);

	//���ա���ӡ
	while( 1 )
	{
		RakNet::Packet* p;
		for (p=m_peer->Receive(); p; m_peer->DeallocatePacket(p), p=m_peer->Receive())
		{
			// We got a packet, get the identifier with our handy function
			m_packetIdentifier = GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch (m_packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				exit(1);
				break;
			case ID_ALREADY_CONNECTED:
				printf("ID_ALREADY_CONNECTED with guid %s\n", p->systemAddress.ToString());
				_sleep(3000);
				m_peer->Connect(m_ServerIP, curPort, CONNECTION_PASSWORD, (int)strlen(CONNECTION_PASSWORD));				
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				// Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;

			case ID_INVALID_PASSWORD:
				printf("ID_INVALID_PASSWORD\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				exit(1);
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				OnConnect();
				break;
			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				printf("Ping from %s\n", p->systemAddress.ToString(true));
				break;
			default:
				// It's a client, so just show the message
				printf("from %s: recv: %d tytes \n", p->systemAddress.ToString(), p->length);
				OnRecv(p);
				break;
			}

		}
	}
	// Be nice and let the server know we quit.
	return 0;
}