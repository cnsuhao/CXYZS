#include "LogicManager.h"
#include <string>
#include "SceneManager.h"
#include "CSVFileManager.h"
#include "UIManager.h"
#include "ResManager.h"
#include "Npc.h"
#include "Hero.h"
#include "Monster.h"
#include "AStar.h"
#include "BaseFunction.h"
#include "UIMain.h"
#include "UIMiniMap.h"
#include "Collect.h"

LogicManager *g_LogicManager = NULL;

LogicManager::LogicManager():m_Hero(NULL)
{		
}

LogicManager::~LogicManager()
{
}
 
void LogicManager::Init()
{
	m_CurrMapId = 1001;
	m_vecGangList.clear();
}

void LogicManager::Release()
{	
}

void LogicManager::Update(float delta)
{
	std::map<int, float>::iterator cdit = m_SkillCDList.begin();	
	for (; cdit !=m_SkillCDList.end(); ++cdit)
	{	
		if (cdit->second > 0)
		{
			cdit->second -= delta;
			if (cdit->second < 0)
			{
				cdit->second = 0.0f;
			}
		}
	}
}

//NPC* LogicManager::GetAttackTarget()
//{
//	//!npc还是玩家得看逻辑
//	//MapInfo *map = g_CSVFileManager->GetMapByID(m_CurrMapId);
//	////安全模式
//	//if (map->iMapPKModel == 1)
//	//{
//	//}
//	//if (m_MonsterList.size() < 1)
//	//{
//	//	return NULL;
//	//}
//	//vector<Monster*>::iterator it = m_MonsterList.begin();
//	//Monster* Target = *it;	
//	//float Distance = m_Hero->getPosition().distance(Target->getPosition());
//	//float tempDistance = 0.0;
//	//for (; it !=m_MonsterList.end(); ++it)
//	//{
//	//	tempDistance = m_Hero->getPosition().distance((*it)->getPosition());
//	//	if(tempDistance / g_SceneManager->m_MainLayer->m_TileMap->getTileSize().width < FINDTARGETMAXDISTANCE && Distance >= tempDistance)
//	//	{
//	//		Distance = tempDistance;
//	//		Target = *it;
//	//	}
//	//}
//	//return Target;
//}

void LogicManager::PlayerUseSkill(MsgG2CPlayer_UseSkill* msg)
{
	MapObj *attobj = GetObjectByType(msg->attacker, msg->attackerType);
	if (!attobj)
	{
		return;
	}

    if (attobj->m_BattleCompoent)
    {
        if(attobj->m_MoveCompoenet)
        {
            attobj->m_MoveCompoenet->MoveStop();
        }

        MapObj *target = GetObjectByType(msg->target, msg->targetType);
        attobj->m_BattleCompoent->DoSkill(msg->SkillID, target);
    }

	//if (attobj == m_Hero)
	//{
	//	//技能cd
	//	g_LogicManager->HeroUseSkillSetCD(msg->SkillID);
	//}
	//NPC *beatenobj = NULL;	
	//HurtData* HurtDataList = &msg->HurtDataList;
	//beatenobj = GetObjectByType(HurtDataList->guid, HurtDataList->ObjType);
	//attobj->Attack(msg->SkillID, beatenobj);

	//for (int i = 0; i < msg->GoalNum; i++)
	//{
	//	beatenobj = GetObjectByType(HurtDataList->guid, HurtDataList->ObjType);
	//	if (beatenobj)
	//	{
	//		//群体特效
	//		if (i == 0)
	//		{
	//			const SkillInfo *skill = g_CSVFileManager->GetSkillByID(msg->SkillID);
	//			//if (skill && skill->iGroupEffect)
	//			//{
	//			//	g_ResManager->ShowSprite(skill->iGroupEffect, g_SceneManager->m_MainLayer->m_TileMap, g_SceneManager->m_MainLayer->m_TileMap->getMapSize().height+LAYER_EFFECT, beatenobj->getPosition(), skill->fHitDelay);
	//			//}
	//		}
	//		//受击
	//		beatenobj->Beaten(msg->SkillID, HurtDataList->hp, HurtDataList->hurtType);
	//	}
	//	HurtDataList++;
	//}	
}

void LogicManager::EnterGame()
{	
	//切换到主场景
	g_SceneManager->Login2Main();  
}

