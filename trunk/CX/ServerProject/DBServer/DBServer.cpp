#include "DBServer.h"
#include "windows.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "CSVParser.h"
#include "LogManage.h"
#include "StackStream.h"

#pragma comment(lib, "ws2_32.lib")


const char* utf82ansi(const char* utfstr, char* ansistr, int maxlen)
{
	int uLen = (int)strlen(utfstr);
	int wLen = 2*uLen +1;
	WCHAR* wStr = (WCHAR*)malloc(2*wLen);
	int Ret = MultiByteToWideChar(CP_UTF8, 0, utfstr, uLen, wStr, wLen -1);
	wStr[Ret] =0;
	Ret = WideCharToMultiByte(CP_ACP, 0, wStr, Ret, ansistr, maxlen, 0, FALSE);
	ansistr[Ret] = 0;
	free(wStr);
	return ansistr;
}

const char* ansi2utf8(const char* ansistr, char* utfstr, int maxlen)
{
	int uLen = (int)strlen(ansistr);
	int wLen = 2*uLen +1;
	WCHAR* wStr = (WCHAR*)malloc(2*wLen);
	int Ret = MultiByteToWideChar(CP_ACP, 0, ansistr, uLen, wStr, wLen -1);
	wStr[Ret] =0;
	Ret = WideCharToMultiByte(CP_UTF8, 0, wStr, Ret, utfstr, maxlen, 0, FALSE);
	utfstr[Ret] = 0;
	free(wStr);
	return utfstr;
}

void CopyPalyerBaseInfo(BaseInfoIt* baseInfo,SQL_player_info* playerInfo )
{
	baseInfo->playerID=playerInfo->playerID;
	memcpy(baseInfo->playerName,playerInfo->playerName,MAX_NAME_SIZE);
	baseInfo->sex=playerInfo->sex;
	baseInfo->headID=playerInfo->headID;
	baseInfo->offlineTime=playerInfo->offlineTime;
	baseInfo->level=playerInfo->level;
	baseInfo->exp=playerInfo->exp;
	baseInfo->money=playerInfo->money;
	baseInfo->gold=playerInfo->gold;
	baseInfo->bindGold=playerInfo->bindGold;
	baseInfo->rechargGoldSum=playerInfo->rechargGoldSum;
	baseInfo->arenaScore=playerInfo->arenaScore;
	baseInfo->arenaMoney=playerInfo->arenaMoney;
	baseInfo->battlefieldScore=playerInfo->battlefieldScore;
	baseInfo->battlefieldMoney=playerInfo->battlefieldMoney;
	baseInfo->mapID=playerInfo->mapID;
	baseInfo->posx=playerInfo->posx;
	baseInfo->posy=playerInfo->posy;
	baseInfo->hp=playerInfo->hp;
	baseInfo->mp=playerInfo->mp;
	baseInfo->xp=playerInfo->xp;
	baseInfo->societyID=playerInfo->societyID;
	baseInfo->realmLevel=playerInfo->realmLevel;
	baseInfo->realmExp=playerInfo->realmExp;
	baseInfo->activeSkillType = playerInfo->activeSkillType;
}

DBServer::DBServer(void)
{
}


DBServer::~DBServer(void)
{
	Stop();
}

void DBServer::test(int i)
{
	gLog.Info("执行定时器：%d\n", i);
	//g_TimerManage->AddTimer(1, 10000, new Callback<DBServer>(this, &DBServer::test));
}
void DBServer::onUpdatePlayerinfo(int i)
{
	 std::map<uint64, SQL_player_info>::iterator it;
	 for(it=m_PlayerInfo.begin();it!=m_PlayerInfo.end();it++)
	 {
		 if(m_Mysql.UpdatePlayerInfo(&it->second))
		 {
			 gLog.Info("更新玩家[%d]信息成功！",it->first);
		 }
		 else
		 {
			 gLog.Warning("更新玩家[%d]信息失败！",it->first);
		 }
	 }
	 std::map<uint64,SQL_gang_info>::iterator itGang;
	 for(itGang = m_GangInfo.begin();itGang != m_GangInfo.end();itGang++)
	 {
		 if(m_Mysql.UpdateGangInfo(&itGang->second))
		 {
			 gLog.Info("更新帮派[%d]信息成功！",itGang->first);
		 }
		 else
		 {
			 gLog.Warning("更新帮派[%d]信息失败！",itGang->first);
		 }
	 }
	 char strBuf[512] = {0};
	 sprintf(strBuf, "UPDATE guid SET item=%llu", guidInfo.itemId);
	 if(m_Mysql.DbExecute(strBuf,eMysqlUpdate)<0)
	 {
		 gLog.Warning("更新Guid失败");
	 }
	 else
	 {
		 gLog.Info("更新Guid[%llu]成功", guidInfo.itemId);
	 }
	
	 g_TimerManage->AddTimer(1, 20000, new Callback<DBServer>(this, &DBServer::onUpdatePlayerinfo));
	 

}

bool DBServer::Start(uint16 ServerID)
{	
	m_ServerID = ServerID;
	gLog.Info("ServerId:%d\n",ServerID);
//日志设置名字
	gLog.SetName("DBServer");
//读csv游戏服务器配置
	CSVParser csv;
	csv.LoadFile("Config.csv");
	csv.GetLine();	//忽略第一行
	while (csv.GetLine())
	{
		if (csv.GetInt() == m_ServerID)
		{
			csv.GetStr();	//服务器名
			csv.GetInt();	//服务器状态
			m_GameServerIP = csv.GetStr();	//游戏服务器ip
			m_GameServerPort = csv.GetInt();//游戏服务器端口
			csv.GetStr();					//数据库服务器ip
			m_Port = csv.GetInt();			//数据库服务器端口
			m_GameDBName = csv.GetStr();	//游戏数据库名
			m_GameDBIP = csv.GetStr();		//游戏数据库IP
			m_GameDBPort = csv.GetInt();	//游戏数据库端口
			m_GameDBAccount = csv.GetStr();	//游戏数据库账号
			m_GameDBPassword = csv.GetStr();//游戏数据库密码

			m_GameAddress.FromStringExplicitPort(m_GameServerIP.c_str(), m_GameServerPort);
			printf("Start~~~DBServer  Index = %d   Port = %d\n", m_ServerID, m_Port);
			break;
		}
	}	

//初始化网络
	m_Net = RakNet::RakPeerInterface::GetInstance();
	m_Net->SetIncomingPassword(SERVER_CONNECTION_PASSWORD, (int)strlen(SERVER_CONNECTION_PASSWORD));	//设置接入密码
	m_Net->SetOccasionalPing(true);
	m_Net->SetUnreliableTimeout(LINK_TIMEOUT_TIME);	//设置超时时间
	if (m_Net->Startup(DBSERVER_CONNECTIONS_MAX, &RakNet::SocketDescriptor(m_Port,0), 1) != RakNet::RAKNET_STARTED)
	{	
		return false;
	}	
	m_Net->SetMaximumIncomingConnections(DBSERVER_CONNECTIONS_MAX);	//最大接入数量，需小于最大连接数量

//连接到数据库
	if(m_Mysql.Init(m_GameDBIP.c_str(),m_GameDBAccount.c_str(),m_GameDBPassword.c_str(),m_GameDBName.c_str(),m_GameDBPort))
	{
		gLog.Info("OK~~~Link DB\n");		
	}
	else
	{
		gLog.Error("~~~Link DB\n");
		gLog.Info("%s,%s,%s,%s",m_GameDBIP.c_str(),m_GameDBAccount.c_str(),m_GameDBPassword.c_str(),m_GameDBName.c_str());
		return false;
	}
	g_TimerManage = new TimerManage;
	getGuidInfo();
	
//运行主循环接收消息
	g_TimerManage->AddTimer(1, 20000, new Callback<DBServer>(this, &DBServer::onUpdatePlayerinfo));
	m_StopFlag = false;
	Run();
	return true;
}

