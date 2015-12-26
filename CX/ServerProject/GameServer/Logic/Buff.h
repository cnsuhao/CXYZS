/********************************************************************
created:	2015/11/23
author:		lishihai
desc:		buff功能
*********************************************************************/

#ifndef __BUFF_H__
#define __BUFF_H__

#include <queue>

struct BuffInfo;

//
//enum BuffFunction
//{
//    Buff_ChenMo = 1,//>沉默, 不释放主动技能
//    Buff_FengXue,   //>封穴, 不触发绝技,
//    Buff_XuanYun,   //>眩晕, 不能攻击,不能移动
//    Buff_HunShui,   //>昏睡, 不能攻击,不能移动, 能被打醒
//    Buff_HunLuan,   //>混乱, 随机攻击非自己的任意目标, 不受玩家控制
//    Buff_Wudi,      //>无敌, 不受任意伤害, 不受有害buff.不受控制buff
//
//    Buff_DisableBuff,   //>免疫, 不受指定类型的buff
//    Buff_HuDun,         //>护盾,吸收一定伤害数值
//    Buff_HuDunPct,      //>护盾,吸收自身血量上限百分比数值
//    Buff_HuDunMp2Hp,    //>护盾,将百分之x的mp转护盾值
//    Buff_DamagePctToHp, //>吸血, 伤害值的百分比变成hp
//    Buff_FanShang,      //>反伤,承受伤害值的百分比
//
//    Buff_HuoQiang,      //>火墙
//    Buff_ZhaoHuang,     //>召唤
//
//    Buff_DropHp,        //>持续掉血(固定值)
//    Buff_DropCurHpPct,  //>持续掉血(当前血量百分比)
//    Buff_DropHpMaxPct,  //>持续掉血(最大血量百分比)
//
//    Buff_AddHpMax,      //>添加Hp上限值
//    Buff_AddHpMaxPct,   //>添加Hp上限百分比
//
//    Buff_AddMpMax,      //>添加Mp上限
//    Buff_AddMpMaxPct,   //>添加Mp上限百分比
//
//    Buff_AddMoveSpeed,      //>添加移动速度
//
//    Buff_AddAttackPower,    //>添加攻击力
//    Buff_AddAttackPowerPct, //>添加攻击力百分比
//    Buff_AddDefencePower,   //>添加防御力
//    Buff_AddDefencePowerPct,//>添加防御力百分比
//    Buff_AddCrit,           //>添加暴击率
//    Buff_AddCritPct,        //>添加暴击率百分比
//    Buff_AddHit,            //>添加命中率
//    Buff_AddHitPct,         //>添加命中率百分比
//    Buff_AddTenacity,       //>添加韧性
//    Buff_AddTenacityPct,    //>添加韧性百分比
//    Buff_AddDodge,          //>添加闪避率
//    Buff_AddDodgePct,       //>添加闪避率百分比
//
//    Buff_AddAtkFactor,      //>添加攻击系数
//    Buff_AddDefFactor,      //>添加防御系数
//    Buff_AddCritFactor,     //>添加暴击系数
//};

class Object;
//> buff
class Buff
{
    friend class BuffFactory;
    friend class Object;
    friend class Skill;
    friend class BattleComponent;
public:
	Buff();
	virtual ~Buff() {}

    virtual void SetData(const float data[3]) {}
    virtual void OnAttach()  {}
    virtual void OnDettach() {}
    virtual void Update(float dt) {}

    const BuffInfo* GetInfo()       { return m_BuffInfo; }
    inline float    GetTime()       { return m_Time; }
    inline int      GetInstanceId() { return m_InstanceId; }
protected:
    Object*     m_Owner;
    const BuffInfo* m_BuffInfo;
    float       m_Time;
    bool        m_NeedUpdate;
    int         m_InstanceId;
};

//>沉默
class ChenMo : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>封穴
class FengXue : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>眩晕
class XuanYun : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>昏睡
class HunShui : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>混乱
class HunLuan : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>无敌
class WuDi : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>免疫
class MainYi : public Buff
{
    MainYi() { m_DisableBuffType = 0; }
public:
    virtual void SetData(float data[3]);

    inline bool IsDisableAdd(int buffType) { return m_DisableBuffType == buffType; }

    int m_DisableBuffType;
};

//>护盾
class HuDun : public Buff
{
public:
    int DamageReduce(int damage);

    void SetHp(int hp) { m_Hp = hp; }
    int m_Hp;
};

//>吸血
class DamagePctToHp : public Buff
{
public:
    int GetHp(int damage);

    float m_HpFactor;
};

//>反伤
class FanShang : public Buff
{
public:
    FanShang() { m_FanShangFactor = 0.0f; }

    virtual void SetData(const float data[3]);

    int FanShangDamage(int damage);

    float m_FanShangFactor;
};


//>持续掉血
class ChiXuDiaoXue : public Buff
{
public:
    virtual void SetData(float data[3]);

    virtual void Update(float dt);

    float m_IntervalCd;
    float m_DropHp;


    float m_Interval;
};

class ChangeObjAttribute : public Buff
{
public:
    virtual void SetData(const float data[3]);

    virtual void OnAttach();
    virtual void OnDettach();

    float m_AddValue;
};


//>buff分配器
template <class T>
class BuffAllocer
{
public:
    T* Alloc();
    void Free(T* ptr);
    std::queue<T*> _freeContainer;
};

template <class T>
T* BuffAllocer<T>::Alloc()
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
void BuffAllocer<T>::Free(T* ptr)
{
    _freeContainer.push(ptr);
}

//>buff工厂
class BuffFactory
{
public:
    static Buff* AllocBuff(const BuffInfo* pInfo, float time);
    static void FreeBuff(Buff* pBuff);

private:
    static BuffAllocer<ChenMo>  _ChenMoAllocer;
    static BuffAllocer<FengXue> _FengXueAllocer;
    static BuffAllocer<XuanYun> _XuanYunAllocer;
    static BuffAllocer<HunShui> _HunShuiAllocer;
    static BuffAllocer<HunLuan> _HunLuanAllocer;
    static BuffAllocer<WuDi>    _WuDiAllocer;
    static BuffAllocer<FanShang> _FanShangAllocer;
    static BuffAllocer<ChangeObjAttribute> _ChangeObjAttrAllocer;
};

#endif	//__BUFF_H__
