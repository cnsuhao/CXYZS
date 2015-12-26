#include "CMysql.h"
#include <stdlib.h>
#include "LogManage.h"
#include "MysqlDef.h"
#include "StackStream.h"
#define LARGE_MSG_SIZE 256000
#define SMALL_MSG_SIZE 1024

//CMysql * g_cMySql=NULL;


CMysql::CMysql()
{
	m_strHost = "localhost";
	m_strUser = "root";
	m_strPassword = "123456";
	m_dwPort = 3306;
	m_strName = "testSql";
	memset(m_strQuery, 0, MAX_QUERY_SIZE);
}

CMysql::CMysql(const char *host, const char *user, const char *password, const char *name, int port)
{
	m_strHost = host;
	m_strUser = user;
	m_strPassword = password;
	m_dwPort = port;
	m_strName = name;
	memset(m_strQuery, 0, MAX_QUERY_SIZE);
}

CMysql::~CMysql()
{

}
//初始化数据库
bool CMysql::Init(const char *host, const char *user, const char *password, const char *name, int port)
{
	m_strHost = host;
	m_strUser = user;
	m_strPassword = password;
	m_dwPort = port;
	m_strName = name;
	memset(m_strQuery, 0, MAX_QUERY_SIZE);

	return DbConnect();
}

// 连接数据库                                                                     

int CMysql::DbConnect()
{
	m_pMysql.disable_exceptions();
	m_pMysql.set_option(new SetCharsetNameOption("GB2312"));
	if(m_pMysql.connect(m_strName.c_str(), m_strHost.c_str(), m_strUser.c_str(), m_strPassword.c_str(), m_dwPort))
	{
		DbQuery("SET NAMES GB2312");
		gLog.Info("CMysql::CMysql() Connect to host %s with user name %s and password %s dbname %s success!", m_strHost.c_str(), m_strUser.c_str(), m_strPassword.c_str(), m_strName.c_str());
		return 1;
	}
	gLog.Warning("CMysql::CMysql() Connect to host %s with user name %s and password %s dbname %s fail!", m_strHost.c_str(), m_strUser.c_str(), m_strPassword.c_str(), m_strName.c_str());
	gLog.Error("DB connection failed:%s\n", getDbLastError());
	return 0;
}

INT32	CMysql::ExecQuery(const char *cpsSQL, StoreQueryResult &rsResult)
{
	DbPing();
	Query query = m_pMysql.query(cpsSQL);
	rsResult = query.store();

	return rsResult.size();
}


// 执行数据库语句（查询）                                               

StoreQueryResult CMysql::DbQuery(const char *sql)
{
	DbPing();
	StoreQueryResult res;
	if (strlen(sql) > 0)
	{
		Query query = m_pMysql.query(sql);
		res = query.store();
	}
	return res;
}

char* CMysql::ConvertBinaryToString(const char* pBinaryData,int nLen)
{
	static char s_BinaryData[10240];
	//mysql_real_escape_string(,s_BinaryData,pBinaryData,nLen);
	mysql_escape_string(s_BinaryData,pBinaryData,nLen);

	return s_BinaryData;
}

bool CMysql::getGuidInfo(const char* strSql,GuidInfoIt* guidInfo)
{
	if(strSql==NULL)
	{
		return false;
	}
	StoreQueryResult res=DbQuery(strSql);
	if(res.size()<=0)
	{
		return false;
	}
	StoreQueryResult::const_iterator it;
	for(it=res.begin();it!=res.end();++it)
	{
		if(!(*it)["item"].is_null())
			guidInfo->itemId=(*it)["item"];
	}
	return true;
}

bool CMysql::FullApplyGangInfo(StoreQueryResult res,SQL_apply_info* applyInfo)
{
	if(res.size() <= 0)
	{
		return false;
	}
	StoreQueryResult::const_iterator it;
	for(it=res.begin();it!=res.end();++it)
	{
		if(!(*it)["ID"].is_null())
			applyInfo->id=(*it)["ID"];
		if(!(*it)["applyID"].is_null())
			applyInfo->iApplyid=(*it)["applyID"];
		if(!(*it)["applyType"].is_null())
			applyInfo->iApplyType=(*it)["applyType"];
		if(!(*it)["targetID"].is_null())
			applyInfo->iTargetid=(*it)["targetID"];
		if(!(*it)["applyTime"].is_null())
			strcpy(applyInfo->szApplyTime,(*it)["applyTime"]);
		if(!(*it)["backupInfo"].is_null())
			strcpy(applyInfo->szApplyTime,(*it)["backupInfo"]);
	}
	return true;
}

