#include "GameServer.h"
#include "INIParser.h"
#include "windows.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "CSVParser.h"
#include "LogManage.h"
#include "CSVFileManager.h"
#include "MapManager.h"
#include "ItemManager.h"
#include "TaskManager.h"
#include "BattleManager.h"
#include "Player.h"
#include "SocialManager.h"
#include "StackStream.h"
#include "Map.h"
#include "CopyManager.h"
#include "Team.h"
#include "Team.h"
#include <time.h>
#include "MoveComponent.h"
#include "BattleComponent.h"

GameServer *g_GameServer = NULL;

uint64 GetAbsoluteMsecTime()
{
#ifdef _WIN32
    static bool Initialized = false;
    static uint32 timeTicksPerMillis = 0;
    if (!Initialized)
    {
        LARGE_INTEGER tps;
        QueryPerformanceFrequency(&tps);
        timeTicksPerMillis = uint32(tps.QuadPart / 1000L);
        Initialized = true;
    }

    LARGE_INTEGER queryTime;
    QueryPerformanceCounter(&queryTime);
    return queryTime.QuadPart / timeTicksPerMillis;
#else
    struct timespec queryTime;
    clock_gettime(CLOCK_MONOTONIC, &queryTime);
    return (1000.0 * queryTime.tv_sec) + (0.000001 * queryTime.tv_nsec);
#endif // _WIN32
}


#pragma comment(lib, "ws2_32.lib")
GameServer::GameServer(void)
{
}


GameServer::~GameServer(void)
{
	Stop();
}

bool GameServer::Start(uint16 ServerID)
{
	m_ServerID = ServerID;
//��־��������
	gLog.SetName("GameServer");
//����������
	INIParser ini;
	if( ! ini.open("ServerConfig.ini") )	
		return false;
	m_GateServerIP = ini.read( "Server" , "GateServerIP" );
	std::string value = ini.read( "Server" , "GateServerPort" );
	m_GateServerPort = atoi(value.c_str());	
	m_GateAddress.FromStringExplicitPort(m_GateServerIP.c_str(), m_GateServerPort);
	m_LoginServerIP = ini.read( "Server" , "LoginServerIP" );
	value = ini.read( "Server" , "LoginServerPort" );
	m_LoginServerPort = atoi(value.c_str());	
	m_LoginAddress.FromStringExplicitPort(m_LoginServerIP.c_str(), m_LoginServerPort);
    std::string tablePath = ini.read("Server", "TabelPath");

//��csv��Ϸ����������
	CSVParser csv;
	csv.LoadFile("Config.csv");
	csv.GetLine();	//���Ե�һ��
	while (csv.GetLine())
	{
		if (csv.GetInt() == m_ServerID)
		{
			csv.GetStr();	//��������
			csv.GetInt();	//������״̬
			csv.GetStr();			//ip
			m_Port = csv.GetInt();	//�˿�
			m_DBServerIP = csv.GetStr();	//���ݿ������ip
			m_DBServerPort = csv.GetInt();	//���ݿ�������˿�
			m_DBAddress.FromStringExplicitPort(m_DBServerIP.c_str(), m_DBServerPort);
			printf("Start~~~GameServer  Index = %d   Port = %d\n", m_ServerID, m_Port);
			break;
		}
	}
// ��Ϸ��������
    g_CSVFileManager = new CSVFileManager();
    if (!tablePath.empty())
    {
        gLog.Notify("set table path=%s", tablePath.c_str());
        g_CSVFileManager->m_TablePath.assign(tablePath);
    }
    g_CSVFileManager->Init();
    gLog.Info("table load finished!");

//��ʼ������	
	m_Net = RakNet::RakPeerInterface::GetInstance();
	m_Net->SetIncomingPassword(SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));	//���ý�������
	m_Net->SetOccasionalPing(true);
	m_Net->SetUnreliableTimeout(LINK_TIMEOUT_TIME);	//���ó�ʱʱ��
	if (m_Net->Startup(GAMESERVER_CONNECTIONS_MAX, &RakNet::SocketDescriptor(m_Port,0), 1) != RakNet::RAKNET_STARTED)
	{
		//����������ʧ��
		m_StopFlag = true;
		return false;
	}	
	m_Net->SetMaximumIncomingConnections(GAMESERVER_CONNECTIONS_MAX);	//��������������С�������������

	//���ӵ�����
	m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));
	//���ӵ���½
	m_Net->Connect(m_LoginServerIP.c_str(), m_LoginServerPort, SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));
	//���ӵ����ݿ������
	m_Net->Connect(m_DBServerIP.c_str(), m_DBServerPort, SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));
	gLog.Info("net work initialize finished!");
//��ʼ��������
	g_MapManager = new MapManager;
	g_ItemManager = new ItemManager;
	g_BattleManager = new BattleManager;
	g_TaskManager = new TaskManager;
	g_TimerManage = new TimerManage;
	g_GangManager = new GangManager;
	g_CopyManager = new CopyManager;   
    g_SocailManager = new SocailManager();
	g_MapManager->Init();
	g_ItemManager->Init();
	g_BattleManager->Init();
	g_TaskManager->Init();
	g_GangManager->Init();
    g_SocailManager->Init();

//������ѭ��������Ϣ
	m_StopFlag = false;
	Run();

	return true;
}