void DBServer::LoadGangInfo()
{	
	//加载帮派信息
	const char* strSql = "SELECT * FROM ganginfo";
	StoreQueryResult res =m_Mysql.DbQuery(strSql);
	if(!OnFullSelectGangInfoMap(res))
	{
		gLog.Warning("Load GangInfo Error!\n");
		return;
	}
	

	//加载申请帮派信息
	const char* strSql1 = "SELECT * FROM applyinfo";
	StoreQueryResult resApply =m_Mysql.DbQuery(strSql1);
	if(!OnFullSelectApplyInfoMap(resApply))
	{
		gLog.Warning("Load ApplyInfo Error!\n");
		return;
	}

	
	//帮派信息发送给客户端
	map<uint64,SQL_gang_info>::iterator itt = m_GangInfo.begin();
	for (; itt != m_GangInfo.end() ; ++itt)
	{
		
		MsgGang msgGangTemp(itt->first,itt->second.vecMembersList.size(),0,"");
		msgGangTemp.SysID =	ID_D2G; 
		msgGangTemp.MsgID =	Msg_Get_GangInfo;
		StackStream<2048> gangInfoStream;
		gangInfoStream.Wirte(&msgGangTemp,sizeof(MsgGang));

		gangInfoStream.Wirte(&itt->second.id,sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.szGangName,32);
		gangInfoStream.Wirte(&itt->second.iGangLevel,sizeof(__int32));
		gangInfoStream.Wirte(&itt->second.iGangShopLevel,sizeof(__int32));
		gangInfoStream.Wirte(&itt->second.iHeaderid,sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.szHeaderName,32);
		gangInfoStream.Wirte(&itt->second.iGangActive,sizeof(__int32));
		gangInfoStream.Wirte(&itt->second.szGangAnnounce,255);
		gangInfoStream.Wirte(&itt->second.iViceHeaderid[0],sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.iViceHeaderid[1],sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.iStarFlex[0],sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.iStarFlex[1],sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.iStarFlex[2],sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.iStarFlex[3],sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.iStarFlex[4],sizeof(__int64));
		gangInfoStream.Wirte(&itt->second.szCreateTime,50);
		gangInfoStream.Wirte(&itt->second.iCopyid,sizeof(__int32));
		gangInfoStream.Wirte(&itt->second.szCopyStartTime,50);
		vector<GangMember>::iterator it = itt->second.vecMembersList.begin();
		for (; it != itt->second.vecMembersList.end(); ++ it)
		{
			gangInfoStream.Wirte(&it->id,sizeof(int64));
			gangInfoStream.Wirte(&it->szName,32);
			gangInfoStream.Wirte(&it->iLevel,sizeof(int32));
			cout<<(*it).id<<"  "<<(*it).szName<<"  "<<(*it).iLevel<<endl;
		}
		gangInfoStream.Wirte(&itt->second.szBackupInfo,255);

		MsgGang* pAddMsg = (MsgGang*)gangInfoStream.Get();
		pAddMsg->Len = gangInfoStream.GetSize();
		SendToGame(pAddMsg);
	}
	//帮派信息发送给客户端
	map<uint64,SQL_apply_info>::iterator ittex = m_ApplyInfo.begin();
	for (; ittex != m_ApplyInfo.end() ; ++ittex)
	{
		MsgGang msgGangTemp(ittex->first,0,0,"");
		msgGangTemp.SysID =	ID_D2G; 
		msgGangTemp.MsgID =	Msg_Get_ApplyInfo; 
		StackStream<1024> gangInfoStream;
		gangInfoStream.Wirte(&msgGangTemp,sizeof(MsgGang));

		gangInfoStream.Wirte(&ittex->second.id,sizeof(__int32));
		gangInfoStream.Wirte(&ittex->second.iApplyid,sizeof(__int64));
		gangInfoStream.Wirte(&ittex->second.iApplyType,sizeof(__int32));
		gangInfoStream.Wirte(&ittex->second.iTargetid,sizeof(__int64));
		gangInfoStream.Wirte(&ittex->second.szApplyTime,50);
		gangInfoStream.Wirte(&ittex->second.szBackupInfo,255);

		MsgGang* pAddMsg = (MsgGang*)gangInfoStream.Get();
		pAddMsg->Len = gangInfoStream.GetSize();
		SendToGame(pAddMsg);
	}
}

void DBServer::Run()
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
				
				break;
					
			case ID_ALREADY_CONNECTED:	//对方说你是重复的连接
			case ID_CONNECTION_BANNED: // 被禁止连接
			case ID_CONNECTION_ATTEMPT_FAILED:	//连接请求无法完成
			case ID_NO_FREE_INCOMING_CONNECTIONS:	//对方连接已满，不接受新的连接
				{
					gLog.Error("~~~SYSMsgID = %d\n", packet->data[0]);		
				}
				break;
			case ID_DISCONNECTION_NOTIFICATION:	//服务器已经关闭		
			case ID_CONNECTION_LOST:		//对方连接丢失
				gLog.Warning("ID_CONNECTION_LOST %s\n", packet->systemAddress.ToString(true));
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
				gLog.Warning("Ping from %s\n", packet->systemAddress.ToString(true));
				break;
			case ID_G2D:		//游戏服务器的包
				OnRecvGame(packet);	
				break;
			default:
				break;
			}
		}
		//定时器
		g_TimerManage->Run();
				

		Sleep(1);
	}	
}
void DBServer::getGuidInfo()
{
	char *strSql="SELECT * FROM guid ";
	m_Mysql.getGuidInfo(strSql,&guidInfo);
}
bool DBServer::Stop()
{
	m_Net->Shutdown(300);
	RakNet::RakPeerInterface::DestroyInstance(m_Net);
	//m_Mysql.Free();
	m_StopFlag = true;
	return true;
}

void DBServer::OnRecvGame(RakNet::Packet* p)
{	
	MsgBase* msg = (MsgBase*)(p->data);
	if (msg->MsgID >= Msg_MAX)
	{
		gLog.Warning("error Packet id = %d", msg->MsgID);
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
			case Notify_C2S_Login_ServerList:	
				{						
				}
				break;
			default:
				break;
			}
		}
		break;
    case Msg_GetGlobalGuid:
        OnGetGlobalGuid(msg);
        break;
	case Msg_Player_Enter_GameServer:
		OnPlayerEnterGameServer(msg);
		break;
	case Msg_Player_Leave_GameServer:
		OnPlayerLeaveGameServer(msg);
		break;
	case Msg_CreateRole:
		OnCreateRole(msg);
		break;
	case Msg_Add_Friends:
		OnAddFriends(msg);
		break;
	case Msg_Create_Gang:
		OnCreateGang(msg);
		break;
	
    case Msg_Item_Add_Set:
        OnAddItemSet(msg);
        break;
    case Msg_Item_Updata_Set:
        OnUpdateItemSet(msg);
        break;
    case Msg_Item_Del_Set:
        OnDelItemSet(msg);
        break;
	case Msg_Task_Take:
		OnTakeTask(msg);
		break;
	case Msg_Task_Make:
		OnMakeTask(msg);
		break;
	case Msg_Task_Submit:
		OnSubmitTask(msg);
		break;
    case Msg_Equip_Update:
        OnUpdateWearEquipment(msg);
        break;
    case Msg_Skill_Add:
        OnAddSkill(msg);
        break;
    case Msg_SKill_Update:
        OnUpdateSkill(msg);
        break;
    case Msg_Skill_Activate:
        OnActivateSkill(msg);
        break;
    case Msg_Skill_ChoseSkill:
        OnChoseCanUseSkill(msg);
        break;
	case Msg_Role_BaseInfo_Update:
		OnUpdateBaseInfo(msg);
		break;
	case Msg_Get_GangInfo:
		OnGetGang(msg);
		break;
	case Msg_Player_Remove_GameAndDBServer:
		{
			std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
			if(it != m_PlayerInfo.end())
			{
				m_Mysql.UpdatePlayerInfo(&it->second);
				m_PlayerInfo.erase(msg->OBJID);
				gLog.Info("Remove Player:[%d] From DBServer!",msg->OBJID);
			}
		}
		break;
	case Msg_Copy_Finish:
		OnCopyFinish(msg);
		break;
	case Msg_Apply_Copy:
		OnCopyApply(msg);
		break;
	case Msg_Reply_ApplyGang:
		OnApplyGangAgree(msg);
		break;
	case Msg_Edit_GangAnnounce:
		OnEditAnnounce(msg);
		break;
	case Msg_Leave_Gang:
		OnQuitGang(msg);
		break;
	case Msg_Transfer_GangHeader:
		OnChangeGangHeader(msg);
		break;
	default:
		printf("error Packet id = %d", msg->MsgID);
		break;
	}
}