bool CMysql::FullSelectGangData(StoreQueryResult res,SQL_gang_info* gangInfo)
{
	if(res.size() <= 0)
	{
		return false;
	}
	StoreQueryResult::const_iterator it;
	for(it=res.begin();it!=res.end();++it)
	{
		if(!(*it)["ID"].is_null())
			gangInfo->id=(*it)["ID"];
		if(!(*it)["gangName"].is_null())
			strcpy(gangInfo->szGangName,(*it)["gangName"].c_str()); 
		if(!(*it)["gangLevel"].is_null())
			gangInfo->iGangLevel=(*it)["gangLevel"];
		if(!(*it)["gangShopLevel"].is_null())
			gangInfo->iGangShopLevel=(*it)["gangShopLevel"];
		if(!(*it)["headerID"].is_null())
			gangInfo->iHeaderid=(*it)["headerID"];
		if(!(*it)["headerName"].is_null())
			strcpy(gangInfo->szHeaderName,(*it)["headerName"]);
		if(!(*it)["gangActive"].is_null())
			gangInfo->iGangActive=(*it)["gangActive"];
		if(!(*it)["gangAnnouncement"].is_null())
			strcpy(gangInfo->szGangAnnounce,(*it)["gangAnnouncement"]);
		if(!(*it)["viceHeader1"].is_null())
			gangInfo->iViceHeaderid[0]=(*it)["viceHeader1"];
		if(!(*it)["viceHeader2"].is_null())
			gangInfo->iViceHeaderid[1]=(*it)["viceHeader2"];
		if(!(*it)["starFlex"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["starFlex"];
			__int64* ptemp = (__int64*)temp.data();
			for(int i=0;i < 5;i++)
			{
				gangInfo->iStarFlex[i] = *ptemp;
				if((i+1) < 5)
					ptemp++;
			}			
		}
		if(!(*it)["createTime"].is_null())
			strcpy(gangInfo->szCreateTime,(*it)["createTime"]);
		if(!(*it)["copyID"].is_null())
			gangInfo->iCopyid=(*it)["copyID"];
		if(!(*it)["copyStartTime"].is_null())
			strcpy(gangInfo->szCopyStartTime,(*it)["copyStartTime"]);
		if(!(*it)["members"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["members"];
			__int64 iLength = temp.length();
			GangMember* ptemp = (GangMember*)temp.data();
			for(int i=0;i<iLength/sizeof(GangMember);i++, ptemp++)
			{
				gangInfo->vecMembersList.push_back(*ptemp);
			}
		}

		if(!(*it)["backupInfo"].is_null())
			strcpy(gangInfo->szBackupInfo,(*it)["backupInfo"]);
	}
	return true;
}

bool CMysql::FullSelectPlayerData(const char* strSql,SQL_player_info* playerInfo)
{
	StoreQueryResult res=DbQuery(strSql);
	if(res.size()<=0)
	{
		return false;
	}
	StoreQueryResult::const_iterator it;
	for(it=res.begin();it!=res.end();++it)
	{
		if(!(*it)["playerID"].is_null())
			playerInfo->playerID=(*it)["playerID"];
		if(!(*it)["playerName"].is_null())
			strcpy(playerInfo->playerName,(*it)["playerName"].c_str()); 
		if(!(*it)["sex"].is_null())
			playerInfo->sex=unsigned char((*it)["sex"]);
		if(!(*it)["headID"].is_null())
			playerInfo->headID=(*it)["headID"];
		if(!(*it)["offlineTime"].is_null())
			playerInfo->offlineTime=(*it)["offlineTime"];
		if(!(*it)["level"].is_null())
			playerInfo->level=(*it)["level"];
		if(!(*it)["exp"].is_null())
			playerInfo->exp=(*it)["exp"];
		if(!(*it)["money"].is_null())
			playerInfo->money=(*it)["money"];
		if(!(*it)["gold"].is_null())
			playerInfo->gold=(*it)["gold"];
		if(!(*it)["bindGold"].is_null())
			playerInfo->bindGold=(*it)["bindGold"];
		if(!(*it)["rechargGoldSum"].is_null())
			playerInfo->rechargGoldSum=(*it)["rechargGoldSum"];
		if(!(*it)["arenaScore"].is_null())
			playerInfo->arenaScore=(*it)["arenaScore"];
		if(!(*it)["arenaMoney"].is_null())
			playerInfo->arenaMoney=(*it)["arenaMoney"];
		if(!(*it)["battlefieldScore"].is_null())
			playerInfo->battlefieldScore=(*it)["battlefieldScore"];
		if(!(*it)["battlefieldMoney"].is_null())
			playerInfo->battlefieldMoney=(*it)["battlefieldMoney"];
		if(!(*it)["mapID"].is_null())
			playerInfo->mapID=(*it)["mapID"];
		if(!(*it)["posx"].is_null())
			playerInfo->posx=(*it)["posx"];
		if(!(*it)["posy"].is_null())
			playerInfo->posy=(*it)["posy"];
		if(!(*it)["hp"].is_null())
			playerInfo->hp=(*it)["hp"];
		if(!(*it)["mp"].is_null())
			playerInfo->mp=(*it)["mp"];
		if(!(*it)["xp"].is_null())
			playerInfo->xp=(*it)["xp"];
		if(!(*it)["societyID"].is_null())
			playerInfo->societyID=(*it)["societyID"];
		if(!(*it)["realmLevel"].is_null())
			playerInfo->realmLevel=(*it)["realmLevel"];
		if(!(*it)["realmExp"].is_null())
			playerInfo->realmExp=(*it)["realmExp"];
		if(!(*it)["activeSkillTreeId"].is_null())
			playerInfo->activeSkillType=(*it)["activeSkillTreeId"];
		if(!(*it)["skillInfo"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["skillInfo"];
			__int64 iLength = temp.length();
			SkillInfoIt* ptemp = (SkillInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(SkillInfoIt);i++)
			{
				playerInfo->skillInfo.push_back(*ptemp);
				if((i+1)<iLength/sizeof(SkillInfoIt))
					ptemp++;
			}			
		}
		if(!(*it)["wingInfo"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["wingInfo"];
			__int64 iLength = temp.length();
			WingInfoIt* ptemp = (WingInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(WingInfoIt);i++)
			{
				playerInfo->wingInfo[ptemp->id] = *ptemp;
				if((i+1)<iLength/sizeof(WingInfoIt))
					ptemp++;
			}			
		}
		if(!(*it)["realmupInfo"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["realmupInfo"];
			__int64 iLength = temp.length();
			MagicWeaponInfoIt* ptemp = (MagicWeaponInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(MagicWeaponInfoIt);i++)
			{
				playerInfo->realmupInfo[ptemp->id] = *ptemp;
				if((i+1)<iLength/sizeof(MagicWeaponInfoIt))
					ptemp++;
			}			
		}
		if(!(*it)["mountInfo"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["mountInfo"];
			__int64 iLength = temp.length();
			MountInfoIt* ptemp = (MountInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(MountInfoIt);i++)
			{
				playerInfo->mountInfo[ptemp->id] = *ptemp;
				if((i+1)<iLength/sizeof(MountInfoIt))
					ptemp++;
			}			
		}
		if(!(*it)["copy"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["copy"];
			__int64 iLength = temp.length();
			CopyInfoIt* ptemp = (CopyInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(CopyInfoIt);i++)
			{
				playerInfo->copy[ptemp->id] = *ptemp;
				if((i+1)<iLength/sizeof(CopyInfoIt))
					ptemp++;
			}			
		}
		if(!(*it)["daily"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["daily"];
			__int64 iLength = temp.length();
			DailyInfoIt* ptemp = (DailyInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(DailyInfoIt);i++)
			{
				playerInfo->daily[ptemp->id] = *ptemp;
				if((i+1)<iLength/sizeof(DailyInfoIt))
					ptemp++;
			}			
		}
		if(!(*it)["task"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["task"];
			__int64 iLength = temp.length();
			TaskInfoIt* ptemp = (TaskInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(TaskInfoIt);i++)
			{
				playerInfo->task[ptemp->id] = *ptemp;
				if((i+1)<iLength/sizeof(TaskInfoIt))
					ptemp++;
			}			
		}
		if(!(*it)["equip"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["equip"];
			ItemInfoIt* ptemp = (ItemInfoIt*)temp.data();
			for(int i=0;i < EQUIPTYPE_MAX;i++)
			{
				playerInfo->equip[i] = *ptemp;
				if((i+1)<EQUIPTYPE_MAX)
					ptemp++;
			}			
		}
		if(!(*it)["bag"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["bag"];
			__int64 iLength = temp.length();
			ItemInfoIt* ptemp = (ItemInfoIt*)temp.data();
			for(int i=0;i<iLength/sizeof(ItemInfoIt);i++, ptemp++)
			{
				playerInfo->bag[ptemp->guid] = *ptemp;
			}
		}
		if(!(*it)["friendsList"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["friendsList"];
			__int64 iLength = temp.length();
			FriendsInfo* ptemp = (FriendsInfo*)temp.data();
			for(int i=0;i<iLength/sizeof(FriendsInfo);i++, ptemp++)
			{
				playerInfo->vecFriendsList.push_back(*ptemp);
			}
		}
		if(!(*it)["blackList"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["blackList"];
			__int64 iLength = temp.length();
			FriendsInfo* ptemp = (FriendsInfo*)temp.data();
			for(int i=0;i<iLength/sizeof(FriendsInfo);i++, ptemp++)
			{
				playerInfo->vecBlackList.push_back(*ptemp);
			}
		}
		if(!(*it)["choseskill"].is_null())
		{
			mysqlpp::sql_blob temp = (*it)["choseskill"];
			__int32* ptemp = (__int32*)temp.data();
			for(int i=0;i < MAX_CHOSESKILL;i++)
			{
				playerInfo->choseSkill[i] = *ptemp;
				if((i+1) < MAX_CHOSESKILL)
					ptemp++;
			}			
		}
		else
		{
			for(int i=0;i < MAX_CHOSESKILL;i++)
			{
				playerInfo->choseSkill[i] = NULL;
			}			
		}
	}
	return true;
}

bool CMysql::UpdatePlayerInfo(SQL_player_info* playerInfo)
{
	if(playerInfo==NULL)
	{
		return false;
	}
	std::stringstream strSql;

	StackStream<> skillStream;
	vector<SkillInfoIt>::iterator iteSkill;
	for(iteSkill=playerInfo->skillInfo.begin();iteSkill!=playerInfo->skillInfo.end();iteSkill++)
	{
		skillStream.Wirte(&(*iteSkill), sizeof(SkillInfoIt));
	}
	StackStream<> wingStream;
	map<int,WingInfoIt>::iterator iteWing;
	for(iteWing=playerInfo->wingInfo.begin();iteWing!=playerInfo->wingInfo.end();iteWing++)
	{
		wingStream.Wirte(&iteWing->second, sizeof(WingInfoIt));
	}
	StackStream<> magicStream;
	map<int,MagicWeaponInfoIt>::iterator iteMagic;
	for(iteMagic=playerInfo->realmupInfo.begin();iteMagic!=playerInfo->realmupInfo.end();iteMagic++)
	{
		magicStream.Wirte(&iteMagic->second,sizeof(MagicWeaponInfoIt));
	} 
	StackStream<> mountStream;
	map<int,MountInfoIt>::iterator iteMount;
	for(iteMount=playerInfo->mountInfo.begin();iteMount!=playerInfo->mountInfo.end();iteMount++)
	{
		mountStream.Wirte(&iteMount->second,sizeof(MountInfoIt));
	}
	StackStream<> copyStream;
	map<int,CopyInfoIt>::iterator iteCopy;
	for(iteCopy=playerInfo->copy.begin();iteCopy!=playerInfo->copy.end();iteCopy++)
	{
		copyStream.Wirte(&iteCopy->second,sizeof(CopyInfoIt));
	} 
	StackStream<> dailyStream;
	map<int,DailyInfoIt>::iterator iteDaily;
	for(iteDaily=playerInfo->daily.begin();iteDaily!=playerInfo->daily.end();iteDaily++)
	{
		dailyStream.Wirte(&iteDaily->second,sizeof(DailyInfoIt));
	} 
	StackStream<> taskStream;
	map<int,TaskInfoIt>::iterator iteTask;
	for(iteTask=playerInfo->task.begin(); iteTask!=playerInfo->task.end();iteTask++)
	{
		taskStream.Wirte(&iteTask->second, sizeof(TaskInfoIt));

	} 

	StackStream<BAG_ITEMS_STREAM_SZIE> bagStream;
	map<uint64,ItemInfoIt>::iterator iteItembag;
	for(iteItembag=playerInfo->bag.begin();iteItembag!=playerInfo->bag.end();iteItembag++)
	{
		bagStream.Wirte(&iteItembag->second,sizeof(ItemInfoIt));

	} 
	StackStream<> friendsStream;
	vector<FriendsInfo>::iterator iteFriend;
	for(iteFriend = playerInfo->vecFriendsList.begin();iteFriend != playerInfo->vecFriendsList.end();iteFriend++)
	{
		friendsStream.Wirte(&(*iteFriend),sizeof(FriendsInfo));
	}
	StackStream<> blackStream;
	vector<FriendsInfo>::iterator iteBlack;
	for(iteBlack = playerInfo->vecBlackList.begin();iteBlack != playerInfo->vecBlackList.end();iteBlack++)
	{
		blackStream.Wirte(&(*iteBlack),sizeof(FriendsInfo));
	}
	char* strSkill=ConvertBinaryToString((const char*)skillStream.Get(),skillStream.GetSize());

	strSql << " update player SET playerid="<<playerInfo->playerID<<",playerName='"<<playerInfo->playerName<<"',sex="<<(int32)playerInfo->sex<<
		",headId="<<playerInfo->headID<<",offlineTime="<<playerInfo->offlineTime<<",level="<<playerInfo->level<<",exp="<<playerInfo->exp<<","<<
		"money="<<playerInfo->money<<",gold="<<playerInfo->gold<<", bindGold="<<playerInfo->bindGold<<",rechargGoldSum="<<playerInfo->rechargGoldSum<<
		",arenaScore="<<playerInfo->arenaScore<<",arenaMoney="<<playerInfo->arenaMoney<<",battlefieldScore="<<playerInfo->battlefieldScore<<","<<
		" battlefieldMoney="<<playerInfo->battlefieldMoney<<",mapID="<<playerInfo->mapID<<",posx="<<playerInfo->posx<<",posy="<<playerInfo->posy<<
		",hp="<<playerInfo->hp<<",mp="<<playerInfo->mp<<",xp="<<playerInfo->xp<<",societyID="<<playerInfo->societyID<<",realmLevel="<<playerInfo->realmLevel<<
		",realmExp="<<playerInfo->realmExp<<",activeSkillTreeId="<<playerInfo->activeSkillType<<",skillInfo='"<<strSkill<<"',";

	char* strWing=ConvertBinaryToString((const char*)wingStream.Get(),wingStream.GetSize());
	strSql <<"wingInfo='"<<strWing<<"',";
	char* strMagic=ConvertBinaryToString((const char*)magicStream.Get(),magicStream.GetSize());
	strSql<<"realmupInfo='"<<strMagic<<"',";
	char* strMount=ConvertBinaryToString((const char*)mountStream.Get(),mountStream.GetSize());
	strSql<<"mountInfo='"<<strMount<<"',";
	char* strCopy=ConvertBinaryToString((const char*)copyStream.Get(),copyStream.GetSize());
	strSql<<"copy='"<<strCopy<<"',";
	char* strDaily=ConvertBinaryToString((const char*)dailyStream.Get(),dailyStream.GetSize());
	strSql<<"daily='"<<strDaily<<"',";
	char* strTask=ConvertBinaryToString((const char*)taskStream.Get(),taskStream.GetSize());
	strSql<<"task='"<<strTask<<"',";
	char* strEqu=ConvertBinaryToString((const char*)&playerInfo->equip[0],EQUIPTYPE_MAX * sizeof(ItemInfoIt));
	strSql<<"equip='"<<strEqu<<"',";
	char* strBag=ConvertBinaryToString((const char*)bagStream.Get(),bagStream.GetSize());
	strSql<<"bag='"<<strBag<<"',";
	char* strFriends = ConvertBinaryToString((const char*)friendsStream.Get(),friendsStream.GetSize());
	strSql <<"friendsList='"<<strFriends<<"', ";
	char* strBlack = ConvertBinaryToString((const char*)blackStream.Get(),blackStream.GetSize());
	strSql <<"blackList='"<<strBlack<<"', ";
	char* strChoseSkill = ConvertBinaryToString((const char*)&playerInfo->choseSkill[0],sizeof(playerInfo->choseSkill));
	strSql <<"choseSkill='"<<strChoseSkill<<"'";
	strSql<<"where playerID="<<playerInfo->playerID;

   // gLog.AddLog(LOG_LEVEL_DEBUG2, "UpdatePlayerInfo run sql=%s", strSql.str().c_str());
	if(DbExecute(strSql.str().c_str(),eMysqlUpdate)<0)
	{
		return false;
	}

	return true;
}

bool CMysql::UpdateGangInfo(SQL_gang_info* gangInfo)
{
	if(gangInfo == NULL)
	{
		return false;
	}

	std::stringstream strSql;
	StackStream<> memberStream;
	vector<GangMember>::iterator iteMember;
	for(iteMember = gangInfo->vecMembersList.begin();iteMember != gangInfo->vecMembersList.end();iteMember++)
	{
		memberStream.Wirte(&(*iteMember),sizeof(GangMember));
	}
	strSql <<" update ganginfo set ID = "<<gangInfo->id<<", gangName='"<<gangInfo->szGangName<<"',gangLevel="<<gangInfo->iGangLevel<<
			",gangShopLevel = "<<gangInfo->iGangShopLevel<<",headerID="<<gangInfo->iHeaderid<<",headerName='"<<gangInfo->szHeaderName<<
			"',gangActive="<<gangInfo->iGangActive<<",gangAnnouncement='"<<gangInfo->szGangAnnounce<<"', viceHeader1="<<gangInfo->iViceHeaderid[0]<<
			",viceHeader2="<<gangInfo->iViceHeaderid[1]<<",";
	char* strFlex=ConvertBinaryToString((const char*)&gangInfo->iStarFlex[0],5 * sizeof(__int64));
	strSql<<"starFlex = '"<<strFlex<<"',createTime='"<<gangInfo->szCreateTime<<"',copyID="<<gangInfo->iCopyid<<",copyStartTime='"<<gangInfo->szCopyStartTime<<"',";
	char* strMember=ConvertBinaryToString((const char*)memberStream.Get(),memberStream.GetSize());
	strSql << "members='"<<strMember<<"',backupInfo='"<<gangInfo->szBackupInfo<<"' where ID = "<<gangInfo->id;
	if(DbExecute(strSql.str().c_str(),eMysqlUpdate)<0)
	{
		return false;
	}

	return true;
}

// 执行写入操作(update、insert、delete)                                  

INT64 CMysql::DbExecute(const char *sql, eMysqlQueryType queryType)
{
	DbPing();
	SimpleResult res;
	INT64 affectedNumRows = 0;
	Query query = m_pMysql.query();
	query.disable_exceptions();
	query << sql;
	res = query.execute();
	switch(queryType){
	case eMysqlProcedure:           //存储过程
	case eMysqlDelete:                  //删除
	case eMysqlUpdate:      //添加和删除返回响应记录数
		if(m_pMysql.errnum())
		{
			affectedNumRows = -1;
		}
		break;
	case eMysqlInsert:          //新增记录返回自增id
		{
			affectedNumRows = res.insert_id();
			if (m_pMysql.errnum())
			{
				affectedNumRows = -1;
			}
		}
		break;
	default:
		break;
	}

	if (affectedNumRows == -1)
	{
		gLog.Error("Mysql exec error: %s", getDbLastError());
		gLog.Warning("Mysql last query: %s", GetLastQuerySql().c_str());
	}
	return affectedNumRows;
}
// 获取最后一次执行错误信息
const char *CMysql::getDbLastError()
{
	return m_pMysql.error();
}
// 查询数据
StoreQueryResult CMysql::DbSelect(const char *tableName, map<string, string> &fields, map<string, string> &condition, unsigned long offSet /* = 0 */, unsigned long limit/* =0  */, const string &orderBy/* =NULL */, const string &groupBy/* =NULL */)
{
	memset(m_strQuery, 0, MAX_QUERY_SIZE);
	string orderString("");
	string groupString("");
	char limitChr[1024];
	memset(limitChr, 0, 1024);
	if (!orderBy.empty())
	{
		orderString = "ORDER BY  "+ orderBy;
	}
	if(!groupBy.empty())
	{
		groupString = "GROUP BY " + groupBy;
	}
			
	if ((limit != 0) && (offSet >= 0))
	{
		snprintf(limitChr, sizeof(limitChr), "LIMIT %d, %d", (int)offSet, (int)limit);
	}
	string fieldString("");
	fieldString = MakeSelect(fields);
	string conditionString = MakeCondition(condition);
	snprintf(m_strQuery, sizeof(limitChr), "SELECT %s FROM %s WHERE %s %s %s %s",fieldString.c_str() , tableName, conditionString.c_str(), groupString.c_str(), orderString.c_str(), limitChr);
	return DbQuery(m_strQuery);
}

// 更新数据                                                             

INT64 CMysql::DbUpdate(const char *tableName, map<string, string> &fields, map<string, string> &condition)
{
	memset(m_strQuery, 0, MAX_QUERY_SIZE);

	string fieldString("");
	fieldString = MakeUpdate(fields);
	string conditionString = MakeCondition(condition);
	snprintf(m_strQuery, sizeof(m_strQuery), "UPDATE %s SET %s WHERE %s", tableName, fieldString.c_str(), conditionString.c_str());
	return DbExecute(m_strQuery, eMysqlUpdate);
}

// 添加纪录                                                                     

INT64 CMysql::DbInsert(const char *tableName, map<string, string> &fields)
{
	memset(m_strQuery, 0, MAX_QUERY_SIZE);

	string fieldString("");
	fieldString = MakeInsert(fields);
	snprintf(m_strQuery, sizeof(m_strQuery), "INSERT INTO %s %s", tableName, fieldString.c_str());
	return DbExecute(m_strQuery, eMysqlInsert);   
}

// 删除记录                                                                     

INT64 CMysql::DbDelete(const char *tableName, map<string, string> &condition)
{
	memset(m_strQuery, 0, MAX_QUERY_SIZE);
	string conditionString = MakeCondition(condition);
	snprintf(m_strQuery, sizeof(m_strQuery), "DELETE FROM %s WHERE %s",tableName, conditionString.c_str());
	return DbExecute(m_strQuery, eMysqlDelete);
}

// 插入记录,如果存在就更新                                              

INT64 CMysql::DbInsertUpdate(const char *tableName, map<string, string> &fields)
{
	//memset(m_strQuery, 0, MAX_QUERY_SIZE);
	CHAR aszSQLBuff[1024 * 1024];
	aszSQLBuff[0] = '\0';
	string fieldString1("");
	string filedString2("");
	fieldString1 = MakeInsert(fields);
	filedString2 = MakeInsertUpdate(fields);
	snprintf(aszSQLBuff, sizeof(aszSQLBuff), "INSERT INTO %s %s on duplicate key update %s", tableName, fieldString1.c_str(), filedString2.c_str());
	return DbExecute(aszSQLBuff, eMysqlInsert);
}
// 构造查询字段数据                                                     
string CMysql::MakeSelect(map<string, string> &fields)
{
	string strField("");
	bool isFirst = true;
	for (map<string, string>::iterator iter = fields.begin(); iter != fields.end(); ++ iter)
	{
		if (!isFirst)
		{
			strField += ",";
		}
		isFirst = false;
		strField += iter->first;
	}
	return strField;
}
// 构造插入语句                                                                     
string CMysql::MakeInsert(map<string, string> &fields)
{
	string strFieldName("");
	string strFieldValue("");
	string strField("");
	bool isFirst = true;
	for (map<string, string>::iterator iter = fields.begin(); iter != fields.end(); ++ iter)
	{
		if (!isFirst)
		{
			strFieldName += ",";
			strFieldValue += ",";
		}
		isFirst = false;
		strFieldName += iter->first;
		static char value[LARGE_MSG_SIZE];
		memset(value, 0, LARGE_MSG_SIZE);
		mysql_escape_string(value, iter->second.c_str(), iter->second.length());
		strFieldValue += "'"+string(value)+"'";
	}
	strField = "(" + strFieldName + ")VALUES(" + strFieldValue + ")";
	return strField;
}

// 构造插入已存在记录时更新语句                                              

string CMysql::MakeInsertUpdate(map<string, string> &fields)
{
	string strField(""); 
	bool isFirst = true;
	for (map<string, string>::iterator iter = fields.begin(); iter != fields.end(); ++ iter)
	{
		if (!isFirst)
		{
			strField += ",";
		}
		isFirst = false;
		strField += iter->first;
		strField += '=';
		char value[LARGE_MSG_SIZE];
		memset(value, 0, LARGE_MSG_SIZE);
		mysql_escape_string(value, iter->second.c_str(), iter->second.length());
		strField += "'"+ string(value)+"'";
	}
	return strField;
}

// 构造更新语句                                                         
  
string CMysql::MakeUpdate(map<string, string> &fields)
{
	string strField("");
	string fieldValue;
	string fieldName;
	bool isFirst = true;
	for (map<string, string>::iterator iter = fields.begin(); iter != fields.end(); ++ iter)
	{
		if (!isFirst)
		{
			strField += ",";
		}
		isFirst = false;

		char value[LARGE_MSG_SIZE];
		char field[SMALL_MSG_SIZE];
		memset(field, 0, SMALL_MSG_SIZE);
		memset(value, 0, LARGE_MSG_SIZE);
		mysql_escape_string(value, iter->second.c_str(), iter->second.length());
		mysql_escape_string(field, iter->first.c_str(), iter->first.length());

		fieldValue = string(value);
		fieldName = string(field);

		if (fieldName.find("=") != string::npos)
		{
			strField += fieldName;
		}
		else
		{
			if (fieldValue.find(iter->first) != string::npos)
			{
				strField += iter->first + "=" + fieldValue;
			}		
			else
			{
				strField += iter->first + "='" + fieldValue + "'";
			}
		}
	}
	return strField;
}

// 构造条件语句                                                         

string CMysql::MakeCondition(map<string, string> &conditions)
{
	string strCondition("");
	bool isFirst = true;
	for (map<string, string>::iterator iter = conditions.begin(); iter != conditions.end(); ++ iter)
	{
		if (!isFirst)
		{
			strCondition += " AND ";
		}
		if ((iter->second.find("<") != string::npos) ||
			(iter->second.find("<=") != string::npos) || 
			(iter->second.find(">") != string::npos) || 
			(iter->second.find(">=") != string::npos) || 
			(iter->second.find("=") != string::npos) ||
			(iter->second.find("LIKE") != string::npos) ||
			(iter->second.find("IN") != string::npos) ||
			(iter->second.find("OR") != string::npos) ||
			(iter->second.find("AND") != string::npos)||
			(iter->second.length() == 0))
		{
			strCondition += "(" + iter->first + iter->second + ")";
		}
		else
		{
			strCondition += "(" + iter->first +" = '" + iter->second + "')";
		}
		isFirst = false;
	}

	if (strCondition.length() == 0)
	{
		strCondition = "1";
	}
	return strCondition;
}
string CMysql::GetLastQuerySql()
{
	return string(m_strQuery);
}

void CMysql::DbPing()
{
	if(m_pMysql && !m_pMysql.ping())
	{
		gLog.Info("MySql Lost Connection,Re connect to the MySql! ");
		m_pMysql.disconnect();
		DbConnect();
	}
}