void GameServer::Run()
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
						MsgGameServer_Register msg(m_ServerID, (char*)m_ServerName.c_str());
						SendToGate(&msg);
						gLog.Info("OK~~~Link GateServer\n");		
					}
					else if (packet->systemAddress == m_LoginAddress)
					{
						gLog.Info("OK~~~Link LoginServer\n");		
					}
					else if (packet->systemAddress == m_DBAddress)
					{						
						gLog.Info("OK~~~Link DataServer\n");
                        //>�����ݿ�����ȡȫ��guid.
                        MsgGetGlobalGuid msg;
                        SendToDB(&msg);

						//���������Ҫ������Ϣ
						MsgGang msgGang(0,0,0,"");
						msgGang.SysID =	ID_G2D; 
						msgGang.MsgID = Msg_Get_GangInfo; 
						SendToDB(&msgGang);

					}
				}
				break;
			case ID_NEW_INCOMING_CONNECTION:	//�յ��µ�����
				break;						
			case ID_ALREADY_CONNECTED:	//�Է�˵�����ظ�������
			case ID_CONNECTION_BANNED: // ����ֹ����
			case ID_CONNECTION_ATTEMPT_FAILED:	//���������޷����
			case ID_NO_FREE_INCOMING_CONNECTIONS:	//�Է������������������µ�����			
				{							
					if (packet->systemAddress == m_GateAddress)
					{					
						m_Net->Connect(m_GateServerIP.c_str(), m_GateServerPort, GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));	//�������ط�����
					}
					else if (packet->systemAddress == m_LoginAddress)
					{					
						m_Net->Connect(m_LoginServerIP.c_str(), m_LoginServerPort, SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));	//������½������
					}
					else if (packet->systemAddress == m_DBAddress)
					{					
						m_Net->Connect(m_DBServerIP.c_str(), m_DBServerPort, SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));	//�������ݿ������
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
				else if (packet->systemAddress == m_LoginAddress)
				{
					//���ӵ���½
					m_Net->Connect(m_LoginServerIP.c_str(), m_LoginServerPort, SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));
				}
				else if (packet->systemAddress == m_DBAddress)
				{
					//���ӵ����ݿ�
					m_Net->Connect(m_DBServerIP.c_str(), m_DBServerPort, SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));
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
			case ID_L2G:	
				OnRecvLogin(packet);	
				break;
			case ID_C2G:	
				OnRecvClient(packet);	
				break;
			case ID_D2G:	
				OnRecvDB(packet);	
				break;
			case ID_D2C:				
				{					
					MsgBase* msg = (MsgBase*)(packet->data);					
					SendToClient(msg);					
				}
				break;
			case ID_GATE2GAME:
				OnRecvGate(packet);
				break;
			default:
				break;
			}
		}

        static uint64 lastTime = GetAbsoluteMsecTime();
        static uint64 lastUpdateTime = lastTime;
        static const int TICK_TIME				= 30;					// ÿ��Tick�Ĵ���
        static const int TICK_PER_SECOND		= (1000/TICK_TIME);		// ÿ��Tick�Ļ��ѵ�ʱ��	

        uint64 time = GetAbsoluteMsecTime();
        uint64 elapsedTime = time - lastUpdateTime;
        lastUpdateTime = time;

        g_TimerManage->Run();
        g_MapManager->Update(elapsedTime * 0.001f);
        
        lastTime = GetAbsoluteMsecTime();
        uint32 ilastTime = uint32(lastTime - time);
        if (ilastTime < TICK_PER_SECOND)
        {
#ifdef _WIN32
            Sleep(TICK_PER_SECOND - ilastTime);
#else
            usleep(TICK_PER_SECOND - ilastTime);
#endif 
        }	
    }
}

bool GameServer::Stop()
{
	m_Net->Shutdown(300);
	RakNet::RakPeerInterface::DestroyInstance(m_Net);
	m_StopFlag = true;
    SAFE_DELETE(g_CSVFileManager);
	SAFE_DELETE(g_MapManager);
	SAFE_DELETE(g_ItemManager);
	SAFE_DELETE(g_BattleManager);
	SAFE_DELETE(g_TaskManager);
	SAFE_DELETE(g_TimerManage);

    SAFE_DELETE(g_SocailManager);
	SAFE_DELETE(g_CopyManager);	
	return true;
}

void GameServer::OnRecvGate(RakNet::Packet* p)
{
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg->MsgID >= Msg_MAX)
	{
		gLog.Warning("Error Packet id = %d", msg->MsgID);
		return;
	}	
	switch (msg->MsgID)
	{
	case Msg_Player_Remove_GameAndDBServer:
		{
			MsgBase* msg = (MsgBase*)(p->data);
			Player* player = g_MapManager->GetPlayerByID(msg->OBJID);
            if (player)
            {
                g_MapManager->DelPlayer(player->m_GUID);
                msg->SysID = ID_G2D;
                SendToDB(msg);
            }
            gLog.Info("Remove Player:[%d] From GameServer!",msg->OBJID);
		}
		break;
	default:
		printf("OnRecvLogin Error Packet id = %d", msg->MsgID);
		break;
	}

}

void GameServer::OnRecvLogin(RakNet::Packet* p)
{	
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg->MsgID >= Msg_MAX)
	{
		gLog.Warning("Error Packet id = %d", msg->MsgID);
		return;
	}	

	switch (msg->MsgID)
	{
	case Msg_Notify:
		{		
			MsgNotify* temp =  (MsgNotify*)(p->data);
			printf("%d\n", temp->Type);
			switch (temp->Type)
			{
			case 0:	
				{						
				}
				break;
			default:
				break;
			}
		}
		break;	
	case Msg_Player_Enter_GameServer:
		OnPlayerEnterGameServer(msg);
		break;
	case Msg_Player_Leave_GameServer:
		OnPlayerLeaveGameServer(msg);
		break;
	default:
		printf("OnRecvLogin eorr Packet id = %d", msg->MsgID);
		break;
	}
}