bool DBServer::Send(RakNet::SystemAddress &address, MsgBase *msg)
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

bool DBServer::SendToGame(MsgBase *msg)
{
	msg->SysID = ID_D2G;
	Send(m_GameAddress, msg);
	return true;
}

bool DBServer::SendToClient(MsgBase *msg)
{
	msg->SysID = ID_D2C;
	Send(m_GameAddress, msg);
	return true;
}

void DBServer::OnGetGlobalGuid(MsgBase* msg)
{
    MsgGetGlobalGuid guidMsg;
    guidMsg.ItemGuid = guidInfo.itemId;
    SendToGame(&guidMsg);
}

void DBServer::OnGetGang(MsgBase* msg)
{
	LoadGangInfo();
}

void DBServer::OnPlayerEnterGameServer(MsgBase* msg)
{
	MsgPlayer_Enter_GameServer *temp = (MsgPlayer_Enter_GameServer *)msg;
	std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(temp->AccountID);
	if(it != m_PlayerInfo.end())
	{

		SQL_player_info record = it->second;

		//发送基础信息
		MsgRole_BaseInfo send(temp->AccountID);
		CopyPalyerBaseInfo(&send.BaseInfo,&record);
		SendToGame(&send);
		//发送详细信息
		SendPlayerDetaiInfo(&record);

		//>已有角色,可以进入地图
		MsgNotify notify(ID_D2G, Notify_S2C_RoleExist);
		notify.OBJID = temp->AccountID;
		SendToGame(&notify);
		return;
	}
	//查看角色是否存在
	char strBuf[512] = {0};
	sprintf(strBuf, "SELECT * FROM player WHERE playerID = %llu", temp->AccountID);

	//m_Mysql.DbQuery(strBuf);	
	//m_Mysql.UseResult();
	SQL_player_info record;
	if(m_Mysql.FullSelectPlayerData(strBuf,&record))
	{
		//m_Mysql.ResetResult();
		//查到数据先缓存，再发送到游戏服务器
		m_PlayerInfo[record.playerID] = record;

		//发送基础信息
		MsgRole_BaseInfo send(temp->AccountID);
		CopyPalyerBaseInfo(&send.BaseInfo,&record);
		SendToGame(&send);
        //发送详细信息
		SendPlayerDetaiInfo(&record);

        //>已有角色,可以进入地图
        MsgNotify notify(ID_D2G, Notify_S2C_RoleExist);
        notify.OBJID = temp->AccountID;
        SendToGame(&notify);
	}
	else
	{
		//查不到数据，通知游戏服务器需要创建角色
		MsgNotify ttt(ID_D2G, Notify_S2C_CreateRole);
		ttt.OBJID = temp->AccountID;
		SendToGame(&ttt);
	}
}

void DBServer::OnPlayerLeaveGameServer(MsgBase* msg)
{
	std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
	if (it != m_PlayerInfo.end())
	{
		//!保存、修改离线时间

		m_PlayerInfo.erase(it);
	}
}

void DBServer::OnCreateRole(MsgBase* msg)
{
	MsgCreateRole *temp = (MsgCreateRole *)msg;
	//! 先查缓存如果改账号已有角色，则通知重新登陆

	//查询名字是否可用
	char strBuf[2048] = {0};
	sprintf(strBuf, "SELECT 'playerID' FROM player WHERE playerName = '%s'", temp->Name);
	if (m_Mysql.DbQuery(strBuf).size() > 0)
	{
		MsgNotify ttt(ID_D2C, Notify_S2C_CreateRole_NameRepeat);
		ttt.OBJID = temp->OBJID;
		SendToClient(&ttt);
		return;
	}

	//初始化玩家身上物品ID都为0
	ItemInfoIt equipInfo[EQUIPTYPE_MAX];
	memset(equipInfo,0,sizeof(equipInfo));
	//初始化玩家选择的技能
	int32 choseSkill[MAX_CHOSESKILL] = {0};
	char* strEquIp = m_Mysql.ConvertBinaryToString((const char*)equipInfo,sizeof(equipInfo));
	sprintf(strBuf, "INSERT INTO player (playerID, playerName, sex, headID,equip,task,choseskill) VALUES(%llu, '%s', %d, %d,'%s','", temp->OBJID, temp->Name, temp->Sex, temp->HeadID,strEquIp);	
	std::stringstream strSql;
	strSql<<strBuf;
	TaskInfoIt taskInfo;
	taskInfo.id=1;
	taskInfo.progress = 1;
	taskInfo.state=2;
	StackStream<> taskStream;
	taskStream.Wirte(&taskInfo,sizeof(TaskInfoIt));
	char* strTask = m_Mysql.ConvertBinaryToString((const char*)taskStream.Get(),taskStream.GetSize());
	strSql<<strTask<<"','";
	char* strChoseSkill = m_Mysql.ConvertBinaryToString((const char*)choseSkill,sizeof(choseSkill));
	strSql<<strChoseSkill<<"')";
	//创建新角色到数据库
	if (m_Mysql.DbExecute(strSql.str().c_str(), eMysqlInsert) >= 0)
	{
		//先缓存，再发送到游戏服务器
		SQL_player_info record;
		sprintf(strBuf, "SELECT * FROM player WHERE playerID = %llu", temp->OBJID);
		if(m_Mysql.FullSelectPlayerData(strBuf,&record))
		{
			//通知客户端
			MsgNotify ttt(ID_D2C, Notify_S2C_CreateRole_OK);
			ttt.OBJID = temp->OBJID;
			SendToClient(&ttt);
		
			m_PlayerInfo[record.playerID] = record;
			//发送基础信息
			MsgRole_BaseInfo send(temp->OBJID);
			CopyPalyerBaseInfo(&send.BaseInfo,&record);
			SendToGame(&send);
			//发送详细信息
			SendPlayerDetaiInfo(&record);
		}
	}	
}

