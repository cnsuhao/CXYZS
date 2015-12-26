/********************************************************************
created:	2014/12/20
author:		LL
desc:		逻辑管理：管理所有游戏逻辑的处理
*********************************************************************/
#ifndef __LOGICMANAGER_H__
#define __LOGICMANAGER_H__

#include "GameDef.h"
#include "ClientDef.h"

class MapObj;
class Hero;
class Fight;
class Player;
class NPC;
class Monster;
class Collect;
struct NPCInfo;
struct MonsterInfo;
struct BaseInfoIt;
struct ResInfo;

USING_NS_CC;

struct GangSimpleInfo
{
	int32		id;
	char		szName[MAX_NAME_SIZE];
	int32		iLevel;
	char		szHeaderName[MAX_NAME_SIZE];
	int32		iGangNum;
	int32		iFight;
};

//怪物刷新计时
struct MapMonsterRefresh2
{
	int time;
	cc2d::Vec2 BirthPos;
	MonsterInfo *mapmonsterrefresh;
};

class LogicManager
{
public: 
	LogicManager();
	~LogicManager();
	void Init();				//初始化
	void Release();				//释放
	void Update(float delta);	//更新	
//战斗
	NPC* GetAttackTarget();
	void PlayerUseSkill(MsgG2CPlayer_UseSkill* msg);
//地图	
	void EnterGame();			//进入游戏
	void EnterCopy(int copyid);	//进入副本
	void AddNPCToMap(NPCInfo* mapnpc);//添加npc到地图
    void AddMonsterToMap(MsgMonster_Baseinfo* monsterInfo);//添加怪物到地图
	void AddOtherPlayerToMap(MsgOtherRole_BaseInfo* playerInfo);//添加玩家到地图

	void AddCollectToMap(ResInfo* parRes);		//添加采集物到地图

	void RemoveNpc(uint64 id);			//将NPC从队列中移除
	void RemoveMonster(Monster* obj);	//将怪物从队列中移除
	void RemovePlayer(uint64 id);		//将玩家从队列中移除
	void RemoveCollect(uint64 id);		//将采集物从队列中移除

	Player* GetPlayerByGUID(uint64 guid);	//根据guid取玩家
    inline Player* GetAllPlayerByGUID(uint64 guid); //获得玩家,包括主角自己
	NPC* GetNPCByGUID(uint64 guid);		//根据guid取npc
	NPC* GetNPCByID(uint64 id);		//根据id取npc
	Monster* GetMonsterByGUID(uint64 guid);//根据guid取怪物
	MapObj* GetObjectByType(uint64 guid, ObjectType type);	//根据类型取对象
    inline bool IsHeroGuid(uint64 guid) const; //>是否是主角唯一ID

	bool	GetPlayerIsTeam(uint64 guid);	//根据guid取玩家是否队友
	bool	GetPlayerIsCamp(uint64 guid);	//根据guid取玩家是否同一阵营

	void HeroUseSkillSetCD(int skillid);	//使用技能，用于记录技能cd

	std::map<int, float> m_SkillCDList;			//技能cd列表，0为公共cd，技能id不带等级
//人物相关    
	BaseInfoIt m_BaseInfo;			//服务器发来的主角数据，创建主角后代入
	Hero* m_Hero;	//主角
	std::vector<Player*>        m_PlayerList;//玩家列表
	std::vector<NPC*>           m_NPCList;	//NPC列表
	std::vector<Monster*>       m_MonsterList;	//怪物列表
	//std::vector<Collect*> m_Collect;	//采集物列表 
//
	int m_CurrCopyId;	//当前副本id
	int m_CurrMapId;	//当前地图id

	AttributeType ListChestType;
	int ListChestNum;
	
	
	std::vector<GangSimpleInfo>	m_vecGangList;
};

#include "Hero.h"
inline Player* LogicManager::GetAllPlayerByGUID(uint64 guid)
{
    if (m_Hero && m_Hero->m_GUID == guid)
    {
        return m_Hero;
    }
    else
    {
        return GetPlayerByGUID(guid);
    }
}

bool LogicManager::IsHeroGuid(uint64 guid) const
{
    if (m_Hero->m_GUID == guid)
    {
        return true;
    }
    return false;
}

extern LogicManager* g_LogicManager;
#endif  // __LOGICMANAGER_H__