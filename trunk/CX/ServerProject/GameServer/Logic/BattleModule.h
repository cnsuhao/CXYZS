/********************************************************************
created:	2015/11/23
author:		lishihai
desc:		ս������,����ս����ս������߼�
*********************************************************************/

#ifndef __BATTLE_MODULE_H__
#define __BATTLE_MODULE_H__

#include "GameDef.h"
#include "Object.h"

class Skill; 

//>����������
struct AttackerData
{
    int64 guid;
    int attackerPower;      //>������������
    float attackerFactor;     //>����������ϵ��
    float skillFactor;        //>����������ϵ��
};

//>�ܻ�������
struct BeHurterData
{

};

class BattleModule
{
public:
    static bool CalcDodoge(Object *AttackObj, Object* TargetObj);      //>��������
    static bool CalcCritical(Object *AttackObj, Object* TargetObj);    //>���㱩��

    static DropHpMode CalcDamage(Object *AttackObj, Skill* pSkill, Object* TargetObj);    //>�����˺�
    static AddHpMode CalcTreatment(Object *AttackObj, Skill* pSkill, Object* TargetObj);          //>��������
};

#endif	//__BATTLE_MODULE_H__
