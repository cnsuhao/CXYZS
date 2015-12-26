/********************************************************************
created:	2015/11/23
author:		lishihai
desc:		技能相关
*********************************************************************/

#include "GameDef.h"
#include "Vec2.h"

#include <queue>
#include <list>

#ifndef __ABLITITY_H__
#define __ABLITITY_H__

class Object;
class Buff;
struct SkillInfo;

struct SvrSkillInfo
{
    SvrSkillInfo() { pCfg = NULL; }
    SkillInfoIt data;
    const SkillInfo* pCfg;
};


//>技能
class Skill
{
    friend class SkillFactory;
public:
	Skill();
	virtual ~Skill();

    virtual void Init(Object* pAttacker, Object* pTarget);

    void InnerBegin();
    void InnerUpdate(float dt);
    void InnerEnd();
    
    void AddBuff(std::vector<Object*>& targets);
    void GetTargetsByType(SkillTargetType type, std::vector<Object*>& targets);
    void GetBuffTargets(std::vector<Object*>& targets);

    virtual void OnBegin() {}
    virtual void Process() {}
    virtual void OnEnd() {}

    inline bool IsEnd() { return m_IsEnd; }
    inline const SkillInfo* GetInfo() { return m_Info; }
protected:
    Object* m_Attacker;           //>攻击者
    const SkillInfo* m_Info;    //>技能配置
    int64 m_TargetGuid;           //>目标
    int32 m_TargetType;         //>目标类型
    float m_Time;
    bool m_IsEnd;
};

//>伤害和buff
class DamageAndBuff : public Skill
{
public:
    virtual void Process();
};

//>治疗
class Cure : public Skill
{
public:
    virtual void Process();
};

//>驱除有益buff,计算伤害
class QuChuYouYi : public Skill
{
public:
    virtual void Process();
};

////>驱除有害buff,不计算伤害
//class QuChuYouHai : public Skill
//{
//public:
//    virtual void Process();
//};

//>溅射伤害
class JianShe : public Skill
{
public:
    virtual void Process();
};


//>嘲讽
class ChaoFeng  : public Skill
{
public:
    virtual void Process();
};

//>吸血
class XiXue  : public Skill
{
public:
    virtual void Process();
};

//>真实伤害
class ZhenShiShangHai  : public Skill
{
public:
    virtual void Process();
};

//>技能分配器
template <class T>
class SkillAllocer
{
public:
    T* Alloc();
    void Free(T* ptr);
    std::queue<T*> _freeContainer;
};

template <class T>
T* SkillAllocer<T>::Alloc()
{
    if (_freeContainer.size() > 0)
    {
        T* ptr = _freeContainer.front();
        _freeContainer.pop();
        return ptr;
    }
    return new T();
}

template <class T>
void SkillAllocer<T>::Free(T* ptr)
{
    _freeContainer.push(ptr);
}


//>技能工厂
class SkillFactory
{
public:
    static Skill* AllocSkill(const SkillInfo* pSkillInfo);
    static void FreeSkill(Skill* pSkill);


private:
    static SkillAllocer<DamageAndBuff>  _DamageAndBuffAllocer;
    static SkillAllocer<Cure>           _CureAllocer;
    static SkillAllocer<QuChuYouYi>     _QuChuYouYiAllocer;
    static SkillAllocer<JianShe>        _JianSheAllocer;
    static SkillAllocer<ChaoFeng>       _ChaoFengAllocer;
    static SkillAllocer<XiXue>          _XiXueAllocer;
    static SkillAllocer<ZhenShiShangHai> _ZhenShiShangHaiAllocer;
};


#endif	//__ABLITITY_H__
