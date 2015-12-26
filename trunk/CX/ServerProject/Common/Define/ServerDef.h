/********************************************************************
created:	2014/7/15
author:		LL
desc:		用于定义服务器内部数据。
*********************************************************************/
#ifndef _SERVERDEF__H_
#define _SERVERDEF__H_

#include "MsgInfo.h"


#define FRAME_GET_MSG_MAX 100		//每帧处理最大消息数量
#define LINK_TIMEOUT_TIME 100000	//连接超时时间ms

#define CUR_CLIENT_VERSION_MAIN		1	//版本号
#define CUR_CLIENT_VERSION_SUB		1	//版本号


#define SERVER_CONNECTION_PASSWORD	 "~Game@44445~"	//其它内网服务器连接密码

#define GATESERVER_CONNECTIONS_MAX	50000	//网关服务器最大连接数量
#define LOGINSERVER_CONNECTIONS_MAX	500		//登陆服务器最大连接数量
#define GAMESERVER_CONNECTIONS_MAX	4		//游戏服务器最大连接数量
#define DBSERVER_CONNECTIONS_MAX	2		//数据库服务器最大连接数量



#endif	//_SERVERDEF__H_