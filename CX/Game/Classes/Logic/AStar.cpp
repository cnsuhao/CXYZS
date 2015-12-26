#include "AStar.h"

USING_NS_CC;

CAStar g_astar;

CAStar::CAStar(void)
{
	memset(m_MapBlockList, 1, sizeof(m_MapBlockList));
	memset(m_MapMaskList, 0, sizeof(m_MapMaskList));
	memset(mapcheck, 0, sizeof(mapcheck));
}

CAStar::~CAStar(void)
{
	ClearMem();
}

void CAStar::ClearMem()
{
	list< node* >::iterator cell;
	for(cell = m_openlist.begin();cell != m_openlist.end(); ++cell)
	{
		if( *cell != NULL )
			delete * cell;
	}
	for(cell = m_closelist.begin();cell != m_closelist.end(); ++cell)
	{

		if( *cell != NULL )
			delete * cell;
	}
	m_openlist.clear();
	m_closelist.clear();
}

bool CAStar::FindPath(int start_x,int start_y,int end_x,int end_y,std::vector<cocos2d::Vec2>& path)
{
	//先判断是否满足寻路的条件
	if( start_x < 0 || start_y < 0 || end_x < 0 || end_y < 0 || start_x >= MAX_MAPCELL_X || start_y >= MAX_MAPCELL_Y || end_x >= MAX_MAPCELL_X || end_y >= MAX_MAPCELL_Y )
	{
		return false;
	}
	//！如果目标地点是阻挡则改变目标地点

	if( ( start_x == end_x && start_y == end_y ) || m_MapBlockList[end_y][end_x])
	{
		return false;
	}

	//把初始点加入开启列表中
	node* start_node = new node;
	start_node->x = start_x;
	start_node->y = start_y;
	mapcheck[start_y][start_x] = 1;
	m_openlist.push_back( start_node );

	list< node* >::iterator cell;
	node* bestnode = NULL;
	int step = 0;
	while(1)
	{
		//如果开启列表中没有点了，寻路失败
		if( m_openlist.size() == 0 )
		{
			ClearMem();
			memset(mapcheck, 0, sizeof(mapcheck));
			return false;
		}
		cell = m_openlist.begin();
		mapcheck[(*cell)->y][(*cell)->x] = -1;

		//左上方向的情况
		if( IsNoZD((*cell)->x-1,(*cell)->y-1) && (*cell)->x-1 == end_x && (*cell)->y-1 == end_y )
		{
			//如果左上方的点是终点，则退出循环
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x-1,(*cell)->y-1) && mapcheck[(*cell)->y-1][(*cell)->x-1] == 0 )
		{
			//把左上方的点放入开启列表中
			node* ins_node = new node;
			ins_node->x = (*cell)->x-1;
			ins_node->y = (*cell)->y-1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y-1][(*cell)->x-1] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x-1,(*cell)->y-1) )
		{
			//如果左上方的点已经在开启列表中了，那么就检查G的值，看是否是更好的点
			node* ins_node = new node;
			ins_node->x = (*cell)->x-1;
			ins_node->y = (*cell)->y-1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}
		//上方向
		if( IsNoZD((*cell)->x,(*cell)->y-1) && (*cell)->x == end_x && (*cell)->y-1 == end_y )
		{
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x,(*cell)->y-1) && mapcheck[(*cell)->y-1][(*cell)->x] == 0 )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x;
			ins_node->y = (*cell)->y-1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y-1][(*cell)->x] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x,(*cell)->y-1) )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x;
			ins_node->y = (*cell)->y-1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}
		//右上方向
		if( IsNoZD((*cell)->x+1,(*cell)->y-1) && (*cell)->x+1 == end_x && (*cell)->y-1 == end_y )
		{
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x+1,(*cell)->y-1) && mapcheck[(*cell)->y-1][(*cell)->x+1] == 0 )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x+1;
			ins_node->y = (*cell)->y-1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y-1][(*cell)->x+1] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x+1,(*cell)->y-1) )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x+1;
			ins_node->y = (*cell)->y-1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}
		//右方向
		if( IsNoZD((*cell)->x+1,(*cell)->y) && (*cell)->x+1 == end_x && (*cell)->y == end_y )
		{
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x+1,(*cell)->y) && mapcheck[(*cell)->y][(*cell)->x+1] == 0 )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x+1;
			ins_node->y = (*cell)->y;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y][(*cell)->x+1] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x+1,(*cell)->y) )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x+1;
			ins_node->y = (*cell)->y;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}
		//右下方向
		if( IsNoZD((*cell)->x+1,(*cell)->y+1) && (*cell)->x+1 == end_x && (*cell)->y+1 == end_y )
		{
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x+1,(*cell)->y+1) && mapcheck[(*cell)->y+1][(*cell)->x+1] == 0 )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x+1;
			ins_node->y = (*cell)->y+1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y+1][(*cell)->x+1] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x+1,(*cell)->y+1) )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x+1;
			ins_node->y = (*cell)->y+1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}
		//下方向
		if( IsNoZD((*cell)->x,(*cell)->y+1) && (*cell)->x == end_x && (*cell)->y+1 == end_y )
		{
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x,(*cell)->y+1) && mapcheck[(*cell)->y+1][(*cell)->x] == 0 )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x;
			ins_node->y = (*cell)->y+1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y+1][(*cell)->x] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x,(*cell)->y+1) )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x;
			ins_node->y = (*cell)->y+1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}
		//左下方向
		if( IsNoZD((*cell)->x-1,(*cell)->y+1) && (*cell)->x-1 == end_x && (*cell)->y+1 == end_y )
		{
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x-1,(*cell)->y+1) && mapcheck[(*cell)->y+1][(*cell)->x-1] == 0 )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x-1;
			ins_node->y = (*cell)->y+1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y+1][(*cell)->x-1] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x-1,(*cell)->y+1) )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x-1;
			ins_node->y = (*cell)->y+1;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}
		//左方向
		if( IsNoZD((*cell)->x-1,(*cell)->y) && (*cell)->x-1 == end_x && (*cell)->y == end_y )
		{
			bestnode = (*cell);
			break;
		}
		else if( CheckMapFile((*cell)->x-1,(*cell)->y) && mapcheck[(*cell)->y][(*cell)->x-1] == 0 )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x-1;
			ins_node->y = (*cell)->y;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			mapcheck[(*cell)->y][(*cell)->x-1] = 1;
			insertnode( ins_node );
		}
		else if( CheckMapCheck((*cell)->x-1,(*cell)->y) )
		{
			node* ins_node = new node;
			ins_node->x = (*cell)->x-1;
			ins_node->y = (*cell)->y;
			ins_node->parentnode = (*cell);
			ins_node->g = get_G_value( ins_node );
			ins_node->h = get_H_value( ins_node,end_x,end_y );
			ins_node->f = ins_node->g + ins_node->h;
			checkG( ins_node,(*cell) );
		}

		m_closelist.push_back((*cell));
		m_openlist.erase(cell);
		step++;
		if (step > 500)
		{
			ClearMem();
			memset(mapcheck, 0, sizeof(mapcheck));
			return false;
		}
	}

	int count = 1;
	Vec2 temp_path[499];
	temp_path[0].x = end_x;
	temp_path[0].y = end_y;
	while( bestnode->parentnode != NULL )
	{
		if( count > 498 )
		{
			ClearMem();
			memset(mapcheck, 0, sizeof(mapcheck));
			return false;
		}
		temp_path[count].x = bestnode->x;
		temp_path[count].y = bestnode->y;
		bestnode = bestnode->parentnode;
		count++;
	}	
	int i;
	for(i = 0;i < count;i++)
	{		
		path.push_back(temp_path[i]);
	}
	//pos.x = -1;
	//pos.y = -1;
	//path.push_back(pos);
	ClearMem();
	memset(mapcheck, 0, sizeof(mapcheck));

	return true;
}

