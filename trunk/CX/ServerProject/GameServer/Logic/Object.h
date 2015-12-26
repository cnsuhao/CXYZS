/********************************************************************
created:	2015/11/26
author:		lishihai
desc:		��ͼ�ϵĵ�λ
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

//λ�����
    void InitPosition(const Vec2& position) { m_Position = position; }
	const Vec2& GetPosition(){ return m_Position; }
    const Vec2& GetTile();
    virtual void OnChangeBlock(int oldBlock, int newBlock) {}
    virtual void OnChangeTile(const Vec2& oldTile, const Vec2& newTile) {}

//�������
    virtual bool AddHp(const AddHpMode& mode);          //>��Ѫ
	virtual bool DropHP(const DropHpMode& mode);		//>��Ѫ
    virtual void Idle();                                //>����
	virtual void Die();					                //>����
    inline bool  IsDead();                              //>�Ƿ�����


	virtual void BroadcastMsg(struct MsgBase* msg, bool isBroadcastSelf = true);		//�㲥��Ϣ

    // ����ʱ��ص�
    void SetTimer(unsigned int time, CallbackBase* callback )
    {
        g_TimerManage->AddTimer(1, time, callback);
    }

	uint64      m_GUID;		            //Ψһid
	Map*        m_Map;		            //���ڵ�ͼ
	ObjectType  m_Type;                 //��������
    bool        m_IsDead;
    Attribute   m_Attribute[ATTRIBUTE_MAX];	//���Լ�

    class BattleComponent*  m_BattleComp;
    class MoveComponent*     m_MoveComp;
    class AIComponent*       m_AIComponet;
private:
    Vec2        m_Position;	//����λ��
};


inline bool Object::IsDead()
{
    return m_IsDead;
}



#endif	//__OBJECT_H__
