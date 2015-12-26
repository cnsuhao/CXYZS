/********************************************************************
created:	2014/7/15
author:		LL
desc:		游戏服务器。连接网关、登陆、数据库服务器
*********************************************************************/

#ifndef _GameServer_
#define _GameServer_
#include <map>
#include "ServerDef.h"
#include "RakPeerInterface.h"
#include "TimerManage.h"

class Player;

class GameServer
{
public:
	GameServer(void);
	~GameServer(void);
	bool Start(uint16 ServerID);		//根据id读表启动服务器，开启监听，根据需要连接到相应服务器
	void Run();							//循环收取消息并处理之
	bool Stop();						//停止运行服务器
	bool m_StopFlag;					//服务器是否停止运行
	


//网络信息
	void OnRecvLogin(RakNet::Packet* p);		//收到登陆服务器数据包
	void OnRecvClient(RakNet::Packet* p);		//收到客户端数据包	
	void OnRecvDB(RakNet::Packet* p);			//收到数据库服务器数据包	
	void OnRecvGate(RakNet::Packet* p);			//收到网关服务器的数据包
	bool SendToGate(MsgBase *msg);				//发送数据到网关服务器
	bool SendToLogin(MsgBase *msg);				//发送数据到登陆服务器
	bool SendToDB(MsgBase *msg);				//发送数据到数据库服务器
	bool SendToClient(MsgBase *msg);			//发送数据到客户端
	

	RakNet::RakPeerInterface* m_Net;			//网络库实例，用于收发消息	
	uint16 m_ServerID;							//服务器ID
	uint16	m_Port;					            //端口
	std::string	m_ServerName;					//登陆服务器名
	std::string	m_LoginServerIP;				//登陆服务器IP
	uint16 	m_LoginServerPort;		            //登陆服务器端口
	RakNet::SystemAddress m_LoginAddress;		//登陆服务器地址
	std::string	m_GateServerIP;					//网关服务器IP
	uint16 	m_GateServerPort;		            //网关服务器端口
	RakNet::SystemAddress 	m_GateAddress;		//网关服务器地址
	std::string	m_DBServerIP;					//数据库服务器IP
	uint16 	m_DBServerPort;			            //数据库服务器端口
	RakNet::SystemAddress 	m_DBAddress;		//数据库务器地址
	char m_SendBuff[1024];						//发送缓存，用于发送变长数据
	
//接收函数
    void OnGetGlobalGuid(MsgBase* msg);         //获得全局guid
	void OnPlayerEnterGameServer(MsgBase* msg);	//玩家进入服务器
	void OnPlayerLeaveGameServer(MsgBase* msg);	//玩家离开服务器
	void OnCreateRoleToDB(MsgBase* msg);			//创建角色
	void OnRoleDetailInfo(MsgBase* msg);		//接受数据库角色其他信息
	void OnGangInfo(MsgBase* msg);				//接受数据库帮派信息
	void OnGangApplyInfo(MsgBase* msg);			
    void OnRoleRequestEnterMap(MsgBase* msg);   //请求进入地图
    void OnPlayerMove(MsgBase* msg);            //角色移动
    void OnPlayerMoveStop(MsgBase* msg);        //角色移动停止
	void OnPlayerUseSkill(MsgBase* msg);        //角色使用技能
	void OnPlayerRelive(MsgBase* msg);			//角色复活
    
    void OnPlayerBagList(MsgBase* msg);         //背包物品清单
    void OnPlayerEquipList(MsgBase* msg);       //穿戴装备清单
    void OnPlayerSkillList(MsgBase* msg);       //技能清单
    void OnPlayerActivateSkill(MsgBase* msg);   //改变主修技能
    void OnPlayerUIChoseSkill(MsgBase* msg);    //选择ui上选择技能
    void OnPlayerShopBuy(MsgBase* msg);         //商店购买
    void OnPlayerShopSell(MsgBase* msg);        //商店出售
    void OnPlayerItemCompose(MsgBase* msg);     //物品合成
    void OnPlayerEquipmentUpStar(MsgBase* msg); //装备升星
    void OnPlayerEquipmentInherit(MsgBase* msg);//装备传承
    void OnPlayerItemUse(MsgBase* msg);         //物品使用
	void OnPlayerTakeTask(MsgBase* msg);		//接取任务
	void OnPlayerDoTask(MsgBase* msg);			//做任务

    void OnTeamApply(MsgBase* msg);             //>请求组队
    void OnTeamReplyApply(MsgBase* msg);        //>回应组队
    void OnTeamChangeLeader(MsgBase* msg);      //>队长转让
    void OnTeamKickMember(MsgBase* msg);        //>踢出队员
    void OnTeamMemberExit(MsgBase* msg);        //>成员离开

	void OnPlayerChest(MsgBase* msg);			//抽箱子
    void OnPlayerDetailInfoQuery(MsgBase* msg);       //>玩家信息查询
private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);
};

extern GameServer *g_GameServer;

#endif	//_GameServer_