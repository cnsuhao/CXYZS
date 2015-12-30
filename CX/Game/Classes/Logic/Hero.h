/********************************************************************
created:	2014/12/27
author:		LL
desc:		主角类
*********************************************************************/

#ifndef __HEROH_H__
#define __HEROH_H__

#include "Player.h"
#include "Bag.h"
#include "Gang.h"

enum AutoActionStepType
{
	STEP_MOVE,		//移动
	STEP_ATTACK,	//攻击
	STEP_TALK,		//与npc对话
	STEP_OPENCOPY,	//打开副本ui
	STEP_GOTPMAP,	//地图跳转
};
//自动寻路的步骤结构
struct  AutoActionStep
{
	AutoActionStepType stepType;	//步骤类型
	int	param;			//参数，具体含义根据类型来确定
	cc2d::Vec2 grid;		//目标格子
};
//法宝数据
struct  MagicWeaponData
{
	int id;			//模板id，对应表中id
	int level;		//法宝等级
	int exp;		//当前经验
	int Mountid;	//器灵id（坐骑）
	float Growth;	//成长系数
};

struct friendInfo
{
	int64	iRoleID;				//角色ID
	char	szName[MAX_NAME_SIZE];	//好友名称
	bool	bIsLine;				//是否在线
	int		iLevel;					//等级
	int		iFight;					//战斗力
};

class Hero: public Player
{
public:
	Hero();
	virtual ~Hero();

	bool Init();
	void Update(float delta);
	
	bool FindPath(const cc2d::Vec2 &tagPos);						//寻找路径
	bool FindPathByDirection(DirectionType direction);		//根据方向寻找路径
    void StopMove();

	//属性相关
	void SetAttribute(AttributeType property, float value);		//设置属性	
	float m_Attribute[ATTRIBUTE_MAX];		//属性
	int64 m_OfflineTime;                //离线时间，用于上线时判断是否刷新每日次数
	int32 m_activeSkillType;		    //激活的技能类型


	void LoadBaseAttribute(BaseInfoIt& baseInfo);   //>加载基础属性
	//void LoadDetailAttribute(AttributeType type, float attributeValue);

    bool IsIdleActionType() { return false; }
    bool IsMoveActionType() { return false; }


	int32 m_ChoseSkillList[MAX_CHOSESKILL];		//当前主ui显示的技能
    int32 m_ActivateSkillType;                  //当前主修技能

	//法宝相关
	std::map<int, MagicWeaponData>  m_MagicWeaponIDList;	//当前拥有法宝列表
	int m_CurrentMagicWeaponID;								//激活的法宝

	//公会id
	int32	m_isocietyID;
	Gang	m_GangInfo;

    MapObj* m_SelTarget;

	//好友相关
	std::map<int64,friendInfo>	m_mapFriendsInfo;			//当前好友
	std::map<int64,friendInfo>	m_mapBlackList;				//当前好友

	std::vector<TaskInfoIt>		m_vecTaskList;				//任务列表

//技能相关
    const SkillInfoIt* GetSkillByType(int32 skillType);
    void SetChoseSkill(int32 index, int32 skillId);     //>设置选择使用的技能
    bool CheckSkillCooling(int skillId);                //>检测技能是否冷却
    float GetSkillLastCD(int skillId);                  //>获得技能cd剩余时间
    vector<SkillInfoIt>  m_SkillList;	                //当前拥有技能
    std::unordered_map<int32, float> m_SkillCDMap;      //>技能CD

	
//状态表  用法：主角做PlayerState_Operation操作之前调用CheckOperInState函数判断操作是否允许执行，若返回true则在调用ChangeState后执行PlayerState_Operation的具体操作。
	bool m_State[State_Max];								//当前状态信息
	bool CheckOperInState(PlayerState_Operation _oper);		//检查该操作在当前状态下是否可进行
	bool ChangeState(PlayerState_Operation _oper);			//根据操作改变状态
	bool GetState(PlayerState_States _state);				//是否处于某种状态下
private:
	void AddState(PlayerState_States _state);		//添加状态	×××非必要情况别使用此函数
	void CancelState(PlayerState_States _state);	//取消状态	×××非必要情况别使用此函数
public:
	TaskInfoIt* GetTaskInfoItByID(int32 id);				//根据taskid取得任务状态信息

	int64	m_iFindPathClickNpc;							//寻路完成后点击NPC


//>动画/显示组件接口
    virtual float AC_GetMoveSpeed();

//>移动组件接口
    virtual float MC_GetMoveSpeed();

//>战斗组件接口
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
