#include "NetManager.h"
#include "SceneManager.h"
#include "LogicManager.h"
#include "ResManager.h"
#include "BaseFunction.h"
#include "UIManager.h"
#include "Hero.h"
#include "UILogin.h"
#include "BaseFunction.h"
#include "RequestServerList.h"
#include "external/tinyxml2/tinyxml2.h"
#include "StackStream.h"
#include "CSVFileManager.h"
#include "ChatSystem.h"
#include "UIBag.h"
#include "UIUpgradeEquip.h"
#include "UIFriends.h"
#include "UITask.h"
#include "UIGongFa.h"
#include "Monster.h"
#include "UIMain.h"
#include "UIShop.h"
#include "UIApplyGang.h"
#include "UIGuild.h"
#include "UIPlayerInfo.h"
#include "CCUserDefault.h"
#include "CCFileUtils.h"
#include "deprecated/CCString.h"

NetManager *g_NetManager = NULL;

NetManager::NetManager():m_Net(NULL),m_CutTip(NULL),m_ReturnLoginTip(NULL)
{
    m_LastTeamInfo = new MsgTeam_Apply(0, "unknwon", 0, 0);
    m_LastApplyMsgBox = NULL;
}

NetManager::~NetManager()
{
	Release();
}

void NetManager::Init()
{
	//连接
	m_Net=RakNet::RakPeerInterface::GetInstance();
	m_Net->AllowConnectionResponseIPMigration(false);

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.socketFamily=AF_INET;
	m_Net->Startup(1,&socketDescriptor, 1);
	m_Net->SetOccasionalPing(true);

    bool rs = LoadConfing();
    if (false == rs)
    {
        CCASSERT(rs, "NetManager LoadConfing fail.");
        return;
    }else
    {
        m_ServerAddress.FromStringExplicitPort(GetGateIP().c_str(), atoi(GetGatePort().c_str()));
    }


	Connect();
}

void NetManager::Release()
{
	if (m_Net)
	{
		m_Net->Shutdown(300);
		RakNet::RakPeerInterface::DestroyInstance(m_Net);
	}	
}

void NetManager::Update(float delta)
{

	// 接收数据（取得缓冲区中的所有消息，直到缓冲区为空）  
	RakNet::Packet* packet;
	for (packet=m_Net->Receive(); packet; m_Net->DeallocatePacket(packet), packet = m_Net->Receive())
	{
		switch(packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:	//连接成功
			{
				
			}
			break;
		case ID_NEW_INCOMING_CONNECTION:	//收到新的连接
			break;						
		case ID_ALREADY_CONNECTED:	//对方说你是重复的连接
		case ID_CONNECTION_BANNED: // 被禁止连接
		case ID_CONNECTION_ATTEMPT_FAILED:	//连接请求无法完成
		case ID_NO_FREE_INCOMING_CONNECTIONS:	//对方连接已满，不接受新的连接			
			{	
				// 提示
				if (!m_ReturnLoginTip && !m_CutTip)
				{
					m_CutTip = g_UIManager->ShowMsgBox(2, false, MSGBOX_EVENT_LINK);
				}
			}
			break;
		case ID_DISCONNECTION_NOTIFICATION:	//服务器已经关闭	
		case ID_CONNECTION_LOST:		//对方连接丢失
			// 提示
			if (!m_ReturnLoginTip && !m_CutTip)
			{
				m_CutTip = g_UIManager->ShowMsgBox(2, false, MSGBOX_EVENT_LINK);
			}
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:  //RAKNET_PROTOCOL_VERSION与远端系统上的版本值不匹配
			log("ID_INCOMPATIBLE_PROTOCOL_VERSION = %d\n", packet->data[2]);
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION: // 在客户端/服务器环境中,一个客户端已经断开了连接
			log("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
			break;
		case ID_REMOTE_CONNECTION_LOST: // 在客户端/服务器环境，客户端被迫断开了连接
			log("ID_REMOTE_CONNECTION_LOST\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // 在客户端/服务器环境，新的客户端接入
			log("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
			break;
		case ID_INVALID_PASSWORD:	//无效的密码
			log("ID_INVALID_PASSWORD\n");
			break;
		case ID_CONNECTED_PING:		//收到一个未连接的Ping
		case ID_UNCONNECTED_PING:	//收到一个已连接的Ping
			log("Ping from %s\n", packet->systemAddress.ToString(true));
			break;	
		default:
			OnRecv(packet);
			break;
		}
	}
}

bool NetManager::Connect()
{
	RakNet::ConnectionAttemptResult car = m_Net->Connect(GetGateIP().c_str(), atoi(GetGatePort().c_str()), GATE_CONNECTION_PASSWORD, (int)strlen(GATE_CONNECTION_PASSWORD));
	
	return true;
}

int NetManager::SendMsg(MsgBase &msg)
{
	if (g_LogicManager->m_Hero)
	{
		msg.OBJID = g_LogicManager->m_Hero->m_GUID;
	}
	if (msg.Len < sizeof(msg))
	{
		log("Error------->Send message %d  Len %8d \n", msg.MsgID, msg.Len);
		return false;
	}	
	if (m_Net)
	{
		log("---------->Send message %d  Len %8d \n", msg.MsgID, msg.Len);
		return m_Net->Send((char*)&msg, msg.Len, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_ServerAddress, false);
	}
	return false;
}

int NetManager::OnRecv(RakNet::Packet* pData)
{
	//!test
	if (pData->data[0] > ID_USER_PACKET_ENUM)
	{
		log("~~GetMsg %d %d", pData->data[0], pData->data[1]);
	}

	MsgBase* msg = (MsgBase*)(pData->data);
	if (msg && (msg->SysID == ID_L2C || msg->SysID == ID_G2C || msg->SysID == ID_D2C))
	{
		switch (msg->MsgID)
		{
		case Msg_Gang_Notify:
			OnRecvGangNotify(msg);
			break;
		case Msg_Task_Notify:
			OnRecvTaskNotify(msg);
			break;
		case Msg_Fri_Notify:
			OnRecvFriendsNotify(msg);
			break;
		case Msg_Notify:
			OnNotify(msg);			
			break;
		case Msg_ServerList:
			{				
				//先清空服务器列表
				g_requestServerList->ClearServerList();

				MsgServerList * temp = (MsgServerList*)msg;
				if (temp)
				{
					ServerListIt* ttt= &(temp->ServerListInfo);
					for (int i =0; i < temp->ServerNum; i++)
					{
						ServerListIt it;
						memcpy(&it, ttt ,sizeof(ServerListIt));
						ttt++;

						tagServer tagSerInfo;
						sprintf(tagSerInfo.strServerName,it.ServerName,NULL);
						tagSerInfo.iServerID = it.ServerID;
						tagSerInfo.serverPos = i;
						if (it.isStart)
							tagSerInfo.serverState = (ServerState)it.ServerState;
						else
							tagSerInfo.serverState = SERSTATE_CLOSE;
						g_requestServerList->AddServerList(tagSerInfo);
					}
				}			
				g_UIManager->OpenUI(UI_DEFAULTSERVER);
				g_UIManager->RemoveUI(UI_LOGIN);
			}
			break;
		case Msg_Role_BaseInfo:
            OnPlayerBaseInfo(msg);
            break;
		case Msg_Role_DetailAttribute:
			OnPlayerDetailAttribute(msg);
			break;
        case Msg_OtherRole_BaseInfo:
            OnOtherPlayerEnterView(msg);
            break;
        case Msg_OtherRole_Leave:
            OnOtherPlayerLeaveView(msg);
            break;
        case Msg_Object_Move:
            OnObjectMove(msg);
            break;
        case Msg_Object_Move_kick:
            OnObjectMoveKick(msg);
            break;
        case Msg_Object_Move_Stop:
            OnObjectMoveStop(msg);
            break;
		case Msg_Object_SyncAttribute:
			OnObjectSyncAttribute(msg);
			break;
        case Msg_Object_AddHp:
            OnObjectAddHp(msg);
            break;
        case Msg_Object_DropHp:
            OnObjectDropHp(msg);
            break;
		case Msg_Player_state:
			OnPlayerstate(msg);
			break;
		case Msg_Player_UseSkill:
			OnPlayerUseSkill(msg);
			break;
		case Msg_Player_ReliveData:
			OnPlayerReliveData(msg);
			break;
		case Msg_Player_Relive:
			OnPlayerRelive(msg);
			break;
        case Msg_Monster_Baseinfo:
            OnMonsterEnterView(msg);
            break;
        case Msg_Monster_Move:
            OnMonsterMoving(msg);
            break;
        case Msg_Monster_Del:
            OnMonsterLeaveView(msg);
            break;
		case Msg_Monster_Goto:
			OnMonsterGoto(msg);
			break;
        case Msg_Item_Add_Set:
            OnAddItem(msg);
            break;
        case Msg_Item_Del_Set:
            OnDelItem(msg);
            break;
        case Msg_Equip_List:
            OnEquipList(msg);
            break;
        case Msg_Item_Updata_Set:
            OnUpdateItem(msg);
            break;
        case Msg_Equip_Update:
            OnEquipUpdate(msg);
            break;
        case Msg_Equip_ShowUpdate:
            OnPlayerChangeEquipment(msg);
            break;
        case Msg_Skill_List:
            OnSkillList(msg);
            break;
        case Msg_Skill_Add:
            OnAddSkill(msg);
            break;
        case Msg_SKill_Update:
            OnUpdateSkill(msg);
            break;
        case Msg_Object_BuffList:
            OnObjBuffList(msg);
            break;
        case Msg_Object_AddBuff:
            OnObjAddBuff(msg);
            break;
        case Msg_Object_DelBuff:
            OnObjDelBuff(msg);
            break;
        case Msg_Chat_CHANNEL:
            OnChatMsg(msg);
            break;
        case Msg_Team_Apply:
            OnTeamApply(msg);
            break;
        case Msg_Team_ReplyApply:
            OnTeamReplyApply(msg);
            break;
        case Msg_Team_Created:
            OnTeamCreated(msg);
            break;
        case Msg_Team_NewMember:
            OnTeamNewMember(msg);
            break;
        case Msg_Team_MemberExit:
            OnTeamMemberExit(msg);
            break;
        case Msg_Team_ChangeLeader:
            OnTeamChangeLeader(msg);
            break;
        case Msg_OtherRole_DetailInfo:
            OnPlayerOtherDetailInfo(msg);
            break;
		default:
			break;
		}
	}
	return 0; 
}

void NetManager::OnNotify(MsgBase* msg)
{
	MsgNotify* temp =  (MsgNotify*)msg;	
	switch (temp->Type)
	{
	case Notify_S2C_Regiser_NameLen:
		g_UIManager->ShowMsgBox(24);
		break;
	case Notify_S2C_Regiser_PasswordLen:
		g_UIManager->ShowMsgBox(29);
		break;
	case Notify_S2C_Regiser_NameShield:
		g_UIManager->ShowMsgBox(28);
		break;
	case Notify_S2C_Regiser_NameRepeat:
		g_UIManager->ShowMsgBox(25);
		break;
	case Notify_S2C_Regiser_OK:
		{
			UILogin* login = (UILogin*)g_UIManager->GetUI(UI_LOGIN);
			if (login)
			{
				g_UIManager->ShowMsgBox(3);
			    login->RecordNewRegisterUserName(login->GetRegisterUserName(), login->GetRegisterPassword());
			    login->SetUserName(login->GetRegisterUserName());
				login->switchWindows(false);
			}		
		}
		break;
	case Notify_S2C_Login_NamePassError:
		g_UIManager->ShowMsgBox(5);
		break;
	case Notify_S2C_Login_OK:
		{
			UILogin* login = (UILogin*)g_UIManager->GetUI(UI_LOGIN);
			if(login)
			{
				const char* regName = login->GetUserName();
				const char* regPassword = login->GetPassword();
			
				//记录账号与密码
				UserDefault::getInstance()->setStringForKey("username",regName);
				string strPW = regPassword;
				strPW = BaseFunction::StrCode(strPW);
				UserDefault::getInstance()->setStringForKey("userpassword",strPW);
			    login->RecordNewRegisterUserName(regName, regPassword);
			}

			//查询服务器列表
			MsgNotify ttt(ID_C2L, Notify_C2S_Login_ServerList);
			SendMsg(ttt);
			break;
		}
	case Notify_S2C_Login_ServerNotOpen:
		g_UIManager->ShowMsgBox(4);
		break;
	case Notify_S2C_Login_ReLogin:
		g_UIManager->ShowMsgBox(13, false, MSGBOX_EVENT_RETURNLOGIN);
		break;
	case Notify_S2C_CreateRole:
		g_SceneManager->Login2CreateRole();
		break;
	case Notify_S2C_CreateRole_NameRepeat:
		g_UIManager->ShowMsgBox(33);
		break;
	case Notify_S2C_CreateRole_OK:
		g_UIManager->ShowMsgBox(34, false, MSGBOX_EVENT_CREATEROLEOK);
		break;
    case Notify_S2C_RoleExist :
        {
            MsgRole_EnterMap enterMap; //>不许要创建角色,直接进入地图就是了.
            SendMsg(enterMap);
        }break;
    case Notify_S2C_EquipmentUpStar_Failture:{
            const TextInfo *pText = g_CSVFileManager->GetTextByID(632);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA);
            g_UIManager->HideUI(UI_WAITTING);
            if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
            {
                UIUpgradeEquip* uiUpgrade = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
                uiUpgrade->OnUpStarFailture();
            }
        }break;
    case Notify_S2C_ItemUse_RepeatedAddSkillTree:{
            const TextInfo *pText = g_CSVFileManager->GetTextByID(662);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //该技能树已经开启!
            g_UIManager->HideUI(UI_ITEM_INFO);
            g_UIManager->HideUI(UI_WAITTING);
        }break;
    case Notify_S2C_ItemUse_SkillTreeExpFull:
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(663);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //该技能树已达经验上限!
            g_UIManager->HideUI(UI_ITEM_INFO);
            g_UIManager->HideUI(UI_WAITTING);
        }break;
    case Notify_S2C_ItemUse_CD_Cooling:
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(659);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //CD尚未冷却!
            g_UIManager->HideUI(UI_WAITTING);
        }break;
	case Notify_S2C_No_Money:
		g_UIManager->ShowMsgBox(62);
		break;
	case Notify_S2C_No_Gold:
		g_UIManager->ShowMsgBox(63);
		break;
    case Notify_S2C_PlayerOffline:
        g_UIManager->Notify(BaseFunction::a2u("角色已下线!"), Color3B::MAGENTA); //>角色已下线!
        break;
	default:
		log("Error~~~MsgNotify %d\n", temp->Type);
		break;	
	}
}

