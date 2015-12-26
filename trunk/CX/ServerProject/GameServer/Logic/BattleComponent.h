/********************************************************************
created:	2015/11/26
author:		lishihai
desc:		ս�����
*********************************************************************/

#ifndef __BATTLECOMPONENT_H__
#define __BATTLECOMPONENT_H__

#include "GameDef.h"
#include <list>
#include <unordered_map>

class Object;
class Buff;
class Skill;
struct SkillInfo;

//>ս�����
class BattleComponent
{
    friend  class Object;
public:
    class Interface
    {
    public:
        virtual bool  IsNormalSkill(int skillId) { return false; }
        virtual bool  HasSkill(int skillId) { return true; }
        virtual void  OnUseSkill(int skillId, Object* target) {}
        virtual void  OnAddBuff(Buff* pBuff) {}
        virtual void  OnRemoveBuff(Buff* pBuff) {}
        virtual void  OnBuffDead(Buff* pBuff) {}

        virtual void  GetSkillsForInterestNormalSkill(std::vector<const SkillInfo*>& skills) {} //>�����ͨ�����������ļ���
        virtual void  GetSkillsForInterestBeHurt(std::vector<const SkillInfo*>& skills){} //>����ܻ��������ļ���
        virtual void  GetSkillsForInterestHpPct(std::vector<const SkillInfo*>& skills) {}  //>���hp�ٷֱ��������ļ���
        virtual void  GetSkillsForInterestAddBuff(std::vector<const SkillInfo*>& skills) {} //>������buff�����������ļ���
    };
public:
    BattleComponent(Object* owner, Interface* pInterface = NULL);


    bool    UseSkill(int skillId, Object* target);
 
    bool    AddBuff(Buff* pBuff);                   //>���buff
    void    RemoveBuffByType(int buffType);         //>����buff����
    bool    IsDisableAddBuff(int buffType);         //>����buff����
    int     DamageReduceByHuDunBuff(int damage);    //>����buff�ֵ��˺�
    int     DamageInverseByFanShangBuff(int damage);//>����buf����
    int     DamageToHpBuff(int damage);             //>�˺�ת��hp
    bool    HasWudiBuff();                          //>�Ƿ����޵�buff
    

    bool    IsTriggerPassiveSkill(const SkillInfo* pSkillInfo);   //>��ⱻ�����ܸ���
    void    BuHurtProcess();                        //>�ܻ�����
    void    BeHurtTriggerPassiveSkill();            //>�ܻ�������������
    void    BeHurtRemoveXuanYunBuff();              //>�ܻ�������˯buff

    void    Update(float dt);

    inline const std::list<Buff*>& GetBuffList() const;
protected:
    inline void ResetBuffInstanceIdCounter();
public:
    Object* m_Owner;    //>��λ
    Interface* m_Interface;

    int     m_ChenMo;   //>��Ĭ
    int     m_FengXue;  //>��Ѩ
    int     m_XuanYun;  //>ѣ��
    int     m_HunShui;  //>��˯
    int     m_HunLuan;  //>����
    int     m_WuDi;     //>�޵�
private:
    void    UpdateSkills(float dt);
    void    UpdateBuffs(float dt);
    void    UpdateCDs(float dt);

    std::list<Skill*> m_SkillInstances; //>��������
    std::list<Buff*> m_BuffDatas;   //>buff����

    //float   m_CommonCD;  //>����CD
    std::unordered_map<int32, float> m_SkillCDMap;    //>����CD
    float           m_CurSkilllLeftTime;    //>��ǰ���ܽ���ʱ��.
    int32           m_CurSkillId;           //>��ǰ�����ͷŵļ���.
    int32           m_BuffInstanceIdCounter;  //>buffʵ��id������
};


//////////////////////////////////////////////////////////////////////////
//��������
inline bool NotHasState(int state)
{
    if (state <= 0)
        return true;

    return false;
}

inline bool HasState(int state)
{
    if (state > 0)
        return true;

    return false;
}

inline const std::list<Buff*>& BattleComponent::GetBuffList() const
{
    return m_BuffDatas;
}

inline void BattleComponent::ResetBuffInstanceIdCounter()
{
    m_BuffInstanceIdCounter = 1;
}

#endif	//__BATTLECOMPONENT_H__
