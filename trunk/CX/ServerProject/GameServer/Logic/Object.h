/********************************************************************
created:	2015/11/26
author:		lishihai
desc:		地图上的单位
*********************************************************************/

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Vec2.h"
#include "TimerManage.h"

struct Attribute
{
public:
    Attribute() { Clear(0.0f); }

    void SetBase(float attributeValue)
    {
        base = attributeValue;
    }

    float GetBase()
    {
        return base;
    }

    void AddBase(float value)
    {
        base += value;
    }
    
    void AddValue(float value)
    {
        addValue += value;
    }

    void AddBuffValue(float value)
    {
        addValueBuff += value;
    }

    void AddPercent(float percent)
    {
        addPercent += percent;
    }
    
    void AddBuffPercent(float percent)
    {
        addPercentBuff += percent;
    }

    float GetValueF()
    {
        return  (base + addValue + addValueBuff) * (1.0f + addPercent + addPercentBuff);
    }

    int GetValueI()
    {
        return (int)((base + addValue + addValueBuff) * (1.0f + addPercent + addPercentBuff));
    }

    float GetValueWithoutBuffF()
    {
        return  (base + addValue) * (1.0f + addPercent);
    }

    int GetValueWithoutBuffI()
    {
        return (int)((base + addValue) * (1.0f + addPercent));
    }

    void Clear(float baseValue)
    {
        base            = baseValue;
        addValue        = 0.0f;
        addValueBuff    = 0.0f;
        addPercent      = 0.0f;
        addPercentBuff  = 0.0f;
    }
private:
    float base;
    float addValue;
    float addValueBuff;
    float addPercent;
    float addPercentBuff;
};


class Map;
class Skill;
class Buff;
class Object
{
    friend class Map;
public:
	Object();
	virtual ~Object();

    virtual void Update(float dt);

//位置相关
    void InitPosition(const Vec2& position) { m_Position = position; }
	const Vec2& GetPosition(){ return m_Position; }
    const Vec2& GetTile();
    virtual void OnChangeBlock(int oldBlock, int newBlock) {}
    virtual void OnChangeTile(const Vec2& oldTile, const Vec2& newTile) {}

//属性相关
    virtual bool AddHp(const AddHpMode& mode);          //>加血
	virtual bool DropHP(const DropHpMode& mode);		//>掉血
    virtual void Idle();                                //>待机
	virtual void Die();					                //>死亡
    inline bool  IsDead();                              //>是否死亡


	virtual void BroadcastMsg(struct MsgBase* msg, bool isBroadcastSelf = true);		//广播消息

    // 设置时间回调
    void SetTimer(unsigned int time, CallbackBase* callback )
    {
        g_TimerManage->AddTimer(1, time, callback);
    }

	uint64      m_GUID;		            //唯一id
	Map*        m_Map;		            //所在地图
	ObjectType  m_Type;                 //对象类型
    bool        m_IsDead;
    Attribute   m_Attribute[ATTRIBUTE_MAX];	//属性集

    class BattleComponent*  m_BattleComp;
    class MoveComponent*     m_MoveComp;
    class AIComponent*       m_AIComponet;
private:
    Vec2        m_Position;	//所在位置
};


inline bool Object::IsDead()
{
    return m_IsDead;
}



#endif	//__OBJECT_H__