bool GetIPAndPort(const tinyxml2::XMLElement* node,string& ip, string& port)
{
    int c = 0;
    const tinyxml2::XMLElement* curNode = node->FirstChildElement();
    while (nullptr != curNode)
    {
        const char* nodeName = curNode->Value();
        if (!strcmp(nodeName, "ip"))
        {
            ip = curNode->GetText();
            c++;
        }

        if (!strcmp(nodeName, "port"))
        {
            port = curNode->GetText();
            c++;
        }

        if (c > 1)
            return true;

        curNode = curNode->NextSiblingElement();
    }
    return false;
}


bool NetManager::LoadConfing()
{
    tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
    std::string xmlBuffer = FileUtils::getInstance()->getStringFromFile("NetConfig.xml");

    if (xmlBuffer.empty())
    {
        CCLOG("can not read xml file");
        return false;
    }
    xmlDoc->Parse(xmlBuffer.c_str(), xmlBuffer.size());

    // get root node
    const tinyxml2::XMLElement*rootNode = xmlDoc->RootElement();
    if (nullptr == rootNode)
    {
        CCLOG("read root node error");
        return false;
    }
    // find the node
    int c = 0;
    const tinyxml2::XMLElement* curNode = rootNode->FirstChildElement();
    while (nullptr != curNode)
    {
        const char* nodeName = curNode->Value();
        if (!strcmp(nodeName, "gate"))
        {
            if (GetIPAndPort(curNode, m_GateIP, m_GatePort))
                c++;
        }

        if (!strcmp(nodeName, "resource"))
        {
            if (GetIPAndPort(curNode, m_ResIP, m_ResPort))
                c++;
        }

        if (c > 1)
            break;

        curNode = curNode->NextSiblingElement();
    }

    // 读取成功
    if (c > 1)
    {
        return true;
    }

    return false;
}

void NetManager::OnPlayerBaseInfo(MsgBase* msg)
{
    MsgRole_BaseInfo* temp = (MsgRole_BaseInfo*)msg;
    //先预存数据，待创建时赋值
    memcpy(&g_LogicManager->m_BaseInfo, &temp->BaseInfo, sizeof(BaseInfoIt));

    //!进入地图
    g_SceneManager->Login2Main();
    
    //>请求背包物品数据.(像背包物品这样的比较常用的数据,很多ui都会使用到背包数据.
    //>等打开ui再请求数据还必然要重新通知ui更新.不同的ui请求.更新不同的ui.显得麻烦.所以直接开始请求了)
    MsgItem_BagList bagListMsg;
    SendMsg(bagListMsg);

    //>角色穿上装备数据.
    MsgEquip_List equipListMsg;
    equipListMsg.SysID = ID_C2G;
    SendMsg(equipListMsg);

    //>技能树清单
    MsgSkill_List treeListMsg;
    treeListMsg.SysID = ID_C2G;
    SendMsg(treeListMsg);

    //>任务清单]
    if (g_LogicManager->m_Hero)
    {
        MsgTasks Msg(g_LogicManager->m_Hero->m_GUID,0,0);	
        Msg.MsgID = Msg_Task_GetList;
        SendMsg(Msg);
    }
}

void NetManager::OnPlayerDetailAttribute(MsgBase* msg)
{
	MsgRole_DetailAttribute* temp = (MsgRole_DetailAttribute*)msg;
	g_LogicManager->m_Hero->LoadDetailAttribute(temp->Attribute);
}

void NetManager::OnPlayerOtherDetailInfo(MsgBase* msg)
{
    MsgOtherRole_DetailInfo* detail = (MsgOtherRole_DetailInfo*)msg;

    UIPlayerInfo* pPlayerInofUI = static_cast<UIPlayerInfo*>(g_UIManager->GetUI(UI_PLAYERINFO));
    if (pPlayerInofUI == NULL)
        g_UIManager->CreatePlayerInfo();

    pPlayerInofUI = static_cast<UIPlayerInfo*>(g_UIManager->GetUI(UI_PLAYERINFO));
    if (pPlayerInofUI)
    {
        pPlayerInofUI->SetInfo(detail);
        g_UIManager->OpenUI(UI_PLAYERINFO);
    }
}

void NetManager::OnObjectMove(MsgBase* msg)
{
    StackStream<> moveStream(msg, msg->Len);
    MsgObject_Move moveMsg;
    moveStream.Read(&moveMsg, sizeof(moveMsg));
	if (moveMsg.PathNum <= 0)
	{
        if (moveMsg.MoverType == OBJECT_PLAYER)
        {
            Player* player = g_LogicManager->GetAllPlayerByGUID(moveMsg.MoverID);
            if (player){
                player->m_MoveCompoenet->MoveByTarget(Vec2(moveMsg.TargetPosX, moveMsg.TargetPosY));
            }
        }
        else if(moveMsg.MoverType == OBJECT_MONSTER)
        {
            Monster* monster = g_LogicManager->GetMonsterByGUID(moveMsg.MoverID);
            if (monster){
                monster->m_MoveCompoenet->MoveByTarget(Vec2(moveMsg.TargetPosX, moveMsg.TargetPosY));
            }
        }
	}
    else
    {
        MapObj* pObj = NULL;
        if (moveMsg.MoverType == OBJECT_PLAYER)
        {
            Player* player = g_LogicManager->GetAllPlayerByGUID(moveMsg.MoverID);
            if (player)
                pObj = player;
        }
        else if(moveMsg.MoverID == OBJECT_MONSTER)
        {
            Monster* monster = g_LogicManager->GetMonsterByGUID(moveMsg.MoverID);
            if (monster)
                pObj = monster;
        }

        if (pObj)
        {
            std::vector<int> paths;
            int posId;
            for (int i = 0; i < moveMsg.PathNum; i++)
            {
                moveStream.Read(&posId, sizeof(posId));
                paths.push_back(posId);       
            }
            pObj->m_MoveCompoenet->MoveByPath(paths, Vec2(moveMsg.TargetPosX, moveMsg.TargetPosY));
        }
    }
}