void GameServer::OnRecvClient(RakNet::Packet* p)
{	
	//!���е���Ϣ���������жϺ���ʹ�ã����Ȳ��ԵĶ����ԣ�ע��䳤��Ϣ
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg->MsgID >= Msg_MAX)
	{
		printf("eorr Packet id = %d", msg->MsgID);
		return;
	}

	switch (msg->MsgID)
	{
	case Msg_Notify:
		{		
			MsgNotify* temp =  (MsgNotify*)(p->data);
			printf("%d\n", temp->Type);
			switch (temp->Type)
			{
			case 0:	
				{						
				}
				break;
			default:
				break;
			}
		}
		break;	
	case Msg_CreateRole:
		OnCreateRoleToDB(msg);
		break;
	case Msg_Add_Friends:
		g_SocailManager->AddFriends(msg);
		break;
	case Msg_Del_Friends:
		g_SocailManager->DelFriends(msg);
		break;
	case Msg_Get_FriendsList:
		g_SocailManager->GetFriendsList(msg);
		break;
	case Msg_Get_PlayInfo:
		g_SocailManager->GetFriendsInof(msg);
		break;
	case Msg_Chat_POINT:
		g_SocailManager->PointMessage(msg);
		break;
	case Msg_Chat_CHANNEL:
		g_SocailManager->ChannelMessage(msg);
		break;
	case Msg_Create_Gang:
		g_SocailManager->CreateGang(msg);
		break;
	case Msg_Get_GangInfo:
		g_SocailManager->GetGangInfo(msg);
		break;
	case Msg_Apply_Gang:
		g_SocailManager->ApplyAddGang(msg);
		break;
	case Msg_Reply_ApplyGang:
		g_SocailManager->ReplyAddGang(msg);
		break;
	case Msg_Get_GangApplyList:
		g_SocailManager->GetGangApplyList(msg);
		break;
	case Msg_Leave_Gang:
		g_SocailManager->LeaveGang(msg);
		break;
	case Msg_Put_Gang:
		g_SocailManager->PutGang(msg);
		break;
	case Msg_Transfer_GangHeader:
		g_SocailManager->GangTransfer(msg);
		break;
	case Msg_Get_GangList:
		g_SocailManager->GetGangList(msg);
		break;
	case Msg_Get_GangAnnounce:
		g_SocailManager->GangAnnouncement(msg);
		break;
	case Msg_Edit_GangAnnounce:
		g_SocailManager->EditAnnouncement(msg);
		break;
	case Msg_Gang_Appoint:
		g_SocailManager->GangAppoint(msg);
		break;
	case Msg_Object_Move:
		OnPlayerMove(msg);
		break;
    case Msg_Object_Move_Stop:
        OnPlayerMoveStop(msg);
        break;
	case Msg_Player_UseSkill:
		{
			OnPlayerUseSkill(msg);
		}
		break;
	case Msg_Player_Relive:
		{
			OnPlayerRelive(msg);
		}
		break;
	case Msg_Task_Collect:
		{
			OnPlayerDoTask(msg);
			
		}
		break;
    case Msg_Item_BagList:
        OnPlayerBagList(msg);
        break;
    case Msg_Equip_List:
        OnPlayerEquipList(msg);
        break;
    case Msg_Skill_List:
        OnPlayerSkillList(msg);
        break;
    case Msg_Item_ShopBuy:
        OnPlayerShopBuy(msg);
        break;
    case Msg_Item_ShopSell:
        OnPlayerShopSell(msg);
        break;
    case Msg_Item_Compose:
        OnPlayerItemCompose(msg);
        break;
    case Msg_Item_UpStar:
        OnPlayerEquipmentUpStar(msg);
        break;
    case Msg_Item_Inherit:
        OnPlayerEquipmentInherit(msg);
        break;
    case Msg_Item_Use:
        OnPlayerItemUse(msg);
        break;
    case Msg_Skill_Activate:
        OnPlayerActivateSkill(msg);
        break;
    case Msg_Skill_ChoseSkill:
        OnPlayerUIChoseSkill(msg);
        break;
	case Msg_Task_Take:
		OnPlayerTakeTask(msg);
		break;
	case Msg_Task_Make:
		OnPlayerDoTask(msg);
		break;
	case Msg_Task_Submit:
		g_TaskManager->OnGCAskSubmitTask(msg);
		break;
	case Msg_Task_GetList:
		g_TaskManager->OnGetTaskList(msg);
		break;
    case Msg_Role_EnterMap:
        OnRoleRequestEnterMap(msg);
        break;
	case Msg_Chest:
		OnPlayerChest(msg);
		break;
    case Msg_Team_Apply:
        OnTeamApply(msg);
        break;
    case Msg_Team_ReplyApply:
        OnTeamReplyApply(msg);
        break;
    case Msg_Team_KickMemberReq:
        OnTeamKickMember(msg);
        break;
    case Msg_Team_ChangeLeader:
        OnTeamChangeLeader(msg);
        break;
    case Msg_Team_MemberExit:
        OnTeamMemberExit(msg);
        break;
    case Msg_OtherRole_DetailInfoQuery:
        OnPlayerDetailInfoQuery(msg);
        break;
	case Msg_Apply_Copy:
		g_CopyManager->OnApplyCopy(msg);
		break;
	default:
		printf("OnRecvClient eorr Packet id = %d", msg->MsgID);
		break;
	}
}

void GameServer::OnRecvDB(RakNet::Packet* p)
{	
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg->MsgID >= Msg_MAX)
	{
		printf("OnRecvDB eorr Packet id = %d", msg->MsgID);
		return;
	}

	switch (msg->MsgID)
	{
	case Msg_Notify:
		{		
			MsgNotify* temp =  (MsgNotify*)(p->data);
			printf("%d\n", temp->Type);
			switch (temp->Type)
			{
			case Notify_S2C_CreateRole:	
				{
					MsgNotify ttt(ID_G2C, Notify_S2C_CreateRole);
					ttt.OBJID = msg->OBJID;
					SendToClient(&ttt);
				}
				break;
            case Notify_S2C_RoleExist:
                {
                    MsgNotify notify(ID_G2C, Notify_S2C_RoleExist); //>֪ͨ�ͻ������н�ɫ,���Խ����ͼ��
                    notify.OBJID = msg->OBJID;
                    SendToClient(&notify);
                }break;
			default:
				break;
			}
		}
		break;
    case Msg_GetGlobalGuid:
        OnGetGlobalGuid(msg);
        break;
	case Msg_CreateRole:
		{
			//�����ݿ������������ɫ�ɹ�

			//֪ͨ�ͻ��˴����ɹ�
			MsgNotify ttt(ID_G2C, Notify_S2C_CreateRole_OK);
			ttt.OBJID = msg->OBJID;
			SendToClient(&ttt);
		}
		break;
	case Msg_Role_BaseInfo:
		{
			MsgRole_BaseInfo* temp =  (MsgRole_BaseInfo*)(p->data);
			//�յ���ɫ������Ϣ
			g_MapManager->CreatePlayer(temp);
		}
		break;
	case Msg_Role_DetailInfo:
		OnRoleDetailInfo(msg);
		break;	
	case Msg_Get_GangInfo:
	    OnGangInfo(msg);
		break;
	case Msg_Get_ApplyInfo:
		OnGangApplyInfo(msg);
		break;
	default:
		printf("OnRecvDB error Packet id = %d", msg->MsgID);
		break;
	}
}

bool GameServer::Send(RakNet::SystemAddress &address, MsgBase *msg)
{
	if (msg->Len < sizeof(MsgBase))
	{
		printf("Error~~~Send Packet id = %d  %d", msg->SysID, msg->MsgID);
		return false;
	}
	//!test
	gLog.MsgIO(true,msg->SysID,msg->MsgID,msg->Len);

	int i = m_Net->Send((const char*)msg, msg->Len, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, address, false);
	return i > 0;
}

bool GameServer::SendToGate(MsgBase *msg)
{
	msg->OBJID = m_ServerID;
	msg->SysID = ID_GAME2GATE;
	Send(m_GateAddress, msg);
	return true;
}

bool GameServer::SendToLogin(MsgBase *msg)
{
	msg->OBJID = m_ServerID;
	msg->SysID = ID_G2L;
	Send(m_LoginAddress, msg);
	return true;
}

bool GameServer::SendToDB(MsgBase *msg)
{
	msg->SysID = ID_G2D;
	Send(m_DBAddress, msg);
	return true;
}

bool GameServer::SendToClient(MsgBase *msg)
{
	msg->SysID = ID_G2C;
	Send(m_GateAddress, msg);
	return true;
}

void GameServer::OnGetGlobalGuid(MsgBase* msg)
{
    MsgGetGlobalGuid* guidMsg = (MsgGetGlobalGuid*)msg;
    g_ItemManager->SetItemGuid(guidMsg->ItemGuid);
	gLog.AddLog(LOG_LEVEL_NITIFY, "SetItemGuid[%llu]", guidMsg->ItemGuid);
}

