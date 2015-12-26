#include "RequestServerList.h"

RequestServerList *RequestServerList::_Instance = NULL;

RequestServerList *RequestServerList::Instance()
{
	if ( _Instance == NULL)		_Instance = new RequestServerList();
	return _Instance;
}

void RequestServerList::UnInstance()
{
	if (_Instance != NULL)
	{
		delete _Instance;
		_Instance = NULL;
	}
}

RequestServerList::RequestServerList()
{
}

RequestServerList::~RequestServerList()
{
	
}

bool RequestServerList::GetServerList(vecServerList &vecList,int iStart,int iend)
{
	if (m_vecServerList.size() <= 0)
		return false;
	vecList.clear();
	int max = iend < (int)m_vecServerList.size() ? iend : m_vecServerList.size();
	for (int i = iStart ; i < max ; i++)
	{
		vecList.push_back(m_vecServerList[i]);
		vecList[i - iStart].serverPos = i - iStart;
	}
	return true;
}

//取新开服的服务器
bool RequestServerList::GetNewSerList(vecServerList &vecList)
{
	if (m_vecServerList.size() <= 0)
		return false;
	vecList.clear();
	for (size_t i = 0 ; i <  m_vecServerList.size() ; i++)
	{
		if (m_vecServerList[i].serverState == SERSTATE_NEW)
		{
			vecList.push_back(m_vecServerList[i]);
		}
		
	}
	return true;
}

//根据ID取服务器
tagServer* RequestServerList::GetSerByID(int iserid)
{
	tagServer *tempSer = NULL;

	if (m_vecServerList.size() <= 0)
		return tempSer;

	for (size_t i = 0 ; i <  m_vecServerList.size() ; i++)
	{
		if (m_vecServerList[i].iServerID == iserid)
		{
			tempSer = &m_vecServerList[i];
			break;
		}
	}

	return tempSer;
}

void RequestServerList::AddServerList(tagServer parServer)
{
	m_vecServerList.push_back(parServer);
}

void RequestServerList::ClearServerList()
{
	m_vecServerList.clear();
}