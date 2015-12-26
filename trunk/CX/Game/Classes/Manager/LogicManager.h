/********************************************************************
created:	2014/12/20
author:		LL
desc:		�߼���������������Ϸ�߼��Ĵ���
*********************************************************************/
#ifndef __LOGICMANAGER_H__
#define __LOGICMANAGER_H__

#include "GameDef.h"
#include "ClientDef.h"

class MapObj;
class Hero;
class Fight;
class Player;
class NPC;
class Monster;
class Collect;
struct NPCInfo;
struct MonsterInfo;
struct BaseInfoIt;
struct ResInfo;

USING_NS_CC;

struct GangSimpleInfo
{
	int32		id;
	char		szName[MAX_NAME_SIZE];
	int32		iLevel;
	char		szHeaderName[MAX_NAME_SIZE];
	int32		iGangNum;
	int32		iFight;
};

//����ˢ�¼�ʱ
struct MapMonsterRefresh2
{
	int time;
	cc2d::Vec2 BirthPos;
	MonsterInfo *mapmonsterrefresh;
};

class LogicManager
{
public: 
	LogicManager();
	~LogicManager();
	void Init();				//��ʼ��
	void Release();				//�ͷ�
	void Update(float delta);	//����	
//ս��
	NPC* GetAttackTarget();
	void PlayerUseSkill(MsgG2CPlayer_UseSkill* msg);
//��ͼ	
	void EnterGame();			//������Ϸ
	void EnterCopy(int copyid);	//���븱��
	void AddNPCToMap(NPCInfo* mapnpc);//���npc����ͼ
    void AddMonsterToMap(MsgMonster_Baseinfo* monsterInfo);//��ӹ��ﵽ��ͼ
	void AddOtherPlayerToMap(MsgOtherRole_BaseInfo* playerInfo);//�����ҵ���ͼ

	void AddCollectToMap(ResInfo* parRes);		//��Ӳɼ��ﵽ��ͼ

	void RemoveNpc(uint64 id);			//��NPC�Ӷ������Ƴ�
	void RemoveMonster(Monster* obj);	//������Ӷ������Ƴ�
	void RemovePlayer(uint64 id);		//����ҴӶ������Ƴ�
	void RemoveCollect(uint64 id);		//���ɼ���Ӷ������Ƴ�

	Player* GetPlayerByGUID(uint64 guid);	//����guidȡ���
    inline Player* GetAllPlayerByGUID(uint64 guid); //������,���������Լ�
	NPC* GetNPCByGUID(uint64 guid);		//����guidȡnpc
	NPC* GetNPCByID(uint64 id);		//����idȡnpc
	Monster* GetMonsterByGUID(uint64 guid);//����guidȡ����
	MapObj* GetObjectByType(uint64 guid, ObjectType type);	//��������ȡ����
    inline bool IsHeroGuid(uint64 guid) const; //>�Ƿ�������ΨһID

	bool	GetPlayerIsTeam(uint64 guid);	//����guidȡ����Ƿ����
	bool	GetPlayerIsCamp(uint64 guid);	//����guidȡ����Ƿ�ͬһ��Ӫ

	void HeroUseSkillSetCD(int skillid);	//ʹ�ü��ܣ����ڼ�¼����cd

	std::map<int, float> m_SkillCDList;			//����cd�б�0Ϊ����cd������id�����ȼ�
//�������    
	BaseInfoIt m_BaseInfo;			//�������������������ݣ��������Ǻ����
	Hero* m_Hero;	//����
	std::vector<Player*>        m_PlayerList;//����б�
	std::vector<NPC*>           m_NPCList;	//NPC�б�
	std::vector<Monster*>       m_MonsterList;	//�����б�
	//std::vector<Collect*> m_Collect;	//�ɼ����б� 
//
	int m_CurrCopyId;	//��ǰ����id
	int m_CurrMapId;	//��ǰ��ͼid

	AttributeType ListChestType;
	int ListChestNum;
	
	
	std::vector<GangSimpleInfo>	m_vecGangList;
};

#include "Hero.h"
inline Player* LogicManager::GetAllPlayerByGUID(uint64 guid)
{
    if (m_Hero && m_Hero->m_GUID == guid)
    {
        return m_Hero;
    }
    else
    {
        return GetPlayerByGUID(guid);
    }
}

bool LogicManager::IsHeroGuid(uint64 guid) const
{
    if (m_Hero->m_GUID == guid)
    {
        return true;
    }
    return false;
}

extern LogicManager* g_LogicManager;
#endif  // __LOGICMANAGER_H__