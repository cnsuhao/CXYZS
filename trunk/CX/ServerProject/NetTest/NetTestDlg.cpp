
// NetTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NetTest.h"
#include "NetTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetTestDlg �Ի���
using namespace std;
#include <vector>


CNetTestDlg::CNetTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNetTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNetTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CNetTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CNetTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CNetTestDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CNetTestDlg ��Ϣ�������

BOOL CNetTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	InitCon();
	InitListCtrl();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNetTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CNetTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//	��ʼ��List Control
void CNetTestDlg::InitListCtrl()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_DATA);
	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	pList->InsertColumn(0,TEXT("INFORMATION"),LVCFMT_LEFT,500);
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


void CNetTestDlg::InitCon()
{
	m_ServerIP = "127.0.0.1";
	m_curPort = 11000;

	//����
	m_peer=RakNet::RakPeerInterface::GetInstance();
	m_peer->AllowConnectionResponseIPMigration(false);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.socketFamily=AF_INET;
	m_peer->Startup(1,&socketDescriptor, 1);
	m_peer->SetOccasionalPing(true);

	RakNet::ConnectionAttemptResult car = m_peer->Connect(m_ServerIP, m_curPort, CONNECTION_PASSWORD, (int)strlen(CONNECTION_PASSWORD));

	//�����߳�
	Start();
}

//�����߳�
void CNetTestDlg::Start()
{
	m_hServerThread	= NULL;
	m_hEvtStop	= CreateEvent(NULL,TRUE,FALSE,NULL);

	DWORD dwRecvThreadID = NULL;
	m_hServerThread	= CreateThread(NULL,0,CNetTestDlg::ThreadFunc,this,0,&dwRecvThreadID);
}

//�ر��߳�
void CNetTestDlg::Stop()
{
	SetEvent(m_hEvtStop);
	WaitForSingleObject(m_hServerThread,INFINITE);
	if (m_hEvtStop != NULL)
	{
		CloseHandle(m_hEvtStop);
		m_hEvtStop = NULL;
	}
	if (m_hServerThread != NULL)
	{
		CloseHandle(m_hServerThread);
		m_hServerThread = NULL;
	}
}

//�̷߳���
DWORD WINAPI CNetTestDlg::ThreadFunc(LPVOID lpParam)
{
	CNetTestDlg *pPar = (CNetTestDlg *)lpParam;
	pPar->subThreadRecv();
	return 0;
}

//�߳�ʵ��
bool CNetTestDlg::subThreadRecv()
{
	USES_CONVERSION;
	while(WAIT_TIMEOUT == WaitForSingleObject(m_hEvtStop,0))
	{
		RakNet::Packet* p;
		for (p=m_peer->Receive(); p; m_peer->DeallocatePacket(p), p=m_peer->Receive())
		{
			// We got a packet, get the identifier with our handy function
			m_packetIdentifier = GetPacketIdentifier(p);

			CString strTemp;
			char szTemp[30];
			
			// Check if this is a network message packet
			switch (m_packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				AddInformation(TEXT("ID_DISCONNECTION_NOTIFICATION"));
				exit(1);
				break;
			case ID_ALREADY_CONNECTED:
				sprintf_s(szTemp,"%s",p->systemAddress.ToString());
				strTemp.Format(TEXT("ID_ALREADY_CONNECTED with guid %s"),A2W(szTemp));
				AddInformation(strTemp);
				Sleep(3000);
				m_peer->Connect(m_ServerIP, m_curPort, CONNECTION_PASSWORD, (int)strlen(CONNECTION_PASSWORD));				
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				AddInformation(TEXT("ID_INCOMPATIBLE_PROTOCOL_VERSION"));
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				AddInformation(TEXT("ID_REMOTE_DISCONNECTION_NOTIFICATION"));
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				AddInformation(TEXT("ID_REMOTE_CONNECTION_LOST"));
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				AddInformation(TEXT("ID_REMOTE_NEW_INCOMING_CONNECTION"));
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				AddInformation(TEXT("We are banned from this server."));
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				AddInformation(TEXT("Connection attempt failed"));
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				// Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				AddInformation(TEXT("ID_NO_FREE_INCOMING_CONNECTIONS"));
				break;

			case ID_INVALID_PASSWORD:
				AddInformation(TEXT("ID_INVALID_PASSWORD"));
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				AddInformation(TEXT("ID_CONNECTION_LOST"));				
				m_peer->Connect(m_ServerIP, m_curPort, CONNECTION_PASSWORD, (int)strlen(CONNECTION_PASSWORD));	
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				AddInformation(TEXT("ID_CONNECTION_REQUEST_ACCEPTED"));
				//OnConnect();
				//SendRegister();
				break;
			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				strTemp.Format(TEXT("Ping from %s\n"),p->systemAddress.ToString(true));
				AddInformation(strTemp);
				break;
			default:
				// It's a client, so just show the message
				sprintf_s(szTemp,"%s",p->systemAddress.ToString());
				strTemp.Format(TEXT("from %s: recv: %d tytes "),A2W(szTemp), p->length);
				AddInformation(strTemp);
				OnRecv(p);
				break;
			}
		}
	}
	return true;
}

