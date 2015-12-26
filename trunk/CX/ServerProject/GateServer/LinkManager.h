/********************************************************************
created:	2014/7/15
author:		LL
desc:		连接管理。用于管理网关服务器的所有连接
*********************************************************************/

#ifndef __LINKMANAGER_H__
#define __LINKMANAGER_H__
#include "RakPeerInterface.h"
#include <iostream>
#include <hash_map>
#include <map>
#include "GameDef.h"

enum OPERATION_RESULT
{
	LINK_EXISTS,					//重复的连接
	ADD_OK,							//添加连接成功
	REMOVE_LINK_SUCCESS,			//移除成功
	LINK_NO_EXISTS,					//连接不存在
};

struct Link_Info
{	
	unsigned long LinkID;				//连接id
	RakNet::SystemAddress Address;		//地址
	std::string name;					//名字
	__int64 AccountId;					//用户的帐号ID	,服务器的ID
	char State;							//状态	0未知、1正常开启
	char LinkType;						//连接类型	0未知、1游戏服务器、2客户端
	uint16 lastServerId;				//最后登陆服务器
	uint64	loginTime;					//最后登陆时间
};

class LinkManager
{
public:
	LinkManager(void);
	~LinkManager(void);

	OPERATION_RESULT AddLink(RakNet::SystemAddress &address);		//增加连接
	OPERATION_RESULT DelLink(RakNet::SystemAddress &address);		//删除连接
	void AddClient(uint16 serverid, uint64 linkid, uint64 accountid);		//增加客户端
	void DelClient(uint64 accountid);		//!删除客户端	设置玩家状态为离开，并开启删除定时器，若时间到玩家还是离开状态，则删除。
	void GameServerRegister(uint64 linkid, uint16 serverid, char* name);	//注册游戏服务器


	void RemoveByAccountID(uint64 accountid);						//移除客户端，根据账号id
	void onCheckPlayerLogin(void* plinkInfo);
	RakNet::SystemAddress* GetAddressByAccountID(uint64 accountid);//获取客户端地址，根据账号id

	Link_Info* GetLinkByLinkID(uint64 linkid);					//获取连接信息
	Link_Info* GetLinkByAccountId(uint64 accountId);					//获取客户端
	Link_Info* GetLinkByServerId(uint16 serverid);						//获取服务器

	//bool ClientList::Exists(std::string& accountName);				//账号名是否在线
	//bool ClientList::Exists(__int64 accountId);						//账号id是否在线

private:	
	std::map<uint64, Link_Info*> m_LinkList;				//连接列表
	std::map<unsigned __int16, Link_Info*> m_ServerList;						//游戏服务器列表
	stdext::hash_map<__int16, stdext::hash_map<__int64, Link_Info*>> m_ServerClientList;	//服务器客户端队列，对应每服务器里的所有客户端
	stdext::hash_map<__int64, stdext::hash_map<__int64, stdext::hash_map<__int64, Link_Info*>>> m_ServerGuildClientList; //服务器公会客户端队列
	std::map<unsigned long, Link_Info*> m_ClientList;				//客户端列表，连接id做键值
	std::map<uint64, Link_Info*> m_ClientListByID;					//客户端列表，玩家id做键值
	std::map<std::string, Link_Info*> m_ClientListByName;			//客户端列表，玩家名做键值
};

#endif	//__LINKMANAGER_H__


