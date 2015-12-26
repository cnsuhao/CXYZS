/********************************************************************
created:	2013/12/10
author:		LL
desc:		NPC类：主角 玩家继承至本类。
*********************************************************************/

#ifndef __NPC_H__
#define __NPC_H__

#include "ClientDef.h"
#include "Object.h"
#include "AnimComponent.h"

struct NPCInfo;

//npc _动作_方向_编号_帧数.png
class NPC : public MapObj, 
            public NpcAnimComp::Interface
{
public:
	NPC();
	~NPC();

	bool Init(int objid);

	virtual void ReleaseSelf(cc2d::Node *sender, long data);	//释放自身,一般都作为回调函数自动调用
    void Update(float delta);
	void InitHP(int hp, int maxhp = -1);	//初始化血量
	

	virtual void Idle();					//待机

public:
//>动画/显示组件接口
    virtual const char* GetModelName();
    virtual void        OnAttcakAnimEnd();

	const NPCInfo* m_NPCInfo;
};

#endif	//__NPC_H__
