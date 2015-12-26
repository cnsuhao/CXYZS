/********************************************************************
created:	2014/7/20
author:		LL
desc:		用于定义数据库结构及数据类型。
*********************************************************************/
#ifndef _MysqlDef__H_
#define _MysqlDef__H_
#include "ServerDef.h"
//数据库执行类别
enum eMysqlQueryType
{
	eMysqlNULL = -1,
	eMysqlSelect = 0,
	eMysqlInsert,
	eMysqlUpdate,
	eMysqlDelete,
	eMysqlInsertUpdate,
	eMysqlProcedure,
	eMysqlNoRecord,
};

#include <vector>

enum MYSQL_DATATYPE
{
	MYSQL_INT8,
	MYSQL_INT16,
	MYSQL_INT32,
	MYSQL_INT64,
	MYSQL_UINT8,	
	MYSQL_UINT16,
	MYSQL_UINT32,
	MYSQL_UINT64,
	MYSQL_FLOAT,
	MYSQL_BIN,
	MYSQL_STR,
	MYSQL_ZIP,
	MYSQL_DATETIME,
};


#define MAX_NAME 32


struct mysqlCol
{
	const char * name;		//字段名字
	int mysqlType;			//MYSQL数据类型
	unsigned int dataSize;	//数据大小	
};

static mysqlCol SQL_account[] =
{	
	{"accountId", MYSQL_UINT64, sizeof(unsigned __int64)},
	//{"accountName", MYSQL_STR, MAX_NAME},
	/*{"pwd", MYSQL_STR, MAX_NAME},
	{"userName", MYSQL_STR, MAX_NAME},
	{"idCard", MYSQL_STR, 20},
	{"mobile", MYSQL_STR, 20},
	{"email", MYSQL_STR, 255},*/
	{"lastServerId", MYSQL_UINT16, sizeof(unsigned __int16)},
	{NULL, 0, 0}
};

static mysqlCol SQL_player[] =
{
	{"playerID", MYSQL_UINT64, sizeof(unsigned __int64)},//玩家id用账号id来填充
	{"playerName", MYSQL_STR, MAX_NAME},				//名字
	{"sex", MYSQL_INT8, sizeof(__int8)},				//性别 0女 1男	
	{"headID", MYSQL_INT16, sizeof(__int16)},			//头像ID
	{"offlineTime", MYSQL_INT64, sizeof(__int64)},		//离线时间，用于上线时判断是否刷新每日次数
	{"level", MYSQL_INT32, sizeof(__int32)},			//等级
	{"exp", MYSQL_INT32, sizeof(__int32)},				//经验值
	{"money", MYSQL_INT32, sizeof(__int32)},			//当前金币数
	{"gold", MYSQL_INT32, sizeof(__int32)},				//绑定元宝数
	{"bindGold", MYSQL_INT32, sizeof(__int32)},			//元宝数量
	{"rechargGoldSum", MYSQL_INT32, sizeof(__int32)},	//总充值元宝
	{"arenaScore", MYSQL_INT32, sizeof(__int32)},		//竞技场积分
	{"arenaMoney", MYSQL_INT32, sizeof(__int32)},		//竞技场货币数	
	{"battlefieldScore", MYSQL_INT32, sizeof(__int32)},	//当前战场积分	
	{"battlefieldMoney", MYSQL_INT32, sizeof(__int32)},	//战场货币数
	{"mapID", MYSQL_INT32, sizeof(__int32)},			//所在地图id
	{"posx", MYSQL_INT32, sizeof(__int32)},				//所在位置x
	{"posy", MYSQL_INT32, sizeof(__int32)},				//所在位置y
	{"hp", MYSQL_INT32, sizeof(__int32)},				//血
	{"mp", MYSQL_INT32, sizeof(__int32)},				//蓝
	{"xp", MYSQL_INT32, sizeof(__int32)},				//怒气
	{"societyID", MYSQL_INT32, sizeof(__int32)},		//公会id
	{"realmLevel", MYSQL_UINT32, sizeof(__int32)},		//境界等级	
	{"realmExp", MYSQL_INT32, sizeof(__int32)},			//境界经验
	{"skillInfo", MYSQL_STR, 255},						//功法信息	"功法ID 功法等级 功法经验"
	{"wingInfo", MYSQL_STR, 255},						//翅膀信息	"（等级 经验）*10"
	{"magicWeaponInfo", MYSQL_STR, 255},				//法宝信息	"法宝ID 法宝等级 法宝经验 法宝成长系数"
	{"mountInfo", MYSQL_STR, 255},						//器灵信息	"器灵ID	创建时间"
	{"buffInfo", MYSQL_STR, 255},						//buff信息	"buffID	创建时间"
	{"copy", MYSQL_STR, 255},							//副本信息	"副本id 进入次数 层数"(公会副本在公会信息中)
	{"daily", MYSQL_STR, 255},							//日常信息	""
	{"task", MYSQL_STR, 255},							//任务信息	""
	{"equip", MYSQL_STR, 255},							//身上装备 "guID 物品ID 前缀ID 星数 是否绑定 数量 创建时间"
	{"bag", MYSQL_STR, 255},							//背包物品	"guID 物品ID 前缀ID 星数 是否绑定 数量 创建时间"
	{"friendsList", MYSQL_STR, 255},					//好友列表	"玩家ID"
	{"blackList", MYSQL_STR, 255},						//黑名单列表"玩家ID"
	//!前4个字节表示长度，变长的数据暂定255长度，后面需要修改

	{NULL, 0, 0}
};

