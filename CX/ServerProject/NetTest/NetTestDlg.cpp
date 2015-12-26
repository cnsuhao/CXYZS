
// NetTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NetTest.h"
#include "NetTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetTestDlg 对话框
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


// CNetTestDlg 消息处理程序

BOOL CNetTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitCon();
	InitListCtrl();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNetTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNetTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//	初始化List Control
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

	//连接
	m_peer=RakNet::RakPeerInterface::GetInstance();
	m_peer->AllowConnectionResponseIPMigration(false);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.socketFamily=AF_INET;
	m_peer->Startup(1,&socketDescriptor, 1);
	m_peer->SetOccasionalPing(true);

	RakNet::ConnectionAttemptResult car = m_peer->Connect(m_ServerIP, m_curPort, CONNECTION_PASSWORD, (int)strlen(CONNECTION_PASSWORD));

	//开启线程
	Start();
}

//开启线程
void CNetTestDlg::Start()
{
	m_hServerThread	= NULL;
	m_hEvtStop	= CreateEvent(NULL,TRUE,FALSE,NULL);

	DWORD dwRecvThreadID = NULL;
	m_hServerThread	= CreateThread(NULL,0,CNetTestDlg::ThreadFunc,this,0,&dwRecvThreadID);
}

//关闭线程
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

//线程方法
DWORD WINAPI CNetTestDlg::ThreadFunc(LPVOID lpParam)
{
	CNetTestDlg *pPar = (CNetTestDlg *)lpParam;
	pPar->subThreadRecv();
	return 0;
}