void DBServer::OnAddItemSet(MsgBase* msg)
{
    MsgItem_Add_Set* pAddItem = static_cast<MsgItem_Add_Set*>(msg);
    
    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        SQL_player_info& pPlayerInfo = it->second;
        
        uint64 itemGuid;
        StackStream<> addItemStream(pAddItem, pAddItem->Len);
        MsgItem_Add_Set msgHead;
        addItemStream.Read(&msgHead, sizeof(msgHead));
        std::map<uint64, ItemInfoIt>::iterator it2;
        for (uint8 i = 0; i < msgHead.addNumItem; ++i)
        {
            //>取得物品信息
            addItemStream.Read(&itemGuid, sizeof(uint64));
            it2 = pPlayerInfo.bag.find(itemGuid);
            if (it2 != pPlayerInfo.bag.end())
            {
                //-----------------------------------------------------------------------------------------
                if (msgHead.operateType == ItemOperate_BuyShopItem || msgHead.operateType == ItemOperate_BuyChest || 
                    msgHead.operateType == ItemOperate_Chest || msgHead.operateType == ItemOperate_ItemDrop ||
                    msgHead.operateType == ItemOperate_ItemMerge || msgHead.operateType == ItemOperate_BuyMallItem ||
                    msgHead.operateType == ItemOperate_TaskReward || msgHead.operateType == ItemOpreate_TakeTaskItem )
                {
                    //>这些操作都会有新guid产生
                    assert(itemGuid > guidInfo.itemId);
                    guidInfo.itemId = max(guidInfo.itemId, itemGuid);   //>保存最后分配的itemGuid.
                }

                ItemInfoIt& itemInfo = it2->second;
                addItemStream.Read(&itemInfo.id, sizeof(int));
                addItemStream.Read(&itemInfo.prefixID, sizeof(int));
                addItemStream.Read(&itemInfo.starNum, sizeof(int));
                addItemStream.Read(&itemInfo.isBind, sizeof(uint8));
                addItemStream.Read(&itemInfo.num, sizeof(int));
                addItemStream.Read(&itemInfo.isLock, sizeof(uint8));
                addItemStream.Read(&itemInfo.time, sizeof(int));
            }
            else
            {
                ItemInfoIt itemInfo;
                itemInfo.guid = itemGuid;
                addItemStream.Read(&itemInfo.id, sizeof(int));
                addItemStream.Read(&itemInfo.prefixID, sizeof(int));
                addItemStream.Read(&itemInfo.starNum, sizeof(int));
                addItemStream.Read(&itemInfo.isBind, sizeof(uint8));
                addItemStream.Read(&itemInfo.num, sizeof(int));
                addItemStream.Read(&itemInfo.isLock, sizeof(uint8));
                addItemStream.Read(&itemInfo.time, sizeof(int));
                pPlayerInfo.bag[itemGuid] = itemInfo;
            }
        }
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnAddItemSet.", msg->OBJID);
    }
}

void DBServer::OnDelItemSet(MsgBase* msg)
{
    MsgItem_Del_Set* pDelItem = static_cast<MsgItem_Del_Set*>(msg);
    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        SQL_player_info& pPlayerInfo = it->second;
        StackStream<> delItemStream(pDelItem, pDelItem->Len);
        MsgItem_Del_Set msgHead;
        delItemStream.Read(&msgHead, sizeof(msgHead));
        std::map<uint64, ItemInfoIt>::iterator it2;
        uint64 delItemId = 0;
        for (int i = 0; i < msgHead.delNumItem; ++i)
        {
            delItemStream.Read(&delItemId, sizeof(delItemId));
            it2 = pPlayerInfo.bag.find(delItemId);
            if (it2 != pPlayerInfo.bag.end())
            {
                pPlayerInfo.bag.erase(it2);
            }
            else
            {
                gLog.Warning("can't find itemGuid=[%llu] on player[%llu] when call OnDelItemSet.",delItemId, msg->OBJID);
            }
        }
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnDelItemSet.", msg->OBJID);
    }
}

void DBServer::OnUpdateItemSet(MsgBase* msg)
{
    MsgItem_Update_Set* pUpdateItem = static_cast<MsgItem_Update_Set*>(msg);

    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        uint64 itemGuid;
        SQL_player_info& pPlayerInfo = it->second;
        StackStream<> updateItemStream(pUpdateItem, pUpdateItem->Len);
        MsgItem_Update_Set msgHead;
        updateItemStream.Read(&msgHead, sizeof(msgHead));
        std::map<uint64, ItemInfoIt>::iterator it2;
        for (uint8 i = 0; i < msgHead.updateNumItem; ++i)
        {
            updateItemStream.Read(&itemGuid, sizeof(uint64));
            it2 = pPlayerInfo.bag.find(itemGuid);
            if (it2 != pPlayerInfo.bag.end())
            {
                ItemInfoIt& itemInfo = it2->second;
                updateItemStream.Read(&itemInfo.id, sizeof(int));
                updateItemStream.Read(&itemInfo.prefixID, sizeof(int));
                updateItemStream.Read(&itemInfo.starNum, sizeof(int));
                updateItemStream.Read(&itemInfo.isBind, sizeof(uint8));
                updateItemStream.Read(&itemInfo.num, sizeof(int));
                updateItemStream.Read(&itemInfo.isLock, sizeof(uint8));
                updateItemStream.Read(&itemInfo.time, sizeof(int));
            }
            else
            {
                updateItemStream.Read(NULL, 5*sizeof(int) + sizeof(uint8));
                gLog.Warning("can't find itemGuid=[%llu] on player[%llu] when call OnDelItemSet.",itemGuid, msg->OBJID);
            }
        }
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnUpdateItemSet.", msg->OBJID);
    }
}

void DBServer::OnUpdateWearEquipment(MsgBase* msg)
{
    MsgEquip_Update* pUpdateItem = static_cast<MsgEquip_Update*>(msg);

    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        SQL_player_info& pPlayerInfo = it->second;
        assert(pUpdateItem->equipmentType < EQUIPTYPE_MAX);
        if (pUpdateItem->equipmentType < EQUIPTYPE_MAX)
        {
            if (pUpdateItem->operateType == ItemOperate_ItemMerge && pUpdateItem->equipment.guid)
            {
                //>身上的物品合成新物品
                assert(pUpdateItem->equipment.guid > guidInfo.itemId);
                guidInfo.itemId = max(guidInfo.itemId, pUpdateItem->equipment.guid);   //>保存最后分配的itemGuid.
            }
            memcpy(&pPlayerInfo.equip[pUpdateItem->equipmentType], &pUpdateItem->equipment, sizeof(ItemInfoIt));
        }
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnUpdateWearEquipment.", msg->OBJID);
    }
}

void DBServer::OnAddSkill(MsgBase* msg)
{
    MsgSkill_Add* pAddSkillTree = static_cast<MsgSkill_Add*>(msg);

    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        SQL_player_info& playerInfo = it->second;
        SkillInfoIt& addSkill = pAddSkillTree->addSkill;
        //check 
        SkillInfoIt* pSkillTree = FindPlayerSkill(&playerInfo, addSkill.skillType);
        if (pSkillTree != NULL)
        {
            gLog.Warning("repeated add skill[%d] when call OnAddSkill.", addSkill.skillType);
            return;   
        }
        playerInfo.skillInfo.push_back(addSkill);
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnAddSkill.", msg->OBJID);
    }
}

void DBServer::OnUpdateSkill(MsgBase* msg)
{
    MsgSkill_Update* pUpdateSkill = static_cast<MsgSkill_Update*>(msg);

    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        SQL_player_info& playerInfo = it->second;
        SkillInfoIt& updateSkill = pUpdateSkill->updateSkill;
        SkillInfoIt* pSkill = FindPlayerSkill(&playerInfo, updateSkill.skillType);
        if (pSkill == NULL)
        {
            gLog.Warning("can't find skillType when call OnUpdateSkill.");
            return;
        }

        pSkill->level = updateSkill.level;
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnUpdateSkill.", msg->OBJID);
    }
}

void DBServer::OnActivateSkill(MsgBase* msg)
{
    MsgSkill_Activate* pActivateSkill = static_cast<MsgSkill_Activate*>(msg);

    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        SQL_player_info& playerInfo = it->second;
        playerInfo.activeSkillType = pActivateSkill->ActivateSkillType;
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnActivateSkill.", msg->OBJID);
    }
}

