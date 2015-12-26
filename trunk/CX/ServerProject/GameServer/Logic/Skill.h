/********************************************************************
created:	2015/11/23
author:		lishihai
desc:		�������
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


//>����
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
    Object* m_Attacker;           //>������
    const SkillInfo* m_Info;    //>��������
    int64 m_TargetGuid;           //>Ŀ��
    int32 m_TargetType;         //>Ŀ������
    float m_Time;
    bool m_IsEnd;
};

//>�˺���buff
class DamageAndBuff : public Skill
{
public:
    virtual void Process();
};

//>����
class Cure : public Skill
{
public:
    virtual void Process();
};

//>��������buff,�����˺�
class QuChuYouYi : public Skill
{
public:
    virtual void Process();
};

////>�����к�buff,�������˺�
//class QuChuYouHai : public Skill
//{
//public:
//    virtual void Process();
//};

//>�����˺�
class JianShe : public Skill
{
public:
    virtual void Process();
};


//>����
class ChaoFeng  : public Skill
{
public:
    virtual void Process();
};

//>��Ѫ
class XiXue  : public Skill
{
public:
    virtual void Process();
};

//>��ʵ�˺�
class ZhenShiShangHai  : public Skill
{
public:
    virtual void Process();
};

//>���ܷ�����
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


//>���ܹ���
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
