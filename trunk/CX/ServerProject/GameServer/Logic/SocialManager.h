//////////////////////////////////////////////////////////////////////////
//    File Name:      SocailManager.h
//    Description:    社交管理类
//    Creator:        zp
//    Version:        2015.08.05
//////////////////////////////////////////////////////////////////////////

#ifndef __SOCAILMANAGER_H__
#define __SOCAILMANAGER_H__

#include "stdio.h"
#include <vector>
#include "Player.h"
#include "ServerDef.h"
#include "Gang.h"
#include <queue>
#include <unordered_map>

class Team;

class SocailManager
{
public:
	SocailManager();
	~SocailManager();

    void Init() {}
public:
	//好友功能
	bool	AddFriends(MsgBase* msg);			//添加好友
	bool	DelFriends(MsgBase* msg);			//删除好友
	bool	GetFriendsList(MsgBase* msg);		//好友列表
	bool	GetFriendsInof(MsgBase* msg);		//查看好友详细信息
	//聊天功能
	bool	ChannelMessage(MsgBase* msg);		//频道消息
	bool	PointMessage(MsgBase* msg);			//单向消息

	//公会功能
	bool	CreateGang(MsgBase* msg);			//创建帮会
	bool	GetGangInfo(MsgBase* msg);			//获得帮派信息
	bool	ApplyAddGang(MsgBase* msg);			//申请加帮
	bool	ReplyAddGang(MsgBase* msg);			//回复加帮申请
	bool	GetGangApplyList(MsgBase* msg);		//获得帮派申请列表
	bool	LeaveGang(MsgBase* msg);			//离开帮会
	bool	PutGang(MsgBase* msg);				//提出帮会
	bool	GetGangList(MsgBase* msg);			//获得帮派列表
	bool	GangAnnouncement(MsgBase* msg);		//帮派公告
	bool	EditAnnouncement(MsgBase* msg);		//编辑公会公告
	bool	SendEmail(MsgBase* msg);			//发送帮派邮件
	bool	AddGangContribution(MsgBase* msg);	//添加帮派贡献
	bool	GetGangContribution(MsgBase* msg);	//查看帮派贡献
	bool	GangAppoint(MsgBase* msg);			//任命
	bool	GangTransfer(MsgBase* msg);			//装让
	bool	GangUpGrade(MsgBase* msg);			//帮派升级
	bool	GetGangLevel(MsgBase* msg);			//获得帮派等级
	bool	GangShopBuy(MsgBase* msg);			//帮派商店购买
	bool	StartCopy(MsgBase* msg);			//开启副本
	bool	CopyReward(MsgBase* msg);			//副本奖励


    Team*   GetTeam(uint32 teamId);				//>查找队伍
    Team*   AllocTeam();                        //>分配队伍
    void    FreeTeam(Team* team);               //>释放队伍
    

    inline Team*   FindTeam(uint64 playerGuid);
    inline void    InserPlayerAndTeam(uint64 playerGuid, uint32 teamId);
    inline void    RemovePlayerAndTeam(uint64 playerGuid);
private:
    void    AllocTeamCapacity();                //>分配容量
    std::vector<Team*> m_teams;                 //>队伍列表
    std::queue<Team*> m_freeTeams;             //>释放的队伍列表
    uint32  m_teamCapacity;                     //>分配计数
    std::unordered_map<uint64, uint32> m_playerTeam;      //玩家队伍列表
    
};
extern SocailManager* g_SocailManager;

inline Team*  SocailManager::FindTeam(uint64 playerGuid)
{
    auto it = m_playerTeam.find(playerGuid);
    if (it != m_playerTeam.end())
    {
        return GetTeam(it->second);
    }
    return NULL;
}

inline void  SocailManager::InserPlayerAndTeam(uint64 playerGuid, uint32 teamId)
{
    m_playerTeam.insert(std::pair<uint64, uint32>(playerGuid, teamId));
}

inline void  SocailManager::RemovePlayerAndTeam(uint64 playerGuid)
{
    auto it = m_playerTeam.find(playerGuid);
    if (it != m_playerTeam.end())
    {
        m_playerTeam.erase(it);
    }
}


class GangManager
{
public:
	GangManager();
	~GangManager();

	void Init();

	map<int64,Gang>	m_mapGang;

	Gang* GetGangByID(int64 id);

	//获得新建帮派ID
	int64 GetGangidForGangMap();
};
extern GangManager* g_GangManager;

#endif	//__SOCAILMANAGER_H__