void GameServer::OnPlayerEnterGameServer(MsgBase* msg)
{	
	MsgPlayer_Enter_GameServer* temp = (MsgPlayer_Enter_GameServer*)msg;
	//֪ͨ������ҽ�����Ϸ
	temp->SysID = ID_GAME2GATE;
	SendToGate(temp);
	//ȥ���ݿ��ѯ�������
	temp->SysID = ID_G2D;
	SendToDB(temp);	
}

void GameServer::OnPlayerLeaveGameServer(MsgBase* msg)
{
	MsgPlayer_Leave_GameServer* temp = (MsgPlayer_Leave_GameServer*)msg;
	g_MapManager->DelPlayer(temp->AccountID);
}

void GameServer::OnCreateRoleToDB(MsgBase* msg)
{
	MsgCreateRole* temp = (MsgCreateRole*)msg;

	//ת�����ݿ������
	temp->SysID = ID_G2D;
	SendToDB(msg);
}

void GameServer::OnRoleDetailInfo(MsgBase* msg)
{
	MsgRole_Detail_Info* temp=(MsgRole_Detail_Info*)msg;
	Player* player = g_MapManager->GetPlayerByID(temp->OBJID);

	StackStream<40960> RoleDetailsStream(temp, temp->Len);
	MsgRole_Detail_Info msgHead;
	RoleDetailsStream.Read(&msgHead, sizeof(msgHead));
	for(int i=0;i<msgHead.SkillTreeNum;i++)
	{
		SvrSkillInfo skillInfo;
		RoleDetailsStream.Read(&skillInfo.data.skillType,sizeof(int));
		RoleDetailsStream.Read(&skillInfo.data.level,sizeof(int));
        const SkillInfo* pInfo = g_CSVFileManager->GetSkillByID(SKILL_TYPE_AND_LEVEL_TO_ID(skillInfo.data.skillType, skillInfo.data.level + 1));
        assert(pInfo);
        if (pInfo)
            skillInfo.pCfg = pInfo;
        
        player->m_SkillList.push_back(skillInfo);
	}
	for(int i=0;i<msgHead.WingNum;i++)
	{
		WingInfoIt winInfo;
		RoleDetailsStream.Read(&winInfo.id,sizeof(int));
		RoleDetailsStream.Read(&winInfo.level,sizeof(int));
        RoleDetailsStream.Read(&winInfo.exp,sizeof(int));
		player->m_WingList[i]=winInfo;
	}
	for(int i=0;i<msgHead.MagicWeaponNum;i++)
	{
		MagicWeaponInfoIt magicInfo;
		RoleDetailsStream.Read(&magicInfo.id,sizeof(int));
		RoleDetailsStream.Read(&magicInfo.level,sizeof(int));
        RoleDetailsStream.Read(&magicInfo.exp,sizeof(int));
		RoleDetailsStream.Read(&magicInfo.growthPlus,sizeof(int));
		player->m_MagicWeaponList.push_back(magicInfo);
	}
	for(int i=0;i<msgHead.MountNum;i++)
	{
		MountInfoIt mountInfo;
		RoleDetailsStream.Read(&mountInfo.id,sizeof(int));
		RoleDetailsStream.Read(&mountInfo.time,sizeof(int));
		player->m_MountList.push_back(mountInfo);
	}
	for(int i=0;i<msgHead.CopyNum;i++)
	{
		CopyInfoIt copyInfo;
		RoleDetailsStream.Read(&copyInfo.id,sizeof(int));
		RoleDetailsStream.Read(&copyInfo.enterNum,sizeof(int));
		RoleDetailsStream.Read(&copyInfo.layer,sizeof(int));
		player->m_CopyList.push_back(copyInfo);
	}
	for(int i=0;i<msgHead.DailyNum;i++)
	{
		DailyInfoIt dailyInfo;
		RoleDetailsStream.Read(&dailyInfo.id,sizeof(int));
		RoleDetailsStream.Read(&dailyInfo.progress,sizeof(int));
		RoleDetailsStream.Read(&dailyInfo.isReward,sizeof(bool));
		player->m_DailyList.push_back(dailyInfo);
	}
	for(int i=0;i<msgHead.TaskNum;i++)
	{
		TaskInfoIt taskInfo;
		RoleDetailsStream.Read(&taskInfo.id,sizeof(int));
		RoleDetailsStream.Read(&taskInfo.progress,sizeof(int));
		RoleDetailsStream.Read(&taskInfo.state,sizeof(int));
		player->m_TaskList.push_back(taskInfo);
	}
	vector<ItemInfoIt> equipList;
	for(int i=0;i<msgHead.EquipNum;i++)
	{
		ItemInfoIt tequipInfo;
		RoleDetailsStream.Read(&tequipInfo.guid, sizeof(uint64));
		RoleDetailsStream.Read(&tequipInfo.id, sizeof(int));
		RoleDetailsStream.Read(&tequipInfo.prefixID, sizeof(int));
		RoleDetailsStream.Read(&tequipInfo.starNum, sizeof(int));
		RoleDetailsStream.Read(&tequipInfo.isBind, sizeof(uint8));
		RoleDetailsStream.Read(&tequipInfo.num, sizeof(int));
		RoleDetailsStream.Read(&tequipInfo.isLock, sizeof(uint8));
        RoleDetailsStream.Read(&tequipInfo.time, sizeof(int));
		equipList.push_back(tequipInfo);
	}
	g_ItemManager->InitEquipItems(player,equipList);
	vector<ItemInfoIt> bagList;
	for(int i=0;i<msgHead.BagNum;i++)
	{
		ItemInfoIt bagInfo;
		RoleDetailsStream.Read(&bagInfo.guid, sizeof(uint64));
		RoleDetailsStream.Read(&bagInfo.id, sizeof(int));
		RoleDetailsStream.Read(&bagInfo.prefixID, sizeof(int));
		RoleDetailsStream.Read(&bagInfo.starNum, sizeof(int));
		RoleDetailsStream.Read(&bagInfo.isBind, sizeof(uint8));
		RoleDetailsStream.Read(&bagInfo.num, sizeof(int));
		RoleDetailsStream.Read(&bagInfo.isLock, sizeof(uint8));
        RoleDetailsStream.Read(&bagInfo.time, sizeof(int));
		bagList.push_back(bagInfo);
	}
	g_ItemManager->InitBagItems(player,bagList);

	//����
	for(int i=0;i<msgHead.FriendsListNum;i++)
	{
		FriendsInfo tempFriend;
		RoleDetailsStream.Read(&tempFriend.roleId,sizeof(uint64));
		RoleDetailsStream.Read(&tempFriend.szName,MAX_NAME_SIZE);
		RoleDetailsStream.Read(&tempFriend.bIsLine,sizeof(bool));
		RoleDetailsStream.Read(&tempFriend.iLevel,sizeof(int));
		RoleDetailsStream.Read(&tempFriend.iFight,sizeof(int));
		player->m_vecFriendsList.push_back(tempFriend);
	}
	//������
	for(int i=0;i<msgHead.BlackListNum;i++)
	{
		FriendsInfo tempFriend;
		RoleDetailsStream.Read(&tempFriend.roleId,sizeof(uint64));
		RoleDetailsStream.Read(&tempFriend.szName,MAX_NAME_SIZE);
		RoleDetailsStream.Read(&tempFriend.bIsLine,sizeof(bool));
		RoleDetailsStream.Read(&tempFriend.iLevel,sizeof(int));
		RoleDetailsStream.Read(&tempFriend.iFight,sizeof(int));
		player->m_vecBlackList.push_back(tempFriend);
	}
	//����Ѿ�ѡ��ļ���
	for(int i=0;i<msgHead.ChoseSkill;i++)
	{
		RoleDetailsStream.Read(&player->m_ChoseSkill[i],sizeof(int));
	}

	//player->LoadPlayerOtherAttribute();
}

