#include "LinkManager.h"
#include <string>
#include "TimerManage.h"
#include "MsgInfo.h"
#include "GateServer.h"
#include <time.h>

//安全释放指针函数
#define SAFE_DELETE(p)			{ if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p); (p) = NULL; } }


class LinkManagerCallback: public Callback<LinkManager>
{
public:
	LinkManagerCallback(LinkManager* obj, func_void f,uint64 accountid_,uint16 lastServerId,uint64 logintime):Callback(obj, f)
	{
		
		m_data.accountid = accountid_;
		m_data.lastServerId = lastServerId;
		m_data.loginTime = logintime;
	}

	struct Data
	{
		uint64 accountid;
		uint16 lastServerId;
		uint64 loginTime;
	};

	Data* GetData()
	{
		return &m_data;
	}

private:
	Data m_data;
};


LinkManager::LinkManager(void)
{
}

LinkManager::~LinkManager(void)
{
}


OPERATION_RESULT LinkManager::AddLink(RakNet::SystemAddress &address)
{
	unsigned long linkid = RakNet::SystemAddress::ToInteger(address);
	if (m_LinkList.end() !=  m_LinkList.find(linkid))
	{
		return LINK_EXISTS;
	}
	Link_Info *theClient = new Link_Info();  //Remove函数中delete
	theClient->LinkID = linkid;	
	theClient->Address = address;
	theClient->State = 0;
	theClient->loginTime = time(NULL);
	m_LinkList[linkid] = theClient;

	return ADD_OK;
}

 void LinkManager::onCheckPlayerLogin(void* plinkInfo_)
 {
	LinkManagerCallback::Data* plinkInfo = (LinkManagerCallback::Data*) plinkInfo_;
	assert(plinkInfo);
	std::map<uint64, Link_Info*>::iterator it;
	for(it = m_LinkList.begin();it != m_LinkList.end();it++)
	{
		time_t now_time =time(NULL);
		printf("time:%llu",(now_time - it->second->loginTime));
		if(it->second->AccountId == plinkInfo->accountid && ((now_time - it->second->loginTime) <= 120))
		{
			return;
		}
	}
	MsgBase msg;
	msg.OBJID = plinkInfo->accountid;
	msg.SysID = ID_GATE2GAME;
	msg.MsgID = Msg_Player_Remove_GameAndDBServer;
	msg.Len = sizeof(msg);
	g_GateServer->SendToGame(plinkInfo->lastServerId,&msg);

 }

OPERATION_RESULT LinkManager::DelLink(RakNet::SystemAddress &address)
{
	unsigned long linkid = RakNet::SystemAddress::ToInteger(address);

	Link_Info *theLink = NULL;
	std::map<uint64, Link_Info*>::iterator it = m_LinkList.find(linkid);
	if (it != m_LinkList.end())
	{
		LinkManagerCallback* pCallback = new LinkManagerCallback(this, &LinkManager::onCheckPlayerLogin,it->second->AccountId, it->second->lastServerId,it->second->loginTime);
		g_TimerManage->AddTimer(0, 90000, pCallback, 1, pCallback->GetData());
		theLink = it->second;
		SAFE_DELETE(theLink);
		m_LinkList.erase(it);
		
		return REMOVE_LINK_SUCCESS;
	}
	return LINK_NO_EXISTS;
}
void LinkManager::AddClient(uint16 serverid, uint64 linkid, uint64 accountid)
{
	//!是否在游戏中，在则踢下线
	Link_Info *theLink = NULL;
	std::map<uint64, Link_Info*>::iterator it = m_LinkList.find(linkid);
	if (it != m_LinkList.end())
	{
		theLink = it->second;
		if (theLink->LinkType)
		{
			theLink->LinkType = 2;
			theLink->lastServerId = serverid;
			theLink->AccountId = accountid;
			theLink->loginTime = time(NULL);
		}
	}
	if (!theLink)
	{
		return;
	}
	m_ClientListByID[accountid] = theLink;
	stdext::hash_map<__int16, stdext::hash_map<__int64, Link_Info*>>::iterator serverit = m_ServerClientList.find(serverid);
	if (serverit == m_ServerClientList.end())
	{
		stdext::hash_map<__int64, Link_Info*> LinkList;
		LinkList.insert(stdext::hash_map<__int64, Link_Info*>::value_type(theLink->AccountId, theLink));
		m_ServerClientList.insert(stdext::hash_map<__int16, stdext::hash_map<__int64, Link_Info*>>::value_type(serverid, LinkList));  //Remove_m_FriendList
	}
	else
	{
		stdext::hash_map<__int64, Link_Info*> *LinkList = &serverit->second;
		LinkList->insert(stdext::hash_map<__int64, Link_Info*>::value_type(serverid, theLink));
	}
}

