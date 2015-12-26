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


struct GangLogInfo
{
	char			recordTime[30];
	char			recordContent[255];
};

class Gang
{
public:
	Gang();
	~Gang();

	int64			m_id;							//帮派id
	char			m_szName[MAX_NAME_SIZE];		//帮派名称
	int32			m_iLevel;						//帮派等级
	int32			m_iShopLevel;					//帮派商店等级
	int64			m_iHeaderid;					//会长ID
	char			m_szHeaderName[MAX_NAME_SIZE];	//会长名称
	int32			m_iAct;							//活跃度
	char			m_szAccounce[255];				//帮派公告
	int64			m_iViveHeaderid[2];				//副会长
	int64			m_iStarFlec[5];					//堂主
	char			m_szCreateTime[50];				//创建时间
	int32			m_iCopyid;						//副本ID
	char			m_szStartTime[50];				//开启时间
	map<int64,GangMember>	m_mapMembers;			//成员列表
	char			m_szBackupInfo[255];			//备注信息
	
	map<int64,GangMember>	m_mapApplys;			//申请加入列表

	vector<GangLogInfo>		m_vecGangLog;			//帮派日志


private:

};
#endif	//__GANGMANAGER_H__