int CAStar::get_G_value( node* curr_node )
{
	int g = 0;
	while( curr_node->parentnode != NULL )
	{
		if( abs(curr_node->y - curr_node->parentnode->y)+abs(curr_node->x - curr_node->parentnode->x) == 2 )
		{
			g += 14;
		}
		else
		{
			g += 10;
		}
		curr_node = curr_node->parentnode;
	}
	return g;
}

int CAStar::get_H_value( node* curr_node,int end_x,int end_y )
{
	int h = ( abs(end_x - curr_node->x) + abs(end_y - curr_node->y) )*10;
	return h;
}

void CAStar::insertnode( node* p )
{
	//有待改进
	list< node* >::iterator cell;
	for(cell = m_openlist.begin();cell != m_openlist.end(); ++cell)
	{
		if( p->f <= (*cell)->f )
		{
			break;
		}
	}
	m_openlist.insert( cell,p );
}

void CAStar::checkG( node* cp_node,node* curr_node )
{
	int check = 0;
	if( abs(cp_node->x - curr_node->x) + abs(cp_node->y - curr_node->y) == 2 )
	{
		check = 14;
	}
	else
	{
		check = 10;
	}

	if( cp_node->g + check < curr_node->g )
	{
		cp_node->parentnode = curr_node;
		cp_node->g = get_G_value( cp_node );
		cp_node->f = cp_node->g + cp_node->h;

		list< node* >::iterator cell,dell,celltemp;
		for(cell = m_openlist.begin();cell != m_openlist.end(); ++cell)
		{
			if( (*cell)->x == cp_node->x && (*cell)->y == cp_node->y )
			{
				break;
			}
		}
		dell = cell;
		if( cp_node->f < (*dell)->f )
		{
			while( dell != m_openlist.begin() && cp_node->f <= (*dell)->f )
			{
				dell--;
			}
			dell++;
			m_openlist.insert( dell,cp_node );
			delete * cell;
			m_openlist.erase(cell);
		}
		else if( cp_node->f > (*dell)->f )
		{
			while( dell != m_openlist.end() && cp_node->f >= (*dell)->f )
			{
				dell++;
			}
			m_openlist.insert( dell,cp_node );
			delete * cell;
			m_openlist.erase(cell);
		}
		else
		{
			delete cp_node;
		}
	}
	else
	{
		delete cp_node;
	}
}