void GameServer::OnGangInfo(MsgBase* msg)
{
	
	MsgGang* temp=(MsgGang*)msg;
	StackStream<2048> StreamTemp(temp, temp->Len);
	MsgGang msgHead(0,0,0,"");
	StreamTemp.Read(&msgHead, sizeof(msgHead));

	Gang tempGang;
	StreamTemp.Read(&tempGang.m_id, sizeof(__int64));
	StreamTemp.Read(&tempGang.m_szName,32);
	StreamTemp.Read(&tempGang.m_iLevel,sizeof(__int32));
	StreamTemp.Read(&tempGang.m_iShopLevel,sizeof(__int32));
	StreamTemp.Read(&tempGang.m_iHeaderid,sizeof(__int64));
	StreamTemp.Read(&tempGang.m_szHeaderName,32);
	StreamTemp.Read(&tempGang.m_iAct,sizeof(__int32));
	StreamTemp.Read(&tempGang.m_szAccounce,255);
	StreamTemp.Read(&tempGang.m_iViveHeaderid[0],sizeof(__int64));
	StreamTemp.Read(&tempGang.m_iViveHeaderid[1],sizeof(__int64));
	StreamTemp.Read(&tempGang.m_iStarFlec[0],sizeof(__int64));
	StreamTemp.Read(&tempGang.m_iStarFlec[1],sizeof(__int64));
	StreamTemp.Read(&tempGang.m_iStarFlec[2],sizeof(__int64));
	StreamTemp.Read(&tempGang.m_iStarFlec[3],sizeof(__int64));
	StreamTemp.Read(&tempGang.m_iStarFlec[4],sizeof(__int64));
	StreamTemp.Read(&tempGang.m_szCreateTime,50);
	StreamTemp.Read(&tempGang.m_iCopyid,sizeof(__int32));
	StreamTemp.Read(&tempGang.m_szStartTime,50);
	cout<<"%d"<<tempGang.m_id<<tempGang.m_iHeaderid<<endl;
	for (int i = 0 ; i < temp->iPar[1]; ++ i)
	{
		GangMember tempgangMember;
		StreamTemp.Read(&tempgangMember.id,sizeof(int64));
		StreamTemp.Read(&tempgangMember.szName,32);
		StreamTemp.Read(&tempgangMember.iLevel,sizeof(int32));
		cout<<tempgangMember.id<<"  "<<tempgangMember.szName<<"  "<<tempgangMember.iLevel<<endl;
		tempGang.m_mapMembers.insert(pair<__int64,GangMember>(tempgangMember.id,tempgangMember));
	}
	StreamTemp.Read(&tempGang.m_szBackupInfo,255);
	//��Ӱ���
	g_GangManager->m_mapGang.insert(pair<int64,Gang>(tempGang.m_id,tempGang));
}

void GameServer::OnGangApplyInfo(MsgBase* msg)
{
	MsgGang* temp=(MsgGang*)msg;
	StackStream<1024> StreamTemp(temp, temp->Len);
	MsgGang msgHead(0,0,0,"");
	StreamTemp.Read(&msgHead, sizeof(msgHead));

	int32 id = 0;
	int32 iApplyType = 0;
	int32 iTargetid = 0;
	char	szBackup[255];
	GangMember tempMember;
	StreamTemp.Read(&id, sizeof(__int32));
	StreamTemp.Read(&tempMember.id,sizeof(__int64));
	StreamTemp.Read(&iApplyType,sizeof(__int32));
	StreamTemp.Read(&iTargetid,sizeof(__int64));
	StreamTemp.Read(&tempMember.szApplyTime,50);
	StreamTemp.Read(&szBackup,255);

	Gang *tempGang = g_GangManager->GetGangByID(iTargetid);
	if (tempGang)
	{
		tempGang->m_mapMembers.insert(pair<int64,GangMember>(tempMember.id,tempMember));
	}
}

void GameServer::OnRoleRequestEnterMap(MsgBase* msg)
{
    MsgRole_EnterMap* enterMsg = (MsgRole_EnterMap*)msg;
    Player* player = g_MapManager->GetPlayerByID(enterMsg->OBJID);
    if (player)
    {
        if (player->m_Map == NULL)
        {
            const int32 defaultMapId = 1001; //>ʹ��Ĭ�ϵ�mapId
            Map* pMap = g_MapManager->GetMapByID(defaultMapId);
            assert(pMap);
            if (pMap)
            {
                g_MapManager->PlayerEnterMap(player, defaultMapId, pMap->GetEnterMapPosition(Mode_Init));
            }
        }
        else
        {
            Map* pMap = g_MapManager->GetMapByID(enterMsg->mapId);
            assert(pMap);
            if (pMap)
            {
                const ConveyInfo* pConvey = g_CSVFileManager->GetConveyByID(enterMsg->conveyId);
                if (pConvey)
                {
                    //>У���Ƿ��ڴ��͵�
                    //if (MyAbs(pConvey->pConvey.x - player->GetPosition().x) > 2 || MyAbs(pConvey->pConvey.y - player->GetPosition().y) > 2);
                    //{
                    //    gLog.Warning("distance not enough nearly.");
                    //    return;
                    //}
                    player->m_MoveComp->StopMove();
                    g_MapManager->PlayerChangeMap(player, enterMsg->mapId, pMap->GetEnterMapPosition(Mode_Convey, enterMsg->conveyId));
                }
                else
                {
                    gLog.Warning("unknown conveyId[%d]", enterMsg->conveyId);
                }
            }
        }
    }
}