//线程实现
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
						AddInformation(TEXT("创建帮派成功！"));
					};
					break;
				case Notity_S2C_Create_Fail_InGang:
					{
						AddInformation(TEXT("你已在帮派之中，不可创建帮派！"));
					};
					break;
				case Notity_S2C_Apply_OK:
					{
						//帮派名 temp->szPar
						AddInformation(TEXT("你已申请加入帮派，请等待管理员审核！"));
					};
					break;
				case Notity_S2C_Apply_Fail_InGang:
					{
						AddInformation(TEXT("你已在帮派之中，不可申请加入帮派！"));
					};
					break;
				case Notity_S2C_Reply_OK:
					{
						AddInformation(TEXT("你的入帮申请已经被批准！"));
					};
					break;
				case Notity_S2C_Reply_Refused:
					{
						AddInformation(TEXT("你的入帮申请已经被拒绝！"));
					};
					break;
				case Notity_S2C_GETAPPLYLIST:
					{
						AddInformation(TEXT("获得入帮申请列表！"));
					};
					break;
				case Notity_S2C_LEAVE_GANG:
					{
						int64 iHeader = temp->iPar[0];
						//比较当前玩家ID
						AddInformation(TEXT("提示玩家退出帮派！"));
					};
					break;
				case Notity_S2C_PUT_GANG:
					{
						AddInformation(TEXT("你被踢出了帮派！"));
					};
					break;
				case Notity_S2C_GET_GANGLIST:
					{
						AddInformation(TEXT("你获得了帮派列表！"));
					};
					break;
				case Notity_S2C_GET_Announce:
					{
						AddInformation(TEXT("你获得了帮派公告！"));
					};
					break;
				case Notity_S2C_EDIT_Announce_OK:
					{
						AddInformation(TEXT("修改帮派公告成功！"));
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
// 						//单向消息
// 						sprintf(szTemp,"%d说:%s",temp->iSendRoleID,temp->szPar);
// 						AddInformation(A2W(szTemp));
// 
// 					};
// 					break;
// 				case Notify_S2C_Recive_Channel:
// 					{
// 						//频道消息，根据temp->iChannel频道
// 						switch (temp->iChannel)
// 						{
// 						case Channel_World:
// 							{
// 								sprintf(szTemp,"[世界]%d说:%s",temp->iSendRoleID,temp->szPar);
// 								AddInformation(A2W(szTemp));
// 							}
// 							break;
// 						case Channel_Near:
// 							{
// 								sprintf(szTemp,"[附近]%d说:%s",temp->iSendRoleID,temp->szPar);
// 								AddInformation(A2W(szTemp));
// 							}
// 							break;
// // 						case Channel_Gang:
// // 							{
// // 								sprintf(szTemp,"[帮派]%d说:%s",temp->iSendRoleID,temp->szPar);
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
						AddInformation(TEXT("创建队伍成功！"));
					}
					break;
				case Notify_S2C_APPLY_HEADER:
					{
						AddInformation(TEXT("有人申请加入你的队伍"));
					};
					break;
				case Notify_S2C_APPLY_OK:
					{
						sprintf(szTemp,"你已申请加入%s的队伍！",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_TeamFull:
					{
						AddInformation(TEXT("队伍已满！"));
					}
					break;
				case Notify_S2C_APPLY_FAILURE:
					{
						sprintf(szTemp,"请确认%s是否为队长！",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_OK_HEADER:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"你已添加ID为%d的玩家为队员！",temp->iRoleID);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_OK:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"你已成功加入队长为%d的队伍！",temp->iRoleID);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_FAILURE_HEADER:
					{
						sprintf(szTemp,"你拒绝了%s的组队申请！",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_ADDTEAM_FAILURE:
					{
						sprintf(szTemp,"%s拒绝了你的组队申请",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_INVITE_HEADER:
					{
						sprintf(szTemp,"你已成功邀请玩家%s加入队伍!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_INVITE_OK:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"玩家%d邀请你加入他的队伍!",temp->iRoleID);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_INVITE_FAILURE:
					{
						sprintf(szTemp,"邀请失败，玩家%s已加入其它队伍!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_GET_TEAMLIST:
					{
						//vector<int>	*vecList = (vector<int> *)temp->szPar;
						sprintf(szTemp,"获取成功!");
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_LEAVETEAM_HEADER:
					{
						sprintf(szTemp,"玩家%s已离开队伍!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_LEAVETEAM:
					{
						sprintf(szTemp,"你已经离开由玩家%s组建的队伍!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_DISSOLVE:
					{
						AddInformation(TEXT("你的队伍已经被解散！"));
					}
					break;
				case Notify_S2C_PUTTEAM_HEADER:
					{
						sprintf(szTemp,"你已经把玩家%s提出队伍!",temp->szPar);
						AddInformation(A2W(szTemp));
					}
					break;
				case Notify_S2C_PUTTEAM:
					{
						sprintf(szTemp,"你已经被玩家%s提出队伍!",temp->szPar);
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
					AddInformation(TEXT("添加好友成功！"));
					break;
				case Notify_S2C_ADDEDFRI_OK:
					//sprintf(szTemp,"%s已经添加你为好友！",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_ADDBLACK_OK:
					AddInformation(TEXT("添加黑名单成功！"));
					break;
				case Notify_S2C_ADDFRI_FAILURE:
					//sprintf(szTemp,"%s已经是你的好友！",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_ADDBLACK_FAILURE:
					//sprintf(szTemp,"%s已经在你的黑名单中！",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_FRINULL:
					AddInformation(TEXT("无此用户！"));
					break;
				case Notify_S2C_DELFRI_OK:
					//sprintf(szTemp,"你已成功将玩家%s从好友列表中删除！",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_DELFRI_FAILURE:
					//sprintf(szTemp,"删除失败，玩家%s不在你的好友列表中！",temp->szPar);
					AddInformation(A2W(szTemp));
					break;
				case Notify_S2C_DELBLACK_FAILURE:
					//sprintf(szTemp,"删除失败，玩家%s不在你的黑名单列表中！",temp->szPar);
					AddInformation(A2W(szTemp));
					break;

				case Notify_S2C_GETFRILIST_OK:
					{
						//vector<int> *vecList = (vector<int> *)temp->szPar;
						AddInformation(TEXT("获取玩家的好友名单成功！"));
					}
					break;
				case Notify_S2C_GETBLACKLIST_OK:
					{
						//vector<int> *vecList = (vector<int> *)temp->szPar;
						AddInformation(TEXT("获取玩家的黑名单成功！"));
					}
					break;
				case Notify_S2C_GETFRI_OK:
					{
						//Player *pTarget = (Player *)temp->szPar;
						AddInformation(TEXT("获取好友信息成功！"));
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
					AddInformation(TEXT("注册账号名长度不对！"));
					break;
				case Notify_S2C_Regiser_PasswordLen:
					AddInformation(TEXT("注册账号密码长度不对！"));
					break;
				case Notify_S2C_Regiser_NameShield:
					AddInformation(TEXT("注册账号名存在屏蔽字！"));
					break;
				case Notify_S2C_Regiser_NameRepeat:
					AddInformation(TEXT("注册账号已存在！"));
					break;
				case Notify_S2C_Regiser_OK:
					AddInformation(TEXT("注册成功！"));
					break;
				case Notify_S2C_Login_NamePassError:
					AddInformation(TEXT("账号或密码错误！"));
					break;
				case Notify_S2C_Login_OK:
					AddInformation(TEXT("登陆成功！"));
					break;
				case Notify_C2S_Login_ServerList:
					AddInformation(TEXT("查询服务器列表！"));
					break;
				case Notify_S2C_Login_ServerNotOpen:
					AddInformation(TEXT("服务器未开启！"));
					break;
				case Notify_S2C_Login_ReLogin:
					AddInformation(TEXT("服务器要求客户端重新登陆！"));
					break;
				case Notify_S2C_CreateRole:
					AddInformation(TEXT("服务器要求客户端创建角色！"));
					break;
				case Notify_S2C_CreateRole_NameRepeat:
					AddInformation(TEXT("角色名已存在！"));
					break;
				case Notify_S2C_CreateRole_OK:
					AddInformation(TEXT("角色创建成功！"));
					break;
				default:
					AddInformation(TEXT("未知 MsgNotify！"));
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
	case Msg_Regiser_Account:	//注册
		{
			MsgRegiser_Login Msg(szPar1, szPar2, 0, Msg_Regiser_Account);
			Send(&Msg);
		}
		break;
	case Msg_Account_Login:				//登录
		{
			MsgRegiser_Login Msg(szPar1, szPar2, 0, Msg_Account_Login);
			Send(&Msg);
		}
		break;		
	case Msg_SelServer:			//选择服务器
		{
			MsgSelServer Msg(atoi(szPar1));	
			Send(&Msg);
		}
		break;	
	case Msg_CreateRole:			//创建角色
		{
			MsgCreateRole Msg(szPar1,atoi(szPar2), atoi(szPar3));				
			Send(&Msg);
		}
		break;		
	case Msg_Add_Friends:			//添加好友
		{
			MsgFriends Msg(atoi(szPar1),atoi(szPar2), atoi(szPar3),"");	
			Send(&Msg);
		}
		break;
	case Msg_Del_Friends:			//删除好友
		{
			MsgFriends Msg(atoi(szPar1),atoi(szPar2), atoi(szPar3),"");
			Msg.MsgID = Msg_Del_Friends;
			Send(&Msg);
		}
		break;
	case Msg_Get_FriendsList:		//请求好友列表
		{
			MsgFriends Msg(atoi(szPar1),atoi(0), atoi(szPar3),"");	
			Msg.MsgID = Msg_Get_FriendsList;
			Send(&Msg);
		}
		break;
	case Msg_Get_PlayInfo:		//请求好友/黑名单信息
		{
			MsgFriends Msg(atoi(szPar1),atoi(szPar2), atoi(szPar3),"");	
			Msg.MsgID = Msg_Get_PlayInfo;
			Send(&Msg);
		}
		break;
	case Msg_Apply_Team:		//申请加队
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),0);	
			Send(&Msg);
		}
		break;
	case Msg_Reply_ApplyTeam:		//回复申请
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Reply_ApplyTeam;
			Send(&Msg);
		}
		break;
	case Msg_Invite_Team:		//邀请加队
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),0);
			Msg.MsgID = Msg_Invite_Team;
			Send(&Msg);
		}
		break;
	case Msg_Reply_Invite:		//回复邀请
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Reply_Invite;
			Send(&Msg);
		}
		break;
	case Msg_Leave_Team:		//离开队伍
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Leave_Team;
			Send(&Msg);
		}
		break;
	case Msg_Put_Team:			//提出队伍
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Put_Team;
			Send(&Msg);
		}
		break;
	case Msg_Get_TeamApplyList:		//获得队伍列表
		{
			MsgTeam Msg(atoi(szPar1),atoi(szPar2),1);	
			Msg.MsgID = Msg_Get_TeamApplyList;
			Send(&Msg);
		}
		break;

// 	case Msg_Chat_POINT:		//点对点聊天
// 		{
// 			MsgChat Msg(atoi(szPar1),atoi(szPar2),szPar3);	
// 			Msg.MsgID = Msg_Chat_POINT;
// 			Send(&Msg);
// 		}
// 		break;
// 	case Msg_Chat_CHANNEL:		//频道聊天
// 		{
// 			MsgChat Msg(atoi(szPar1),atoi(szPar2),szPar3);	
// 			Msg.MsgID = Msg_Chat_CHANNEL;
// 			Send(&Msg);
// 		}
	case Msg_Create_Gang:		//创建帮派
		{
			MsgGang Msg(atoi(szPar1),0,szPar3);	
			Msg.MsgID = Msg_Create_Gang;
			Send(&Msg);
		}
		break;
	case Msg_Apply_Gang:		//申请加入帮派,申请人，帮派ID
 		{
 			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
 			Msg.MsgID = Msg_Apply_Gang;
 			Send(&Msg);
 		}
 		break;
	case Msg_Reply_ApplyGang:	//回复入帮申请，申请人，帮派ID
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Reply_ApplyGang;
			Send(&Msg);
		}
		break;
	case Msg_Get_GangApplyList:	//获得申请列表，申请人，帮派ID
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Get_GangApplyList;
			Send(&Msg);
		}
		break;
	case Msg_Put_Gang:			//请离人，帮派id，
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Put_Gang;
			Send(&Msg);
		}
		break;
	case Msg_Get_GangList:		//申请人，帮派id，
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Get_GangList;
			Send(&Msg);
		}
		break;
	case Msg_Get_GangAnnounce:	//申请人，帮派id，
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),NULL);
			Msg.MsgID = Msg_Get_GangAnnounce;
			Send(&Msg);
		}
		break;
	case Msg_Edit_GangAnnounce:	//玩家ID，帮派ID，帮派公告
		{
			MsgGang Msg(atoi(szPar1),atoi(szPar2),szPar3);
			Msg.MsgID = Msg_Edit_GangAnnounce;
			Send(&Msg);
		}
		break;
	case Msg_Gang_Appoint:	//玩家ID，玩家ID
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
	//模拟登录
	MsgRegiser_Login Msg("ziwen001", "123456", 0, Msg_Account_Login);
	Send(&Msg);

	((CButton *)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
}


void CNetTestDlg::OnBnClickedButton3()
{
	//模拟选择服务器
	MsgSelServer Msg(1);	
	Send(&Msg);

	((CButton *)GetDlgItem(IDC_BUTTON3))->EnableWindow(FALSE);
}