void LinkManager::DelClient(uint64 accountid)
{

}

void LinkManager::GameServerRegister(uint64 linkid, uint16 serverid, char* name)
{
	Link_Info* link = GetLinkByLinkID(linkid);
	if (link)
	{
		link->LinkType = 1;	//游戏服务器
		link->State = 1;
		link->loginTime =  time(NULL);
		//建立服务器队列
		m_ServerList[serverid] = link;
		std::map<unsigned __int16, Link_Info*>::iterator it;
		for(it = m_ServerList.begin();it != m_ServerList.end();it++)
		{
			printf("ServerID:%d\n",it->first);
		}
		stdext::hash_map<__int64, Link_Info*> linklist;
		m_ServerClientList.insert(stdext::hash_map<__int16, stdext::hash_map<__int64, Link_Info*>>::value_type(serverid, linklist)); 
	}
}

void LinkManager::RemoveByAccountID(uint64 accountid)
{
	//std::map<unsigned long, Client*>::iterator it = m_ClientList.find(accountid);
	//if( it != this->m_ClientList.end() )
	//{
	//	delete (*it).second; //delete_theClient
	//	//_printf( TEXT("帐号为%d的角色被踢出登录服务器"),accountid );
	//	this->m_ClientList.erase(it);
	//}
}

RakNet::SystemAddress* LinkManager::GetAddressByAccountID(uint64 accountid)
{
	/*std::map<int, Client*>::iterator it = this->m_ClientList.find(accountid);
	if( it != this->m_ClientList.end() )
	{
		return &((*it).second->m_NetGUID);
	}*/
	return NULL;
}

Link_Info* LinkManager::GetLinkByLinkID(uint64 linkid)
{
	Link_Info *pTheLink = NULL;
	std::map<uint64, Link_Info*>::iterator clientIterator = m_LinkList.find(linkid);
	if (clientIterator != m_LinkList.end())
	{
		pTheLink = clientIterator->second;
	}	
	return pTheLink;
}

Link_Info* LinkManager::GetLinkByAccountId(uint64 accountId)
{	
	Link_Info *pTheLink = NULL;
	std::map<uint64, Link_Info*>::iterator it = m_ClientListByID.find(accountId);
	if (it != m_ClientListByID.end())
	{
		pTheLink = it->second;
	}	
	return pTheLink;
}

Link_Info* LinkManager::GetLinkByServerId(unsigned __int16 serverid)
{
	std::map<unsigned __int16, Link_Info*>::iterator it = m_ServerList.find(serverid);

	if (it != m_ServerList.end())
	{
		return it->second;
	}
	return NULL;
}
//
//bool LinkManager::Exists(__int64 accountId)
//{
//	return (this->m_ClientList.find(accountId) != this->m_ClientList.end());
//}
//
//bool LinkManager::Exists(std::string& accountName)
//{
//	if (accountName.size() <= 0)
//	{
//		return false;
//	}
//	/*std::map<int, Client*>::iterator it = this->m_ClientList.begin(); 
//	for(; it != this->m_ClientList.end(); ++it)
//	{
//		if (it->second->m_nAccountName == accountName)
//		{
//			return true;
//		}
//	}*/
//
//	return false;
//}