void LogicManager::EnterCopy(int copyid)
{	
}

void LogicManager::AddNPCToMap(NPCInfo* mapnpc)
{
    if (!mapnpc)
    {
        return;
    }	

    NPC* npc = new NPC();
    if(npc->Init(mapnpc->id))
    {
        ////可接的任务
        //map<int32,TaskInfo>::iterator itt;
        //for(itt = g_CSVFileManager->m_TaskInfoMap.begin(); itt != g_CSVFileManager->m_TaskInfoMap.end() ; itt++)
        //{
        //	TaskInfoIt *taskinfoitTemp = g_LogicManager->m_Hero->GetTaskInfoItByID(itt->first);
        //	//可领取
        //	if (itt->second.iTaskNpcID == npc->m_NPCInfo->id)
        //	{
        //		//检测等级
        //		if (g_LogicManager->m_Hero->m_Level >= itt->second.iLevel)
        //		{
        //			//检测境界
        //			if (g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_REALMLEVEL] >= itt->second.iRealmID)
        //			{
        //				//检测前置任务是否有前置任务
        //				if (itt->second.iReqTaskID == 0)
        //				{
        //					if (!taskinfoitTemp)
        //					{
        //						npc->SetTaskState(1);
        //					}
        //					else
        //					{
        //						if (taskinfoitTemp->state == TASK_STATE_NOTASK)
        //						{
        //							npc->SetTaskState(1);
        //						}
        //					}
        //				}
        //				else
        //				{
        //					//如果有前置任务，看是否已经完成
        //					TaskInfoIt *protaskinfoIt = g_LogicManager->m_Hero->GetTaskInfoItByID(itt->second.iReqTaskID);
        //					if (protaskinfoIt)
        //					{
        //						if (protaskinfoIt->state == TASK_STATE_FINISHTASK)
        //						{
        //							if (!taskinfoitTemp)
        //							{
        //								npc->SetTaskState(1);
        //							}
        //							else
        //							{
        //								if (taskinfoitTemp->state == TASK_STATE_NOTASK)
        //								{
        //									npc->SetTaskState(1);
        //								}
        //							}
        //						}
        //					}

        //				}
        //			}
        //		}
        //	}
        //	else if (itt->second.iReferNpcID == npc->m_NPCInfo->id)//可交接
        //	{
        //		if (taskinfoitTemp)
        //		{
        //			if (taskinfoitTemp->state == TASK_STATE_SUBMITTASK)
        //			{
        //				npc->SetTaskState(2);
        //			}
        //		}
        //	}
        //}

        Vec2 pos;
        pos.x = mapnpc->pPos.x;
        pos.y = mapnpc->pPos.y;
        m_NPCList.push_back(npc);
        npc->SetTile(pos);
        g_SceneManager->m_MainLayer->m_TileMap->addChild(npc);

        auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
        if (uiMain && uiMain->m_pMiniMap)
        {
            uiMain->m_pMiniMap->AddNode(npc, 2);
        }
    }
}

void LogicManager::AddMonsterToMap(MsgMonster_Baseinfo* monsterInfo)
{
    const MonsterInfo *pInfo = g_CSVFileManager->GetMonsterByID(monsterInfo->MonsterId);
    if (pInfo)
    {
        Monster* monster = new Monster();
        monster->Init(monsterInfo->MonsterId);
        monster->m_GUID = monsterInfo->MonsterGuid;
        monster->m_MonsterInfo = pInfo;
        Vec2 serverPos(monsterInfo->posX, monsterInfo->posY);
        monster->SetMapPosition(serverPos);
        g_ResManager->ShowSprite(6, g_SceneManager->m_MainLayer->m_TileMap, g_SceneManager->m_MainLayer->m_TileMap->getMapSize().height+LAYER_EFFECT, serverPos);
        m_MonsterList.push_back(monster);
        g_SceneManager->m_MainLayer->m_TileMap->addChild(monster);

        auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
        if (uiMain && uiMain->m_pMiniMap)
        {
            uiMain->m_pMiniMap->AddNode(monster, 1);
        }
    }
}

