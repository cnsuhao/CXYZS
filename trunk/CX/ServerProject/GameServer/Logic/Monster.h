/********************************************************************
created:	2015/11/29
author:		lishihai
desc:		������
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
	virtual void Attack(int64 guid);	    //����	
	virtual void MoveCallback(int type);	//�ƶ���ʱ�ص�
	virtual void AttackCallback(int type);	//������ʱ�ص�
    virtual bool AddHp(const AddHpMode& mode);          //>��Ѫ
    virtual bool DropHP(const DropHpMode& mode);		//>��Ѫ
    virtual void Idle();                                //>����
    virtual void Die();					                //>����
	virtual void RefreshCallback(int type);	//����ˢ�»ص�
	float CalcMonsterOddsAttribute(AttributeType attribute,AttributeType odds);	//ȡ���Ｘ������
    
    void OnEnterMap();
    void OnLeaveMap();

//>�ƶ�����ӿ�
    virtual void    OnMoveByTarget(const Vec2& moveTarget);
    virtual void    OnMoveByPath(std::vector<int>& paths, const Vec2& moveTarget);
    virtual void    OnMoveEnd();

//>ս������ӿ�
    virtual bool  IsNormalSkill(int skillId);
    virtual bool  HasSkill(int skillId);
    virtual void  OnUseSkill(int skillId, Object* target);

    virtual void  GetSkillsForInterestNormalSkill(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestBeHurt(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestHpPct(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestAddBuff(std::vector<const SkillInfo*>& skills);


	int64 m_AttTargteGuid;					//����Ŀ��guid
	Vec2 m_MonsterBirthPos;					//��������㣨���ӣ�
	MonsterInfo* m_MonsterInfo;				//��Ӧcsv������
	int m_HP;
	int m_HPMax;

    inline bool IsMoveing() const;
private:
    bool m_IsMoveing; 

    std::vector<int64>        m_SelectedMePlayers; //>ѡ��Ŀ��Ϊ�Լ������id. 
};


inline bool Monster::IsMoveing() const
{
    return m_IsMoveing;
}

#endif	//__Monster_H__
