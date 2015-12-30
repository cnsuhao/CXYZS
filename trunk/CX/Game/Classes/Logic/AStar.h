#ifndef __ASTAR_H_
#define __ASTAR_H_


#include <list>
#include <vector>

#include "math/Vec2.h"

using namespace std;
#define MAX_MAPCELL_X 512
#define MAX_MAPCELL_Y 512

//a*节点
struct node
{
	int x,y;
	int g,h,f;
	node * parentnode;
	node()
	{
		x = -1;
		y = -1;
		g = 0;
		h = 0;
		f = 0;
		parentnode = NULL;
	}
};

class CAStar
{
public:
	CAStar(void);
	~CAStar(void);

private:
	list< node* >    m_openlist;	//开放列表
	list< node* >	 m_closelist;	//关闭列表
	char mapcheck[MAX_MAPCELL_X][MAX_MAPCELL_Y];

public:
	bool FindPath(int start_x,int start_y,int end_x,int end_y,std::vector<cocos2d::Vec2>& path);

	int get_G_value( node* curr_node );
	int get_H_value( node* curr_node,int end_x,int end_y );
	void insertnode( node* p );
	void checkG( node* cp_node,node* curr_node );
	void ClearMem();
	


	//关于阻挡判断
	bool IsNoZD(int i,int j);
	
	//检查是否阻挡
	bool CheckMapFile(int i,int j);


	bool CheckMapCheck(int i,int j);


    bool CanLinearMotion(const cc2d::Vec2& from, const cc2d::Vec2& to);

public:
	bool m_MapBlockList[MAX_MAPCELL_X][MAX_MAPCELL_Y];	//阻挡
	bool m_MapMaskList[MAX_MAPCELL_X][MAX_MAPCELL_Y];	//遮罩
};

extern CAStar g_astar;

#endif