/********************************************************************
created:	2015/11/26
author:		lishihai
desc:		战斗组件
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

//>战斗组件
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

        virtual void  GetSkillsForInterestNormalSkill(std::vector<const SkillInfo*>& skills) {} //>获得普通攻击所触发的技能
        virtual void  GetSkillsForInterestBeHurt(std::vector<const SkillInfo*>& skills){} //>获得受击所触发的技能
        virtual void  GetSkillsForInterestHpPct(std::vector<const SkillInfo*>& skills) {}  //>获得hp百分比所触发的技能
        virtual void  GetSkillsForInterestAddBuff(std::vector<const SkillInfo*>& skills) {} //>获得添加buff类型所触发的技能
    };
public:
    BattleComponent(Object* owner, Interface* pInterface = NULL);


    bool    UseSkill(int skillId, Object* target);
 
    bool    AddBuff(Buff* pBuff);                   //>添加buff
    void    RemoveBuffByType(int buffType);         //>驱除buff类型
    bool    IsDisableAddBuff(int buffType);         //>免疫buff类型
    int     DamageReduceByHuDunBuff(int damage);    //>护盾buff抵挡伤害
    int     DamageInverseByFanShangBuff(int damage);//>计算buf反伤
    int     DamageToHpBuff(int damage);             //>伤害转换hp
    bool    HasWudiBuff();                          //>是否有无敌buff
    

    bool    IsTriggerPassiveSkill(const SkillInfo* pSkillInfo);   //>检测被动技能概率
    void    BuHurtProcess();                        //>受击处理
    void    BeHurtTriggerPassiveSkill();            //>受击触发被动技能
    void    BeHurtRemoveXuanYunBuff();              //>受击消除昏睡buff

    void    Update(float dt);

    inline const std::list<Buff*>& GetBuffList() const;
protected:
    inline void ResetBuffInstanceIdCounter();
public:
    Object* m_Owner;    //>单位
    Interface* m_Interface;

    int     m_ChenMo;   //>沉默
    int     m_FengXue;  //>封穴
    int     m_XuanYun;  //>眩晕
    int     m_HunShui;  //>昏睡
    int     m_HunLuan;  //>混乱
    int     m_WuDi;     //>无敌
private:
    void    UpdateSkills(float dt);
    void    UpdateBuffs(float dt);
    void    UpdateCDs(float dt);

    std::list<Skill*> m_SkillInstances; //>技能数据
    std::list<Buff*> m_BuffDatas;   //>buff数据

    //float   m_CommonCD;  //>公共CD
    std::unordered_map<int32, float> m_SkillCDMap;    //>技能CD
    float           m_CurSkilllLeftTime;    //>当前技能结束时间.
    int32           m_CurSkillId;           //>当前正则释放的技能.
    int32           m_BuffInstanceIdCounter;  //>buff实例id计数器
};


//////////////////////////////////////////////////////////////////////////
//帮助函数
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
