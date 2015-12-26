#include "Team.h"
#include "Player.h"
#include "MapManager.h"
#include "MsgInfo.h"
#include "StackStream.h"
#include "GameServer.h"
#include "LogManage.h"
#include "SocialManager.h"

Team::Team(uint32 teamId):m_TeamId(teamId)
{

}

Team::~Team(void)
{
}

void Team::Init(Player* leader)
{
    Clear();
    m_Members.push_back(Member(leader->m_GUID, MA_Leaber));
    leader->m_teamId = m_TeamId;


    g_SocailManager->InserPlayerAndTeam(leader->m_GUID, m_TeamId);

    OnCreateSucess();
}

void Team::Clear()
{
    m_Members.clear();
}

const Team::Member* Team::GetTeamLeader()
{
    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        if (it->attribute == MA_Leaber)
        {
            return &(*it);
        }
    }

    return NULL;
}


bool Team::HasMember(uint64 playerId)
{
    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        if (it->playerId == playerId)
        {
            return true;
        }
    }

    return false;
}

void Team::RequsetJoin(Player* player)
{
    if (IsFull())
    {
        gLog.Warning("can't add new member, the team is full when call RequsetJoin.");
        return;
    }
   
    const Member* leader = GetTeamLeader();
    Player* playerLeader = g_MapManager->GetPlayerByID(leader->playerId);
    if (playerLeader)
    {
        //>������Ϣ���ӳ�,����������.
        MsgTeam_Apply replyMsg(player->m_GUID, player->m_Name, player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI(), leader->playerId);
        replyMsg.OBJID = leader->playerId;
        g_GameServer->SendToClient(&replyMsg);
    }
    else
    {
        gLog.Warning("can't find leader when call RequsetJoin.");
        assert(player);
    }
}

void Team::OnPlayerJoin(Player* player)
{
    assert(!IsFull());

    MsgTeam_NewMember newMemberMsg;
    newMemberMsg.memberId = player->m_GUID;
    strncpy(newMemberMsg.szName, player->m_Name, sizeof(char)*MAX_NAME_SIZE);
    newMemberMsg.memberLevel = player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
    newMemberMsg.Hp = player->m_Attribute[ATTRIBUTE_HP].GetValueI();
    newMemberMsg.MaxHp = player->m_Attribute[ATTRIBUTE_HPMAX].GetValueI();

    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        const Member& member = *it;
        //>֪ͨ�ϳ�Ա�����³�Ա
        newMemberMsg.OBJID = member.playerId;
        g_GameServer->SendToClient(&newMemberMsg);
    }

    m_Members.push_back(Member(player->m_GUID, MA_None));
    player->m_teamId = m_TeamId;
    g_SocailManager->InserPlayerAndTeam(player->m_GUID, m_TeamId);

    //>֪ͨ�³�Ա������������ϳ�Ա��Ϣ
    MsgTeam_Created msg;
    msg.memberNumber = m_Members.size();

    StackStream<> msgStream;
    msgStream.Wirte(&msg, sizeof(msg));

    int count = 0;
    it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        Member& member = *it;
        Player* player = g_MapManager->GetPlayerByID(member.playerId);
        if (player)
        {
            ++count;
            msgStream.Wirte(&member.playerId, sizeof(member.playerId));
            msgStream.Wirte(&member.attribute, sizeof(member.attribute));
            msgStream.Wirte(&player->m_Name, sizeof(char)*MAX_NAME_SIZE);
            int level = player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
            msgStream.Wirte(&level, sizeof(level));
            int Hp = player->m_Attribute[ATTRIBUTE_HP].GetValueI();
            msgStream.Wirte(&Hp, sizeof(Hp));
            int maxHp = player->m_Attribute[ATTRIBUTE_HPMAX].GetValueI();
            msgStream.Wirte(&maxHp, sizeof(maxHp));
        }
        else
        {
            gLog.Warning("can't find member, the member is offline ?????? when call OnPlayerJoin.");
        }
    }

    if (count > 0)
    {
        //>����Ϣ֪ͨ�ͻ���
        MsgTeam_Created* pMsg = (MsgTeam_Created*)msgStream.Get();
        pMsg->memberNumber = count;
        pMsg->OBJID = player->m_GUID;
        pMsg->Len = msgStream.GetSize();
        g_GameServer->SendToClient(pMsg);
    }
}

void Team::OnPlayerExit(uint64 Guid)
{
    if (m_Members.size() <= 2)
    {
        //>���С��2����,��ɢ����
        OnDismiss();
    }
    else
    {
        MsgTeam_MemberExit exitMsg;
        exitMsg.memberId = Guid;
        auto it = m_Members.begin();
        for (auto end = m_Members.end(); it != end; ++it)
        {
            const Member& member = *it;
            exitMsg.OBJID = it->playerId;
            //>֪ͨ�г�Ա�뿪
            g_GameServer->SendToClient(&exitMsg);
        }

        const Member* leader = GetTeamLeader();
        assert(leader);

        bool isLeaderExit = (leader->playerId == Guid);
        //>�Ƴ�����
        RemoveMember(Guid);
        Player* player = g_MapManager->GetPlayerByID(Guid);
        if (player)
            player->m_teamId = 0;

        if (isLeaderExit)
        {
            //>����Ƕӳ��뿪
            auto it = m_Members.begin();
            it->attribute = MA_Leaber;

            MsgTeam_ChangeLeader changeMsg;
            changeMsg.memberId = it->playerId;
            changeMsg.memberAttribute = it->attribute;

            for (auto end = m_Members.end(); it != end; ++it)
            {
                const Member& member = *it;
                //>����Ϣ֪ͨ�ͻ���
                changeMsg.OBJID = member.playerId;
                g_GameServer->SendToClient(&changeMsg);
            }
        }
    }
}