void DBServer::OnChoseCanUseSkill(MsgBase* msg)
{
    MsgSkill_ChoseSkill* pChoseSkillMsg = static_cast<MsgSkill_ChoseSkill*>(msg);

    std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
    if (it != m_PlayerInfo.end())
    {
        SQL_player_info& playerInfo = it->second;
        int index = pChoseSkillMsg->setIndex;
        if (0 <= index && index < MAX_CHOSESKILL)
        {
            int oldIndex = -1;
            for (int i = 0; i < MAX_CHOSESKILL; ++i)
            {
                if (playerInfo.choseSkill[i] == pChoseSkillMsg->skillId)
                {
                    oldIndex = i;
                    break;
                }
            }

            if (oldIndex >= 0)
            {
                playerInfo.choseSkill[oldIndex] = playerInfo.choseSkill[index];
            }

            playerInfo.choseSkill[index] = pChoseSkillMsg->skillId;
        }
    }
    else
    {
        gLog.Warning("can't find player[%llu] when call OnChoseCanUseSkill.", msg->OBJID);
    }
}

void DBServer::SendPlayerDetaiInfo(SQL_player_info* playerInfo)
{
	MsgRole_Detail_Info msgPlayerDetailHead;
	msgPlayerDetailHead.SysID = ID_D2G;
	msgPlayerDetailHead.OBJID = playerInfo->playerID;
	msgPlayerDetailHead.MsgID=Msg_Role_DetailInfo;

	StackStream<40960> playerDetailInfoStream;
	playerDetailInfoStream.Wirte(&msgPlayerDetailHead, sizeof(msgPlayerDetailHead));
	//功法信息
	vector<SkillInfoIt>::iterator it2 = playerInfo->skillInfo.begin();
	for (vector<SkillInfoIt>::iterator end =playerInfo->skillInfo.end();
		it2 != end; 
		++it2)
	{
		SkillInfoIt& skillInfo = *it2;
		playerDetailInfoStream.Wirte(&skillInfo.skillType, sizeof(int));
		playerDetailInfoStream.Wirte(&skillInfo.level, sizeof(int));
	}
	//翅膀信息
	std::map<int, WingInfoIt>::iterator it3 =playerInfo->wingInfo.begin();
	for (std::map<int, WingInfoIt>::iterator end =playerInfo->wingInfo.end();
		it3 != end; 
		++it3)
	{
		WingInfoIt& wingInfo = it3->second;
		playerDetailInfoStream.Wirte(&wingInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&wingInfo.level, sizeof(int));
		playerDetailInfoStream.Wirte(&wingInfo.exp, sizeof(int));	
	}
	//法宝信息
	std::map<int, MagicWeaponInfoIt>::iterator it4 =playerInfo->realmupInfo.begin();
	for (std::map<int, MagicWeaponInfoIt>::iterator end =playerInfo->realmupInfo.end();
		it4 != end; 
		++it4)
	{
		MagicWeaponInfoIt& magicInfo = it4->second;
		playerDetailInfoStream.Wirte(&magicInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&magicInfo.level, sizeof(int));
		playerDetailInfoStream.Wirte(&magicInfo.exp, sizeof(int));	
		playerDetailInfoStream.Wirte(&magicInfo.growthPlus, sizeof(int));	
	}
	//器灵信息
	std::map<int, MountInfoIt>::iterator it5 =playerInfo->mountInfo.begin();
	for (std::map<int, MountInfoIt>::iterator end =playerInfo->mountInfo.end();
		it5 != end; 
		++it5)
	{
		MountInfoIt& mountInfo = it5->second;
		playerDetailInfoStream.Wirte(&mountInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&mountInfo.time, sizeof(int));	
	}
	//副本信息
	std::map<int, CopyInfoIt>::iterator it7 =playerInfo->copy.begin();
	for (std::map<int, CopyInfoIt>::iterator end =playerInfo->copy.end();
		it7 != end; 
		++it7)
	{
		CopyInfoIt& copyInfo = it7->second;
		playerDetailInfoStream.Wirte(&copyInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&copyInfo.enterNum, sizeof(int));	
		playerDetailInfoStream.Wirte(&copyInfo.layer, sizeof(int));	
	}
	//日常信息
	std::map<int, DailyInfoIt>::iterator it8 =playerInfo->daily.begin();
	for (std::map<int, DailyInfoIt>::iterator end =playerInfo->daily.end();
		it8 != end; 
		++it8)
	{
		DailyInfoIt& dailyInfo = it8->second;
		playerDetailInfoStream.Wirte(&dailyInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&dailyInfo.progress, sizeof(int));	
		playerDetailInfoStream.Wirte(&dailyInfo.isReward, sizeof(bool));	
	}
	//任务信息
	std::map<int, TaskInfoIt>::iterator it9 =playerInfo->task.begin();
	for (std::map<int, TaskInfoIt>::iterator end =playerInfo->task.end();
		it9 != end; 
		++it9)
	{
		TaskInfoIt& taskInfo = it9->second;
		playerDetailInfoStream.Wirte(&taskInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&taskInfo.progress, sizeof(int));	
		playerDetailInfoStream.Wirte(&taskInfo.state, sizeof(int));	
	}
	//身上物品信息
	uint8 equipCount = EQUIPTYPE_MAX;
	for (int i=0;i<EQUIPTYPE_MAX;i++)
	{
		ItemInfoIt& tequipInfo = playerInfo->equip[i];
		playerDetailInfoStream.Wirte(&tequipInfo.guid, sizeof(uint64));
		playerDetailInfoStream.Wirte(&tequipInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&tequipInfo.prefixID, sizeof(int));
		playerDetailInfoStream.Wirte(&tequipInfo.starNum, sizeof(int));
		playerDetailInfoStream.Wirte(&tequipInfo.isBind, sizeof(uint8));
		playerDetailInfoStream.Wirte(&tequipInfo.num, sizeof(int));
		playerDetailInfoStream.Wirte(&tequipInfo.isLock, sizeof(uint8));
        playerDetailInfoStream.Wirte(&tequipInfo.time, sizeof(int));
	}
	//背包物品信息
	std::map<uint64, ItemInfoIt>::iterator it11 =playerInfo->bag.begin();
	for (std::map<uint64, ItemInfoIt>::iterator end =playerInfo->bag.end();
		it11 != end; 
		++it11)
	{
		ItemInfoIt& bagInfo = it11->second;
		playerDetailInfoStream.Wirte(&bagInfo.guid, sizeof(uint64));
		playerDetailInfoStream.Wirte(&bagInfo.id, sizeof(int));
		playerDetailInfoStream.Wirte(&bagInfo.prefixID, sizeof(int));
		playerDetailInfoStream.Wirte(&bagInfo.starNum, sizeof(int));
		playerDetailInfoStream.Wirte(&bagInfo.isBind, sizeof(uint8));
		playerDetailInfoStream.Wirte(&bagInfo.num, sizeof(int));
		playerDetailInfoStream.Wirte(&bagInfo.isLock, sizeof(uint8));
        playerDetailInfoStream.Wirte(&bagInfo.time, sizeof(int));
	}

	//好友列表信息
	std::vector<FriendsInfo>::iterator iter;
	for (iter = playerInfo->vecFriendsList.begin(); iter != playerInfo->vecFriendsList.end() ; ++iter)
	{
		FriendsInfo& friends = *iter;
		playerDetailInfoStream.Wirte(&friends.roleId, sizeof(uint64));
		playerDetailInfoStream.Wirte(&friends.szName, MAX_NAME_SIZE);
		playerDetailInfoStream.Wirte(&friends.bIsLine,sizeof(bool));
		playerDetailInfoStream.Wirte(&friends.iLevel,sizeof(int));
		playerDetailInfoStream.Wirte(&friends.iFight,sizeof(int));
	}
	//黑名单信息 
	std::vector<FriendsInfo>::iterator iterBlack;
	for (iterBlack = playerInfo->vecBlackList.begin(); iterBlack != playerInfo->vecBlackList.end() ; ++iterBlack)
	{
		FriendsInfo& black = *iter;
		playerDetailInfoStream.Wirte(&black.roleId, sizeof(uint64));
		playerDetailInfoStream.Wirte(&black.szName, MAX_NAME_SIZE);
		playerDetailInfoStream.Wirte(&black.bIsLine,sizeof(bool));
		playerDetailInfoStream.Wirte(&black.iLevel,sizeof(int));
		playerDetailInfoStream.Wirte(&black.iFight,sizeof(int));
	}

	//玩家已选择的技能
	for (int i=0;i<MAX_CHOSESKILL;i++)
	{
		playerDetailInfoStream.Wirte(&playerInfo->choseSkill[i], sizeof(int32));
	}

	MsgRole_Detail_Info* playerDetailMsg = static_cast<MsgRole_Detail_Info*>(playerDetailInfoStream.Get());
	playerDetailMsg->Len = playerDetailInfoStream.GetSize();

	playerDetailMsg->SkillTreeNum   =   playerInfo->skillInfo.size();
	playerDetailMsg->WingNum        =   playerInfo->wingInfo.size();
	playerDetailMsg->MountNum       =   playerInfo->mountInfo.size();
	playerDetailMsg->MagicWeaponNum =   playerInfo->realmupInfo.size();
	playerDetailMsg->CopyNum        =   playerInfo->copy.size();
	playerDetailMsg->DailyNum       =   playerInfo->daily.size();
	playerDetailMsg->TaskNum        =   playerInfo->task.size();
	playerDetailMsg->EquipNum       =   equipCount;
	playerDetailMsg->BagNum         =   playerInfo->bag.size();
	playerDetailMsg->FriendsListNum =   playerInfo->vecFriendsList.size();
	playerDetailMsg->BlackListNum   =   playerInfo->vecBlackList.size();
	playerDetailMsg->ChoseSkill     =   MAX_CHOSESKILL;

	SendToGame(playerDetailMsg);
}

