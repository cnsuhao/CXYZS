/********************************************************************
created:	2014/12/27
author:		LL
desc:		怪物类
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

    virtual void Idle();							    //>待机
    virtual void Die();						            //>死亡

//>动画/显示组件接口
    virtual const char* GetModelName();
    virtual void        OnAttcakAnimEnd();

//>移动组件接口
    virtual float   MC_GetMoveSpeed();
    virtual void    OnChangeDirection(DirectionType direction);
    virtual void    OnMoveByTarget(const cc2d::Vec2 &tagPos);
    virtual void    OnMoveByPath(std::vector<cc2d::Vec2>& paths, const cc2d::Vec2 &tagPos);
    virtual void    OnMoveEnd();

public:
	const MonsterInfo* m_MonsterInfo;		//对应csv表数据

};

#endif	//__Monster_H__
