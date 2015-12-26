
/********************************************************************
created:	2015/7/27
author:		LL
desc:		战斗管理,处理战斗及战斗相关逻辑
*********************************************************************/

#ifndef __BATTLEMANAGER_H__
#define __BATTLEMANAGER_H__
#include "GameDef.h"
struct MsgC2GPlayer_UseSkill;
class Object;
class Player;
class BattleManagerCallback;
class BattleManager
{
public:
	BattleManager();
	~BattleManager();

	void Init();				//初始化
	void PlayerUseSkill(MsgC2GPlayer_UseSkill *battle);			//玩家使用技能，客户端驱动

	void UseSkill(Object *AttackObj, int SkillID, Object* TargetObj);	//怪物使用技能，服务器驱动
private:
	AttackType getAttackType(Object* attackObj,Object* TargetObj);
	int32 onBattleHurt(Object* attackObj,Object* TargetObj,int SkillID,AttackType attackType);
	//void OnBuffHurt(void* pData);
	//void OnBuffHurt(int buffId);
};
extern BattleManager* g_BattleManager;
#endif	//__BATTLEMANAGER_H__
