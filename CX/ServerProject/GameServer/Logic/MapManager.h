/********************************************************************
created:	2015/7/27
author:		LL
desc:		地图管理,处理地图的创建和释放，建立玩家、怪物、地图的索引
*********************************************************************/

#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__
#include "GameDef.h"
#include "Vec2.h"
#include <hash_map>
class Player;
class Monster;
class Map;
struct MsgRole_BaseInfo;

class MapManager
{
public:
	MapManager();
	~MapManager();
	
	void Init();								//初始化
	void OpenMap(int32 mapid);					//打开地图,根据不同地图类型调用不同逻辑创建地图
	uint64 NewCopyMapGuid(int32 mapid);			//生成一个副本地图的全局id,场景地图直接用模板id
	uint64 NewMonsterGuid();					//生成一个怪物全局id
    void Update(float deltaTime);               //更新

	void CreatePlayer(MsgRole_BaseInfo *info);	//创建一个玩家
    void PlayerEnterMap(Player* player, int32 mapid, const Vec2& position);  //玩家进入地图
    void PlayerLeaveMap(Player* player);                    //离开地图
    void PlayerChangeMap(Player* player, int32 mapid, const Vec2& position);	//玩家切换地图


	//void CopyPlayerAttribute(BaseInfoIt baseInfo,Player* player);
	void DelPlayer(uint64 accountid);			//删除玩家
	Player* GetPlayerByID(uint64 accountid);	//获取玩家,根据id取
	Player* GetPlayerByName(char* name);		//获取玩家,根据名字取
	Monster* GetMonsterByID(uint64 guid);		//获取玩家,根据id取
    inline Map* GetMapByID(uint64 mapId);       //获得map

    
    void PlayerOffline(Player* player);         //玩家离线

	stdext::hash_map<uint64, Map*> m_MapList;
	stdext::hash_map<uint64, Player*> m_PlayerList;
	stdext::hash_map<uint64, Monster*> m_MonsterList;
	stdext::hash_map<std::string, Player*> m_PlayerListByName;
};

inline Map* MapManager::GetMapByID(uint64 mapId)
{
    stdext::hash_map<uint64, Map*>::iterator it = m_MapList.find(mapId);
    if (it != m_MapList.end())
    {
        return it->second;
    }
    return NULL;
}

extern MapManager* g_MapManager;
#endif	//__MAPMANAGER_H__
