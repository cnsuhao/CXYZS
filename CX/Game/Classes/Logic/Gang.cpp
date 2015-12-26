#include "Gang.h"
#include <time.h>


Gang::Gang()
{
	m_id = 0;
	memset(m_szName,0,sizeof(m_szName));
	m_iLevel = 0;
	m_iShopLevel = 0;
	m_iHeaderid = 0;
	memset(m_szHeaderName,0,sizeof(m_szHeaderName));
	m_iAct = 0;
	memset(m_szAccounce,0,sizeof(m_szAccounce));
	m_iViveHeaderid[0] = 0;
	m_iViveHeaderid[1] = 0;
	for (int i = 0; i < 5 ; ++i)
	{
		m_iStarFlec[i] = 0;
	}
	memset(m_szCreateTime,0,sizeof(m_szCreateTime));
	//time_t t = time(0);
	//strftime(m_szCreateTime, sizeof(m_szCreateTime), "%Y%m%d %H%M%S",localtime(&t) ); 

	m_iCopyid = 0;
	memset(m_szStartTime,0,sizeof(m_szStartTime));
	m_mapMembers.clear();
	memset(m_szBackupInfo,0,sizeof(m_szBackupInfo));

	m_mapApplys.clear();
	m_vecGangLog.clear();
}

Gang::~Gang()
{
}

