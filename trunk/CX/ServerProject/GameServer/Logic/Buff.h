/********************************************************************
created:	2015/11/23
author:		lishihai
desc:		buff����
*********************************************************************/

#ifndef __BUFF_H__
#define __BUFF_H__

#include <queue>

struct BuffInfo;

//
//enum BuffFunction
//{
//    Buff_ChenMo = 1,//>��Ĭ, ���ͷ���������
//    Buff_FengXue,   //>��Ѩ, ����������,
//    Buff_XuanYun,   //>ѣ��, ���ܹ���,�����ƶ�
//    Buff_HunShui,   //>��˯, ���ܹ���,�����ƶ�, �ܱ�����
//    Buff_HunLuan,   //>����, ����������Լ�������Ŀ��, ������ҿ���
//    Buff_Wudi,      //>�޵�, ���������˺�, �����к�buff.���ܿ���buff
//
//    Buff_DisableBuff,   //>����, ����ָ�����͵�buff
//    Buff_HuDun,         //>����,����һ���˺���ֵ
//    Buff_HuDunPct,      //>����,��������Ѫ�����ްٷֱ���ֵ
//    Buff_HuDunMp2Hp,    //>����,���ٷ�֮x��mpת����ֵ
//    Buff_DamagePctToHp, //>��Ѫ, �˺�ֵ�İٷֱȱ��hp
//    Buff_FanShang,      //>����,�����˺�ֵ�İٷֱ�
//
//    Buff_HuoQiang,      //>��ǽ
//    Buff_ZhaoHuang,     //>�ٻ�
//
//    Buff_DropHp,        //>������Ѫ(�̶�ֵ)
//    Buff_DropCurHpPct,  //>������Ѫ(��ǰѪ���ٷֱ�)
//    Buff_DropHpMaxPct,  //>������Ѫ(���Ѫ���ٷֱ�)
//
//    Buff_AddHpMax,      //>���Hp����ֵ
//    Buff_AddHpMaxPct,   //>���Hp���ްٷֱ�
//
//    Buff_AddMpMax,      //>���Mp����
//    Buff_AddMpMaxPct,   //>���Mp���ްٷֱ�
//
//    Buff_AddMoveSpeed,      //>����ƶ��ٶ�
//
//    Buff_AddAttackPower,    //>��ӹ�����
//    Buff_AddAttackPowerPct, //>��ӹ������ٷֱ�
//    Buff_AddDefencePower,   //>��ӷ�����
//    Buff_AddDefencePowerPct,//>��ӷ������ٷֱ�
//    Buff_AddCrit,           //>��ӱ�����
//    Buff_AddCritPct,        //>��ӱ����ʰٷֱ�
//    Buff_AddHit,            //>���������
//    Buff_AddHitPct,         //>��������ʰٷֱ�
//    Buff_AddTenacity,       //>�������
//    Buff_AddTenacityPct,    //>������԰ٷֱ�
//    Buff_AddDodge,          //>���������
//    Buff_AddDodgePct,       //>��������ʰٷֱ�
//
//    Buff_AddAtkFactor,      //>��ӹ���ϵ��
//    Buff_AddDefFactor,      //>��ӷ���ϵ��
//    Buff_AddCritFactor,     //>��ӱ���ϵ��
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

//>��Ĭ
class ChenMo : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>��Ѩ
class FengXue : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>ѣ��
class XuanYun : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>��˯
class HunShui : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>����
class HunLuan : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>�޵�
class WuDi : public Buff
{
public:
    virtual void OnAttach();
    virtual void OnDettach();
};

//>����
class MainYi : public Buff
{
    MainYi() { m_DisableBuffType = 0; }
public:
    virtual void SetData(float data[3]);

    inline bool IsDisableAdd(int buffType) { return m_DisableBuffType == buffType; }

    int m_DisableBuffType;
};

//>����
class HuDun : public Buff
{
public:
    int DamageReduce(int damage);

    void SetHp(int hp) { m_Hp = hp; }
    int m_Hp;
};

//>��Ѫ
class DamagePctToHp : public Buff
{
public:
    int GetHp(int damage);

    float m_HpFactor;
};

//>����
class FanShang : public Buff
{
public:
    FanShang() { m_FanShangFactor = 0.0f; }

    virtual void SetData(const float data[3]);

    int FanShangDamage(int damage);

    float m_FanShangFactor;
};


//>������Ѫ
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


//>buff������
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

//>buff����
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