int CNetTestDlg::Send(MsgBase* msg)
{
	if (msg->Len < sizeof(msg))
	{
		CString strTemp;
		strTemp.Format(TEXT("Error~~~%d msg->Len %d"), msg->MsgID, msg->Len);
		AddInformation(strTemp);
	}
	
	RakNet::SystemAddress m_peerSystemAddress(m_ServerIP, m_curPort);
	return m_peer->Send((char*)msg, msg->Len, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_peerSystemAddress, false);
}

void CNetTestDlg::OnRecv(RakNet::Packet* pData)
{
	USES_CONVERSION;
	char szTemp[50];
	MsgBase* msg = (MsgBase*)(pData->data);
	if (msg)
	{
		switch (msg->MsgID)
		{
		case Msg_Gang_Notify:
			{
				MsgGangNotify* temp =  (MsgGangNotify*)(pData->data);
				switch (temp->Type)
				{
				case Notity_S2C_Create_OK:
					{
						AddInformation(TEXT("�������ɳɹ���"));
					};
					break;
				case Notity_S2C_Create_Fail_InGang:
					{
						AddInformation(TEXT("�����ڰ���֮�У����ɴ������ɣ�"));
					};
					break;
				case Notity_S2C_Apply_OK:
					{
						//������ temp->szPar
						AddInformation(TEXT("�������������ɣ���ȴ�����Ա��ˣ�"));
					};
					break;
				case Notity_S2C_Apply_Fail_InGang:
					{
						AddInformation(TEXT("�����ڰ���֮�У��������������ɣ�"));
					};
					break;
				case Notity_S2C_Reply_OK:
					{
						AddInformation(TEXT("�����������Ѿ�����׼��"));
					};
					break;
				case Notity_S2C_Reply_Refused:
					{
						AddInformation(TEXT("�����������Ѿ����ܾ���"));
					};
					break;
				case Notity_S2C_GETAPPLYLIST:
					{
						AddInformation(TEXT("�����������б�"));
					};
					break;
				case Notity_S2C_LEAVE_GANG:
					{
						int64 iHeader = temp->iPar[0];
						//�Ƚϵ�ǰ���ID
						AddInformation(TEXT("��ʾ����˳����ɣ�"));
					};
					break;
				case Notity_S2C_PUT_GANG:
					{
						AddInformation(TEXT("�㱻�߳��˰��ɣ�"));
					};
					break;
				case Notity_S2C_GET_GANGLIST:
					{
						AddInformation(TEXT("�����˰����б�"));
					};
					break;
				case Notity_S2C_GET_Announce:
					{
						AddInformation(TEXT("�����˰��ɹ��棡"));
					};
					break;
				case Notity_S2C_EDIT_Announce_OK:
					{
						AddInformation(TEXT("�޸İ��ɹ���ɹ���"));
					};
					break;
				default:
					break;
				}
			}
			break;
// 		case Msg_Chat_Notify:
// 			{
// 				MsgChatNotify* temp =  (MsgChatNotify*)(pData->data);
// 				switch (temp->Type)
// 				{
// 				case Notify_S2C_Recive_Point:
// 					{
// 						//������Ϣ
// 						sprintf(szTemp,"%d˵:%s",temp->iSendRoleID,temp->szPar);
// 						AddInformation(A2W(szTemp));
// 
// 					};
// 					break;
// 				case Notify_S2C_Recive_Channel:
// 					{
// 						//Ƶ����Ϣ������temp->iChannelƵ��
// 						switch (temp->iChannel)
// 						{
// 						case Channel_World:
// 							{
// 								sprintf(szTemp,"[����]%d˵:%s",temp->iSendRoleID,temp->szPar);
// 								AddInformation(A2W(szTemp));
// 							}
// 							break;
// 						case Channel_Near:
// 							{
// 								sprintf(szTemp,"[����]%d˵:%s",temp->iSendRoleID,temp->szPar);
// 								AddInformation(A2W(szTemp));
// 							}
// 							break;
// // 						case Channel_Gang:
// // 							{
// // 								sprintf(szTemp,"[����]%d˵:%s",temp->iSendRoleID,temp->szPar);
// // 								AddInformation(A2W(szTemp));
// // 							}
// // 							break;
// 						default:
							break;
// 						}
// 					};
// 					break;
// 				default:
					break;
// 				}
// 			};
// 			break;
		case Msg_Team_Notify:
			{
				MsgTeamNotify* temp =  (MsgTeamNotify*)(pData->data);
				switch (temp->Type)
				{
				case Notify_S2C_CRATE_OK:
					{
						AddInformation(TEXT("��������ɹ���"));
					}
					break;
				case Notify_S2C_APPLY_HEADER:
					{
						AddInformation(TEXT("�������������Ķ���"));
					};
					break;
				case Notify_S2C_APPLY_OK:
					{
						sprintf(szTemp,"�����������%s�Ķ��飡",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_TeamFull:
					{
						AddInformation(TEXT("����������"));
					}
					break;
				case Notify_S2C_APPLY_FAILURE:
					{
						sprintf(szTemp,"��ȷ��%s�Ƿ�Ϊ�ӳ���",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_OK_HEADER:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"�������IDΪ%d�����Ϊ��Ա��",temp->iRoleID);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_OK:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"���ѳɹ�����ӳ�Ϊ%d�Ķ��飡",temp->iRoleID);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_FAILURE_HEADER:
					{
						sprintf(szTemp,"��ܾ���%s��������룡",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_FAILURE:
					{
						sprintf(szTemp,"%s�ܾ�������������",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_INVITE_HEADER:
					{
						sprintf(szTemp,"���ѳɹ��������%s�������!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_INVITE_OK:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"���%d������������Ķ���!",temp->iRoleID);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_INVITE_FAILURE:
					{
						sprintf(szTemp,"����ʧ�ܣ����%s�Ѽ�����������!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_GET_TEAMLIST:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"��ȡ�ɹ�!");
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_LEAVETEAM_HEADER:
					{
						sprintf(szTemp,"���%s���뿪����!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_LEAVETEAM:
					{
						sprintf(szTemp,"���Ѿ��뿪�����%s�齨�Ķ���!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_DISSOLVE:
					{
						AddInformation(TEXT("��Ķ����Ѿ�����ɢ��"));
					}
					break;
				case Notify_S2C_PUTTEAM_HEADER:
					{
						sprintf(szTemp,"���Ѿ������%s�������!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_PUTTEAM:
					{
						sprintf(szTemp,"���Ѿ������%s�������!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				default:
					break;
				}
			};
			break;
		case Msg_Fri_Notify:
			{
				MsgFriNotify* temp =  (MsgFriNotify*)(pData->data);
				switch (temp->Type)
				{
				case Notify_S2C_ADDFRI_OK:
					AddInformation(TEXT("��Ӻ��ѳɹ���"));
					break;
				case Notify_S2C_ADDEDFRI_OK:
					//sprintf(szTemp,"%s�Ѿ������Ϊ���ѣ�",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_ADDBLACK_OK:
					AddInformation(TEXT("��Ӻ������ɹ���"));
					break;
				case Notify_S2C_ADDFRI_FAILURE:
					//sprintf(szTemp,"%s�Ѿ�����ĺ��ѣ�",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_ADDBLACK_FAILURE:
					//sprintf(szTemp,"%s�Ѿ�����ĺ������У�",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_FRINULL:
					AddInformation(TEXT("�޴��û���"));
					break;
				case Notify_S2C_DELFRI_OK:
					//sprintf(szTemp,"���ѳɹ������%s�Ӻ����б���ɾ����",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_DELFRI_FAILURE:
					//sprintf(szTemp,"ɾ��ʧ�ܣ����%s������ĺ����б��У�",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_DELBLACK_FAILURE:
					//sprintf(szTemp,"ɾ��ʧ�ܣ����%s������ĺ������б��У�",temp->szPar);
					AddInformation(A2W(szTemp));
					break;

				case Notify_S2C_GETFRILIST_OK:
					{
						//vector<int> *vecList = (vector<int> *)temp->szPar;
						AddInformation(TEXT("��ȡ��ҵĺ��������ɹ���"));
					}
					break;
				case Notify_S2C_GETBLACKLIST_OK:
					{
						//vector<int> *vecList = (vector<int> *)temp->szPar;
						AddInformation(TEXT("��ȡ��ҵĺ������ɹ���"));
					}
					break;
				case Notify_S2C_GETFRI_OK:
					{
						//Player *pTarget = (Player *)temp->szPar;
						AddInformation(TEXT("��ȡ������Ϣ�ɹ���"));
					}
					break;
				default:
					break;
				}

			};
			break;
		case Msg_Notify:
			{
				MsgNotify* temp =  (MsgNotify*)(pData->data);

				switch (temp->Type)
				{
				case Notify_S2C_Regiser_NameLen:
					AddInformation(TEXT("ע���˺������Ȳ��ԣ�"));
					break;
				case Notify_S2C_Regiser_PasswordLen:
					AddInformation(TEXT("ע���˺����볤�Ȳ��ԣ�"));
					break;
				case Notify_S2C_Regiser_NameShield:
					AddInformation(TEXT("ע���˺������������֣�"));
					break;
				case Notify_S2C_Regiser_NameRepeat:
					AddInformation(TEXT("ע���˺��Ѵ��ڣ�"));
					break;
				case Notify_S2C_Regiser_OK:
					AddInformation(TEXT("ע��ɹ���"));
					break;
				case Notify_S2C_Login_NamePassError:
					AddInformation(TEXT("�˺Ż��������"));
					break;
				case Notify_S2C_Login_OK:
					AddInformation(TEXT("��½�ɹ���"));
					break;
				case Notify_C2S_Login_ServerList:
					AddInformation(TEXT("��ѯ�������б�"));
					break;
				case Notify_S2C_Login_ServerNotOpen:
					AddInformation(TEXT("������δ������"));
					break;
				case Notify_S2C_Login_ReLogin:
					AddInformation(TEXT("������Ҫ��ͻ������µ�½��"));
					break;
				case Notify_S2C_CreateRole:
					AddInformation(TEXT("������Ҫ��ͻ��˴�����ɫ��"));
					break;
				case Notify_S2C_CreateRole_NameRepeat:
					AddInformation(TEXT("��ɫ���Ѵ��ڣ�"));
					break;
				case Notify_S2C_CreateRole_OK:
					AddInformation(TEXT("��ɫ�����ɹ���"));
					break;
				default:
					AddInformation(TEXT("δ֪ MsgNotify��"));
					break;
				}				
			}
			break;
		default:
			break;
		}
	}
}


void CNetTestDlg::OnBnClickedButton2()
{
	USES_CONVERSION;
	char szTemp[128],szPar1[128],szPar2[128],szPar3[128],szPar4[128],szPar5[128];

	CString strTemp,strPar1,strPar2,strPar3,strPar4,strPar5;
	GetDlgItemText(IDC_EDIT1,strTemp);
	GetDlgItemText(IDC_EDIT2,strPar1);
	GetDlgItemText(IDC_EDIT3,strPar2);
	GetDlgItemText(IDC_EDIT4,strPar3);
	GetDlgItemText(IDC_EDIT5,strPar4);
	GetDlgItemText(IDC_EDIT6,strPar5);	
	sprintf_s(szTemp,"%s",W2A(strTemp));
	sprintf_s(szPar1,"%s",W2A(strPar1));
	sprintf_s(szPar2,"%s",W2A(strPar2));
	sprintf_s(szPar3,"%s",W2A(strPar3));
	sprintf_s(szPar4,"%s",W2A(strPar4));
	sprintf_s(szPar5,"%s",W2A(strPar5));
	switch (atoi(szTemp))
	{
	case Msg_Regiser_Account:	//ע��
		{
			MsgRegiser_Login Msg(szPar1, szPar2, 0, Msg_Regiser_Account);
			Send(&Msg);
		}
		break;
	case Msg_Account_Login:				//��¼
		{
			MsgRegiser_Login Msg(szPar1, szPar2, 0, Msg_Account_Login);
			Send(&Msg);
		}
		break;		
	case Msg_SelServer:			//ѡ�������
		{
			MsgSelServer Msg(atoi(szPar1));	
			Send(&Msg);
		}
		break;	
	case Msg_CreateRole:			//������ɫ
		{
			MsgCreateRole Msg(szPar1,atoi(szPar2), atoi(szPar3));				
			Send(&Msg);
		}
		break;		
	case Msg_Add_Friends:			//��Ӻ���
		{
			MsgFriends Msg(atoi(szPar1),atoi(szPar2), atoi(szPar3),"");	
			Send(&Msg);
		}
		break;
	case Msg_Del_Friends:			//ɾ������
		{
			MsgFriends Msg(atoi(szPar1),atoi(szPar2), atoi(szPar3),"");
			Msg.MsgID = Msg_Del_Friends;
			Send(&Msg);
		}
		break;
	case Msg_Get_FriendsList:		//��������б�
		{
			MsgFriends Msg(atoi(szPar1),atoi(0), atoi(szPar3),"");	
			Msg.MsgID = Msg_Get_FriendsList;
			Send(&Msg);
		}
		break;
	case Msg_Get_PlayInfo:		//�������/��������Ϣ
		{
			MsgFriends Msg(atoi(szPar1),atoi(szPar2), atoi(szPar3),"");	
			Msg.MsgID = Msg_Get_PlayInfo;
			Send(&Msg);
		}
		break;
	case Msg_Apply_Team:		//����Ӷ�
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),0);	
			Send(&Msg);
		}
		break;
	case Msg_Reply_ApplyTeam:		//�ظ�����
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Reply_ApplyTeam;
			Send(&Msg);
		}
		break;
	case Msg_Invite_Team:		//����Ӷ�
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),0);
			Msg.MsgID = Msg_Invite_Team;
			Send(&Msg);
		}
		break;
	case Msg_Reply_Invite:		//�ظ�����
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Reply_Invite;
			Send(&Msg);
		}
		break;
	case Msg_Leave_Team:		//�뿪����
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Leave_Team;
			Send(&Msg);
		}
		break;
	case Msg_Put_Team:			//�������
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Put_Team;
			Send(&Msg);
		}
		break;
	case Msg_Get_TeamApplyList:		//��ö����б�
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Get_TeamApplyList;
			Send(&Msg);
		}
		break;

// 	case Msg_Chat_POINT:		//��Ե�����
// 		{
// 			MsgChat Msg(atoi(szPar1),atoi(szPar2),szPar3);	
// 			Msg.MsgID = Msg_Chat_POINT;
// 			Send(&Msg);
// 		}
// 		break;
// 	case Msg_Chat_CHANNEL:		//Ƶ������
// 		{
// 			MsgChat Msg(atoi(szPar1),atoi(szPar2),szPar3);	
// 			Msg.MsgID = Msg_Chat_CHANNEL;
// 			Send(&Msg);
// 		}
	case Msg_Create_Gang:		//��������
		{
			MsgGang Msg(atoi(szPar1),0,szPar3);	
			Msg.MsgID = Msg_Create_Gang;
			Send(&Msg);
		}
		break;
	case Msg_Apply_Gang:		//����������,�����ˣ�����ID
 		{
 			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
 			Msg.MsgID = Msg_Apply_Gang;
 			Send(&Msg);
 		}
 		break;
	case Msg_Reply_ApplyGang:	//�ظ�������룬�����ˣ�����ID
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Reply_ApplyGang;
			Send(&Msg);
		}
		break;
	case Msg_Get_GangApplyList:	//��������б������ˣ�����ID
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Get_GangApplyList;
			Send(&Msg);
		}
		break;
	case Msg_Put_Gang:			//�����ˣ�����id��
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Put_Gang;
			Send(&Msg);
		}
		break;
	case Msg_Get_GangList:		//�����ˣ�����id��
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Get_GangList;
			Send(&Msg);
		}
		break;
	case Msg_Get_GangAnnounce:	//�����ˣ�����id��
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Get_GangAnnounce;
			Send(&Msg);
		}
		break;
	case Msg_Edit_GangAnnounce:	//���ID������ID�����ɹ���
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),szPar3);
			Msg.MsgID = Msg_Edit_GangAnnounce;
			Send(&Msg);
		}
		break;
	case Msg_Gang_Appoint:	//���ID�����ID
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Gang_Appoint;
			Send(&Msg);
		}
		break;
	case Msg_Task_Take:
		{
			MsgTask_TakeTask msg;
			msg.taskId=atoi(szPar1);
			msg.iReqTaskID = atoi(szPar2);
			msg.itemId=atoi(szPar3);
			msg.MsgID=Msg_Task_Take;
			Send(&msg);
			
		}
		break;
	default:
		break;
	}
}

void CNetTestDlg::OnBnClickedButton1()
{
	//ģ���¼
	MsgRegiser_Login Msg("ziwen001", "123456", 0, Msg_Account_Login);
	Send(&Msg);

	((CButton *)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
}


void CNetTestDlg::OnBnClickedButton3()
{
	//ģ��ѡ�������
	MsgSelServer Msg(1);	
	Send(&Msg);

	((CButton *)GetDlgItem(IDC_BUTTON3))->EnableWindow(FALSE);
}
