/********************************************************************
created:	2014/12/27
author:		LL
desc:		������
*********************************************************************/

#ifndef __Monster_H__
#define __Monster_H__

#include "ClientDef.h"
#include "Object.h"
#include "MoveComponent.h"
#include "AnimComponent.h"

struct MonsterInfo;
class Monster : public MapObj, 
                public NpcAnimComp::Interface, 
                public MoveComponent::Interface
{
public:
	Monster();
	virtual ~Monster();

	bool Init(int objid);
	void Update(float delta);

    virtual void Idle();							    //>����
    virtual void Die();						            //>����

//>����/��ʾ����ӿ�
    virtual const char* GetModelName();
    virtual void        OnAttcakAnimEnd();

//>�ƶ�����ӿ�
    virtual float   MC_GetMoveSpeed();
    virtual void    OnChangeDirection(DirectionType direction);
    virtual void    OnMoveByTarget(const cc2d::Vec2 &tagPos);
    virtual void    OnMoveByPath(std::vector<cc2d::Vec2>& paths, const cc2d::Vec2 &tagPos);
    virtual void    OnMoveEnd();

public:
	const MonsterInfo* m_MonsterInfo;		//��Ӧcsv������

};

#endif	//__Monster_H__
