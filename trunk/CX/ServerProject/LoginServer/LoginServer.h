/********************************************************************
created:	2014/7/15
author:		LL
desc:		登陆服务器。用于登陆注册。连接网关服务器、账号数据库，被游戏服务器连接
*********************************************************************/

#ifndef _LoginServer_
#define _LoginServer_
#include <map>
#include "ServerDef.h"
#include "RakPeerInterface.h"
#include "TimerManage.h"
#include "windows.h"
#include "CMySql.h"

struct GameServerInfo
{
	uint16 ServerID;						//服务器id
	std::string ServerName;					//服务器名
	RakNet::SystemAddress ServerAddress;	//服务器地址
	int ServerState;						//服务器类型 1老服 2新服 3合服
	bool isStart;							//服务器是否开启
};

struct LoginPlayerInfo
{
	uint64 accountId;				//账号id
	uint64 LinkID;					//连接id
	char accountName[MAX_NAME];		//名字
	char state;						//状态 0离线, 1登陆，2游戏中
	uint16 lastServerId;			//最后登陆服务器id,进入游戏时需要更新此id
};

class LoginServer
{
public:
	LoginServer(void);
	~LoginServer(void);
	bool Start();	//根据端口启动服务器，开启监听，根据需要连接到相应服务器
	void Run();		//循环收取消息并处理之
	bool Stop();	//停止运行服务器

//网络信息
	void OnRecvClient(RakNet::Packet* p);				//收到客户端自定义数据包
	void OnRecvGame(RakNet::Packet* p);					//收到游戏服务器自定义数据包
	bool SendToClient(uint64 &objid, MsgBase *msg);	//发送数据到客户端
	bool SendToGame(uint16 &serverid, MsgBase *msg);		//发送数据到游戏服务器	

	RakNet::RakPeerInterface* m_Net;				//网络库实例，用于收发消息
	uint16 m_Port;						            //端口
	std::string	m_GateServerIP;						//网关服务器IP
	uint16	m_GateServerPort;			            //网关服务器端口
	RakNet::SystemAddress m_GateAddress;			//网关服务器地址,由ip+端口组成
	
//游戏服务器
	void AddServerLink(RakNet::SystemAddress &address);		//增加游戏服务器连接
	void DelServerLink(RakNet::SystemAddress &address);		//删除游戏服务器连接	
	GameServerInfo* GetServerInfo(uint16 id);					//获取游戏服务器信息，根据id

	std::map<uint16, GameServerInfo>	m_GameServerList;					//连接到本服务器的游戏服务器列表

//玩家
	void AddLoginPlayer(uint64 linkid, uint64 accountid, char* accountname, uint16 lastserverid);			//增加登陆玩家
	void DelLoginPlayer(uint64 accountid);						//删除登陆玩家
	LoginPlayerInfo* GetLoginPlayer(uint64 accountid);			//获取登陆玩家
	LoginPlayerInfo* GetLoginPlayerByLinkID(uint64 linkid);	//获取登陆玩家,根据连接id

	std::map<uint64, LoginPlayerInfo*> m_LoginPlayerListByID;	//客户端列表，玩家id做键值

//数据库信息	
	std::string	m_AccountDBName;		//账号数据库名
	std::string	m_AccountDBIP;			//账号数据库IP
	uint16 m_AccountDBPort;	            //账号数据库端口
	std::string	m_AccountDBAccount;		//账号数据库账号
	std::string	m_AccountDBPassword;	//账号数据库密码
	CMysql m_Mysql;				//用于操作数据库
//
	bool m_StopFlag;					//服务器是否停止运行
	TimerManage m_TimerManage;			//定时器管理

	
	
//消息处理函数
	void Regiser(MsgBase* msg);			//注册
	void Login(MsgBase* msg);			//登陆
	void SelServer(MsgBase* msg);		//选择服务器


private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);

	char m_SendBuff[1024];	//发送缓存，用于发送变长数据
//内部消息处理函数
	
};

#endif	//_LoginServer_