void NetManager::OnObjectMoveKick(MsgBase* msg)
{
    MsgObject_Move_Kick* pTickMsg = static_cast<MsgObject_Move_Kick*>(msg);
    MapObj* pObj = NULL;
    if (pTickMsg->MoverType == OBJECT_PLAYER)
    {
        Player* player = g_LogicManager->GetAllPlayerByGUID(pTickMsg->MoverID);
        if (player)
            pObj = player;
    }
    else
    {
        Monster* monster = g_LogicManager->GetMonsterByGUID(pTickMsg->MoverID);
        if (monster)
            pObj = monster;
    }

    if (pObj)
    {
        pObj->m_MoveCompoenet->MoveEnd();
        pObj->SetMapPosition(Vec2(pTickMsg->MoverPosX, pTickMsg->MoverPosY));
    }
}

void NetManager::OnObjectMoveStop(MsgBase* msg)
{
    MsgObject_Move_Stop* pStopMsg = static_cast<MsgObject_Move_Stop*>(msg);
    MapObj* pObj = NULL;
    if (pStopMsg->MoverType == OBJECT_PLAYER)
    {
        Player* player = g_LogicManager->GetAllPlayerByGUID(pStopMsg->MoverID);
        if (player)
            pObj = player;
    }
    else
    {
        Monster* monster = g_LogicManager->GetMonsterByGUID(pStopMsg->MoverID);
        if (monster)
            pObj = monster;
    }

    if (pObj)
    {
        pObj->m_MoveCompoenet->MoveEnd();
        Vec2 endPosition(pStopMsg->MoverPosX, pStopMsg->MoverPosY);
        pObj->SetMapPosition(Vec2(pStopMsg->MoverPosX, pStopMsg->MoverPosY));
        if (endPosition.distance(pObj->getPosition())> 128.0f)
        {
            pObj->SetMapPosition(endPosition);
        }
    }
}

void NetManager::OnOtherPlayerEnterView(MsgBase* msg)
{
    MsgOtherRole_BaseInfo* otherRole = (MsgOtherRole_BaseInfo*)msg;
    g_LogicManager->AddOtherPlayerToMap(otherRole);
}

void NetManager::OnOtherPlayerLeaveView(MsgBase* msg)
{
    MsgOtherRole_Leave* leaveMsg = (MsgOtherRole_Leave*)msg;
    g_LogicManager->RemovePlayer(leaveMsg->LeaverID);
}

void NetManager::OnObjectSyncAttribute(MsgBase* msg)
{
    MsgObject_SyncAttribute* pSyncAttr = (MsgObject_SyncAttribute*)msg;

    if (g_LogicManager->m_Hero->m_GUID == pSyncAttr->OwnerID)
    {
		//!还需处理为何改变属性
		g_LogicManager->m_Hero->SetProperty(pSyncAttr->Property,	pSyncAttr->Value);

        //>设置界面
        if (pSyncAttr->Property == ATTRIBUTE_MONEY || 
            pSyncAttr->Property == ATTRIBUTE_GOLD || 
            pSyncAttr->Property == ATTRIBUTE_BINDGOLD || 
            pSyncAttr->Property == ATTRIBUTE_ARENAMONEY ||
            pSyncAttr->Property == ATTRIBUTE_BATTLEFIELDMONEY )
        {
            if (g_UIManager->IsOpened(UI_BAG))
            {
                UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
                if (uiBag)
                {
                    uiBag->ReloadHeroMoney();
                }
            }

            if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
            {
                UIUpgradeEquip* uiUpgread = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
                if (uiUpgread)
                {
                    uiUpgread->ReloadHeroMoney();
                }
            }

            if (g_UIManager->IsOpened(UI_SHOP))
            {
                UIShop* uiShop = static_cast<UIShop*>(g_UIManager->GetUI(UI_SHOP));
                if (uiShop)
                {
                    uiShop->SetMoneyType(pSyncAttr->Property, g_LogicManager->m_Hero->m_Attribute[pSyncAttr->Property]);
                }
            }
        }

	}else
    {
        Player* player = g_LogicManager->GetPlayerByGUID(pSyncAttr->OwnerID);
        if (player)
        {
            if (pSyncAttr->Property == ATTRIBUTE_HP)
            {
                player->SetHp((int)pSyncAttr->Value);
            }
        }
    }
}

void NetManager::OnObjectAddHp(MsgBase* msg)
{
    MsgObject_AddHp* addHpMsg = (MsgObject_AddHp*)msg;
    if (addHpMsg->OwnerType == OBJECT_PLAYER)
    {
        if (addHpMsg->OwnerID == g_LogicManager->m_Hero->m_GUID)
        {
            if (addHpMsg->Mode.mode == AddHpMode::Relive)
            {
                g_LogicManager->m_Hero->Idle();
            }

            g_LogicManager->m_Hero->SetProperty(ATTRIBUTE_HP, addHpMsg->LastHp);
        }
        else
        {
            Player* player = g_LogicManager->GetPlayerByGUID(addHpMsg->OwnerID);
            if (player)
            {
                if (addHpMsg->Mode.mode == AddHpMode::Relive)
                {
                    player->Idle();
                }
                player->SetHp(addHpMsg->LastHp);
                //player->m_AnimCompoenet->PlayDropHpAnim(addHpMsg->Mode);
            }
        }

    }
    else if(addHpMsg->OwnerType == OBJECT_MONSTER)
    {

    }
    else
    {
        CCASSERT(0, "");
    }
}

void NetManager::OnObjectDropHp(MsgBase* msg)
{
    MsgObject_DropHp* dropHpMsg = (MsgObject_DropHp*)msg;
    if (dropHpMsg->OwnerType == OBJECT_PLAYER)
    {
        if (dropHpMsg->OwnerID == g_LogicManager->m_Hero->m_GUID)
        {
            g_LogicManager->m_Hero->SetProperty(ATTRIBUTE_HP, dropHpMsg->LastHp);
        
            g_LogicManager->m_Hero->m_AnimCompoenet->PlayDropHpAnim(dropHpMsg->Mode);
        }
        else
        {
            Player* player = g_LogicManager->GetPlayerByGUID(dropHpMsg->OwnerID);
            if (player)
            {
                player->SetHp(dropHpMsg->LastHp);

                player->m_AnimCompoenet->PlayDropHpAnim(dropHpMsg->Mode);
            }
        }
    }
    else if(dropHpMsg->OwnerType == OBJECT_MONSTER)
    {

    }
    else
    {
        CCASSERT(0, "");
    }
}

void NetManager::OnPlayerstate(MsgBase* msg)
{
	MsgPlayer_state* state = (MsgPlayer_state*)msg;
	for (int i = 0; i < State_Max; i++)
	{
		g_LogicManager->m_Hero->m_State[i] = state->State[i];
	}
}

void NetManager::OnPlayerUseSkill(MsgBase* msg)
{
	MsgG2CPlayer_UseSkill* temp = (MsgG2CPlayer_UseSkill*)msg;

	g_LogicManager->PlayerUseSkill(temp);
}

void NetManager::OnPlayerReliveData(MsgBase* msg)
{
	MsgPlayer_ReliveData* ReliveData = (MsgPlayer_ReliveData*)msg;
	g_UIManager->ShowMsgBox("您已死亡，点确定原地复活！", false, MSGBOX_EVENT_RELIVE);
    g_SceneManager->m_MainLayer->EnableGrey(true);
}

void NetManager::OnPlayerRelive(MsgBase* msg)
{
    MsgG2CPlayer_Relive* reliveMsg = (MsgG2CPlayer_Relive*)msg;
    if (reliveMsg->mapid != g_SceneManager->m_MainLayer->m_MapId)
    {
        
    }
    else
    {
        g_LogicManager->m_Hero->Idle();
        g_LogicManager->m_Hero->SetTile(g_SceneManager->m_MainLayer->TileId2Tile(reliveMsg->posid));
        g_SceneManager->m_MainLayer->EnableGrey(false);
    }
}

void NetManager::OnPlayerChangeEquipment(MsgBase* msg)
{
    MsgEquip_ShowUpdate* showUpdate = (MsgEquip_ShowUpdate*)msg;
    Player* pPlayer = g_LogicManager->GetAllPlayerByGUID(showUpdate->whoGuid);
    if (pPlayer)
    {
        int FaceId[EQUIPTYPE_MAX];
        pPlayer->EquipIdToFaceId((int*)showUpdate->equip, FaceId);
        //>刷新地图上的角色
        pPlayer->ReloadFace(FaceId, !g_LogicManager->IsHeroGuid(showUpdate->whoGuid));

        //>刷新界面上的角色
        if (g_LogicManager->IsHeroGuid(showUpdate->whoGuid) && g_UIManager->IsOpened(UI_BAG))
        {
            UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
            uiBag->LoadHeroFace();
        }
    }
}