void GameServer::OnPlayerMove(MsgBase* msg)
{
    //!��֤�ƶ��Ƿ�Ϸ�
	MsgObject_Move moveMsg;
    StackStream<> moveStream(msg, msg->Len);
    moveStream.Read(&moveMsg, sizeof(moveMsg));
	Player* player = g_MapManager->GetPlayerByID(moveMsg.OBJID);
	if (player)
	{
        const Vec2& server = player->GetPosition();
        Vec2 clinet(moveMsg.MoverPosX, moveMsg.MoverPosY);
        if (server.distance(clinet) > 128.0f) //>����128����������
        {
            gLog.AddLog(LOG_LEVEL_WARNING, "position different when process OnPlayerMove clentPos=(%f,%f), serverPos(%f,%f).", clinet.x, clinet.y, server.x, server.y);
            MsgObject_Move_Kick kickMsg;
            kickMsg.OBJID = moveMsg.OBJID;
            kickMsg.MoverID = moveMsg.OBJID;
            kickMsg.MoverType = OBJECT_PLAYER;
            kickMsg.MoverPosX = player->GetPosition().x;
            kickMsg.MoverPosY = player->GetPosition().y;
            SendToClient(&kickMsg);
            player->m_MoveComp->StopMove();
        }
        else
        {
            int posId;
            std::vector<int> paths;
            for (int i = 0; i < moveMsg.PathNum; i++)
            {
                moveStream.Read(&posId, sizeof(int));
                paths.push_back(posId);
            }
            gLog.Info("TargetPos(%f,%f)", moveMsg.TargetPosX, moveMsg.TargetPosY);
            Vec2 moveTarget(moveMsg.TargetPosX, moveMsg.TargetPosY);
            if (paths.empty())
            {
                player->m_MoveComp->MoveByTarget(moveTarget);
            }else
            {
                player->m_MoveComp->MoveByPath(paths, moveTarget);
            }
        }
	}
}

void GameServer::OnPlayerMoveStop(MsgBase* msg)
{
    MsgObject_Move_Stop* stopMsg = (MsgObject_Move_Stop*)msg;
    Player* player = g_MapManager->GetPlayerByID(msg->OBJID);
    if (player)
    {
        stopMsg->SysID = ID_G2C;
        stopMsg->MoverID = msg->OBJID;
        stopMsg->MoverType = OBJECT_PLAYER;
        Vec2 clientPosition(stopMsg->MoverPosX, stopMsg->MoverPosY);
        Vec2 ServetPosition = player->GetPosition();
        if (ServetPosition.distance(clientPosition)> 128.0f)
        {
            stopMsg->MoverPosX = ServetPosition.x;
            stopMsg->MoverPosY = ServetPosition.y;
        }
        player->m_MoveComp->StopMove();
        player->BroadcastMsg(stopMsg);
    }
}

void GameServer::OnPlayerUseSkill(MsgBase* msg)
{
	MsgC2GPlayer_UseSkill *pUseSkill = (MsgC2GPlayer_UseSkill*)msg;
	//g_BattleManager->PlayerUseSkill(temp);

    MsgC2GPlayer_Relive *temp = (MsgC2GPlayer_Relive*)msg;
    Player* player = g_MapManager->GetPlayerByID(msg->OBJID);
    if (player)
    {
        if (pUseSkill->ObjType == OBJECT_PLAYER)
        {
            Player* other = g_MapManager->GetPlayerByID(pUseSkill->target);
            if (other)
            {
                player->m_BattleComp->UseSkill(pUseSkill->SkillID, other);
            }
        }
    }

}

void GameServer::OnPlayerRelive(MsgBase* msg)
{
	MsgC2GPlayer_Relive *temp = (MsgC2GPlayer_Relive*)msg;
	Player* player = g_MapManager->GetPlayerByID(msg->OBJID);
	if (player)
	{
		player->Relive(temp->relive_type);
	}
}

void GameServer::OnPlayerBagList(MsgBase* msg)


{
    MsgItem_BagList* pListMsg = (MsgItem_BagList*)msg;
    Player* player = g_MapManager->GetPlayerByID(pListMsg->OBJID);
    if (player)
    {
        g_ItemManager->SendBagItemsToClient(player, ItemOperate_BagLoading);
    }
}

void GameServer::OnPlayerEquipList(MsgBase* msg)
{
    MsgEquip_List* pListMsg = (MsgEquip_List*)msg;
    Player* player = g_MapManager->GetPlayerByID(pListMsg->OBJID);
    if (player)
    {
        g_ItemManager->SendEquipItemsToClient(player, ItemOperate_EquipLoading);
    }
}

void GameServer::OnPlayerSkillList(MsgBase* msg)
{
    MsgSkill_List* pListMsg = (MsgSkill_List*)msg;
    Player* player = g_MapManager->GetPlayerByID(pListMsg->OBJID);
    if (player)
    {
        player->SendSkillList();
    }
}

void GameServer::OnPlayerActivateSkill(MsgBase* msg)
{
    MsgSkill_Activate* pActivateMsg = (MsgSkill_Activate*)msg;
    Player* player = g_MapManager->GetPlayerByID(pActivateMsg->OBJID);
    if (player)
        player->ActivateSkill(pActivateMsg->ActivateSkillType);
}

void GameServer::OnPlayerUIChoseSkill(MsgBase* msg)
{
    MsgSkill_ChoseSkill* pChoseMsg = (MsgSkill_ChoseSkill*)msg;
    Player* player = g_MapManager->GetPlayerByID(pChoseMsg->OBJID);
    if (player)
        player->SetChoseSkill(pChoseMsg->setIndex, pChoseMsg->skillId);
}

void GameServer::OnPlayerShopBuy(MsgBase* msg)
{
    MsgItem_ShopBuy* pBuyMsg = (MsgItem_ShopBuy*)msg;
    Player* player = g_MapManager->GetPlayerByID(pBuyMsg->OBJID);
    if (player)
    {
        g_ItemManager->BuyShopItem(player, pBuyMsg->shopId, pBuyMsg->goodesId);
    }
}

void GameServer::OnPlayerShopSell(MsgBase* msg)
{
    MsgItem_ShopSell* pSellMsg = (MsgItem_ShopSell*)msg;
    Player* player = g_MapManager->GetPlayerByID(pSellMsg->OBJID);
    if (player)
    {
        g_ItemManager->SellItem(player, pSellMsg->itemGuid, pSellMsg->number);
    }
}

void GameServer::OnPlayerItemCompose(MsgBase* msg)
{
    MsgItem_Compose* pComposeMsg = (MsgItem_Compose*)msg;
    Player* player = g_MapManager->GetPlayerByID(pComposeMsg->OBJID);
    if (player)
    {
        g_ItemManager->ComposeItem(player, pComposeMsg->mainStuffGuid, 
                                   pComposeMsg->Stuff1Guid,
                                   pComposeMsg->Stuff2Guid,
                                   pComposeMsg->Stuff3Guid,
                                   pComposeMsg->Stuff4Guid);
    }
}

