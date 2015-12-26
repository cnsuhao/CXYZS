#include "MapManager.h"
#include "CSVFileManager.h"
#include "Map.h"
#include "Player.h"
#include "MsgInfo.h"
#include "LogManage.h"
#include "GameServer.h"

MapManager *g_MapManager = NULL;

MapManager::MapManager()
{		
}

MapManager::~MapManager()
{
}

void MapManager::Init()
{
	//取地图表开启所有场景地图，其它地图按需开启
	std::map<int32,MapInfo>::iterator it = g_CSVFileManager->m_MapInfoMap.begin();
	for (; it != g_CSVFileManager->m_MapInfoMap.end(); ++it)
	{
		OpenMap(it->second.id);
	}

}

void MapManager::OpenMap(int32 mapid)
{
	const MapInfo* info = g_CSVFileManager->GetMapByID(mapid);
	if (!info)
	{
		gLog.AddLog(LOG_LEVEL_ERROR, "GetMapByID(%d)", mapid);
		return;
	}
	switch (info->iMapType)
	{
	case MAPTYPE_NORMAL:		//场景地图
		{
			stdext::hash_map<uint64, Map*>::iterator it = m_MapList.find(mapid);
			if (it == m_MapList.end())
			{
				//创建地图
				Map* map = new Map;
                //正常地图map就是Guid
				map->Init(mapid, mapid, info->sMapSize.width, info->sMapSize.height);
				m_MapList[mapid] = map;
                
                /*
				//加载怪物
				std::map<uint32,MonsterInfo>::iterator it = g_CSVFileManager->m_MonsterInfoMap.begin();
				for (; it != g_CSVFileManager->m_MonsterInfoMap.end(); ++it)
				{					
					if (it->second.iMapid == mapid)
					{
						for (size_t j = 0; j < it->second.vecPos.size(); j++)
						{
							Monster* monster = new Monster;
							monster->m_MonsterBirthPos = it->second.vecPos[j]; 			
							monster->Init(&it->second);
							map->AddMonster(monster);
						}
					}
				}
                */
			}
		}
		break;
	case MAPTYPE_BOOS:			//世界boos地图
		break;
	case MAPTYPE_COPY:			//副本地图
		//创建副本需要一个函数获取MapGUID
		//NewCopyMapGuid(mapid);

		{
            /*
			uint64 mapGuid = NewCopyMapGuid(mapid);

			stdext::hash_map<uint64, Map*>::iterator it = m_MapList.find(mapGuid);
			if (it == m_MapList.end())
			{
				//创建地图
				Map* map = new Map;
				map->Init(mapid, mapGuid, info->sMapSize.width, info->sMapSize.height);
				m_MapList[mapGuid] = map;
				//加载怪物
				std::map<uint32,MonsterInfo>::iterator it = g_CSVFileManager->m_MonsterInfoMap.begin();
				for (; it != g_CSVFileManager->m_MonsterInfoMap.end(); ++it)
				{					
					if (it->second.iMapid == mapGuid)
					{
						for (size_t j = 0; j < it->second.vecPos.size(); j++)
						{
							Monster* monster = new Monster;
							monster->m_MonsterBirthPos = it->second.vecPos[j]; 			
							monster->Init(&it->second);
							map->AddMonster(monster);
						}
					}
				}
			}
            */
			
		}
		break;
	case MAPTYPE_PVP:			//竞技场地图
		break;
	case MAPTYPE_BATTLEFIELD:	//战场地图
		break;
	default:
		break;
	}
}

uint64 MapManager::NewCopyMapGuid(int32 mapid)
{
	static uint64 id = 0;
	
	return (uint64)mapid*1000000 + id++;
}

uint64 MapManager::NewMonsterGuid()
{
	static uint64 id = 1;
	return id++;
}

void MapManager::Update(float deltaTime)
{
    stdext::hash_map<uint64, Player*>::iterator it = m_PlayerList.begin();
    for (stdext::hash_map<uint64, Player*>::iterator end = m_PlayerList.end(); it != end; ++it)
    {
        Player* pPlayer = it->second;
        if (pPlayer && pPlayer->m_Map)
        {
            pPlayer->Update(deltaTime);
        }
    }
}