void NetManager::OnAddItem(MsgBase* msg)
{
    MsgItem_Add_Set* pAddItem = static_cast<MsgItem_Add_Set*>(msg);
    if (g_LogicManager->m_Hero)
    {
        Bag& bag = g_LogicManager->m_Hero->m_Bag;

        CliItemInfo* pItem;
        StackStream<> addItemStream(pAddItem, pAddItem->Len);
        MsgItem_Add_Set msgHead;
        addItemStream.Read(&msgHead, sizeof(msgHead));

        if (msgHead.operateType == ItemOperate_BagLoading)
            bag.Clear();

        for (uint8 i = 0; i < msgHead.addNumItem; ++i)
        {
            pItem = new CliItemInfo();
            if (pItem != NULL)
            {
                //>取得物品信息
                addItemStream.Read(&pItem->Data.guid, sizeof(uint64));
                addItemStream.Read(&pItem->Data.id, sizeof(int));
                addItemStream.Read(&pItem->Data.prefixID, sizeof(int));
                addItemStream.Read(&pItem->Data.starNum, sizeof(int));
                addItemStream.Read(&pItem->Data.isBind, sizeof(uint8));
                addItemStream.Read(&pItem->Data.num, sizeof(int));
                addItemStream.Read(&pItem->Data.isLock, sizeof(uint8));
                addItemStream.Read(&pItem->Data.time, sizeof(int));
               
                pItem->pCfg = g_CSVFileManager->GetItemsByID(pItem->Data.id);
                //>添加物品
                bag.AddItem(pItem);
                log("add itemGuid[%llu]", pItem->Data.guid);

				if (msgHead.operateType == ItemOperate_Chest)
				{
					g_UIManager->AddChestItem(pItem);
				}
            }
        }

        //>根据操作类型做提示或者其他逻辑
        switch (msgHead.operateType)
        {
        case ItemOperate_BagLoading:
            log("获得背包物品!");
            break;
        case ItemOperate_BuyMallItem:
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(627);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //购买成功!
                g_UIManager->HideUI(UI_WAITTING);
            }break;
        case ItemOperate_ItemMerge:
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(628);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //合成成功!
                g_UIManager->HideUI(UI_WAITTING);
                if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
                {
                    UIUpgradeEquip* uiUpgrade = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
                    uiUpgrade->SetPage(uiUpgrade->GetPage());
                }
            }break;
        case ItemOperate_EquipGetOff:
        case ItemOperate_WearEquipment:
            {
                assert(pItem->IsEquipment());
                assert(pItem->pCfg->iSamllClass < EQUIPTYPE_MAX);
                if (g_UIManager->IsOpened(UI_BAG))
                {
                    UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
                    uiBag->SetPage(uiBag->GetPage());
                }
            }break;
        default:
            break;
        }

    }
}

void NetManager::OnDelItem(MsgBase* msg)
{
    MsgItem_Del_Set* pDelItem = static_cast<MsgItem_Del_Set*>(msg);

    bool realDelete = true;
    CliItemInfo* pReserveItem = NULL;

    if (g_LogicManager->m_Hero)
    {
        Bag& bag = g_LogicManager->m_Hero->m_Bag;

        uint64 guid;
        StackStream<> delItemStream(pDelItem, pDelItem->Len);
        MsgItem_Del_Set msgHead;
        delItemStream.Read(&msgHead, sizeof(msgHead));

        if (msgHead.operateType == ItemOperate_WearEquipment)
        {
            realDelete = false;
        }

        for (uint8 i = 0; i < msgHead.delNumItem; ++i)
        {
            //>取得物品guid
            delItemStream.Read(&guid, sizeof(uint64));
            //>删除物品
            bool rs;
            if (realDelete){
                rs = bag.ReomveItem(guid);
            }
            else{
                pReserveItem = bag.ReomveItemPtr(guid);
                rs = (pReserveItem != NULL);
            }
            if (false == rs)
            {
                log("bag can't find item guid=%llu when call OnDelItem.", guid);
            }
            else
            {
                log("delete itemGuid[%llu]", guid);
            }
        }

        //>根据操作类型做提示或者其他逻辑
        switch (msgHead.operateType)
        {
        case ItemOperate_ItemUsed:
            {
                g_UIManager->HideUI(UI_ITEM_INFO);
                g_UIManager->HideUI(UI_WAITTING);
                const TextInfo *pText = g_CSVFileManager->GetTextByID(629);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //物品使用成功!
                if (g_UIManager->IsOpened(UI_BAG))
                {
                    UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
                    uiBag->SetPage(uiBag->GetPage());
                }
                return;
            }
            break;
        case ItemOperate_SellBagItem:
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(630);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //出售成功!
                g_UIManager->HideUI(UI_ITEM_INFO);
                g_UIManager->HideUI(UI_WAITTING);
                if (g_UIManager->IsOpened(UI_BAG))
                {
                    UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
                    uiBag->SetPage(uiBag->GetPage());
                }
            }break;
        case ItemOperate_WearEquipment:
            {
                assert(pReserveItem->IsEquipment());
                assert(pReserveItem->pCfg->iSamllClass < EQUIPTYPE_MAX);
                if (g_UIManager->IsOpened(UI_BAG))
                {
                    UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
                    uiBag->SetPage(uiBag->GetPage());
                }
            }break;
        default:
            break;
        }
    }
}

void NetManager::OnUpdateItem(MsgBase* msg)
{
    MsgItem_Update_Set* pUpdateItem = static_cast<MsgItem_Update_Set*>(msg);
    if (g_LogicManager->m_Hero)
    {
        Bag& bag = g_LogicManager->m_Hero->m_Bag;

        ItemInfoIt data;
        CliItemInfo* pItem;
        StackStream<> updateItemStream(pUpdateItem, pUpdateItem->Len);
        MsgItem_Update_Set msgHead;
        updateItemStream.Read(&msgHead, sizeof(msgHead));
        for (uint8 i = 0; i < msgHead.updateNumItem; ++i)
        {
            //>取得物品guid
            updateItemStream.Read(&data.guid, sizeof(uint64));
            updateItemStream.Read(&data.id, sizeof(int));
            updateItemStream.Read(&data.prefixID, sizeof(int));
            updateItemStream.Read(&data.starNum, sizeof(int));
            updateItemStream.Read(&data.isBind, sizeof(uint8));
            updateItemStream.Read(&data.num, sizeof(int));
            updateItemStream.Read(&data.isLock, sizeof(uint8));
            updateItemStream.Read(&data.time, sizeof(int));

            pItem = bag.GetItemByGuid(data.guid);
            if (pItem)
            {
                //>属性赋值
                pItem->Data.prefixID = data.prefixID;
                pItem->Data.starNum = data.starNum;
                pItem->Data.isBind = data.isBind;
                pItem->Data.num = data.num;
                pItem->Data.isLock = data.isLock;
                pItem->Data.time = data.time;
                log("update itemGuid[%llu]", data.guid);
            }
            else
            {
                log("bag can't find item guid=%llu when call OnUpdateItem.", data.guid);
            }

			if (msgHead.operateType == ItemOperate_Chest)
			{
				g_UIManager->AddChestItem(pItem);
			}
        }

        //>根据操作类型做提示或者其他逻辑
        switch (msgHead.operateType)
        {
        case ItemOperate_ItemUsed:
            {
                g_UIManager->HideUI(UI_ITEM_INFO);
                g_UIManager->HideUI(UI_WAITTING);
                const TextInfo *pText = g_CSVFileManager->GetTextByID(629);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //物品使用成功!
                if (g_UIManager->IsOpened(UI_BAG))
                {
                    UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
                    uiBag->SetPage(uiBag->GetPage());
                }
                return;
            }
            break;
        case ItemOperate_BuyMallItem:
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(627);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //购买成功!
                g_UIManager->HideUI(UI_WAITTING);
            }break;
        case ItemOperate_SellBagItem:
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(630);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //出售成功!
                g_UIManager->HideUI(UI_ITEM_INFO);
                g_UIManager->HideUI(UI_WAITTING);
                if (g_UIManager->IsOpened(UI_BAG))
                {
                    UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
                    uiBag->SetPage(uiBag->GetPage());
                }
            }break;
        case ItemOperate_EquipUpLevel:
            {
                CliItemInfo* pItem = bag.GetItemByGuid(data.guid);
                if (pItem->CanUpStar())
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(631);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //升星成功!
                    g_UIManager->HideUI(UI_WAITTING);
                    if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
                    {
                        UIUpgradeEquip* uiUpgrade = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
                        uiUpgrade->OnUpStarSuccess();
                    }
                }   
            }break;
        case ItemOperate_EquipInherite: 
            {
                CliItemInfo* pItem = bag.GetItemByGuid(data.guid);
                if (pItem->Data.starNum != 0)
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(634);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //传承成功!
                    g_UIManager->HideUI(UI_WAITTING);
                    if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
                    {
                        UIUpgradeEquip* uiUpgrade = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
                        uiUpgrade->OnInheritSuccess();
                    }
                }
            }break;
        default:
            break;
        }
    }
}

void NetManager::OnEquipList(MsgBase* msg)
{
    MsgEquip_List* pEquipList = static_cast<MsgEquip_List*>(msg);
    Bag& bag = g_LogicManager->m_Hero->m_Bag;

    CliItemInfo* addList[EQUIPTYPE_MAX];
    CliItemInfo* pItem;
    int faceId[EQUIPTYPE_MAX];
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if (pEquipList->equipments[i].guid != INVALID_ID)
        {
            pItem = new CliItemInfo();
            if (pItem != NULL)
            {
                //>取得物品信息
                memcpy(&pItem->Data , &pEquipList->equipments[i], sizeof(ItemInfoIt));
                pItem->pCfg = g_CSVFileManager->GetItemsByID(pItem->Data.id);
                assert(pItem->pCfg);
                faceId[i] = pItem->pCfg->iFaceID;
            }
            addList[i] = pItem;
        }
        else
        {
            addList[i] = NULL;
            faceId[i] = 0;
        }
    }

    bag.LoadEquip(addList);
    g_LogicManager->m_Hero->ReloadFace(faceId);
}

