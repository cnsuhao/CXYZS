/********************************************************************
created:	2014/7/15
author:		LL
desc:		数据库服务器。连接游戏数据库,被游戏服务器连接
*********************************************************************/
#ifndef _DBServer_
#define _DBServer_
#include <map>
#include "ServerDef.h"
#include "RakPeerInterface.h"
#include "TimerManage.h"
#include "CMySql.h"

class DBServer :public TimerManage
{
public:
	DBServer(void);
	~DBServer(void);
	bool Start(uint16 ServerID);	//根据索引读表启动服务器，开启监听，根据需要连接到相应服务器
	void Run();		//循环收取消息并处理之
	bool Stop();	//停止运行服务器

	bool m_StopFlag;				//服务器是否停止运行	

//网络信息
	void OnRecvGame(RakNet::Packet* p);		//收到游戏服务器数据包	
	bool SendToGame(MsgBase *msg);			//发送数据到游戏服务器	
	bool SendToClient(MsgBase *msg);		//发送数据到客户端	

	RakNet::RakPeerInterface* m_Net;			//网络库实例，用于收发消息
	uint16 m_ServerID;							//服务器ID
	uint16	m_Port;					            //端口
	std::string	m_GameServerIP;					//游戏服务器IP
	uint16 	m_GameServerPort;		            //游戏服务器端口
	RakNet::SystemAddress 	m_GameAddress;		//游戏务器地址
	
//数据库信息	
	std::string	m_GameDBName;		//游戏数据库名
	std::string	m_GameDBIP;			//游戏数据库IP
	uint16 m_GameDBPort;	        //游戏数据库端口
	std::string	m_GameDBAccount;	//游戏数据库账号
	std::string	m_GameDBPassword;	//游戏数据库密码
	CMysql m_Mysql;			//用于操作数据库

	GuidInfoIt guidInfo;


/////////////////	
	std::map<uint64, SQL_player_info> m_PlayerInfo;	//!玩家数据库信息

	std::map<uint64,SQL_gang_info>		m_GangInfo;
	std::map<uint64,SQL_apply_info>		m_ApplyInfo;
	void test(int i);
	void onUpdatePlayerinfo(int i);
	void getGuidInfo();
	void LoadGangInfo();

//接收函数	
	void OnGetGlobalGuid(MsgBase* msg);         //获取全局guid
	void OnGetGang(MsgBase* msg);				//获取帮派信息
    void OnPlayerEnterGameServer(MsgBase* msg);	//玩家进入服务器
	void OnPlayerLeaveGameServer(MsgBase* msg);	//玩家离开服务器
	void OnCreateRole(MsgBase* msg);			//创建角色
    void OnAddItemSet(MsgBase* msg);            //>添加物品
    void OnDelItemSet(MsgBase* msg);            //>删除物品
    void OnUpdateItemSet(MsgBase* msg);         //>更新物品
    void OnUpdateWearEquipment(MsgBase* msg);   //>更新穿戴装备

    void OnAddSkill(MsgBase* msg);              //>添加技能
    void OnUpdateSkill(MsgBase* msg);           //>更新技能
    void OnActivateSkill(MsgBase* msg);         //>激活技能
    void OnChoseCanUseSkill(MsgBase* msg);      //>选择使用的技能

	void OnAddFriends(MsgBase* msg);			//添加好友
	void OnDelFriends(MsgBase* msg);			//删除好友

	void OnCopyFinish(MsgBase* msg);			//副本完成 主要针对爬塔类的副本
	void OnCopyApply (MsgBase* msg);			//副本申请成功 插入数据库

	void OnCreateGang(MsgBase* msg);			//创建帮派
	void OnDelGang(MsgBase* msg);				//解散帮派
	void OnApplyGang(MsgBase* msg);				//申请加入帮派
	void OnDelApplyInfo(MsgBase* msg);			//删除加入帮派信息
	void OnDelApplyInfo(__int64 applyId);		//申请信息删除
	void OnApplyGangAgree(MsgBase* msg);		//同意申请人加入帮派
	void OnQuitGang(MsgBase* msg);				//退出帮派
	void OnEditAnnounce(MsgBase* msg);			//编辑帮派公告
	void OnChangeGangHeader(MsgBase* msg);		//转让帮主

	void OnTakeTask(MsgBase* msg);
	

	bool OnFullSelectGangInfoMap(StoreQueryResult res);
	bool OnFullSelectApplyInfoMap(StoreQueryResult res);
	void OnMakeTask(MsgBase* msg);
	void OnSubmitTask(MsgBase* msg);
	void OnUpdateBaseInfo(MsgBase* msg);
private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);
	void SendPlayerDetaiInfo(SQL_player_info* playerInfo);
    SkillInfoIt* FindPlayerSkill(SQL_player_info* playerInfo, int32 skillType);
};

#endif	//_DBServer_