void GameServer::OnPlayerEquipmentUpStar(MsgBase* msg)
{
    MsgItem_UpStar* pUpStarMsg = (MsgItem_UpStar*)msg;
    Player* player = g_MapManager->GetPlayerByID(pUpStarMsg->OBJID);
    if (player)
    {
        g_ItemManager->UpStarEquipment(player, pUpStarMsg->equipmentGuid);
    }
}

void GameServer::OnPlayerEquipmentInherit(MsgBase* msg)
{
    MsgItem_Inherit* pInheritMsg = (MsgItem_Inherit*)msg;
    Player* player = g_MapManager->GetPlayerByID(pInheritMsg->OBJID);
    if (player)
    {
        g_ItemManager->InheritEquipment(player, pInheritMsg->targetEquipmentGuid, pInheritMsg->sourceEquipmentGuid);
    }
}

void GameServer::OnPlayerItemUse(MsgBase* msg)
{
    MsgItem_Use* pUseMsg = (MsgItem_Use*)msg;
    Player* player = g_MapManager->GetPlayerByID(pUseMsg->OBJID);
    if (player)
    {
        if(pUseMsg->operateType == ItemOperate_WearEquipment)
        {
            g_ItemManager->WearEquipment(player, ItemOperate_WearEquipment, pUseMsg->useItemGuid);
        }
        else if (pUseMsg->operateType == ItemOperate_EquipGetOff)
        {
            g_ItemManager->TakeoffEquipmentPutInBag(player, ItemOperate_EquipGetOff, pUseMsg->useItemGuid);
        }
        else if(pUseMsg->operateType == ItemOperate_ItemUsed)
        {
             g_ItemManager->ExpendItemUse(player, pUseMsg->useItemGuid, 1);
        }
    }
}

void GameServer::OnPlayerTakeTask(MsgBase* msg)
{
	MsgTask_TakeTask* pTakeTask=(MsgTask_TakeTask*)msg;
	Player* player = g_MapManager->GetPlayerByID(pTakeTask->OBJID);
	if(player)
	{
		g_TaskManager->OnGCAskActiveTask(player,msg);
	}
}
void GameServer::OnPlayerDoTask(MsgBase* msg)
{
	MsgTask_MakeTask* pMakeTask = (MsgTask_MakeTask*)msg;
	Player* player = g_MapManager->GetPlayerByID(pMakeTask->OBJID);

	if(pMakeTask->MsgID == Msg_Task_Collect)
	{
		g_TaskManager->OnDoTaskCollect(pMakeTask->taskId,player);
		return;
	}
	const TaskInfo* taskInfo = g_CSVFileManager->GetTaskByID(pMakeTask->taskId);
	if(player)
	{
		if(taskInfo->iTermsType == TASK_TERMS_USEITEM || taskInfo->iTermsType == TASK_TERMS_DELIVERYITEM)
		{
			g_TaskManager->OnDoTaskCheckItem(taskInfo,player);
		}
	}
	
}

void GameServer::OnTeamApply(MsgBase* msg)
{
    MsgTeam_Apply* applyMsg = (MsgTeam_Apply*)msg;

    Player* requester = g_MapManager->GetPlayerByID(applyMsg->iRequesterId);
    assert(requester);
    if (requester)
    {
        Player* partner = g_MapManager->GetPlayerByID(applyMsg->iPartnerId);
        if (partner == NULL || partner->m_Map == NULL)
        {
            //>Ҫ����ӵ�����Ѿ�����
            MsgTeam_ReplyApply replyMsg(applyMsg->iRequesterId, applyMsg->szRequesterName, applyMsg->iPartnerId, "unknwon", 1);
            replyMsg.OBJID = applyMsg->iRequesterId;
            SendToClient(&replyMsg);
            return;
        }

        if (requester)
        {
            Team* pRequsterTeam = g_SocailManager->GetTeam(requester->m_teamId);
            Team* pPartnerTeam = g_SocailManager->GetTeam(partner->m_teamId);
            if (pRequsterTeam && pPartnerTeam)
            {
                //>��������ж���. �Է�Ҳ�ж���. ����ӻ�ʧ��
                MsgTeam_ReplyApply replyMsg(applyMsg->iRequesterId, applyMsg->szRequesterName, applyMsg->iPartnerId, partner->m_Name, 2);
                replyMsg.OBJID = applyMsg->iRequesterId;
                SendToClient(&replyMsg);
                return;
            }

            if (pRequsterTeam == NULL && pPartnerTeam)
            {
                //>����Լ�û�ж���,�Է��ж���.���͸��ӳ� , ˵��requesterҪ����ӳ��Ķ���
                const Team::Member* pLeader = pPartnerTeam->GetTeamLeader();
                if (pLeader)
                {
                    if(pPartnerTeam->IsFull())
                    { 
                        //>������Ա.
                        MsgTeam_ReplyApply replyMsg(applyMsg->iRequesterId, applyMsg->szRequesterName, applyMsg->iPartnerId, partner->m_Name, 5);
                        replyMsg.OBJID = applyMsg->iRequesterId;
                        SendToClient(&replyMsg);
                        gLog.Warning("team[teamId=%d] is full when call OnTeamApply.", pPartnerTeam->GetId());
                        return;
                    }
                    
                    applyMsg->SysID = ID_G2C;
                    applyMsg->OBJID = pLeader->playerId;
                    SendToClient(applyMsg);
                }
                else
                {
                    gLog.Warning("can't find team[teamId=%d] leader when call OnTeamApply.", pPartnerTeam->GetId());
                }
                return;
            }

            if (pPartnerTeam == NULL)
            {
                if (pRequsterTeam)
                {
                    if (pRequsterTeam->GetTeamLeader()->playerId != applyMsg->iRequesterId)
                    {
                        //>ֻ�жӳ����������Ա.
                        gLog.Warning("only team leader can invite member OnTeamApply.", pRequsterTeam->GetId());
                        return;
                    }

                    if(pRequsterTeam->IsFull())
                    {
                        //>������Ա.
                        gLog.Warning("team[teamId=%d] is full when call OnTeamApply.", pRequsterTeam->GetId());
                        return;
                    }
                }

                //>����Լ�û�ж�������ж���.�����͸�partner ,˵��parter�Ƿ�Ը�����requester�Ķ���
                applyMsg->SysID = ID_G2C;
                applyMsg->OBJID = partner->m_GUID;
                SendToClient(applyMsg);
                return;
            }
        }
    }
}