void LogicManager::AddOtherPlayerToMap(MsgOtherRole_BaseInfo* playerInfo)
{
    Player* player = NULL;
    size_t count = m_PlayerList.size();
    for (size_t i = 0; i < count; ++i)
    {
        if (m_PlayerList[i]->m_GUID == playerInfo->guid)
        {
            player = m_PlayerList[i];
            break;
        }
    }

    if (player == NULL)
    {
        player = new Player();
        player->Init(playerInfo->guid, playerInfo->sex, playerInfo->playerName, NULL, playerInfo->hp <= 0);
        player->SetMoveSpeed(playerInfo->moveSpeed);
        //>添加到列表
        m_PlayerList.push_back(player);
        g_SceneManager->m_MainLayer->m_TileMap->addChild(player);
    }

    int faceId[EQUIPTYPE_MAX];
    player->EquipIdToFaceId(playerInfo->equip, faceId); //>直接转换成外观
    player->ReloadFace(faceId);  //>重新加载外观
    player->m_Level = playerInfo->level;
    player->m_HeadIconIndex = playerInfo->headIconIndex;
    player->SetHp(playerInfo->hp);
    player->SetHpMax(playerInfo->maxHp);

    player->SetMapPosition(Vec2(playerInfo->posX, playerInfo->posY));
    
    auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
    if (uiMain && uiMain->m_pMiniMap)
    {
    	uiMain->m_pMiniMap->AddNode(player, 1);
    }
}

void LogicManager::AddCollectToMap(ResInfo* parRes)
{
	//if (!parRes)
	//{
	//	return;
	//}

	//Collect *TempCollect = NULL;
	//size_t count = m_Collect.size();
	//for (size_t i = 0; i < count ; ++i )
	//{
	//	if (m_Collect[i]->m_GUID == parRes->id)
	//	{
	//		TempCollect = m_Collect[i];
	//		break;
	//	}
	//}

	//if (TempCollect == NULL)
	//{
	//	Vec2 pos;
	//	pos.x = parRes->pPos.x;
	//	pos.y = parRes->pPos.y;
	//	TempCollect = new Collect();
	//	TempCollect->Init(parRes->id);
	//	m_Collect.push_back(TempCollect);
 //       TempCollect->SetTilePos(pos);
	//	g_SceneManager->m_MainLayer->m_TileMap->addChild(TempCollect);
	//}
}

//
//void LogicManager::AddPlayerToMap(Player_Asyn_Move* playerdata)
//{
//	if (!playerdata)
//	{
//		return;
//	}
//	Player* player = new Player();
//	player->m_GUID = playerdata->id_player;
//	//!性别
//	player->m_Sex = 1;
//	player->m_Name = String::createWithFormat("%d", player->m_GUID)->getCString();
//	player->CreatePlayer();
//	player->Init();
//	
//	
//	Vec2 pos = g_SceneManager->m_MainLayer->GetTileByPosID(playerdata->id_pos);		
//			
//	m_PlayerList.push_back(player);
//	g_SceneManager->m_MainLayer->m_TileMap->addChild(player);
//	player->SetTilePos(pos);
//
//	auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
//	if (uiMain && uiMain->miniMap)
//	{
//		uiMain->miniMap->AddNode(player);
//	}
//}

void LogicManager::RemoveCollect(uint64 id)
{
	//vector<Collect*>::iterator it = m_Collect.begin();
	//for (; it != m_Collect.end(); ++ it)
	//{
	//	if ((*it)->m_GUID == id)
	//	{
	//		auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
	//		if (uiMain && uiMain->m_pMiniMap)
	//		{
	//			uiMain->m_pMiniMap->RemoveNode((*it));
	//		}
	//		m_Collect.erase(it);

	//		return;
	//	}
	//}
}

void LogicManager::RemoveNpc(uint64 id)
{
    //vector<NPC*>::iterator it = m_NPCList.begin();	
    //for (; it != m_NPCList.end(); ++it)
    //{
    //    if ((*it)->m_GUID == id)
    //    {
    //        if ((*it) == m_Hero->m_SelTarget)
    //        {
    //            m_Hero->m_SelTarget = NULL;
    //            ((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
    //        }

    //        auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
    //        if (uiMain && uiMain->m_pMiniMap)
    //        {
    //            uiMain->m_pMiniMap->RemoveNode((*it));
    //        }
    //        m_NPCList.erase(it);
    //        return;
    //    }
    //}
}