void NetManager::OnEquipUpdate(MsgBase* msg)
{
    MsgEquip_Update* updateMsg = (MsgEquip_Update*)msg;
    Bag& bag = g_LogicManager->m_Hero->m_Bag;
    if (updateMsg->equipment.guid != 0)
    {
        EquipType type = (EquipType)updateMsg->equipmentType;
        CliItemInfo* pItem = bag.GetEquipment(type);
        if (pItem == NULL){
            pItem = new CliItemInfo();
            pItem->pCfg = g_CSVFileManager->GetItemsByID(updateMsg->equipment.id);
            assert(pItem->pCfg);
        }
        memcpy(&pItem->Data , &updateMsg->equipment, sizeof(ItemInfoIt));
        bag.WearEquipment((EquipType)updateMsg->equipmentType, pItem);
    }
    else
    {
        EquipType type = (EquipType)updateMsg->equipmentType;
        CliItemInfo* pDeleteItem = bag.GetEquipment(type);
        if (pDeleteItem){
            delete pDeleteItem; //>这里要删除原始装备
        }
        bag.TakeoffEquipment(type);
    }

    if (updateMsg->operateType == ItemOperate_WearEquipment)
    {
        const TextInfo *pText = g_CSVFileManager->GetTextByID(633);
        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //装备成功!
        if (g_UIManager->IsOpened(UI_BAG))
        {
            UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
            uiBag->OnWearSuccess();
        }   
    }
    else if (updateMsg->operateType == ItemOperate_EquipGetOff)
    {
        if (g_UIManager->IsOpened(UI_BAG))
        {
            UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
            uiBag->OnTakeOffSuccess();
        }
    }
    else if (updateMsg->operateType == ItemOperate_EquipUpLevel)
    {
        const TextInfo *pText = g_CSVFileManager->GetTextByID(631);
        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //升星成功!
        g_UIManager->HideUI(UI_WAITTING);
        if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
        {
            UIUpgradeEquip* uiUpgrade = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
            uiUpgrade->OnUpStarSuccess();
        }
    }
    else if (updateMsg->operateType == ItemOperate_ItemMerge)
    {
        if (updateMsg->equipment.guid != 0)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(628);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //合成成功!
            g_UIManager->HideUI(UI_WAITTING);
            if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
            {
                UIUpgradeEquip* uiUpgrade = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
                uiUpgrade->SetPage(uiUpgrade->GetPage());
            }
        }
    }
    else if (updateMsg->operateType == ItemOperate_SellBagItem)
    {
        const TextInfo *pText = g_CSVFileManager->GetTextByID(630);
        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //出售成功!
        g_UIManager->HideUI(UI_ITEM_INFO);
        g_UIManager->HideUI(UI_WAITTING);
        if (g_UIManager->IsOpened(UI_BAG))
        {
            UIBag* uiBag = static_cast<UIBag*>(g_UIManager->GetUI(UI_BAG));
            uiBag->OnTakeOffSuccess();
        }
    }
    else if (updateMsg->operateType == ItemOperate_EquipInherite)
    {
        if (updateMsg->equipment.starNum != 0)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(634);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA);  //传承成功!
            g_UIManager->HideUI(UI_WAITTING);
            if (g_UIManager->IsOpened(UI_EQUIP_UPGRADE))
            {
                UIUpgradeEquip* uiUpgrade = static_cast<UIUpgradeEquip*>(g_UIManager->GetUI(UI_EQUIP_UPGRADE));
                uiUpgrade->OnInheritSuccess();
            }
        }
    }
}

void NetManager::OnMonsterEnterView(MsgBase* msg)
{
    MsgMonster_Baseinfo* pMsg = static_cast<MsgMonster_Baseinfo*>(msg);
    g_LogicManager->AddMonsterToMap(pMsg);
}

void NetManager::OnMonsterLeaveView(MsgBase* msg)
{
    MsgMonster_Del* pMsg = static_cast<MsgMonster_Del*>(msg);
    Monster* pMonster = g_LogicManager->GetMonsterByGUID(pMsg->MonsterGuid);
    if (pMonster)
    {
        g_LogicManager->RemoveMonster(pMonster);
    }
}

void NetManager::OnMonsterGoto(MsgBase* msg)
{
	//MsgMonster_Goto* pMsg = (MsgMonster_Goto*)msg;
	//Monster* pMonster = g_LogicManager->GetMonsterByGUID(pMsg->MonsterGuid);
	//if (pMonster)
	//{
	//	//!特效
	//	pMonster->SetMapPosition(Vec2(pMsg->posX, pMsg->posY));
	//}
}

void NetManager::OnMonsterMoving(MsgBase* msg)
{ 
	//StackStream<> moveStream(msg, msg->Len);
	//MsgMonster_Move moveMsg;
	//moveStream.Read(&moveMsg, sizeof(moveMsg));
	//Monster* pMonster = g_LogicManager->GetMonsterByGUID(moveMsg.MonsterGuid);
	//if (pMonster)
	//{		
	//	pMonster->m_Paths.clear();
	//	pMonster->m_Paths.push_back(g_SceneManager->m_MainLayer->TileId2Tile(moveMsg.MovePath));
	//	int posId;
	//	for (int i = 1; i < moveMsg.PathNum; i++)
	//	{
	//		moveStream.Read(&posId, sizeof(posId));
	//		pMonster->m_Paths.push_back(g_SceneManager->m_MainLayer->TileId2Tile(posId));
	//	}
 //       pMonster->m_MoveTarget = g_SceneManager->m_MainLayer->Tile2Pos(pMonster->m_Paths[0]);
	//	pMonster->MoveBegin();
	//}
}

//获得好友通知
void NetManager::OnRecvFriendsNotify(MsgBase* msg)
{
	MsgFriNotify* temp =  (MsgFriNotify*)msg;
	switch (temp->Type)
	{
	case Notify_S2C_GETFRILIST_OK:			//获取好友列表成功
		{
			g_LogicManager->m_Hero->m_mapFriendsInfo.clear();

			friendInfo Tempfriend;
			StackStream<4096> RoleDetailsStream(temp, temp->Len);
			MsgFriNotify msgHead(ID_G2C, Notify_S2C_GETFRILIST_OK,0,0);
			RoleDetailsStream.Read(&msgHead, sizeof(msgHead));
			for(int i=0;i<msgHead.iNumFriend;i++)
			{
				RoleDetailsStream.Read(&Tempfriend.iRoleID,sizeof(int64));
				RoleDetailsStream.Read(&Tempfriend.szName,MAX_NAME_SIZE);
				RoleDetailsStream.Read(&Tempfriend.bIsLine,sizeof(bool));
				RoleDetailsStream.Read(&Tempfriend.iFight,sizeof(int));
				RoleDetailsStream.Read(&Tempfriend.iLevel,sizeof(int));
				g_LogicManager->m_Hero->m_mapFriendsInfo.insert(pair<int64,friendInfo>(Tempfriend.iRoleID,Tempfriend));
			}
			//更新至界面
			UIFriends *ui = (UIFriends*)g_UIManager->GetUI(UI_FRIENDS);
			ui->UpdateFriendList();
		}
		break;
	case Notify_S2C_GETBLACKLIST_OK:		//获取黑名单列表成功
		{
			g_LogicManager->m_Hero->m_mapBlackList.clear();

			friendInfo Tempfriend;
			StackStream<4096> RoleDetailsStream(temp, temp->Len);
			MsgFriNotify msgHead(ID_G2C, Notify_S2C_GETBLACKLIST_OK,0,0);
			RoleDetailsStream.Read(&msgHead, sizeof(msgHead));
			for(int i=0;i<msgHead.iNumFriend;i++)
			{
				RoleDetailsStream.Read(&Tempfriend.iRoleID,sizeof(int64));
				RoleDetailsStream.Read(&Tempfriend.szName,MAX_NAME_SIZE);
				RoleDetailsStream.Read(&Tempfriend.bIsLine,sizeof(bool));
				RoleDetailsStream.Read(&Tempfriend.iFight,sizeof(int));
				RoleDetailsStream.Read(&Tempfriend.iLevel,sizeof(int));
				g_LogicManager->m_Hero->m_mapBlackList.insert(pair<int64,friendInfo>(Tempfriend.iRoleID,Tempfriend));
			}
			//更新至界面
			UIFriends *ui = (UIFriends*)g_UIManager->GetUI(UI_FRIENDS);
			ui->UpdateBlackList();
		}
		break;
	case Notify_S2C_GETFRI_OK:				//获取好友信息成功
		{
			UIFriends *ui = (UIFriends*)g_UIManager->GetUI(UI_FRIENDS);

			StackStream<1024> RoleDetailsStream(temp, temp->Len);
			MsgFriNotify msgHead(ID_G2C, Notify_S2C_GETFRILIST_OK,0,0);
			RoleDetailsStream.Read(&msgHead, sizeof(msgHead));
			RoleDetailsStream.Read(&ui->m_FindFriend.iRoleID,sizeof(int64));
			RoleDetailsStream.Read(&ui->m_FindFriend.szName,MAX_NAME_SIZE);
			RoleDetailsStream.Read(&ui->m_FindFriend.iLevel,sizeof(int));
			RoleDetailsStream.Read(&ui->m_FindFriend.iFight,sizeof(int));
			ui->m_FindFriend.bIsLine = 1;
			//更新至界面
			ui->UpdateSearch();
		}
		break;
	case Notify_S2C_FRINULL:				//获取好友信息失败
		{
			g_UIManager->ShowMsgBox(700);
			UIFriends *ui = (UIFriends*)g_UIManager->GetUI(UI_FRIENDS);
			ui->m_FindFriend.iRoleID = 0;
			ui->UpdateSearch();
		}
		break;
	case Notify_S2C_ADDFRI_FAILURE:			//添加好友失败，重复添加
		{
			g_UIManager->ShowMsgBox(701);
		}
		break;
	case Notify_S2C_ADDFRI_OK:				//添加好友成功
		{
			g_UIManager->ShowMsgBox(702);
		}
		break;
	case Notify_S2C_ADDEDFRI_OK:			//被添加好友成功
		{
			char szName[MAX_NAME_SIZE];

			StackStream<1024> RoleDetailsStream(temp, temp->Len);
			MsgFriNotify msgHead(ID_G2C, Notify_S2C_ADDEDFRI_OK,0,0);
			RoleDetailsStream.Read(&msgHead, sizeof(msgHead));
			RoleDetailsStream.Read(&szName,MAX_NAME_SIZE);

			const TextInfo* textinfo = g_CSVFileManager->GetTextByID(703);
			string strTemp = szName;
			strTemp = strTemp + textinfo->strContent;

			g_UIManager->ShowMsgBox(strTemp.c_str(),false,MSGBOX_EVENT_ADDFRIEND,MSGBOX_STYLE_CONFIRMANDCANCEL,(long long)msgHead.iRoleID);
		}
		break;
	case Notify_S2C_ADDBLACK_OK:			//添加好友失败，重复添加
		{
			g_UIManager->ShowMsgBox(704);
		}
		break;
	case Notify_S2C_ADDBLACK_FAILURE:		//添加好友成功
		{
			g_UIManager->ShowMsgBox(705);
		}
		break;
	case Notify_S2C_DELFRI_OK:				//删除好友成功
		{
			g_UIManager->ShowMsgBox(706);

 			MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,0, Relation_Friends,"");	
 			Msg.MsgID = Msg_Get_FriendsList;
 			g_NetManager->SendMsg(Msg);
		}
		break;
	case Notify_S2C_DELBLACK_OK:			//删除黑名单成功
		{
			g_UIManager->ShowMsgBox(706);

			MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,0, Relation_BlackList,"");	
			Msg.MsgID = Msg_Get_FriendsList;
			g_NetManager->SendMsg(Msg);
		}
		break;
	}
}