SkillInfoIt* DBServer::FindPlayerSkill(SQL_player_info* pPlayerInfo, int32 skillType)
{
    //check 
    vector<SkillInfoIt>::iterator it = pPlayerInfo->skillInfo.begin();
    for (vector<SkillInfoIt>::iterator end = pPlayerInfo->skillInfo.end();
        it != end; ++it)
    {
        SkillInfoIt& info = *it;
        if (info.skillType == skillType)
        {
            return &info;
        }
    }
    return NULL;
}

//添加好友
void DBServer::OnAddFriends(MsgBase* msg)
{
	MsgFriends *temp = (MsgFriends *)msg;
	FriendsInfo tempFriend;
	tempFriend.roleId = temp->iAddRoleID;
	tempFriend.bIsLine = 0;
	tempFriend.iLevel = 0;
	tempFriend.iFight = 0;
	sprintf(tempFriend.szName,temp->szName);
	if (temp->iType == Relation_Friends)
	{
		m_PlayerInfo[temp->iRequestRoleID].vecFriendsList.push_back(tempFriend);
	}
	if (temp->iType == Relation_BlackList)
	{
		m_PlayerInfo[temp->iRequestRoleID].vecBlackList.push_back(tempFriend);
	}
}

//删除好友
void DBServer::OnDelFriends(MsgBase* msg)
{
	MsgFriends *temp = (MsgFriends *)msg;
	if (temp->iType == Relation_Friends)
	{
		vector<FriendsInfo>::iterator iVec;
		for ( iVec = m_PlayerInfo[temp->iRequestRoleID].vecFriendsList.begin(); iVec != m_PlayerInfo[temp->iRequestRoleID].vecFriendsList.end();++iVec)
		{
			if (iVec->roleId == temp->iAddRoleID)
			{
				m_PlayerInfo[temp->iRequestRoleID].vecFriendsList.erase(iVec);
				break;
			}
		}
	}
	if (temp->iType == Relation_BlackList)
	{
		vector<FriendsInfo>::iterator iVec;
		for ( iVec = m_PlayerInfo[temp->iRequestRoleID].vecBlackList.begin(); iVec != m_PlayerInfo[temp->iRequestRoleID].vecBlackList.end();++iVec)
		{
			if (iVec->roleId == temp->iAddRoleID)
			{
				m_PlayerInfo[temp->iRequestRoleID].vecBlackList.erase(iVec);
				break;
			}
		}
	}
}

//创建帮派
void DBServer::OnCreateGang(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;
	std::map<uint64,SQL_player_info>::iterator it = m_PlayerInfo.find(temp->OBJID);
  	if(it != m_PlayerInfo.end())
	{
		char strBuf[1024];
 		sprintf(strBuf, "INSERT INTO ganginfo (ID,gangName,headerID,headerName,createTime,copyStartTime) VALUES (%llu, '%s', %llu, '%s',localtime(),localtime())", temp->iPar[1], temp->szPar, temp->iPar[0], it->second.playerName);
		if (m_Mysql.DbExecute(strBuf, eMysqlInsert) >= 0)
		{
			SQL_gang_info record;
			sprintf(strBuf, "SELECT * FROM ganginfo WHERE ID = %llu limit 0,1", temp->iPar[1]);
			StoreQueryResult res =m_Mysql.DbQuery(strBuf);
			if(m_Mysql.FullSelectGangData(res,&record))
			{
				GangMember tmpMember;
				tmpMember.id = it->second.playerID;
				strncpy(tmpMember.szName,it->second.playerName, MAX_NAME_SIZE);
				tmpMember.iLevel = it->second.level;
				strncpy(tmpMember.szApplyTime,"", MAX_NAME_SIZE);
				strncpy(tmpMember.szPosition,"", MAX_NAME_SIZE);
				tmpMember.iContribution = 0;				
				strncpy(tmpMember.szLeaveTime,"", MAX_NAME_SIZE);
				record.vecMembersList.push_back(tmpMember);
				m_GangInfo.insert(pair<uint64,SQL_gang_info>(temp->iPar[1],record));
				it->second.societyID = record.id;				
			}
		}
	}
	
	//创建人temp->iPar[0]
	//帮派名称temp->szPar
}

//申请加入帮派
void DBServer::OnApplyGang(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;
	char strBuf[1024];
	sprintf(strBuf, "INSERT INTO applyinfo (ID,applyID,applyType,targetID,applyTime,backupInfo) VALUES (%d, '%d', %d, %d,localtime(),'%s')", temp->iPar[0], temp->iPar[1], 0, 1,temp->szPar);	
	if (m_Mysql.DbExecute(strBuf, eMysqlInsert) >= 0)
	{
		SQL_apply_info record;
		sprintf(strBuf, "SELECT * FROM applyinfo WHERE ID = %llu limit 0,1", temp->iPar[0]);
		StoreQueryResult res =m_Mysql.DbQuery(strBuf);
		if(m_Mysql.FullApplyGangInfo(res,&record))
		{
			m_ApplyInfo.insert(pair<uint64,SQL_apply_info>(temp->iPar[0],record));

		}
	}
}

