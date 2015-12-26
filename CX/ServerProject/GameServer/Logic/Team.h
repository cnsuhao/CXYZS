#pragma once
/********************************************************************
created:	2015/10/28
author:		lishihai
desc:		队伍类:提供主要的队伍功能
*********************************************************************/

#include "GameDef.h"
#include <vector>

#define		TEAM_MAX_NUM		5

class Player;

class Team
{
public:
    Team(uint32 teamId);

    ~Team(void);

    void Init(Player* leader);

    void Clear();

    enum MemberAttribute
    {
        MA_None,
        MA_Leaber = 1,
    };

    struct Member
    {
        uint64              playerId;
        MemberAttribute     attribute;

        Member(uint64 id, MemberAttribute attr):playerId(id),attribute(attr)
        {

        }
    };

    uint32 GetId() { return m_TeamId; }

    //>得到队长
    const Team::Member* GetTeamLeader();

    //>是否满员
    bool IsFull() { return m_Members.size() >= TEAM_MAX_NUM; }

    //>是否是队员
    bool HasMember(uint64 playerId);
    
    //>申请加入退伍
    void RequsetJoin(Player* player);

    //>玩家加入
    void OnPlayerJoin(Player* player);

    //>玩家退出
    void OnPlayerExit(uint64 Guid);
       
    //>创建成功
    void OnCreateSucess();

    //>队伍解散
    void OnDismiss();

    //>队长更变
    void OnChangeLeader(Player* NewLeader);

    //>获得成员
    const std::vector<Member>& GetMembers() { return m_Members; }

    void SendTeamInfo(uint64 playerId);
private:
    void RemoveMember(uint64 playerId);
    Team::Member* GetMember(uint64 playerId);

    uint32 m_TeamId;
    std::vector<Member> m_Members;
};