int skillSort(const SkillInfoIt& a, const SkillInfoIt& b)
{
    return a.skillType < b.skillType;
}

void NetManager::OnSkillList(MsgBase* msg)
{
    MsgSkill_List* pListMsg = static_cast<MsgSkill_List*>(msg);
    Hero* pHero = g_LogicManager->m_Hero;
    vector<SkillInfoIt>& skillList = pHero->m_SkillList;

    skillList.clear();

    StackStream<> listStream(pListMsg, pListMsg->Len);
    MsgSkill_List listHeader;
    SkillInfoIt addSkill;
    listStream.Read(&listHeader, sizeof(listHeader));
    pHero->m_ActivateSkillType = 0;
    for (int i = 0; i < MAX_CHOSESKILL; ++i)
    {
        pHero->m_ChoseSkillList[i] = 0;
    }
    for (uint32 i = 0; i < listHeader.numberSkillTree; ++i)
    {
        listStream.Read(&addSkill.skillType, sizeof(addSkill.skillType)); 
        listStream.Read(&addSkill.level, sizeof(addSkill.level));
        skillList.push_back(addSkill);
    }

    std::sort(skillList.begin(), skillList.end(), skillSort);
}

void NetManager::OnAddSkill(MsgBase* msg)
{
    MsgSkill_Add* pAddMsg = static_cast<MsgSkill_Add*>(msg);
    vector<SkillInfoIt>& skillList = g_LogicManager->m_Hero->m_SkillList;
    int count = skillList.size();
    for(int i = 0; i < count; ++i)
    {
        if (skillList[i].skillType == pAddMsg->addSkill.skillType)
        {
            CCASSERT(0, "duplicate add skilltree.");
            return;
        }
    }
    
    skillList.push_back(pAddMsg->addSkill);
    //更新ui
    //UIGongFa* uiGongFa = static_cast<UIGongFa*>(g_UIManager->GetUI(UI_GONGFA));
    //if (uiGongFa)
    //{
    //    uiGongFa->RelaodSkillTree();
    //}
}

void NetManager::OnUpdateSkill(MsgBase* msg)
{
    MsgSkill_Update* pUpdateMsg = static_cast<MsgSkill_Update*>(msg);
    vector<SkillInfoIt>& skillList = g_LogicManager->m_Hero->m_SkillList;

    bool find = false;
    int count = skillList.size();
    int idx = 0;
    for(; idx < count; ++idx)
    {
        if (skillList[idx].skillType == pUpdateMsg->updateSkill.skillType)
        {
            find = true;
            break;
        }
    }

    if (find)
    {
        SkillInfoIt& updateTree = skillList[idx];
        updateTree.level = pUpdateMsg->updateSkill.level;

        const TextInfo *pText = g_CSVFileManager->GetTextByID(650);
        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::GREEN); //技能树已升级!
    }
    else
    {
        CCASSERT(0, "can't find skilltree.");
        return;
    }
}

//获得任务通知
void NetManager::OnRecvTaskNotify(MsgBase* msg)
{
	MsgFriNotify* temp =  (MsgFriNotify*)msg;
	switch (temp->Type)
	{
	case Notify_S2C_GetTaskList_OK:			//获取任务列表成功
		{
			g_LogicManager->m_Hero->m_vecTaskList.clear();

			StackStream<2048> ListStream(temp, temp->Len);
			MsgTaskNotify msgHead(ID_G2C,Notify_S2C_GetTaskList_OK,0,0);
			ListStream.Read(&msgHead,sizeof(msgHead));
			for (int i = 0 ; i < msgHead.iNum; i ++)
			{
				TaskInfoIt taskTemp;
				ListStream.Read(&taskTemp.id,sizeof(int));
				ListStream.Read(&taskTemp.progress,sizeof(int));
				ListStream.Read(&taskTemp.state,sizeof(int));
				g_LogicManager->m_Hero->m_vecTaskList.push_back(taskTemp);
			}

			//更新至界面
			UITask *ui = (UITask*)g_UIManager->GetUI(UI_TASK);
			if (ui)
				ui->UpdateTaskList();

			//更新至主界面
			UIMain *mainui = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			if (mainui)
				mainui->UpdateTask();
		}
		break;
	case Notify_S2C_TakeTask_OK:
		{
			//领取任务成功
			//g_UIManager->ShowMsgBox(725);
			//const TextInfo *TempText = g_CSVFileManager->GetTextByID(725);
			//g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));

			//MsgTaskNotify *ttt = (MsgTaskNotify*)msg;
			//const TaskInfo *TempTask = g_CSVFileManager->GetTaskByID(ttt->taskId);
			//TaskInfoIt NewTaskIt;
			//NewTaskIt.id = ttt->taskId;
			//NewTaskIt.progress = ttt->iNum;
			//NewTaskIt.state = TASK_STATE_HAVETASK;
			//if (NewTaskIt.progress == TempTask->iTermsParam[1])
			//	NewTaskIt.state = TASK_STATE_SUBMITTASK;
			//g_LogicManager->m_Hero->m_vecTaskList.push_back(NewTaskIt);

			////更新至界面
			//UITask *ui = (UITask*)g_UIManager->GetUI(UI_TASK);
			//if (ui)
			//	ui->UpdateTaskList();

			////更新至主界面
			//UIMain *mainui = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			//if (mainui)
			//	mainui->UpdateTask();

			////领取任务后改变NPC头顶图标
			////领取 TempTask->iTaskNpcID 需要改变
			//NPC *npcTemp = g_LogicManager->GetNPCByID(TempTask->iTaskNpcID);
			//if (npcTemp)
			//	npcTemp->SetTaskState(0);
			////交付 TempTask->iReferNpcID
			//if (NewTaskIt.state == TASK_STATE_SUBMITTASK)  //需要改变
			//{
			//	npcTemp = g_LogicManager->GetNPCByID(TempTask->iReferNpcID);
			//	if (npcTemp)
			//		npcTemp->SetTaskState(2);
			//}
		}
		break;
	case Notify_S2C_TakeTask_LevelError:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(726);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(726);
		break;
	case Notify_S2C_TakeTask_RealmError:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(727);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(727);
		break;
	case Notify_S2C_TakeTask_TaskExists:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(728);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(728);
		break;
	case Notify_S2C_TakeTask_ReqTaskError:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(729);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(729);
		break;
	case Notify_S2C_TaskFinish:
		{
			//任务完成可提交
			//g_UIManager->ShowMsgBox(730);
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(730);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));

			MsgTaskNotify *mtn = (MsgTaskNotify*)msg;
			const TaskInfo *TempTask = g_CSVFileManager->GetTaskByID(mtn->taskId);
			TaskInfoIt *TempTaskIt = g_LogicManager->m_Hero->GetTaskInfoItByID(mtn->taskId);
			if (TempTaskIt)
			{
				TempTaskIt->progress = mtn->iNum;
				const TaskInfo *TempTask = g_CSVFileManager->GetTaskByID(mtn->taskId);
				if (TempTaskIt->progress == TempTask->iTermsParam[1])
				{
					TempTaskIt->state = TASK_STATE_SUBMITTASK;
				}
			}
			//更新至主界面
			UIMain *mainui = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			if (mainui)
				mainui->UpdateTask();

			////只修改需要交付任务的NPC
			//NPC *npcTemp = g_LogicManager->GetNPCByID(TempTask->iReferNpcID);
			//if (npcTemp)
			//	npcTemp->SetTaskState(2);
		}
		break;
	case Notify_S2C_TakeTask_NoTask:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(731);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(731);
		break;
	case Notify_S2C_TakeTask_ItemError:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(732);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(732);
		break;
	case NotiFy_S2C_Task_BagFull:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(733);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(733);
		break;
	case Notify_S2C_MakeTask_ItemError:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(732);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(732);
		break;
	case Notify_S2C_SubmitTaskError:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(734);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		//g_UIManager->ShowMsgBox(734);
		break;
	case Notify_S2C_SubmitTaskFinish:
		{
			//提交任务成功
			//g_UIManager->ShowMsgBox(735);
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(735);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));

			MsgTaskNotify *ttt = (MsgTaskNotify*)msg;
			const TaskInfo *TempTask = g_CSVFileManager->GetTaskByID(ttt->taskId);
			TaskInfoIt *TempTaskIt = g_LogicManager->m_Hero->GetTaskInfoItByID(ttt->taskId);
			if (TempTaskIt)
				TempTaskIt->state = TASK_STATE_FINISHTASK;

			//更新至界面
			UITask *ui = (UITask*)g_UIManager->GetUI(UI_TASK);
			if (ui)
				ui->UpdateTaskList();

			//更新至主界面
			UIMain *mainui = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			if (mainui)
				mainui->UpdateTask();


			//交付任务后改变NPC头顶图标
			//交付 TempTask->iReferNpcID 需要改变
			//NPC *npcTemp = g_LogicManager->GetNPCByID(TempTask->iReferNpcID);
			//if (npcTemp)
			//	npcTemp->SetTaskState(0);
			////领取
			//int32 iNpcid = 0;
			//map<int32,TaskInfo>::iterator itt;
			//for(itt = g_CSVFileManager->m_TaskInfoMap.begin(); itt != g_CSVFileManager->m_TaskInfoMap.end() ; itt++)
			//{
			//	TaskInfoIt *TestTaskIt = g_LogicManager->m_Hero->GetTaskInfoItByID(itt->first);
			//	if (TestTaskIt)
			//	{
			//		if (TestTaskIt->state != TASK_STATE_NOTASK)
			//			continue;
			//		//检测等级
			//		if (g_LogicManager->m_Hero->m_Level >= itt->second.iLevel)
			//		{
			//			//检测境界
			//			if (g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_REALMLEVEL] >= itt->second.iRealmID)
			//			{
			//				//检测前置任务是否有前置任务
			//				if (itt->second.iReqTaskID == 0)
			//				{
			//					iNpcid = itt->second.iTaskNpcID;
			//				}
			//				else
			//				{
			//					TaskInfoIt *TestTaskItEx = g_LogicManager->m_Hero->GetTaskInfoItByID(itt->second.iReqTaskID);
			//					if (TestTaskItEx)
			//					{
			//						if (TestTaskItEx->state == TASK_STATE_FINISHTASK)
			//						{
			//							iNpcid = itt->second.iTaskNpcID;
			//						}
			//					}
			//				}
			//			}
			//		}
			//		
			//	}
			//}
			//npcTemp = g_LogicManager->GetNPCByID(TempTask->iReferNpcID);
			//if (npcTemp)
			//	npcTemp->SetTaskState(1);

		}
		break;
	case Notify_S2C_MakeTask_OK:
		{
			MsgTaskNotify *mtn = (MsgTaskNotify*)msg;
			TaskInfoIt *TempTaskIt = g_LogicManager->m_Hero->GetTaskInfoItByID(mtn->taskId);
			if (TempTaskIt)
			{
				TempTaskIt->progress = mtn->iNum;
			}
			//更新至主界面
			UIMain *mainui = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			if (mainui)
				mainui->UpdateTask();
		}
	}
}