//同意入派申请
void DBServer::OnApplyGangAgree(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;

	std::map<uint64,SQL_gang_info>::iterator itGang = m_GangInfo.find(temp->iPar[0]);
	if(itGang != m_GangInfo.end())
	{
		SQL_player_info record;
		bool isOnLine = false;
		std::map<uint64,SQL_player_info>::iterator itPlayer = m_PlayerInfo.find(temp->iPar[1]);
		if(itPlayer != m_PlayerInfo.end())
		{
			isOnLine = true;
			record = itPlayer->second;
			itPlayer->second.societyID = temp->iPar[0];
		}
		if(!isOnLine)
		{
			char strBuf[512] = {0};
			sprintf(strBuf, "SELECT * FROM player WHERE playerID = %llu", temp->iPar[1]);
			if(!m_Mysql.FullSelectPlayerData(strBuf,&record))
			{
				gLog.Warning("查找玩家信息[%llu]信息失败！",temp->iPar[1]);
				return;
			}
			record.societyID = temp->iPar[0];
			if(m_Mysql.UpdatePlayerInfo(&record))
			{
				gLog.Info("更新玩家(Gang)[%llu]信息成功！",temp->iPar[1]);
			}
			else
			{
				gLog.Info("更新玩家(Gang)[%llu]信息失败！",temp->iPar[1]);
			}
		}
		std::map<uint64,SQL_apply_info>::iterator itApply;
		for(itApply = m_ApplyInfo.begin();itApply != m_ApplyInfo.end();itApply++)
		{
			if(itApply->second.iTargetid == temp->iPar[1])
			{
				OnDelApplyInfo(itApply->first);
			}
		}
			GangMember tmpMember;
			tmpMember.id = record.playerID;
			strncpy(tmpMember.szName,record.playerName, MAX_NAME_SIZE);
			tmpMember.iLevel = record.level;
			memset(tmpMember.szApplyTime,0,sizeof(tmpMember.szApplyTime));			
			strncpy(tmpMember.szPosition," ", MAX_NAME_SIZE);
			tmpMember.iContribution = 0;		
			memset(tmpMember.szLeaveTime,0,sizeof(tmpMember.szLeaveTime));
			itGang->second.vecMembersList.push_back(tmpMember);
	}
}
//解散帮派
void DBServer::OnDelGang(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;
	m_GangInfo.erase(temp->iPar[0]);
	char strBuf[512];
	sprintf(strBuf,"DELETE FROM ganginfo WHERE id = %llu;",temp->iPar[0]);
	if (m_Mysql.DbExecute(strBuf, eMysqlDelete) >= 0)
	{
		gLog.Info("Delete Gang:[%llu] Successful!\n",temp->iPar[0]);		
		return;
	}
	gLog.Warning("Delete Gang:[%llu] Failed!\n",temp->iPar[0]);		
}

//删除申请帮派信息
void DBServer::OnDelApplyInfo(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;
	m_ApplyInfo.erase(temp->iPar[0]);
	char strBuf[512];
	sprintf(strBuf,"DELETE FROM applyinfo WHERE id = %llu;",temp->iPar[0]);
	if (m_Mysql.DbExecute(strBuf, eMysqlDelete) >= 0)
	{
		gLog.Info("Delete ApplyInfo:[%llu] Successful!\n",temp->iPar[0]);		
		return;
	}
	gLog.Warning("Delete ApplyInfo:[%llu] Failed!\n",temp->iPar[0]);		
}

void DBServer::OnDelApplyInfo(__int64 applyId)
{
	m_ApplyInfo.erase(applyId);
	char strBuf[512];
	sprintf(strBuf,"DELETE FROM applyinfo WHERE id = %llu;",applyId);
	if (m_Mysql.DbExecute(strBuf, eMysqlDelete) >= 0)
	{
		gLog.Info("Delete ApplyInfo:[%llu] Successful!\n",applyId);		
		return;
	}
	gLog.Warning("Delete ApplyInfo:[%llu] Failed!\n",applyId);		
}

void DBServer::OnQuitGang(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;
	std::map<uint64,SQL_gang_info>::iterator itGang = m_GangInfo.find(temp->iPar[0]);
	if(itGang != m_GangInfo.end())
	{
		std::vector<GangMember>::iterator itMember;
		for(itMember = itGang->second.vecMembersList.begin();itMember != itGang->second.vecMembersList.end();itMember++)
		{
			if(itMember->id == temp->iPar[1])
			{
				itGang->second.vecMembersList.erase(itMember);
				break;
			}
		}
		std::map<uint64,SQL_player_info>::iterator itPlayer = m_PlayerInfo.find(temp->iPar[1]);
		if(itPlayer != m_PlayerInfo.end())
		{
			itPlayer->second.societyID = 0;
			return;
		}
		SQL_player_info record;
		char strBuf[512] = {0};
		sprintf(strBuf, "SELECT * FROM player WHERE playerID = %llu", temp->iPar[1]);
		if(!m_Mysql.FullSelectPlayerData(strBuf,&record))
		{
			gLog.Warning("查找玩家信息[%llu]信息失败！",temp->iPar[1]);
			return;
		}
		record.societyID = 0;
		if(m_Mysql.UpdatePlayerInfo(&record))
		{
			gLog.Info("更新玩家(Gang)[%llu]信息成功！",temp->iPar[1]);
		}
		else
		{
			gLog.Info("更新玩家(Gang)[%llu]信息失败！",temp->iPar[1]);
		}
	}

}

void DBServer::OnChangeGangHeader(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;//iPar[0]工会ID iPar[1]新帮主ID iPar[2]原帮主ID
	std::map<uint64,SQL_gang_info>::iterator it = m_GangInfo.find(temp->iPar[0]);
	if(it != m_GangInfo.end())
	{
		it->second.iHeaderid = temp->iPar[1];
	}
}

void DBServer::OnTakeTask(MsgBase* msg)
{
	MsgTask_TakeTask* pTakeTask = (MsgTask_TakeTask*)msg;
	std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);

	if (it != m_PlayerInfo.end())
	{
		SQL_player_info& pPlayerInfo = it->second;
		StackStream<> addTaskStream(pTakeTask, pTakeTask->Len);
		MsgTask_TakeTask msgHead;
		addTaskStream.Read(&msgHead, sizeof(msgHead));
		TaskInfoIt taskInfo;
		addTaskStream.Read(&taskInfo.id, sizeof(int));
		addTaskStream.Read(&taskInfo.progress, sizeof(int));
		addTaskStream.Read(&taskInfo.state, sizeof(int));
		pPlayerInfo.task[msgHead.taskId] = taskInfo;
	}
	else
	{
		gLog.Warning("can't find player[%llu] when call OnTakeTask.", msg->OBJID);
	}
	
}

void DBServer::OnCopyApply(MsgBase* msg)
{
	MsgCopy_Apply* msgApply = (MsgCopy_Apply*)msg;
	std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
	if (it != m_PlayerInfo.end())
	{
		int copyId=0;
		SQL_player_info& pPlayerInfo = it->second;
		StackStream<> applyCopyStream(msgApply, msgApply->Len);
		MsgCopy_Apply msgHead;
		applyCopyStream.Read(&msgHead, sizeof(msgHead));

		CopyInfoIt copyInfoIt;
		applyCopyStream.Read(&copyInfoIt.id, sizeof(int));
		applyCopyStream.Read(&copyInfoIt.enterNum, sizeof(int));
		applyCopyStream.Read(&copyInfoIt.layer, sizeof(int));

		std::map<int, CopyInfoIt>::iterator itCopy = pPlayerInfo.copy.find(msgHead.copyId);
		if( itCopy != pPlayerInfo.copy.end())
		{
			itCopy->second = copyInfoIt;
		}
		else
		{
			pPlayerInfo.copy.insert(pair<int,CopyInfoIt>(copyInfoIt.id,copyInfoIt));
		}
	}
}



void DBServer::OnCopyFinish(MsgBase* msg)
{
	MsgCopy_Finish* copyFinish = (MsgCopy_Finish*)msg;
	std::map<uint64,SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);
	if(it != m_PlayerInfo.end())
	{
		std::map<int, CopyInfoIt>::iterator itCopy = it->second.copy.find(copyFinish->copyInfoIt.id);
		if(itCopy != it->second.copy.end())
		{
			itCopy->second = copyFinish->copyInfoIt;
		}
	}
}

