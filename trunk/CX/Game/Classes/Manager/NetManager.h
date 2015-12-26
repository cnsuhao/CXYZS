/********************************************************************
created:	2015/3/20
author:		LL
desc:		网络管理：管理与服务器端的通讯，连接、断开连接、收消息、发消息
*********************************************************************/
#ifndef __NETMANAGER_H__
#define __NETMANAGER_H__

#include "ClientDef.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "CCLayer.h"

using namespace std;

struct MsgBase;
struct Show_GameSrv;
class UIMsgBox;

class NetManager: public cocos2d::Layer
{
public:
	NetManager();
	~NetManager();
	void Init();					//初始化	
	void Release();					//释放
	void Update(float delta);		//更新，用于循环获取服务器数据

	bool Connect();					//连接服务器
	int SendMsg(MsgBase &msg);		//发送消息	
	
	RakNet::RakPeerInterface* m_Net;				//网络库实例，用于连接服务器、收发数据
	RakNet::SystemAddress 	m_ServerAddress;		//服务器地址

	UIMsgBox* m_CutTip;				//断线重连
	UIMsgBox* m_ReturnLoginTip;		//服务器断开，返回登陆提示

    const std::string& GetGateIP() const { return m_GateIP; }
    const std::string& GetGatePort() const { return m_GatePort; }
    const std::string& GetResIP() const { return m_ResIP; }
    const std::string& GetResPort() const { return m_ResPort; }
	//Show_GameSrv *m_showServer;

	char m_SendBuff[1024];	//发送缓存，用于发送变长数据
private:
	int  OnRecv(RakNet::Packet* pData);		//收到服务器游戏数据

    bool LoadConfing();

    std::string m_GateIP;
    std::string m_GatePort;

    std::string m_ResIP;
    std::string m_ResPort;

public:
    struct MsgTeam_Apply* m_LastTeamInfo;
    UIMsgBox* m_LastApplyMsgBox;
//////////////////////////////////////////////////////////////////////////////////////////
//以下空间用作接收数据处理函数的申明
public:
	void OnNotify(MsgBase* msg);

    void OnPlayerBaseInfo(MsgBase* msg);        //玩家基础信息
    void OnObjectMove(MsgBase* msg);            //处理玩家移动消息
    void OnObjectMoveKick(MsgBase* msg);        //处理服务器移动踢回
    void OnObjectMoveStop(MsgBase* msg);        //玩家停止移动
    void OnOtherPlayerEnterView(MsgBase* msg);  //其他玩家进入视野
    void OnOtherPlayerLeaveView(MsgBase* msg);  //其他玩家离开视野
	void OnObjectSyncAttribute(MsgBase* msg);	//服务器同步对象属性
    void OnObjectAddHp(MsgBase* msg);           //对象加血
    void OnObjectDropHp(MsgBase* msg);          //对象掉血
	void OnPlayerstate(MsgBase* msg);			//服务器同步玩家状态(表)
	void OnPlayerUseSkill(MsgBase* msg);		//玩家或怪物使用技能
	void OnPlayerReliveData(MsgBase* msg);		//服务器通知客户端原地复活所需要的信息
	void OnPlayerRelive(MsgBase* msg);			//玩家复活
    void OnPlayerChangeEquipment(MsgBase* msg); //装备改变
	void OnPlayerDetailAttribute(MsgBase* msg);	//玩家其他基础信息（如攻击力等）
    void OnPlayerOtherDetailInfo(MsgBase* msg); //其他玩家的详细信息

    void OnAddItem(MsgBase* msg);               //添加物品
    void OnDelItem(MsgBase* msg);               //删除物品
    void OnUpdateItem(MsgBase* msg);            //更新物品
    void OnEquipList(MsgBase* msg);             //穿戴物品清单
    void OnEquipUpdate(MsgBase* msg);           //穿戴物更新

    void OnMonsterEnterView(MsgBase* msg);      //怪物添加
    void OnMonsterLeaveView(MsgBase* msg);      //怪物删除
    void OnMonsterMoving(MsgBase* msg);         //怪物移动
	void OnMonsterGoto(MsgBase* msg);			//怪物传送

    void OnSkillList(MsgBase* msg);             //技能清单
    void OnAddSkill(MsgBase* msg);              //添加技能
    void OnUpdateSkill(MsgBase* msg);           //更新技能

    void OnObjBuffList(MsgBase* msg);           //>buff列表
    void OnObjAddBuff(MsgBase* msg);            //>buff添加
    void OnObjDelBuff(MsgBase* msg);            //>buff删除
    
	void OnRecvFriendsNotify(MsgBase* msg);     //获得好友通知

	void OnRecvTaskNotify(MsgBase* msg);		//获得任务通知

	void OnRecvGangNotify(MsgBase* msg);		//获得工会通知

    void OnChatMsg(MsgBase* msg);               //聊天消息

    void OnTeamApply(MsgBase* msg);             //>邀请组队
    void OnTeamReplyApply(MsgBase* msg);        //>组队回应
    void OnTeamCreated(MsgBase* msg);           //>创建队伍
    void OnTeamNewMember(MsgBase* msg);         //>加入新成员
    void OnTeamMemberExit(MsgBase* msg);        //>成员离开
    void OnTeamChangeLeader(MsgBase* msg);      //>队长更变
};

extern NetManager* g_NetManager;
#endif  // __NETMANAGER_H__