void GameServer::OnTeamReplyApply(MsgBase* msg)
{
    MsgTeam_ReplyApply* replyApplyMsg = (MsgTeam_ReplyApply*)msg;
    Player* requester = g_MapManager->GetPlayerByID(replyApplyMsg->iRequesterId);
    Player* partner = g_MapManager->GetPlayerByID(replyApplyMsg->OBJID);
    if (requester && partner)
    {
        Team* pPartnerTeam = g_SocailManager->GetTeam(partner->m_teamId);
        Team* pRequesterTeam = g_SocailManager->GetTeam(requester->m_teamId);
        if (pPartnerTeam)
        {
            if (pRequesterTeam == NULL)
            {
                if (!pPartnerTeam->IsFull())
                {
                    pPartnerTeam->OnPlayerJoin(requester);
                }
                else
                {
                    //>��������
                }
            }
            else
            {
                //>�������Ѿ�������������.
            }
        }
        else
        {
            if (pRequesterTeam == NULL)
            {
                Team* pRequesterTeam = g_SocailManager->AllocTeam();
                pRequesterTeam->Init(requester);
                pRequesterTeam->OnPlayerJoin(partner);
            }
            else
            {
                if (pRequesterTeam->GetTeamLeader()->playerId == replyApplyMsg->iRequesterId)
                {
                    if (!pRequesterTeam->IsFull())
                    {
                        pRequesterTeam->OnPlayerJoin(partner);
                    }
                    else
                    {
                       //>��������
                    }
                }
                else
                {
                    //>�������Ѿ�������������.�λ�ӦʧЧ.
                }
            }
        }
    }
}

void GameServer::OnTeamChangeLeader(MsgBase* msg)
{
    MsgTeam_ChangeLeader* changeMsg = (MsgTeam_ChangeLeader*)msg;
    Player* player = g_MapManager->GetPlayerByID(changeMsg->OBJID);
    if (player)
    {
        Team* pTeam = g_SocailManager->GetTeam(player->m_teamId);
        if (pTeam && pTeam->GetTeamLeader()->playerId == changeMsg->OBJID)
        {
            Player* newLeader = g_MapManager->GetPlayerByID(changeMsg->memberId);
            if (newLeader)
            {
                pTeam->OnChangeLeader(newLeader);
            }   
        }
    }
}

void GameServer::OnTeamKickMember(MsgBase* msg)
{
    MsgTeam_KickMemberReq* kickReqMsg = (MsgTeam_KickMemberReq*)msg;
    Player* player = g_MapManager->GetPlayerByID(kickReqMsg->OBJID);
    if (player)
    {
        Team* pTeam = g_SocailManager->GetTeam(player->m_teamId);
        if (pTeam && pTeam->GetTeamLeader()->playerId == kickReqMsg->OBJID)
        {
            if (kickReqMsg->badGuyId == kickReqMsg->OBJID)
            {
                pTeam->OnDismiss();
            }
            else
            {
                pTeam->OnPlayerExit(kickReqMsg->badGuyId);
            }
        }
    }
}

void GameServer::OnTeamMemberExit(MsgBase* msg)
{
    MsgTeam_MemberExit* exitMsg = (MsgTeam_MemberExit*)msg;
    Player* player = g_MapManager->GetPlayerByID(exitMsg->OBJID);
    if (player)
    {
        Team* pTeam = g_SocailManager->GetTeam(player->m_teamId);
        if (pTeam)
        {
            pTeam->OnPlayerExit(exitMsg->OBJID);   
        }
    }
}

void GameServer::OnPlayerChest(MsgBase* msg)
{
	MsgChest* pChest = (MsgChest*)msg;
	Player* player = g_MapManager->GetPlayerByID(pChest->OBJID);
	if(player)
	{
		player->Chest(pChest->MoneyType, pChest->Num);
	}
}

void GameServer::OnPlayerDetailInfoQuery(MsgBase* msg)
{
    static char noneName[MAX_NAME_SIZE] = { "------" };

    MsgOtherRole_DetailInfoQuery * pQuery = (MsgOtherRole_DetailInfoQuery*)msg;
    Player* player = g_MapManager->GetPlayerByID(pQuery->queryGuid);
    if (player)
    {
        MsgOtherRole_DetailInfo detail;
        detail.OBJID = pQuery->OBJID;

        detail.guid         = player->m_GUID;
        strncpy(detail.name, player->m_Name, MAX_NAME_SIZE);
        detail.level        = player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
        detail.sex          = player->m_Sex;
        detail.battlePower  = player->m_BattlePower;
        if (player->m_SocietyID)
        {
            Gang* pGang = g_GangManager->GetGangByID(player->m_SocietyID);
            if (pGang){
                strncpy(detail.gangName, pGang->m_szName, MAX_NAME_SIZE);
            }
            else{
                strncpy(detail.gangName, noneName, MAX_NAME_SIZE);
            }
        }
        else {
            strncpy(detail.gangName, noneName, MAX_NAME_SIZE);
        }

        detail.hp               = player->m_Attribute[ATTRIBUTE_HP].GetValueI();
        detail.maxHp            = player->m_Attribute[ATTRIBUTE_HPMAX].GetValueI();
        detail.mp               = player->m_Attribute[ATTRIBUTE_MP].GetValueI();
        detail.maxMp            = player->m_Attribute[ATTRIBUTE_MPMAX].GetValueI();
        detail.attack           = player->m_Attribute[ATTRIBUTE_ATTACK].GetValueI();
        detail.defence          = player->m_Attribute[ATTRIBUTE_DEFENSE].GetValueI();
        detail.hit              = player->m_Attribute[ATTRIBUTE_HIT].GetValueI();
        detail.dodge            = player->m_Attribute[ATTRIBUTE_DODGE].GetValueI();
        detail.crit             = player->m_Attribute[ATTRIBUTE_CRIT].GetValueI();
        detail.tenacity         = player->m_Attribute[ATTRIBUTE_TENACITY].GetValueI();
        detail.critProb         = player->m_Attribute[ATTRIBUTE_CRIT_PROB].GetValueF();
        detail.hitProb          = player->m_Attribute[ATTRIBUTE_HIT_PROB].GetValueF();

        SvrItemInfo*  pEquipment;
        for (int i = 0; i < EQUIPTYPE_MAX; ++i)
        {
            pEquipment = player->m_Equip[i];
            if (pEquipment){
                memcpy(&detail.equipments[i], &pEquipment->Data, sizeof(ItemInfoIt));
            }
            else{
                memset(&detail.equipments[i], 0, sizeof(ItemInfoIt));
            }
        }

        SendToClient(&detail);
    }
    else
    {
        MsgNotify notify(ID_G2C, Notify_S2C_PlayerOffline); //>֪ͨ�ͻ��˽�ɫ������
        notify.OBJID = pQuery->OBJID;
        SendToClient(&notify);
    }
}
