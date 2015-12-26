/********************************************************************
created:	2015/7/27
author:		LL
desc:		地图类：处理地图内逻辑，地图分块便于查找玩家
*********************************************************************/

#ifndef __MAP_H__
#define __MAP_H__
#include "Vec2.h"
#include <vector>
#include <list>

#include "Monster.h"
#include "Player.h"

// 地图块.
struct Block
{
	Block() {}
	~Block() {}

	static float GetWidth() { return 4.0f; }

	void AddObject(Object* obj);
	bool RemoveObject(Object* obj);
    bool RemoveByGuid(uint64 guid);

	std::list<Object*> objects;
};

enum SelectFilter
{
    Select_Player,
    Select_Monster,
    Select_ByMapMode,
    Select_TeamMember,
    Select_All,
};

enum MapEnterMode
{
    Mode_Init,      //>初始化刚进入地图
    Mode_Convey,    //>传送进入地图
    Mode_Rebirth,   //>复活进入地图
};

struct MapInfo;
class Map
{
public:
	Map();	
	~Map();

	bool Init(int mapId, int64 guid, float w, float h);

	void AddMonster(Monster* monster);
	void RemoveMonster(Monster* monster);
	void AddPlayer(Player* player, const Vec2& position);
    Vec2 GetEnterMapPosition(MapEnterMode mode, int conveyId = 0);
    void RemovePlayer(Player* player);

	//>地图宽和高
	inline float GetWidth() const       { return m_Width; }
	inline float GetHeight() const      { return m_Height; }
	inline int GetMapID() const       { return m_MapId; }
	//>地图行和列最大索引 [0 到 number row -1] 
	inline unsigned int GetRow() const  { return m_RowBlock; }
	inline unsigned int GetCol() const  { return m_ColBlock; }
    //>tile和position转换
    inline Vec2 Tile2Pos(const Vec2& tilePos);
    inline Vec2 TileId2Pos(int tileId);
    inline Vec2 Pos2Tile(const Vec2& position);
	//>取得实体所在的Block索引
	inline unsigned int     GetIndexBlock(Object* obj);
	inline void             GetIndexBlock(Object* obj, unsigned int& atX, unsigned int& atY);
	inline Block*           GetBlock(Object* obj);
	inline unsigned int     ToIndexBlock(unsigned int atX, unsigned int atY);
	inline void             ToAtBlock(unsigned int blockIdx, unsigned int& atX, unsigned int& atY);

	// 获得制定距离内的T类型实体集合
	void  GetRangeSet(float distance, Object* self, std::vector<Object*>& outTset, unsigned int maxNum = 100);

    //>获目标范围内的实体集合.
    void GetTargetRangeSet(float distance, Object* target, Object* self, std::vector<Object*>& outTset, SelectFilter filter, unsigned int maxNum = 100);

    //>地图对象改变了位置
    void    ObjectChangePosition(Object* obj, const Vec2& newPosition);

protected:	
	//>围绕着格子一圈一圈往外扩散查找. step 表示第几圈. 第0圈就是自己本身.
	void DoStepAroundBlock(unsigned int atBlockX, unsigned int atBlockY, unsigned int step, float distance, Object* self, std::vector<Object*>& outTset, unsigned int maxNum);

    //>围绕着格子一圈一圈往外扩散查找. step 表示第几圈. 第0圈就是自己本身.
    void DoStepAroundBlock(unsigned int atBlockX, unsigned int atBlockY, unsigned int step, float distance, Object* target, Object* self, std::vector<Object*>& outTset, SelectFilter filter, unsigned int maxNum);

	//>根据距离做赛选,结果到outSet集合中
	void GetByDistanceSet(Block* block, float distance, Object* self, std::vector<Object*>& outTset, unsigned int maxNum);

    //>根据距离做赛选,结果到outSet集合中
    void GetByDistanceSet(Block* block, float distance, Object* target, Object* self, std::vector<Object*>& outTset, SelectFilter filter, unsigned int maxNum);
private:
    int             m_MapId;
    int64           m_Guid;
	float           m_Width;
	float           m_Height;
	unsigned int    m_RowBlock;
	unsigned int    m_ColBlock;
    const MapInfo*  m_MapInfo;
public:
	std::vector<Block*> m_Blocks;
};

//////////////////////////////////////////////////////////////////////////
// inline function implement.


inline Vec2 Map::Tile2Pos(const Vec2& tilePos)
{
    return Vec2(tilePos.x * 64.0f + 32.0f, (GetHeight() - (tilePos.y + 0.5f)) * 64.0f);
}

inline Vec2 Map::TileId2Pos(int tileId)
{
    return Tile2Pos(TileId2Tile(tileId));
}

inline Vec2 Map::Pos2Tile(const Vec2& position)
{
    return Vec2(floor(position.x / 64.0f), floor(GetHeight() - (position.y / 64.0f)));
}

inline unsigned int Map::GetIndexBlock(Object* obj)
{
	const Vec2& tilePosition = Pos2Tile(obj->GetPosition());
	float blockWidth = Block::GetWidth();
	unsigned int x = (unsigned int)floor(tilePosition.x / blockWidth);
	unsigned int y = (unsigned int)floor(tilePosition.y / blockWidth); 

	return y * m_RowBlock + x;
}

//-----------------------------------------------------------------------------------------
inline void Map::GetIndexBlock(Object* obj, unsigned int& atX, unsigned int& atY)
{
	const Vec2& tilePosition = Pos2Tile(obj->GetPosition());
	float blockWidth = Block::GetWidth();
	atX = (unsigned int)floor(tilePosition.x / blockWidth);
	atY = (unsigned int)floor(tilePosition.y / blockWidth); 
}

//-----------------------------------------------------------------------------------------
inline Block* Map::GetBlock(Object* obj)
{
	unsigned int index = GetIndexBlock(obj);
	if (index < m_Blocks.size())
	{
		return m_Blocks[index];
	}
	return NULL;
}

//-----------------------------------------------------------------------------------------
inline unsigned int Map::ToIndexBlock(unsigned int atX, unsigned int atY)
{
	return atY * m_RowBlock + atX;
}

//-----------------------------------------------------------------------------------------
inline void Map::ToAtBlock(unsigned int blockIdx, unsigned int& atX, unsigned int& atY)
{
	atX = blockIdx % m_RowBlock;
	atY = blockIdx / m_RowBlock; 
}

#endif	//__MAP_H__