void MapManager::CreatePlayer(MsgRole_BaseInfo *info)
{
	Player* temp = GetPlayerByID(info->BaseInfo.playerID);
	if (temp)
	{
		DelPlayer(info->BaseInfo.playerID);
	}

	Player* player= new Player;
	player->Init(info->BaseInfo.playerID);
	player->LoadBaseAttribute(info->BaseInfo);
    m_PlayerList[info->OBJID] = player;
	m_PlayerListByName[info->BaseInfo.playerName] = player;

    player->OnOnline();
}

void MapManager::DelPlayer(uint64 accountid)
{
	//!从周围玩家队列中删除
	//!删除不安全需改
	stdext::hash_map<uint64, Player*>::iterator it = m_PlayerList.find(accountid);
	if (it != m_PlayerList.end())
	{
		stdext::hash_map<std::string, Player*>::iterator it2 = m_PlayerListByName.find(it->second->m_Name);
		if (it2 != m_PlayerListByName.end())
		{
			m_PlayerListByName.erase(it2);
		}
        Player* player = it->second;
        //PlayerLeaveMap(player);
        PlayerOffline(player);
        player = NULL; //>赋空值,不要再使用player
		SAFE_DELETE(it->second);
		m_PlayerList.erase(it);
	}
}

Player* MapManager::GetPlayerByID(uint64 accountid)
{
	stdext::hash_map<uint64, Player*>::iterator it = m_PlayerList.find(accountid);
	if (it != m_PlayerList.end())
	{
		return it->second;
	}
	return NULL;
}

Player* MapManager::GetPlayerByName(char* name)
{
	stdext::hash_map<std::string, Player*>::iterator it = m_PlayerListByName.find(name);
	if (it != m_PlayerListByName.end())
	{
		return it->second;
	}
	return NULL;
}

Monster* MapManager::GetMonsterByID(uint64 guid)
{
	stdext::hash_map<uint64, Monster*>::iterator it = m_MonsterList.find(guid);
	if (it != m_MonsterList.end())
	{
		return it->second;
	}
	return NULL;
}

void MapManager::PlayerEnterMap(Player* player, int32 mapid, const Vec2& position)
{
    stdext::hash_map<uint64, Map*>::iterator it = m_MapList.find(mapid);
    if (it != m_MapList.end())
    {
        Map* pMap = it->second;
        it->second->AddPlayer(player, position);
    }
}

void MapManager::PlayerLeaveMap(Player* player)
{
    if (player->m_Map)
    {
        player->m_Map->RemovePlayer(player);
    }
}

void MapManager::PlayerChangeMap(Player* player, int32 mapid, const Vec2& position)
{
    PlayerLeaveMap(player);
    PlayerEnterMap(player, mapid, position);
}

void MapManager::PlayerOffline(Player* player)
{
    PlayerLeaveMap(player);
    player->OnOffline();
}

// void MapManager::CopyPlayerAttribute(BaseInfoIt baseInfo,Player* player)
// {
// 	player->m_Attribute[ATTRIBUTE_MONEY] = (float)baseInfo.money;
// 	player->m_Attribute[ATTRIBUTE_HP] = (float)baseInfo.hp;
// 	player->m_Attribute[ATTRIBUTE_MP] = (float)baseInfo.mp;
// 	player->m_Attribute[ATTRIBUTE_GOLD] = (float)baseInfo.gold;
// 	player->m_Attribute[ATTRIBUTE_BINDGOLD] = (float)baseInfo.bindGold;
// 	player->m_Attribute[ATTRIBUTE_ARENAMONEY] = (float)baseInfo.arenaMoney;
// 	player->m_Attribute[ATTRIBUTE_BATTLEFIELDMONEY] = (float)baseInfo.battlefieldMoney;
// 	player->m_Attribute[ATTRIBUTE_XP] = (float)baseInfo.xp;
// 	player->m_Attribute[ATTRIBUTE_LEVEL] = (float)baseInfo.level;
// 	player->m_Attribute[ATTRIBUTE_EXP] = (float)baseInfo.exp;
// 	player->m_Attribute[ATTRIBUTE_REALMLEVEL] = (float)baseInfo.realmLevel;
// 	player->m_Attribute[ATTRIBUTE_REALMEXP] = (float)baseInfo.realmExp;
// 	strncpy(player->m_Name,baseInfo.playerName, MAX_NAME_SIZE);
// 	player->m_HeadIconIndex = baseInfo.headID; 
// 	player->m_Sex = baseInfo.sex;
// 
// }