void Team::OnCreateSucess()
{
    MsgTeam_Created msg;
    msg.memberNumber = m_Members.size();

    StackStream<> msgStream;
    msgStream.Wirte(&msg, sizeof(msg));

    int count = 0;
    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        Member& member = *it;

        //int64	memberId;
        //char	szName[MAX_NAME_SIZE];
        //int32   memberLevel;
        //int32   Hp;
        //int32   MaxHp;

        Player* player = g_MapManager->GetPlayerByID(member.playerId);
        if (player)
        {
            ++count;
            msgStream.Wirte(&player->m_GUID, sizeof(player->m_GUID));
            msgStream.Wirte(&member.attribute, sizeof(member.attribute));
            msgStream.Wirte(&player->m_Name, sizeof(char)*MAX_NAME_SIZE);
            int level = player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
            msgStream.Wirte(&level, sizeof(level));
            int Hp = player->m_Attribute[ATTRIBUTE_HP].GetValueI();
            msgStream.Wirte(&Hp, sizeof(Hp));
            int maxHp = player->m_Attribute[ATTRIBUTE_HPMAX].GetValueI();
            msgStream.Wirte(&maxHp, sizeof(maxHp));
        }
        else
        {
            gLog.Warning("can't find member, the member is offline ?????? when call OnCreateSucess.");
        }
    }

    if (count > 0)
    {
        //>����Ϣ֪ͨ�ͻ���
        MsgTeam_Created* pMsg = (MsgTeam_Created*)msgStream.Get();
        pMsg->memberNumber = count;
        pMsg->Len = msgStream.GetSize();
        auto it = m_Members.begin();
        for (auto end = m_Members.end(); it != end; ++it)
        {
            pMsg->OBJID = it->playerId;
            g_GameServer->SendToClient(pMsg);
        }
    }
}

void Team::OnDismiss()
{
    MsgTeam_MemberExit exitMsg;
    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        const Member& member = *it;
        exitMsg.OBJID = it->playerId;
        exitMsg.memberId = it->playerId;
        //>֪ͨ�������뿪.�����ɢ
        g_GameServer->SendToClient(&exitMsg);

        //>���ö���id
        Player* player = g_MapManager->GetPlayerByID(it->playerId);
        if (player)
            player->m_teamId = 0;

        g_SocailManager->RemovePlayerAndTeam(it->playerId);
    }

    Clear();
}

void Team::OnChangeLeader(Player* NewLeader)
{
    if (!HasMember(NewLeader->m_GUID))
    {
        gLog.Warning("new leader is not member when call OnChangeLeader.");
        return;
    }

    const Team::Member* leader = GetTeamLeader();
    if (leader && leader->playerId != NewLeader->m_GUID)
    {
        Team::Member* oldLeader = GetMember(leader->playerId);
        assert(oldLeader);
        oldLeader->attribute = MA_None;
        

        Team::Member* newLeader = GetMember(NewLeader->m_GUID);
        newLeader->attribute = MA_Leaber;

        MsgTeam_ChangeLeader changeMsg;
        changeMsg.memberId = newLeader->playerId;
        changeMsg.memberAttribute = newLeader->attribute;

        auto it = m_Members.begin();
        for (auto end = m_Members.end(); it != end; ++it)
        {
            const Member& member = *it;
            //>����Ϣ֪ͨ�ͻ���
            changeMsg.OBJID = member.playerId;
            g_GameServer->SendToClient(&changeMsg);
        }
    }
    else
    {
        gLog.Warning("old leader is new leader when call OnChangeLeader.");
    }
}

void Team::SendTeamInfo(uint64 playerId)
{
    MsgTeam_Created msg;
    msg.memberNumber = m_Members.size();

    StackStream<> msgStream;
    msgStream.Wirte(&msg, sizeof(msg));

    int count = 0;
    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        Member& member = *it;

        Player* player = g_MapManager->GetPlayerByID(member.playerId);
        if (player)
        {
            ++count;
            msgStream.Wirte(&player->m_GUID, sizeof(player->m_GUID));
            msgStream.Wirte(&member.attribute, sizeof(member.attribute));
            msgStream.Wirte(&player->m_Name, sizeof(char)*MAX_NAME_SIZE);
            int level = player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
            msgStream.Wirte(&level, sizeof(level));
            int Hp = player->m_Attribute[ATTRIBUTE_HP].GetValueI();
            msgStream.Wirte(&Hp, sizeof(Hp));
            int maxHp = player->m_Attribute[ATTRIBUTE_HPMAX].GetValueI();
            msgStream.Wirte(&maxHp, sizeof(maxHp));
        }
        else
        {
            gLog.Warning("can't find member, the member is offline ?????? when call OnCreateSucess.");
        }
    }

    if (count > 0)
    {
        //>����Ϣ֪ͨ�ͻ���
        MsgTeam_Created* pMsg = (MsgTeam_Created*)msgStream.Get();
        pMsg->memberNumber = count;
        pMsg->Len = msgStream.GetSize();
        auto it = m_Members.begin();
        pMsg->OBJID = playerId;
        g_GameServer->SendToClient(pMsg);
    }
}

void Team::RemoveMember(uint64 playerId)
{
    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        if (it->playerId == playerId)
        {
            g_SocailManager->RemovePlayerAndTeam(it->playerId);
            m_Members.erase(it);
            break;
        }
    }
}

Team::Member* Team::GetMember(uint64 playerId)
{
    auto it = m_Members.begin();
    for (auto end = m_Members.end(); it != end; ++it)
    {
        if (it->playerId == playerId)
        {
            return &(*it);
        }
    }
    return NULL;
}
