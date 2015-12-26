/********************************************************************
created:	2014/7/15
author:		LL
desc:		网关服务器。对外接口，用于转发消息。被客户端、登陆、游戏服务器连接
*********************************************************************/
#ifndef _GateServer_
#define _GateServer_
#include <map>
#include "LinkManager.h"
#include "ServerDef.h"
#include "TimerManage.h"



class GateServer :public LinkManager
{
public:
	GateServer(void);
	~GateServer(void);
	bool Start();	//根据端口启动服务器，开启监听，根据需要连接到相应服务器
	void Run();		//循环收取消息并处理之
	bool Stop();	//停止运行服务器
	
	void OnRecvGame(RakNet::Packet* p);			//收到游戏服务器数据包
	bool SendToClientByLinkID(MsgBase *msg);	//发送数据到客户端
	bool SendToClientByAccountID(MsgBase *msg);	//发送数据到客户端
	bool SendToGame(uint16 &serverid, MsgBase *msg);		//发送数据到游戏服务器
	bool SendToLogin(MsgBase *msg);	//发送数据到登陆服务器

//网络信息
	RakNet::RakPeerInterface*	m_Net;				//网络库实例，用于收发消息
	uint16	m_Port;						            //端口
	std::string	m_LoginServerIP;					//登陆服务器IP
	uint16	m_LoginServerPort;			            //登陆服务器端口
	RakNet::SystemAddress m_LoginAddress;			//登陆服务器地址
	/////////////////
	bool m_StopFlag;					//服务器是否停止运行	
	//TimerManage m_TimerManage;
private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);



	void OnGameServerRegister(RakNet::Packet* msg);		//游戏服务器注册
	void OnPlayerEnterGameServer(RakNet::Packet* msg);	//玩家进入游戏服务器
};
extern GateServer* g_GateServer;
#endif	//_GateServer_