void DBServer::OnMakeTask(MsgBase* msg)
{
	MsgTask_Progress* pMakeTask = (MsgTask_Progress*)msg;
	std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);

	if (it != m_PlayerInfo.end())
	{
		std::map<int32,TaskInfoIt>::iterator it1 = it->second.task.find(pMakeTask->taskInfo.id);
		if( it1 != it->second.task.end())
		{
			 it1->second = pMakeTask->taskInfo;
			
		}
		else
		{
			gLog.Warning("can't find task[%llu] when call OnMakeTask.",pMakeTask->taskInfo.id);
		}
	}
	else
	{
		gLog.Warning("can't find player[%llu] when call OnMakeTask.", msg->OBJID);
	}
}

void DBServer::OnSubmitTask(MsgBase* msg)
{
	MsgTask_SubmitTask* pSubmitTask = (MsgTask_SubmitTask*)msg;
	std::map<uint64, SQL_player_info>::iterator it = m_PlayerInfo.find(msg->OBJID);

	if (it != m_PlayerInfo.end())
	{
		SQL_player_info& pPlayerInfo = it->second;
		StackStream<> submitTaskStream(pSubmitTask, pSubmitTask->Len);
		MsgTask_SubmitTask msgHead;
		submitTaskStream.Read(&msgHead, sizeof(msgHead));
		int taskId = msgHead.taskId;
		std::map<int32,TaskInfoIt>::iterator it1 = it->second.task.find(taskId);
		if( it1 != it->second.task.end())
		{
			pPlayerInfo.money = pSubmitTask->money;
			TaskInfoIt& taskInfo = it1->second; 
			submitTaskStream.Read(&taskInfo.id, sizeof(int));
			submitTaskStream.Read(&taskInfo.progress, sizeof(int));
			submitTaskStream.Read(&taskInfo.state, sizeof(int));
		}
		else
		{
			submitTaskStream.Read(NULL, 6*sizeof(int));

			gLog.Warning("can't find task[%llu] when call OnMakeTask.",taskId);
		}
	}
	else
	{
		gLog.Warning("can't find player[%llu] when call OnMakeTask.", msg->OBJID);
	}
}

void DBServer::OnUpdateBaseInfo(MsgBase* msg)
{
	SQL_player_info& playerInfo  = m_PlayerInfo.find(msg->OBJID)->second;
	MsgUpdate_BaseInfo* pUdpBaseInfo = (MsgUpdate_BaseInfo*)msg;
	switch (pUdpBaseInfo->attributeType)
	{
	case ATTRIBUTE_MONEY:
		playerInfo.money = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_HPMAX:
		break;
	case ATTRIBUTE_MPMAX:
		break;
	case ATTRIBUTE_ATTACK:
		break;
	case ATTRIBUTE_DEFENSE:
		break;
	case ATTRIBUTE_HIT:
		break;
	case ATTRIBUTE_DODGE:
		break;
	case ATTRIBUTE_CRIT:
		break;
	case ATTRIBUTE_HP:
		playerInfo.hp = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_MP:
		playerInfo.mp = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_GOLD:
		playerInfo.gold = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_BINDGOLD:
		playerInfo.bindGold = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_ARENAMONEY:
		playerInfo.arenaMoney = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_BATTLEFIELDMONEY:
		playerInfo.battlefieldMoney = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_XPMAX:
		break;
	case ATTRIBUTE_XP:
		break;
	case ATTRIBUTE_LEVEL:
		playerInfo.level = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_EXP:
		playerInfo.exp = (int)pUdpBaseInfo->attributeValue;	
		break;
	case ATTRIBUTE_REALMLEVEL:
		playerInfo.realmLevel = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_REALMEXP:
		playerInfo.realmExp = (int)pUdpBaseInfo->attributeValue;
		break;
	case ATTRIBUTE_MAX:
		break;
	default:
		break;
	}
}

bool DBServer::OnFullSelectGangInfoMap(StoreQueryResult res)
{
	if(res.size() < 0)
	{
		return false;
	}
	StoreQueryResult::const_iterator it;
	for(it=res.begin();it!=res.end();++it)
	{
		SQL_gang_info gangInfo;
		if(!(*it)["ID"].is_null())
			gangInfo.id=(*it)["ID"];
		if(!(*it)["gangName"].is_null())
			strncpy(gangInfo.szGangName,(*it)["gangName"].c_str(), MAX_NAME_SIZE); 
		if(!(*it)["gangLevel"].is_null())
			gangInfo.iGangLevel=(*it)["gangLevel"];
		if(!(*it)["gangShopLevel"].is_null())
			gangInfo.iGangShopLevel=(*it)["gangShopLevel"];
		if(!(*it)["headerID"].is_null())
			gangInfo.iHeaderid=(*it)["headerID"];
		if(!(*it)["headerName"].is_null())
			strncpy(gangInfo.szHeaderName,(*it)["headerName"], MAX_NAME_SIZE);
		if(!(*it)["gangActive"].is_null())
			gangInfo.iGangActive=(*it)["gangActive"];
		if(!(*it)["gangAnnouncement"].is_null())
			strncpy(gangInfo.szGangAnnounce,(*it)["gangAnnouncement"], 255);
		if(!(*it)["viceHeader1"].is_null())
			gangInfo.iViceHeaderid[0]=(*it)["viceHeader1"];
		if(!(*it)["viceHeader2"].is_null())
			gangInfo.iViceHeaderid[1]=(*it)["viceHeader2"];
		if(!(*it)["starFlex"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["starFlex"];
			__int64* ptemp = (__int64*)temp.data();
			for(int i=0;i < 5;i++)
			{
				gangInfo.iStarFlex[i] = *ptemp;
				if((i+1) < 5)
					ptemp++;
			}			
		}
		if(!(*it)["createTime"].is_null())
			strncpy(gangInfo.szCreateTime,(*it)["createTime"], 50);
		if(!(*it)["copyID"].is_null())
			gangInfo.iCopyid=(*it)["copyID"];
		if(!(*it)["copyStartTime"].is_null())
			strncpy(gangInfo.szCopyStartTime,(*it)["copyStartTime"], 50);
		if(!(*it)["members"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["members"];
			__int64 iLength = temp.length();
			GangMember* ptemp = (GangMember*)temp.data();
			for(int i=0;i<iLength/sizeof(GangMember);i++, ptemp++)
			{
				gangInfo.vecMembersList.push_back(*ptemp);
			}
		}
		if(!(*it)["backupInfo"].is_null())
			strncpy(gangInfo.szBackupInfo,(*it)["backupInfo"], 255);
		m_GangInfo.insert(pair<uint64,SQL_gang_info>(gangInfo.id,gangInfo));
	}
	return true;
}


bool DBServer::OnFullSelectApplyInfoMap(StoreQueryResult res)
{
	
	if(res.size() < 0)
	{
		return false;
	}
	StoreQueryResult::const_iterator it;
	for(it=res.begin();it!=res.end();++it)
	{
		SQL_apply_info applyInfo;
		if(!(*it)["ID"].is_null())
			applyInfo.id=(*it)["ID"];
		if(!(*it)["applyID"].is_null())
			applyInfo.iApplyid=(*it)["applyID"];
		if(!(*it)["applyType"].is_null())
			applyInfo.iApplyType=(*it)["applyType"];
		if(!(*it)["targetID"].is_null())
			applyInfo.iTargetid=(*it)["targetID"];
		if(!(*it)["applyTime"].is_null())
			strncpy(applyInfo.szApplyTime,(*it)["applyTime"], 50);
		if(!(*it)["backupInfo"].is_null())
			strncpy(applyInfo.szApplyTime,(*it)["backupInfo"], 50);
		m_ApplyInfo.insert(pair<uint64,SQL_apply_info>(applyInfo.id,applyInfo));
	}

	return true;
}

void DBServer::OnEditAnnounce(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	std::map<uint64,SQL_gang_info>::iterator itt =  m_GangInfo.find(temp->iPar[0]);
	if (itt != m_GangInfo.end())
	{
		strncpy(itt->second.szGangAnnounce,temp->szPar, 255);
	}
}


