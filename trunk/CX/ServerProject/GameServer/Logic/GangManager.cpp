#include "GangManager.h"

GangManager *g_GangManager = nullptr;

Gang::Gang()
{
}

Gang::~Gang()
{
}

GangManager::GangManager()
{		
}

GangManager::~GangManager()
{
}

void GangManager::Init()
{
	//�������ݿ�����
}

Gang* GangManager::GetGangByID(int64 id)
{
	std::map<int64, Gang>::iterator it = m_mapGang.find(id);
	if (it != m_mapGang.end())
	{
		return &it->second;
	}

	return NULL;
}