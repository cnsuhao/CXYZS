/********************************************************************
created:	2015/11/29
author:		lishihai
desc:		怪物类
*********************************************************************/

#ifndef __Monster_H__
#define __Monster_H__

#include "Object.h"
#include "MoveComponent.h"
#include "BattleComponent.h"
#include "AIComponent.h"

struct MonsterInfo;

class Monster : public Object, 
                public MoveComponent::Interface , 
                public BattleComponent::Interface,
                public AIComponent::Interface
{
public:
	Monster();
	virtual ~Monster();   
	virtual bool Init(MonsterInfo *info);
	virtual void Attack(int64 guid);	    //攻击	
	virtual void MoveCallback(int type);	//移动定时回调
	virtual void AttackCallback(int type);	//攻击定时回调
    virtual bool AddHp(const AddHpMode& mode);          //>加血
    virtual bool DropHP(const DropHpMode& mode);		//>掉血
    virtual void Idle();                                //>待机
    virtual void Die();					                //>死亡
	virtual void RefreshCallback(int type);	//怪物刷新回调
	float CalcMonsterOddsAttribute(AttributeType attribute,AttributeType odds);	//取怪物几率属性
    
    void OnEnterMap();
    void OnLeaveMap();

//>移动组件接口
    virtual void    OnMoveByTarget(const Vec2& moveTarget);
    virtual void    OnMoveByPath(std::vector<int>& paths, const Vec2& moveTarget);
    virtual void    OnMoveEnd();

//>战斗组件接口
    virtual bool  IsNormalSkill(int skillId);
    virtual bool  HasSkill(int skillId);
    virtual void  OnUseSkill(int skillId, Object* target);

    virtual void  GetSkillsForInterestNormalSkill(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestBeHurt(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestHpPct(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestAddBuff(std::vector<const SkillInfo*>& skills);


	int64 m_AttTargteGuid;					//攻击目标guid
	Vec2 m_MonsterBirthPos;					//怪物出生点（格子）
	MonsterInfo* m_MonsterInfo;				//对应csv表数据
	int m_HP;
	int m_HPMax;

    inline bool IsMoveing() const;
private:
    bool m_IsMoveing; 

    std::vector<int64>        m_SelectedMePlayers; //>选择目标为自己的玩家id. 
};


inline bool Monster::IsMoveing() const
{
    return m_IsMoveing;
}

#endif	//__Monster_H__