bool CAStar::IsNoZD(int i,int j)
{
	if( i >= 0 && j >= 0 && i < MAX_MAPCELL_X && j < MAX_MAPCELL_Y )
	{
		return !m_MapBlockList[j][i];		
	}
	return false;
}

bool CAStar::CheckMapFile(int i,int j)
{
	if( i >= 0 && j >= 0 && i < MAX_MAPCELL_X && j < MAX_MAPCELL_Y )
	{
		return !m_MapBlockList[j][i];	
	}
	return false;
}

bool CAStar::CheckMapCheck(int i,int j)
{
	if( i >= 0 && j >= 0 && i < MAX_MAPCELL_X && j < MAX_MAPCELL_Y && mapcheck[j][i] == 1 )
	{
		return true;
	}
	return false;
}

#include "BaseFunction.h"

bool CAStar::CanLinearMotion(const Vec2& from, const Vec2& to)
{
    float tileSize = 64.0f;
    int idxY = (int)floor(to.y / tileSize);
    int idxX = (int)floor(to.x / tileSize);

    if (!IsNoZD(idxX, idxY))
    {
        return false;
    }

    std::vector<Vec2> tiles;
    BaseFunction::GetTilesFromLine(from, to, tileSize, tiles);
    if (tiles.empty())
    {
        return false;
    }

    int count = tiles.size();
    for (int i = 0; i < count; ++i)
    {
        if (!IsNoZD((int)tiles[i].x, (int)tiles[i].y))
        {
            return false;
        }
    }

    return true;
}