void LogicManager::RemoveMonster(Monster* obj)
{
	//if (obj == m_Hero->m_SelTarget)
	//{
	//	m_Hero->m_SelTarget = NULL;
	//	((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
	//}

	vector<Monster*>::iterator it = m_MonsterList.begin();	
	for (; it != m_MonsterList.end(); ++it)
	{
		if (*it == obj)
		{
			//MapInfo *map = g_CSVFileManager->GetMapByID(m_CurrMapId);
			//if (map && map->bClientRefresh)
			//{
			//	MapMonsterRefresh2 temp;
			//	temp.mapmonsterrefresh = obj->m_MonsterInfo;
			//	temp.BirthPos = obj->m_MonsterBirthPos;				
			//	temp.time = obj->m_MonsterInfo->iDealRefreshTime;
			//	m_ReviveWaitList.push_back(temp);				
			//}
            g_SceneManager->m_MainLayer->m_TileMap->removeChild(obj);
			m_MonsterList.erase(it);

			auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			if (uiMain && uiMain->m_pMiniMap)
			{
				uiMain->m_pMiniMap->RemoveNode(obj);
			}

			return;
		}
	}
}

void LogicManager::RemovePlayer(uint64 id)
{
	vector<Player*>::iterator it = m_PlayerList.begin();	
	for (; it != m_PlayerList.end(); ++it)
	{
		if ((*it)->m_GUID == id)
		{
			if ((*it) == m_Hero->m_SelTarget)
			{
				m_Hero->m_SelTarget = NULL;
				((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
			}

			auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			if (uiMain && uiMain->m_pMiniMap)
			{
				uiMain->m_pMiniMap->RemoveNode((*it));
			}

            (*it)->removeFromParent();
			m_PlayerList.erase(it);
			return;
		}
	}
}

Player* LogicManager::GetPlayerByGUID(uint64 guid)
{
	vector<Player*>::iterator it = m_PlayerList.begin();	
	for (; it != m_PlayerList.end(); ++it)
	{
		if ((*it)->m_GUID == guid)
		{
			return *it;
		}
	}
	return NULL;
}

NPC* LogicManager::GetNPCByGUID(uint64 guid)
{
    vector<NPC*>::iterator it = m_NPCList.begin();	
    for (; it != m_NPCList.end(); ++it)
    {
        if ((*it)->m_GUID == guid)
        {
            return *it;
        }
    }
    return NULL;
}

NPC* LogicManager::GetNPCByID(uint64 id)
{
    vector<NPC*>::iterator it = m_NPCList.begin();	
    for (; it != m_NPCList.end(); ++it)
    {
        if ((*it)->m_NPCInfo->id == id)
        {
            return *it;
        }
    }
    return NULL;
}


Monster* LogicManager::GetMonsterByGUID(uint64 guid)
{
    vector<Monster*>::iterator it = m_MonsterList.begin();	
    for (; it != m_MonsterList.end(); ++it)
    {
        if ((*it)->m_GUID == guid)
        {
            return *it;
        }
    }
    return NULL;
}

MapObj* LogicManager::GetObjectByType(uint64 guid, ObjectType type)
{
    switch (type)
    {
    case OBJECT_NPC:
    	return GetNPCByGUID(guid);
    	break;
    case OBJECT_PLAYER:
    	{
    		if (guid == m_Hero->m_GUID)
    		{
    			return m_Hero;
    		}
    		else
    		{
    			return GetPlayerByGUID(guid);
    		}
    	}
    	break;
    case OBJECT_MONSTER:
    case OBJECT_RESOURCE:
    	{
    		return GetMonsterByGUID(guid);
    	}
    	break;
    default:
    	break;
    }
    return NULL;
}

bool LogicManager::GetPlayerIsTeam(uint64 guid)
{
    if (g_LogicManager->m_Hero)
    {
        return g_LogicManager->m_Hero->IsTeamMember(guid);
    }
	return false;
}

bool LogicManager::GetPlayerIsCamp(uint64 guid)
{
	return false;
}

void LogicManager::HeroUseSkillSetCD(int skillid)
{
	const SkillInfo* skill = g_CSVFileManager->GetSkillByID(skillid);
	if (skill)
	{
		m_SkillCDList[0] = g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_SKILL_PUBLIC_CD];
		m_SkillCDList[skillid/1000] = skill->CD;
	}
}