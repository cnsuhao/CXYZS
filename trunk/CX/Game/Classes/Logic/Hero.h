/********************************************************************
created:	2014/12/27
author:		LL
desc:		������
*********************************************************************/

#ifndef __HEROH_H__
#define __HEROH_H__

#include "Player.h"
#include "Bag.h"
#include "Gang.h"

enum AutoActionStepType
{
	STEP_MOVE,		//�ƶ�
	STEP_ATTACK,	//����
	STEP_TALK,		//��npc�Ի�
	STEP_OPENCOPY,	//�򿪸���ui
	STEP_GOTPMAP,	//��ͼ��ת
};
//�Զ�Ѱ·�Ĳ���ṹ
struct  AutoActionStep
{
	AutoActionStepType stepType;	//��������
	int	param;			//���������庬�����������ȷ��
	cc2d::Vec2 grid;		//Ŀ�����
};
//��������
struct  MagicWeaponData
{
	int id;			//ģ��id����Ӧ����id
	int level;		//�����ȼ�
	int exp;		//��ǰ����
	int Mountid;	//����id�����
	float Growth;	//�ɳ�ϵ��
};

struct friendInfo
{
	int64	iRoleID;				//��ɫID
	char	szName[MAX_NAME_SIZE];	//��������
	bool	bIsLine;				//�Ƿ�����
	int		iLevel;					//�ȼ�
	int		iFight;					//ս����
};

class Hero: public Player
{
public:
	Hero();
	virtual ~Hero();

	bool Init();
	void Update(float delta);
	
	bool FindPath(const cc2d::Vec2 &tagPos);						//Ѱ��·��
	bool FindPathByDirection(DirectionType direction);		//���ݷ���Ѱ��·��
    void StopMove();

	//�������
	void SetAttribute(AttributeType property, float value);		//��������	
	float m_Attribute[ATTRIBUTE_MAX];		//����
	int64 m_OfflineTime;                //����ʱ�䣬��������ʱ�ж��Ƿ�ˢ��ÿ�մ���
	int32 m_activeSkillType;		    //����ļ�������


	void LoadBaseAttribute(BaseInfoIt& baseInfo);   //>���ػ�������
	//void LoadDetailAttribute(AttributeType type, float attributeValue);

    bool IsIdleActionType() { return false; }
    bool IsMoveActionType() { return false; }


	int32 m_ChoseSkillList[MAX_CHOSESKILL];		//��ǰ��ui��ʾ�ļ���
    int32 m_ActivateSkillType;                  //��ǰ���޼���

	//�������
	std::map<int, MagicWeaponData>  m_MagicWeaponIDList;	//��ǰӵ�з����б�
	int m_CurrentMagicWeaponID;								//����ķ���

	//����id
	int32	m_isocietyID;
	Gang	m_GangInfo;

    MapObj* m_SelTarget;

	//�������
	std::map<int64,friendInfo>	m_mapFriendsInfo;			//��ǰ����
	std::map<int64,friendInfo>	m_mapBlackList;				//��ǰ����

	std::vector<TaskInfoIt>		m_vecTaskList;				//�����б�

//�������
    const SkillInfoIt* GetSkillByType(int32 skillType);
    void SetChoseSkill(int32 index, int32 skillId);     //>����ѡ��ʹ�õļ���
    bool CheckSkillCooling(int skillId);                //>��⼼���Ƿ���ȴ
    float GetSkillLastCD(int skillId);                  //>��ü���cdʣ��ʱ��
    vector<SkillInfoIt>  m_SkillList;	                //��ǰӵ�м���
    std::unordered_map<int32, float> m_SkillCDMap;      //>����CD

	
//״̬��  �÷���������PlayerState_Operation����֮ǰ����CheckOperInState�����жϲ����Ƿ�����ִ�У�������true���ڵ���ChangeState��ִ��PlayerState_Operation�ľ��������
	bool m_State[State_Max];								//��ǰ״̬��Ϣ
	bool CheckOperInState(PlayerState_Operation _oper);		//���ò����ڵ�ǰ״̬���Ƿ�ɽ���
	bool ChangeState(PlayerState_Operation _oper);			//���ݲ����ı�״̬
	bool GetState(PlayerState_States _state);				//�Ƿ���ĳ��״̬��
private:
	void AddState(PlayerState_States _state);		//���״̬	�������Ǳ�Ҫ�����ʹ�ô˺���
	void CancelState(PlayerState_States _state);	//ȡ��״̬	�������Ǳ�Ҫ�����ʹ�ô˺���
public:
	TaskInfoIt* GetTaskInfoItByID(int32 id);				//����taskidȡ������״̬��Ϣ

	int64	m_iFindPathClickNpc;							//Ѱ·��ɺ���NPC


//>����/��ʾ����ӿ�
    virtual float AC_GetMoveSpeed();

//>�ƶ�����ӿ�
    virtual float MC_GetMoveSpeed();

//>ս������ӿ�
    virtual void  OnSkillBegin(int skillId, MapObj* target);
public:
    Bag m_Bag;
    int m_BattlePower;
    int m_VIPLevel;
public:
    bool IsTeamMember(uint64 guid);
    bool IsTeamLeader();
    bool HasTeam() { return m_team.m_Members.size() > 0; }

    struct Team
    {
        enum MemberAttribute
        {
            MA_None,
            MA_Leaber = 1,
        };

        struct Member
        {
            uint64 memberId;
            MemberAttribute attribute;
            char szName[MAX_NAME_SIZE];
            int32 memberLevel;
            int32 Hp;
            int32 MaxHp;
        };

        std::vector<Member> m_Members;
    };

    Team m_team;
};

#endif	//__HEROH_H__
