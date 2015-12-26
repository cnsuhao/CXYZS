#include "BattleManager.h"
#include "MapManager.h"
#include "MsgInfo.h"
#include "CSVFileManager.h"
#include "Player.h"
#include "Monster.h"
#include "Vec2.h"
#include "Map.h"
#include "GameServer.h"
#include "LogManage.h"
#include "Team.h"
#include "SocialManager.h"
#include "TimerManage.h"




// class BattleManagerCallback: public Callback<BattleManager>
// {
// public:
// 	BattleManagerCallback(BattleManager* obj, func_void f, Object* attackObj,Object* targetObj,int buffId,int SkillId):Callback(obj, f)
// 	{
// 
// 		m_data.attackObj = attackObj;
// 		m_data.targetObj = targetObj;
// 		m_data.buffId = buffId;
// 		m_data.SkillId = SkillId;
// 	}
// 
// 	struct Data
// 	{
// 		Object* attackObj;
// 		Object* targetObj;
// 		int buffId;
// 		int SkillId;
// 	};
// 
// 	Data* GetData()
// 	{
// 		return &m_data;
// 	}
// 
// private:
// 	Data m_data;
// };


BattleManager *g_BattleManager = nullptr;


BattleManager::BattleManager()
{		
}

BattleManager::~BattleManager()
{
}

void BattleManager::Init()
{
}

void BattleManager::PlayerUseSkill(MsgC2GPlayer_UseSkill *battle)
{


}

void BattleManager::UseSkill(Object *AttackObj, int SkillID, Object* TargetObj)
{
	
}

//计算物理攻击伤害
int32 BattleManager::onBattleHurt(Object* attackObj,Object* TargetObj,int SkillID,AttackType attackType)
{ 
  return 0;
}

//获取玩家的攻击状态
AttackType BattleManager::getAttackType(Object* attackObj,Object* TargetObj)
{	
	return AttackType_BUFF;
}

// void BattleManager::OnBuffHurt(void* pData_)
// {
// 
// 	BattleManagerCallback::Data* pData = (BattleManagerCallback::Data*)pData_;
// 	assert(pData);
// 
// 
// 	bool isFind = false;
// 	std::vector<BuffInfoIt>::iterator itBuff; 
// 	for(itBuff = pData->targetObj->m_BuffList.begin();itBuff != pData->targetObj->m_BuffList.end();itBuff++)
// 	{
// 		if(itBuff->id == pData->buffId)
// 		{
// 			printf("Obj:[%llu],Find Buff:[%d] SuccessFull!\n",pData->targetObj->m_GUID,pData->buffId);
// 			isFind = true;
// 		}
// 	}
// 	if(!isFind)
// 	{
// 		printf("Obj:[%llu],Find Buff:[%d] Error!\n",pData->targetObj->m_GUID,pData->buffId);
// 		return;
// 	}
// 		
// 	float hp =0.0f;
// 	const BuffInfo* buffInfo = g_CSVFileManager->GetBuffByID(pData->buffId);
// 	if(buffInfo)
// 	{
// 		if(buffInfo->iEffectType1 == ATTRIBUTE_HP)
// 		{
// 			 pData->targetObj->DropHP( pData->targetObj,-buffInfo->fEffectValue1);
// 			 hp += -buffInfo->fEffectValue1;
// 		}
// 		if(buffInfo->iEffectType2 == ATTRIBUTE_HP )
// 		{
// 			 pData->targetObj->DropHP( pData->targetObj,-buffInfo->fEffectValue2);
// 			 hp += -buffInfo->fEffectValue2;
// 		}
// 	}
// 	else
// 	{
// 		return;
// 	}
// 	 MsgG2CPlayer_BuffHurt send;
// 	 send.SkillID = pData->SkillId;
// 	 send.BuffId = pData->buffId;
// 	 send.AttactGuid = pData->attackObj->m_GUID;
// 	 send.TargetGuid = pData->targetObj->m_GUID;
// 	 send.hp = hp;
// 	 if(pData->targetObj->type == OBJECT_PLAYER)
// 	 {
// 		 send.OBJID = pData->targetObj->m_GUID;
// 		 g_GameServer->SendToClient(&send);
// 	 }	 
// 	 send.OBJID = pData->attackObj->m_GUID;
// 	 g_GameServer->SendToClient(&send);
// 
// 	 BattleManagerCallback* pCallback = new BattleManagerCallback(this, &BattleManager::OnBuffHurt,pData->attackObj,pData->targetObj,pData->buffId,pData->SkillId);
// 	 g_TimerManage->AddTimer(0,1000, pCallback, 1, pCallback->GetData());
// }



