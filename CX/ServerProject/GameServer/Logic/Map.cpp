#include "Map.h"
#include "CSVFileManager.h"
#include <cmath>
#include <cassert>
#include "LogManage.h"
#include "MsgInfo.h"
#include "MapManager.h"

void Block::AddObject(Object* obj)
{
	objects.push_back(obj);
}

bool Block::RemoveObject(Object* obj)
{
	std::list<Object*>::iterator it = objects.begin();
	for (std::list<Object*>::iterator end = objects.end(); 
		it != end; ++it)
	{
		if (*it == obj)
		{
			objects.erase(it);
			return true;
		}
	}
	return false;
}

bool Block::RemoveByGuid(uint64 guid)
{
    std::list<Object*>::iterator it = objects.begin();
    for (std::list<Object*>::iterator end = objects.end(); 
        it != end; ++it)
    {
        if ((*it)->m_GUID == guid)
        {
            objects.erase(it);
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////
Map::Map()
{
	
}

Map::~Map()
{

}

bool Map::Init(int mapId, int64 guid, float w, float h)
{
    m_MapId = mapId;
    m_Guid = guid;
    m_MapInfo = g_CSVFileManager->GetMapByID(m_MapId);
    assert(m_MapInfo);

	m_Width = w;
	m_Height = h;
    assert(m_Width <= 128 && m_Height <= 128);

	m_RowBlock = (int)ceil(m_Width / Block::GetWidth());
	m_ColBlock = (int)ceil(m_Height / Block::GetWidth());
	assert(m_RowBlock > 0);
	assert(m_ColBlock > 0);

	int count = m_RowBlock * m_ColBlock;
	m_Blocks.resize(count);
	for (int i = 0; i < count; ++i)
	{
		m_Blocks[i] = new Block();
	}
	return true;
}

void Map::AddMonster(Monster* monsters)
{	
	g_MapManager->m_MonsterList[monsters->m_GUID] = monsters;

    monsters->m_Map = this;
    monsters->InitPosition(Tile2Pos(monsters->m_MonsterBirthPos));

	Block* block = GetBlock(monsters);
	if (block)
	{
		block->AddObject(monsters);
        monsters->OnEnterMap();
	}
}

void Map::RemoveMonster(Monster* monster)
{
	stdext::hash_map<uint64, Monster*>::iterator it2 = g_MapManager->m_MonsterList.find(monster->m_GUID);
	if (it2 != g_MapManager->m_MonsterList.end())
	{
		g_MapManager->m_MonsterList.erase(it2);
	}

	Block* block = NULL;
	std::vector<Block*>::iterator it = m_Blocks.begin();
	for (std::vector<Block*>::iterator end = m_Blocks.end();
		it != end; ++it)
	{
		block = *it;
		if(block->RemoveObject(monster))
		{
			monster->OnLeaveMap();
			return;
		}
	}
}

void Map::AddPlayer(Player* player, const Vec2& position)
{	
    player->m_Map = this;

    //>设置出生点.
    player->InitPosition(position);

	Block* block = GetBlock(player);
	if (block)
	{
		block->AddObject(player);
	    player->OnEnterMap();
    }
}

Vec2 Map::GetEnterMapPosition(MapEnterMode mode, int conveyId /*= 0*/)
{
    Vec2 birthPos = Vec2::ZERO;
    if (Mode_Rebirth == mode || Mode_Init == mode) //>暂时把初始化放这里.
    {
        MapType mapType = (MapType)m_MapInfo->iMapType;
        switch (mapType)
        {
        case MAPTYPE_NORMAL:
        case MAPTYPE_BOOS:
        case MAPTYPE_COPY:
        case MAPTYPE_PVP:
            {
                birthPos.x = m_MapInfo->pBornPos[0].x;
                birthPos.y = m_MapInfo->pBornPos[0].y;
            }
            break;
        case MAPTYPE_BATTLEFIELD:   //>战场阵营
            {
                int filed = 0; //player->GetBattleFiled();
                if (filed == 0)
                {
                    birthPos.x = m_MapInfo->pBornPos[0].x;
                    birthPos.y = m_MapInfo->pBornPos[0].y;
                }
                else
                {
                    birthPos.x = m_MapInfo->pBornPos[1].x;
                    birthPos.y = m_MapInfo->pBornPos[1].y;
                }
            }
            break;
        case MAPTYPE_MAX:
        default:
            gLog.AddLog(LOG_LEVEL_WARNING, "error map type.(mapid=%d, type=%d)",m_MapId, (int)mapType);
            break;
        }
    }
    else if (Mode_Convey == mode)
    {
        const ConveyInfo* pInfo = g_CSVFileManager->GetConveyByID(conveyId);
        assert(pInfo);
        if (pInfo)
        {
            birthPos = pInfo->pTarget;
        }
    }
    //else if (Mode_Init == mode)
    //{

    //}
    return Tile2Pos(birthPos);
}

void Map::RemovePlayer(Player* player)
{
    Block* block = NULL;
    std::vector<Block*>::iterator it = m_Blocks.begin();
    for (std::vector<Block*>::iterator end = m_Blocks.end();
        it != end; ++it)
    {
        block = *it;
        if(block->RemoveObject(player))
        {
            player->OnLeaveMap();
            player->m_Map = NULL;
            return;
        }
    }
}


//-----------------------------------------------------------------------------------------

void Map::GetRangeSet(float distance, Object* self, std::vector<Object*>& outTset, unsigned int maxNum)
{
	unsigned int atX, atY;
	GetIndexBlock(self, atX, atY);
	unsigned int count = (unsigned int)ceil(distance / (Block::GetWidth() * MAP_CELL_SIZE) );
	for (unsigned int i = 0; i < count && i < maxNum; ++i)
	{
		if (i >= MyMax(m_RowBlock, m_ColBlock))
			break;

		DoStepAroundBlock(atX, atY, i, distance, self, outTset, maxNum);
	}
}

void Map::GetTargetRangeSet(float distance, Object* target, Object* self, std::vector<Object*>& outTset, SelectFilter filter, unsigned int maxNum /*= 100*/)
{
    unsigned int atX, atY;
    GetIndexBlock(target, atX, atY);
    unsigned int count = (unsigned int)ceil(distance / (Block::GetWidth() * MAP_CELL_SIZE) );
    for (unsigned int i = 0; i < count && i < maxNum; ++i)
    {
        if (i >= MyMax(m_RowBlock, m_ColBlock))
            break;

        DoStepAroundBlock(atX, atY, i, distance, target, self, outTset, filter, maxNum);
    }
}

void Map::ObjectChangePosition(Object* obj, const Vec2& newPosition)
{
    if (obj)
    {
        const Vec2& oldPosition = obj->GetPosition();
        //Vec2 oldTile = Pos2Tile(oldPosition);
        unsigned int oldBlock = GetIndexBlock(obj);
        obj->m_Position = newPosition;
        unsigned int newBlock = GetIndexBlock(obj);
        if (newBlock >= m_Blocks.size())
        {
            obj->m_Position = oldPosition;
            gLog.Warning("obj[%llu] out of the map range, invalid blockIndex=%d.",obj->m_GUID, newBlock);
            return;
        }
        //gLog.AddLog(LOG_LEVEL_DEBUG2, "set obj[%llu] old pos(%f,%f) and new pos(%f,%f). oldIndex(%d), newIndex(%d)",obj->m_GUID, oldPos.x, oldPos.y, newPosition.x, newPosition.y, oldBlock, newBlock);
        if (oldBlock != newBlock)
        {
            if(m_Blocks[oldBlock]->RemoveObject(obj))
            {
                gLog.AddLog(LOG_LEVEL_INFO, "remove obj[%llu] to block=%d.",obj->m_GUID, oldBlock);
                gLog.AddLog(LOG_LEVEL_INFO, "add obj[%llu] to block=%d.",obj->m_GUID, GetIndexBlock(obj));
                m_Blocks[newBlock]->AddObject(obj);
                obj->OnChangeBlock(oldBlock, newBlock);
            }
            else
            {
                gLog.AddLog(LOG_LEVEL_WARNING, "why obj not in map block when it change position!");
            }
        }
        //else
        //{//>效果差不多.注解去掉了
        //    Vec2 newTile = Pos2Tile(newPosition);
        //    if (oldTile != newTile)
        //    {
        //        obj->OnChangeTile(oldTile, newTile);
        //    }
        //}
    }
}

//-----------------------------------------------------------------------------------------

void Map::DoStepAroundBlock(unsigned int atBlockX, unsigned int atBlockY, unsigned int step, float distance, Object* self, std::vector<Object*>& outTset, unsigned int maxNum)
{
	if (step == 0)
	{
		GetByDistanceSet(m_Blocks[ToIndexBlock(atBlockX, atBlockY)], distance, self, outTset, maxNum) ;
	}
	else
	{
		if (atBlockY >= step)
		{
			unsigned int y = atBlockY - step;
			unsigned int x = atBlockX > step ? atBlockX - step : 0;
			for ( ; x <= atBlockX + step; ++x)  //>循环顶行
			{
				if (x <= MyMin(atBlockX + step, m_RowBlock - 1))
				{
					GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, self, outTset, maxNum);
				}
			}    
		}

		if (atBlockY + step <= m_ColBlock - 1)
		{
			unsigned int y = atBlockY + step;
			unsigned int x = atBlockX > step ? atBlockX - step : 0;
			for ( ; x <= atBlockX + step; ++x) //>循环底行
			{
				if (x <= MyMin(atBlockX + step, m_RowBlock - 1))
				{
					GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, self, outTset, maxNum);
				}
			}
		}

		if (atBlockX >= step)
		{
			unsigned int y = (atBlockY + 1) > step ? (atBlockY + 1) - step : 0;
			unsigned int x = atBlockX - step;
			for ( ; y < atBlockY + step; ++y) //>循环左边列
			{
				if (y <= MyMin(atBlockY + step - 1, m_ColBlock - 1))
				{
					GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, self, outTset, maxNum);
				}
			} 
		}

		if (atBlockX + step <= m_RowBlock - 1)
		{
			unsigned int y = (atBlockY + 1) > step ? (atBlockY + 1) - step : 0;
			unsigned int x = atBlockX + step;
			for ( ; y < atBlockY + step; ++y) //>循环右边列
			{
				if (y <= MyMin(atBlockY + step - 1, m_ColBlock - 1))
				{
					GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, self, outTset, maxNum);
				}
			} 
		}
	}
}

