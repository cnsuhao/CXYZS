#pragma once
/********************************************************************
created:	2015/10/28
author:		lishihai
desc:		������:�ṩ��Ҫ�Ķ��鹦��
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

    //>�õ��ӳ�
    const Team::Member* GetTeamLeader();

    //>�Ƿ���Ա
    bool IsFull() { return m_Members.size() >= TEAM_MAX_NUM; }

    //>�Ƿ��Ƕ�Ա
    bool HasMember(uint64 playerId);
    
    //>�����������
    void RequsetJoin(Player* player);

    //>��Ҽ���
    void OnPlayerJoin(Player* player);

    //>����˳�
    void OnPlayerExit(uint64 Guid);
       
    //>�����ɹ�
    void OnCreateSucess();

    //>�����ɢ
    void OnDismiss();

    //>�ӳ�����
    void OnChangeLeader(Player* NewLeader);

    //>��ó�Ա
    const std::vector<Member>& GetMembers() { return m_Members; }

    void SendTeamInfo(uint64 playerId);
private:
    void RemoveMember(uint64 playerId);
    Team::Member* GetMember(uint64 playerId);

    uint32 m_TeamId;
    std::vector<Member> m_Members;
};

