/********************************************************************
created:	2015/11/23
author:		lishihai
desc:		战斗管理,处理战斗及战斗相关逻辑
*********************************************************************/

#ifndef __BATTLE_MODULE_H__
#define __BATTLE_MODULE_H__

#include "GameDef.h"
#include "Object.h"

class Skill; 

//>攻击者数据
struct AttackerData
{
    int64 guid;
    int attackerPower;      //>进攻方攻击力
    float attackerFactor;     //>进攻方攻击系数
    float skillFactor;        //>进攻方技能系数
};

//>受击者数据
struct BeHurterData
{

};

class BattleModule
{
public:
    static bool CalcDodoge(Object *AttackObj, Object* TargetObj);      //>计算闪避
    static bool CalcCritical(Object *AttackObj, Object* TargetObj);    //>计算暴击

    static DropHpMode CalcDamage(Object *AttackObj, Skill* pSkill, Object* TargetObj);    //>计算伤害
    static AddHpMode CalcTreatment(Object *AttackObj, Skill* pSkill, Object* TargetObj);          //>计算治疗
};

#endif	//__BATTLE_MODULE_H__