void NetManager::OnChatMsg(MsgBase* msg)
{
    MsgChat* pChatMsg = static_cast<MsgChat*>(msg);

    StackStream<> chatMsgStream(pChatMsg, pChatMsg->Len);
    MsgChat chatMsg;
    chatMsgStream.Read(&chatMsg, sizeof(chatMsg));
    std::string content;
    if (chatMsg.msgSize > 0)
    {
        content.resize(chatMsg.msgSize);
        chatMsgStream.Read((void*)content.data(), chatMsg.msgSize);
    }

    ChatMessage* showMsg = new ChatMessage();
    showMsg->channel = (Chat_Channel)chatMsg.iChannel;
    showMsg->senderId = chatMsg.iSendRoleID;
    showMsg->senderName.assign(chatMsg.SenderName);
    showMsg->senderName.append(":");
    showMsg->msg.assign(content);
    int itemGuid;
    for (int i = 0; i < chatMsg.itemCount; ++i)
    {
        chatMsgStream.Read(&itemGuid, sizeof(itemGuid));
        showMsg->appendix.push_back(itemGuid);
    }
    
    g_ChatSystem.AddMessage(showMsg);
}

void NetManager::OnTeamApply(MsgBase* msg)
{
    MsgTeam_Apply* pApplyMsg = static_cast<MsgTeam_Apply*>(msg);
    bool rs = pApplyMsg->OBJID == g_LogicManager->m_Hero->m_GUID;
    CCASSERT(rs, "");
    if (rs)
    {
        if (m_LastApplyMsgBox && m_LastApplyMsgBox->getParent() != NULL)
        {
            m_LastApplyMsgBox->removeFromParent();
            m_LastApplyMsgBox = NULL;
        }

        if (g_LogicManager->m_Hero->HasTeam())
        {
            std::string msg = StringUtils::format("%s(等级:%d)申请加入您的队伍.", pApplyMsg->szRequesterName, pApplyMsg->iRequesterlevel); 
            m_LastApplyMsgBox = g_UIManager->ShowMsgBox(msg.c_str(), false, MSGBOX_EVENT_TEAM_APPLAY, MSGBOX_STYLE_CONFIRMANDCANCEL);
        }
        else
        {
            std::string msg = StringUtils::format("%s(等级:%d)邀请您加入他的队伍.", pApplyMsg->szRequesterName, pApplyMsg->iRequesterlevel); 
            m_LastApplyMsgBox = g_UIManager->ShowMsgBox(msg.c_str(), false, MSGBOX_EVENT_TEAM_APPLAY, MSGBOX_STYLE_CONFIRMANDCANCEL);
        }

        memcpy(m_LastTeamInfo, pApplyMsg, sizeof(MsgTeam_Apply));
    }
}

void NetManager::OnTeamReplyApply(MsgBase* msg)
{
    MsgTeam_ReplyApply* pReplyMsg = static_cast<MsgTeam_ReplyApply*>(msg);
    switch (pReplyMsg->iResult)
    {
    case 1:
        {
            //>要求组队的玩家已经离线
            const TextInfo *pText = g_CSVFileManager->GetTextByID(735);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //组队失败,玩家已离线!
            break;
        }
    case 2:
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(642);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //组队失败,对方已有队伍!
            break;
        }
    case 3:
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(664);
            std::string msg = StringUtils::format(pText->strContent.c_str(), pReplyMsg->szPartnerName); //组队失败,[%s]拒绝加入您的队伍!
            g_UIManager->Notify(BaseFunction::a2u(msg), Color3B::RED);
            break;
        }
    case 4:
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(665);
            std::string msg = StringUtils::format(pText->strContent.c_str(), pReplyMsg->szPartnerName); //组队失败,队长[%s]拒绝您加入!
            g_UIManager->Notify(BaseFunction::a2u(msg), Color3B::RED);
            break;
        }
    default:
        break;
    }
}

void NetManager::OnTeamCreated(MsgBase* msg)
{
    const TextInfo *pText = g_CSVFileManager->GetTextByID(644);
    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //队伍创建成功!
    MsgTeam_Created* pCreateMsg = static_cast<MsgTeam_Created*>(msg);

    StackStream<> createStream(pCreateMsg, pCreateMsg->Len);
    MsgTeam_Created createMsg;
    createStream.Read(&createMsg, sizeof(MsgTeam_Created));

    int count = createMsg.memberNumber;
    std::vector<Hero::Team::Member>& teamMembers = g_LogicManager->m_Hero->m_team.m_Members;
    teamMembers.resize(count);
    for (int i = 0; i < count; ++i)
    {
        Hero::Team::Member& member = teamMembers[i];
        createStream.Read(&member.memberId, sizeof(member.memberId));
        createStream.Read(&member.attribute, sizeof(member.attribute));
        createStream.Read(&member.szName, sizeof(char)*MAX_NAME_SIZE);
        createStream.Read(&member.memberLevel, sizeof(member.memberLevel));
        createStream.Read(&member.Hp, sizeof(member.Hp));
        createStream.Read(&member.MaxHp, sizeof(member.MaxHp));
    }

    UIMain *puiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
    if (puiMain)
        puiMain->LoadTeamInfo();
}

void NetManager::OnTeamNewMember(MsgBase* msg)
{
    const TextInfo *pText = g_CSVFileManager->GetTextByID(645);
    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //新队员加入
    MsgTeam_NewMember* pNewMemberMsg = static_cast<MsgTeam_NewMember*>(msg);

    std::vector<Hero::Team::Member>& teamMembers = g_LogicManager->m_Hero->m_team.m_Members;
    int newIdx = teamMembers.size();
    teamMembers.resize(newIdx + 1);

    teamMembers[newIdx].memberId = pNewMemberMsg->memberId;
    teamMembers[newIdx].attribute = (Hero::Team::MemberAttribute)pNewMemberMsg->memberAttribute;
    strncpy(teamMembers[newIdx].szName, pNewMemberMsg->szName, MAX_NAME_SIZE);
    teamMembers[newIdx].memberLevel = pNewMemberMsg->memberLevel;
    teamMembers[newIdx].Hp = pNewMemberMsg->Hp;
    teamMembers[newIdx].MaxHp = pNewMemberMsg->MaxHp;

    UIMain *puiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
    if (puiMain)
        puiMain->LoadTeamInfo();
}

void NetManager::OnTeamMemberExit(MsgBase* msg)
{
    const TextInfo *pText = g_CSVFileManager->GetTextByID(646);
    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //队员离开
    MsgTeam_MemberExit* pExitMsg = static_cast<MsgTeam_MemberExit*>(msg);
    if (g_LogicManager->m_Hero)
    {
        std::vector<Hero::Team::Member>& teamMembers = g_LogicManager->m_Hero->m_team.m_Members;
        if (pExitMsg->memberId == g_LogicManager->m_Hero->m_GUID)
        {
            teamMembers.clear();
        }
        else
        {
            auto it = teamMembers.begin();
            for (auto end = teamMembers.end(); it != end; ++it)
            {
                if(it->memberId == pExitMsg->memberId)
                {
                    teamMembers.erase(it);
                    break;
                }
            }
        }

        UIMain *puiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
        if (puiMain)
            puiMain->LoadTeamInfo();
    }
}