void Map::DoStepAroundBlock(unsigned int atBlockX, unsigned int atBlockY, unsigned int step, float distance, Object* target, Object* self, std::vector<Object*>& outTset, SelectFilter filter, unsigned int maxNum)
{
    if (step == 0)
    {
        GetByDistanceSet(m_Blocks[ToIndexBlock(atBlockX, atBlockY)], distance, target, self, outTset, filter, maxNum) ;
    }
    else
    {
        if (atBlockY >= step)
        {
            unsigned int y = atBlockY - step;
            unsigned int x = atBlockX > step ? atBlockX - step : 0;
            for ( ; x <= atBlockX + step; ++x)  //>循环顶行
            {
                if (x <= MyMin(atBlockX + step, m_RowBlock - 1))
                {
                    GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, target, self, outTset, filter, maxNum);
                }
            }    
        }

        if (atBlockY + step <= m_ColBlock - 1)
        {
            unsigned int y = atBlockY + step;
            unsigned int x = atBlockX > step ? atBlockX - step : 0;
            for ( ; x <= atBlockX + step; ++x) //>循环底行
            {
                if (x <= MyMin(atBlockX + step, m_RowBlock - 1))
                {
                    GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, target, self, outTset, filter, maxNum);
                }
            }
        }

        if (atBlockX >= step)
        {
            unsigned int y = (atBlockY + 1) > step ? (atBlockY + 1) - step : 0;
            unsigned int x = atBlockX - step;
            for ( ; y < atBlockY + step; ++y) //>循环左边列
            {
                if (y <= MyMin(atBlockY + step - 1, m_ColBlock - 1))
                {
                    GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, target, self, outTset, filter, maxNum);
                }
            } 
        }

        if (atBlockX + step <= m_RowBlock - 1)
        {
            unsigned int y = (atBlockY + 1) > step ? (atBlockY + 1) - step : 0;
            unsigned int x = atBlockX + step;
            for ( ; y < atBlockY + step; ++y) //>循环右边列
            {
                if (y <= MyMin(atBlockY + step - 1, m_ColBlock - 1))
                {
                    GetByDistanceSet(m_Blocks[ToIndexBlock(x, y)], distance, target, self, outTset, filter, maxNum);
                }
            } 
        }
    }
}

