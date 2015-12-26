/********************************************************************
created:	2013/12/10
author:		LL
desc:		NPC�ࣺ���� ��Ҽ̳������ࡣ
*********************************************************************/

#ifndef __NPC_H__
#define __NPC_H__

#include "ClientDef.h"
#include "Object.h"
#include "AnimComponent.h"

struct NPCInfo;

//npc _����_����_���_֡��.png
class NPC : public MapObj, 
            public NpcAnimComp::Interface
{
public:
	NPC();
	~NPC();

	bool Init(int objid);

	virtual void ReleaseSelf(cc2d::Node *sender, long data);	//�ͷ�����,һ�㶼��Ϊ�ص������Զ�����
    void Update(float delta);
	void InitHP(int hp, int maxhp = -1);	//��ʼ��Ѫ��
	

	virtual void Idle();					//����

public:
//>����/��ʾ����ӿ�
    virtual const char* GetModelName();
    virtual void        OnAttcakAnimEnd();

	const NPCInfo* m_NPCInfo;
};

#endif	//__NPC_H__
