//////////////////////////////////////////////////////////////////////////
//    File Name:      SocailManager.h
//    Description:    �罻������
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
	//���ѹ���
	bool	AddFriends(MsgBase* msg);			//��Ӻ���
	bool	DelFriends(MsgBase* msg);			//ɾ������
	bool	GetFriendsList(MsgBase* msg);		//�����б�
	bool	GetFriendsInof(MsgBase* msg);		//�鿴������ϸ��Ϣ
	//���칦��
	bool	ChannelMessage(MsgBase* msg);		//Ƶ����Ϣ
	bool	PointMessage(MsgBase* msg);			//������Ϣ

	//���Ṧ��
	bool	CreateGang(MsgBase* msg);			//�������
	bool	GetGangInfo(MsgBase* msg);			//��ð�����Ϣ
	bool	ApplyAddGang(MsgBase* msg);			//����Ӱ�
	bool	ReplyAddGang(MsgBase* msg);			//�ظ��Ӱ�����
	bool	GetGangApplyList(MsgBase* msg);		//��ð��������б�
	bool	LeaveGang(MsgBase* msg);			//�뿪���
	bool	PutGang(MsgBase* msg);				//������
	bool	GetGangList(MsgBase* msg);			//��ð����б�
	bool	GangAnnouncement(MsgBase* msg);		//���ɹ���
	bool	EditAnnouncement(MsgBase* msg);		//�༭���ṫ��
	bool	SendEmail(MsgBase* msg);			//���Ͱ����ʼ�
	bool	AddGangContribution(MsgBase* msg);	//��Ӱ��ɹ���
	bool	GetGangContribution(MsgBase* msg);	//�鿴���ɹ���
	bool	GangAppoint(MsgBase* msg);			//����
	bool	GangTransfer(MsgBase* msg);			//װ��
	bool	GangUpGrade(MsgBase* msg);			//��������
	bool	GetGangLevel(MsgBase* msg);			//��ð��ɵȼ�
	bool	GangShopBuy(MsgBase* msg);			//�����̵깺��
	bool	StartCopy(MsgBase* msg);			//��������
	bool	CopyReward(MsgBase* msg);			//��������


    Team*   GetTeam(uint32 teamId);				//>���Ҷ���
    Team*   AllocTeam();                        //>�������
    void    FreeTeam(Team* team);               //>�ͷŶ���
    

    inline Team*   FindTeam(uint64 playerGuid);
    inline void    InserPlayerAndTeam(uint64 playerGuid, uint32 teamId);
    inline void    RemovePlayerAndTeam(uint64 playerGuid);
private:
    void    AllocTeamCapacity();                //>��������
    std::vector<Team*> m_teams;                 //>�����б�
    std::queue<Team*> m_freeTeams;             //>�ͷŵĶ����б�
    uint32  m_teamCapacity;                     //>�������
    std::unordered_map<uint64, uint32> m_playerTeam;      //��Ҷ����б�
    
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

	//����½�����ID
	int64 GetGangidForGangMap();
};
extern GangManager* g_GangManager;

#endif	//__SOCAILMANAGER_H__