//-----------------------------------------------------------------------------------------

void Map::GetByDistanceSet(Block* block, float distance, Object* self, std::vector<Object*>& outTset, unsigned int maxNum)
{
	std::list<Object*>::iterator it = block->objects.begin();
	for (std::list<Object*>::iterator end = block->objects.end(); 
		it != end && outTset.size() < maxNum; ++it)
	{
        Object* other = *it;
		if (self != other)
		{
            if(self->GetPosition().distancePow2(other->GetPosition()) <= distance * distance)
            {
                outTset.push_back(other);
            }

			//if (TileDistance(Pos2Tile(self->GetPosition()), Pos2Tile(other->GetPosition())) <= distance)
			//{
			//	outTset.push_back(other);
			//}
		}
	}
}


void Map::GetByDistanceSet(Block* block, float distance, Object* target, Object* self, std::vector<Object*>& outTset, SelectFilter filter, unsigned int maxNum)
{
    std::list<Object*>::iterator it = block->objects.begin();
    for (std::list<Object*>::iterator end = block->objects.end(); 
        it != end && outTset.size() < maxNum; ++it)
    {
        Object* other = *it;

		switch (other->m_Type)
		{
		case OBJECT_PLAYER:
			if (((Player*)other)->m_Attribute[ATTRIBUTE_HP].GetValueI() <= 0)
			{
				return;
			}
			break;
		case OBJECT_MONSTER:
			if (((Monster*)other)->m_HP <= 0)
			{
				return;
			}
			break;
		default:
			return;
		}

        switch (filter)
        {
        case Select_Player:
            if (other->m_Type != OBJECT_PLAYER)
                return;
            break;
        case Select_Monster:
            if (other->m_Type != OBJECT_MONSTER)
                return;
            break;
        case Select_ByMapMode:
            {
                const MapInfo* pMapInfo = g_CSVFileManager->GetMapByID(self->m_Map->GetMapID());
                if (pMapInfo->iMapPKModel == MAPPKTYPE_PEACE) //>怪物
                {
                    if (other->m_Type == OBJECT_PLAYER)	//排除玩家
                        return;									
                }
                else if (pMapInfo->iMapPKModel == MAPPKTYPE_TEAM)   //>非队伍的玩家和怪
                {
                    if (self->m_Type == OBJECT_PLAYER && other->m_Type == OBJECT_PLAYER)
                    {
                        if (((Player*)self)->IsTeamMembers(other->m_GUID))	//排除队友玩家
                            return;
                    }
                }
                else if (pMapInfo->iMapPKModel == MAPPKTYPE_GUILD) //>非公会的玩家和怪
                {
                    if (self->m_Type == OBJECT_PLAYER && other->m_Type == OBJECT_PLAYER)
                    {
                        if (((Player*)self)->m_SocietyID == ((Player*)other)->m_SocietyID)	//排除本公会玩家
                            return;
                    }
                }
            }
            break;
        case Select_TeamMember:
            {
                if (self->m_Type != OBJECT_PLAYER)
                {
                    //>不是玩家取不到队伍成员
                    return;
                }
                else
                {
                    if(!((Player*)self)->IsTeamMembers(other->m_GUID))
                    {
                        //>不是队伍成员
                        return;
                    }
                }
            }
            break;
        case Select_All:
            break;
        }


        if (self != other && target != other)
        {

            if(self->GetPosition().distancePow2(other->GetPosition()) <= distance * distance)
            {
                outTset.push_back(other);
            }

            //if (TileDistance(Pos2Tile(target->GetPosition()), Pos2Tile(other->GetPosition())) <= distance)
            //{
            //    outTset.push_back(other);
            //}
        }
    }
}