struct SQL_player_info
{
	unsigned __int64 playerID;	//玩家id用账号id来填充
	char playerName[MAX_NAME];	//名字
	__int8 sex;					//性别 0女 1男	
	__int16 headID;				//头像ID
	__int64 offlineTime;		//离线时间，用于上线时判断是否刷新每日次数
	__int32 level;				//等级
	__int32 exp;				//经验值
	__int32 money;				//当前金币数
	__int32 gold;				//绑定元宝数
	__int32 bindGold;			//元宝数量
	__int32 rechargGoldSum;		//总充值元宝
	__int32 arenaScore;			//竞技场积分
	__int32 arenaMoney;			//竞技场货币数	
	__int32 battlefieldScore;	//当前战场积分	
	__int32 battlefieldMoney;	//战场货币数
	__int32 mapID;				//所在地图id
	__int32 posx;				//所在位置x
	__int32 posy;				//所在位置y
	__int32 hp;					//血
	__int32 mp;					//蓝
	__int32 xp;					//怒气
	__int32 societyID;			//公会id
	__int32 realmLevel;			//境界等级	
	__int32 realmExp;			//境界经验	
	__int32 activeSkillType;    //选中技能
	std::vector<SkillInfoIt> skillInfo;	//技能信息
	std::map<int, WingInfoIt> wingInfo;			//翅膀信息	"（等级 经验）*10"
	std::map<int, MagicWeaponInfoIt> realmupInfo;//法宝信息	"法宝ID 法宝等级 法宝经验 法宝成长系数"
	std::map<int, MountInfoIt> mountInfo;		//器灵信息	"器灵ID	剩余时间"
	std::map<int, CopyInfoIt> copy;				//副本信息	"副本id	进入次数 层数"
	std::map<int, DailyInfoIt> daily;			//日常信息	"日常id	完成进度 是否领奖"
	std::map<int, TaskInfoIt> task;				//任务信息	"任务id	完成进度 任务状态，0未接 1已接 2已完成"
	ItemInfoIt equip[EQUIPTYPE_MAX];			//身上装备 "guID 物品ID 前缀ID 星数 是否绑定 数量 剩余时间"
	std::map<uint64, ItemInfoIt> bag;			//背包物品	"guID 物品ID 前缀ID 星数 是否绑定 数量 剩余时间"
	std::vector<FriendsInfo> vecFriendsList;	//好友列表	"玩家ID,玩家ID"
	std::vector<FriendsInfo> vecBlackList;		//黑名单列表"玩家ID,玩家ID"
	__int32   choseSkill[MAX_CHOSESKILL];		//玩家选中的技能信息

};

struct SQL_gang_info
{
	__int64		id;							//帮派ID
	char		szGangName[MAX_NAME];		//帮派名称
	__int32		iGangLevel;					//帮派等级
	__int32		iGangShopLevel;				//帮派商店等级
	__int64		iHeaderid;					//帮主ID
	char		szHeaderName[MAX_NAME];		//帮主名称
	__int32		iGangActive;				//活跃度
	char		szGangAnnounce[255];		//帮派公告
	__int64		iViceHeaderid[2];			//副帮主列表
	__int64		iStarFlex[5];				//堂主列表
	char		szCreateTime[50];			//创建时间
	__int32		iCopyid;					//副本ID
	char		szCopyStartTime[50];		//副本开始时间
	std::vector<GangMember> vecMembersList;	//帮派成员列表
	char		szBackupInfo[255];			//备注

	SQL_gang_info()
	{
		id = 0;
		memset(szGangName,0,32);
		iGangLevel = 0;
		iGangShopLevel = 0;
		iHeaderid = 0;
		memset(szHeaderName,0,32);
		iGangActive = 0;
		memset(szGangAnnounce,0,255);
		iViceHeaderid[0] = 0;
		iViceHeaderid[1] = 0;
		iStarFlex[0] = 0;
		iStarFlex[1] = 0;
		iStarFlex[2] = 0;
		iStarFlex[3] = 0;
		iStarFlex[4] = 0;
		memset(szCreateTime,0,50);
		iCopyid = 0;
		memset(szCopyStartTime,0,50);
		vecMembersList.clear();
		memset(szBackupInfo,0,255);
	}
};

struct SQL_apply_info
{
	__int32		id;							//申请ID
	__int64		iApplyid;					//申请人
	__int32		iApplyType;					//申请类别
	__int64		iTargetid;					//目标ID
	char		szApplyTime[50];			//申请时间
	char		szBackupInfo[255];			//备注

	SQL_apply_info()
	{
		id = 0;
		iApplyid = 0;
		iApplyType = 0;
		iTargetid = 0;
		memset(szApplyTime,0,50);
		memset(szBackupInfo,0,255);
	}
};

#endif	//_MysqlDef__H_