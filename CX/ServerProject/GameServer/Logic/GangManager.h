/********************************************************************
//created:	2015/8/19
//author:		zp
//desc:		帮派管理
//*********************************************************************/

#ifndef __GANGMANAGER_H__
#define __GANGMANAGER_H__

#include "GameDef.h"
#include <vector>
#include <map>
using namespace std;

class Gang
{
public:
	Gang();
	~Gang();

	int64			m_id;							//帮派id
	char			m_szName[MAX_NAME_SIZE];		//帮派名称
	int8			m_iLevel;						//帮派等级
	int8			m_iShopLevel;					//帮派商店等级
	int64			m_iHeaderid;					//会长ID
	char			m_szHeaderName[MAX_NAME_SIZE];	//会长名称
	int32			m_iAct;							//活跃度
	int8			m_iNum;							//帮派人数
	char			m_szAccounce[255];				//帮派公告
	vector<int64>	m_vecViveHeaderid;				//副会长
	char			m_szCreateTime[50];				//创建时间
	int32			m_iCopyid;						//副本ID
	char			m_szStartTime[50];				//开启时间

	vector<int64>	m_vecMembersid;					//成员名单
	vector<int64>	m_vecApplyid;					//申请加入名单
private:

};



class GangManager
{
public:
	GangManager();
	~GangManager();

	void Init();

	map<int64,Gang>	m_mapGang;

	Gang* GetGangByID(int64 id);
};
extern GangManager* g_GangManager;
#endif	//__GANGMANAGER_H__