void NetManager::OnTeamChangeLeader(MsgBase* msg)
{
    g_UIManager->Notify(BaseFunction::a2u("队长已变更!"), Color3B::MAGENTA);
    MsgTeam_ChangeLeader* pChangeMsg = static_cast<MsgTeam_ChangeLeader*>(msg);
    if (g_LogicManager->m_Hero)
    {
        std::vector<Hero::Team::Member>& teamMembers = g_LogicManager->m_Hero->m_team.m_Members;
        auto it = teamMembers.begin();
        for (auto end = teamMembers.end(); it != end; ++it)
        {
            if(it->memberId == pChangeMsg->memberId)
            {
                it->attribute = (Hero::Team::MemberAttribute)pChangeMsg->memberAttribute;
            }
            else if (it->attribute == Hero::Team::MA_Leaber)
            {
                it->attribute = Hero::Team::MA_None;
            }
        }

        UIMain *puiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
        if (puiMain)
            puiMain->LoadTeamInfo();
    }
}

//获得工会通知
void NetManager::OnRecvGangNotify(MsgBase* msg)
{
	MsgGangNotify* temp =  (MsgGangNotify*)msg;
	switch (temp->Type)
	{
	case Notity_S2C_Create_Fail_RepeatName:			//创建失败，帮派名重复
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(810);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_Create_Fail_InGang:				//创建失败，已在帮派中
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(811);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_Create_OK:						//创建成功
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(812);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));

			g_LogicManager->m_Hero->m_isocietyID = temp->iPar[0];
		}
		break;
	case Notity_S2C_Get_GangInfo:
		{
			StackStream<4096> ListStream(temp, temp->Len);
			MsgGangNotify msgHead(ID_G2C,Notify_S2C_GetTaskList_OK,0,0,0,"");
			ListStream.Read(&msgHead,sizeof(msgHead));

			g_LogicManager->m_Hero->m_GangInfo.m_mapMembers.clear();
			g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.clear();
			g_LogicManager->m_Hero->m_GangInfo.m_vecGangLog.clear();

			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_id,sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_szName,32);
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iLevel,sizeof(int32));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iShopLevel,sizeof(int32));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iHeaderid,sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_szHeaderName,32);
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iAct,sizeof(int32));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_szAccounce,255);
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iViveHeaderid[0],sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iViveHeaderid[1],sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iStarFlec[0],sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iStarFlec[1],sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iStarFlec[2],sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iStarFlec[3],sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iStarFlec[4],sizeof(int64));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_szCreateTime,50);
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_iCopyid,sizeof(int32));
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_szStartTime,50);
			for (int i = 0 ; i < msgHead.iPar[0] ; ++i)
			{
				GangMember tempMember;
				ListStream.Read(&tempMember.id,sizeof(int64));
				ListStream.Read(&tempMember.szName,32);
				ListStream.Read(&tempMember.iLevel,sizeof(int32));
				ListStream.Read(&tempMember.szApplyTime,50);
				ListStream.Read(&tempMember.szPosition,32);
				ListStream.Read(&tempMember.iContribution,sizeof(int32));
				ListStream.Read(&tempMember.szLeaveTime,50);
				g_LogicManager->m_Hero->m_GangInfo.m_mapMembers.insert(pair<int64,GangMember>(tempMember.id,tempMember));
			}
			ListStream.Read(&g_LogicManager->m_Hero->m_GangInfo.m_szBackupInfo,sizeof(255));
			for (int i = 0 ; i < msgHead.iPar[1] ; ++i)
			{
				GangMember tempMember;
				ListStream.Read(&tempMember.id,sizeof(int64));
				ListStream.Read(&tempMember.szName,32);
				ListStream.Read(&tempMember.iLevel,sizeof(int32));
				ListStream.Read(&tempMember.szApplyTime,50);
				ListStream.Read(&tempMember.szPosition,32);
				ListStream.Read(&tempMember.iContribution,sizeof(int32));
				ListStream.Read(&tempMember.szLeaveTime,50);
				g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.insert(pair<int64,GangMember>(tempMember.id,tempMember));
			}
			for (int i = 0 ; i < msgHead.iPar[2] ; ++i)
			{
				GangLogInfo tempGangLog;
				ListStream.Read(&tempGangLog.recordTime,30);
				ListStream.Read(&tempGangLog.recordContent,255);
				g_LogicManager->m_Hero->m_GangInfo.m_vecGangLog.push_back(tempGangLog);
			}

			//更新至主界面
			UIGuild *mainui = (UIGuild*)g_UIManager->GetUI(UI_GUILD);
			if (mainui)
				mainui->UpdateList();
		}
		break;
	case Notity_S2C_GET_GANGLIST:
		{
			g_LogicManager->m_vecGangList.clear();
			StackStream<4096> ListStream(temp, temp->Len);
			MsgGangNotify msgHead(ID_G2C,Notity_S2C_GET_GANGLIST,0,0,0,"");
			ListStream.Read(&msgHead,sizeof(msgHead));
			
			GangSimpleInfo tempGang;
			for (int i = 0 ; i < msgHead.iPar[0] ; ++i)
			{
				ListStream.Read(&tempGang.id,sizeof(int64));
				ListStream.Read(&tempGang.szName,32);
				ListStream.Read(&tempGang.iLevel,sizeof(int32));
				ListStream.Read(&tempGang.szHeaderName,32);
				ListStream.Read(&tempGang.iGangNum,sizeof(int32));
				ListStream.Read(&tempGang.iFight,sizeof(int32));

				g_LogicManager->m_vecGangList.push_back(tempGang);
			}

			//更新至主界面
			UIApplyGang *mainui = (UIApplyGang*)g_UIManager->GetUI(UI_APPLYGANG);
			if (mainui)
				mainui->UpdateList();
		}
		break;
	case Notity_S2C_Apply_OK:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(813);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_Apply_Fail_InGang:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(814);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_Apply_Fail_NoGang:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(815);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_Apply_Fail_Repeat:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(816);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_Reply_Refused:
		{
			char szTemp[50];
			memset(szTemp,0,50);
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(817);
			sprintf(szTemp,"%s%s",temp->szPar,TempText->strContent.c_str());
			g_UIManager->Notify(BaseFunction::a2u(szTemp),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_Reply_OK:
		{
			char szTemp[50];
			memset(szTemp,0,50);
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(818);
			sprintf(szTemp,"%s%s",temp->szPar,TempText->strContent.c_str());
			g_UIManager->Notify(BaseFunction::a2u(szTemp),Color3B(255,255,255));

			g_LogicManager->m_Hero->m_isocietyID = temp->iPar[0];
			
		}
		break;
	case Notity_S2C_Reply_OK_INGang:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(819);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_EDIT_Announce_OK:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(820);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));

			strncpy(g_LogicManager->m_Hero->m_GangInfo.m_szAccounce,temp->szPar, 255);
			//更新至主界面
			UIGuild *mainui = (UIGuild*)g_UIManager->GetUI(UI_GUILD);
			if (mainui)
				mainui->UpdateList();
		}
		break;
	case Notity_S2C_LEAVE_GANG:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(821);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));

			g_LogicManager->m_Hero->m_isocietyID = 0;
		}
		break;
	case Notity_S2C_LEAVE_GANG_HEADER:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(822);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	case Notity_S2C_PUT_GANG:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(823);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));

			g_LogicManager->m_Hero->m_isocietyID = 0;
		}
		break;
	case Notity_S2C_TransferHeader_OK:
		{
			const TextInfo *TempText = g_CSVFileManager->GetTextByID(824);
			g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		}
		break;
	}
}

void NetManager::OnObjBuffList(MsgBase* msg)
{
    StackStream<> listStream(msg, msg->Len);
    MsgObject_BuffList listMsg;
    listStream.Read(&listMsg, sizeof(listMsg));
    if (listMsg.buffNumber > 0)
    {
        MapObj* pObj = NULL;
        if (listMsg.OwnerType == OBJECT_PLAYER)
        {
            pObj = g_LogicManager->GetAllPlayerByGUID(listMsg.OwnerID);
        }
        else if (listMsg.OwnerType == OBJECT_MONSTER)
        {
            pObj= g_LogicManager->GetMonsterByGUID(listMsg.OwnerID);
        }

        if (pObj)
        {
           std::vector<BattleComponent::BuffData> buffListVct;
           buffListVct.resize(listMsg.buffNumber);

           for (int i = 0; i < listMsg.buffNumber; ++i)
           {
               auto& buffData = buffListVct[i];
               listStream.Read(&buffData.buffId, sizeof(buffData.buffId));
               listStream.Read(&buffData.instanceId, sizeof(buffData.instanceId));
               listStream.Read(&buffData.lastTime, sizeof(buffData.lastTime));
               buffData.pBuffInfo = NULL;
               buffData.pUserData = NULL;
           }

           pObj->m_BattleCompoent->InitBuffList(buffListVct);
        }
    }
}

void NetManager::OnObjAddBuff(MsgBase* msg)
{
    MsgObject_AddBuff* pAddBuff = (MsgObject_AddBuff*)msg;
    MapObj* pObj = NULL;
    if (pAddBuff->OwnerType == OBJECT_PLAYER)
    {
        pObj = g_LogicManager->GetAllPlayerByGUID(pAddBuff->OwnerID);
    }
    else if (pAddBuff->OwnerType == OBJECT_MONSTER)
    {
        pObj= g_LogicManager->GetMonsterByGUID(pAddBuff->OwnerID);
    }

    if (pObj)
    {
        pObj->m_BattleCompoent->AddBuff(pAddBuff->buffId, 
                                        pAddBuff->buffInstanceId, 
                                        pAddBuff->lastTime);
    }

}

void NetManager::OnObjDelBuff(MsgBase* msg)
{
    MsgObject_DelBuff* pDelBuff = (MsgObject_DelBuff*)msg;
    MapObj* pObj = NULL;
    if (pDelBuff->OwnerType == OBJECT_PLAYER)
    {
        pObj = g_LogicManager->GetAllPlayerByGUID(pDelBuff->OwnerID);
    }
    else if (pDelBuff->OwnerType == OBJECT_MONSTER)
    {
        pObj= g_LogicManager->GetMonsterByGUID(pDelBuff->OwnerID);
    }

    if (pObj)
    {
        pObj->m_BattleCompoent->RemoveBuff(pDelBuff->buffInstanceId);
    }
}

