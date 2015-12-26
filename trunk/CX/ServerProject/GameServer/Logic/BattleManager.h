
/********************************************************************
created:	2015/7/27
author:		LL
desc:		ս������,����ս����ս������߼�
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

	void Init();				//��ʼ��
	void PlayerUseSkill(MsgC2GPlayer_UseSkill *battle);			//���ʹ�ü��ܣ��ͻ�������

	void UseSkill(Object *AttackObj, int SkillID, Object* TargetObj);	//����ʹ�ü��ܣ�����������
private:
	AttackType getAttackType(Object* attackObj,Object* TargetObj);
	int32 onBattleHurt(Object* attackObj,Object* TargetObj,int SkillID,AttackType attackType);
	//void OnBuffHurt(void* pData);
	//void OnBuffHurt(int buffId);
};
extern BattleManager* g_BattleManager;
#endif	//__BATTLEMANAGER_H__
