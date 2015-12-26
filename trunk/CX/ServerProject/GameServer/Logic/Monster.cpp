#include "Monster.h"
#include "CSVFileManager.h"
#include "MapManager.h"
#include "Map.h"
#include "TaskManager.h"
#include "GameServer.h"
#include "BattleManager.h"
#include "LogManage.h"
#include <time.h>
#include "CopyManager.h"
#include "StackStream.h"

Monster::Monster():m_MonsterInfo(NULL),m_AttTargteGuid(0)
{
    m_Type = OBJECT_MONSTER;
    m_IsMoveing = false;
}

Monster::~Monster()
{
}

bool Monster::Init(MonsterInfo *info)
{	
    
	m_GUID = g_MapManager->NewMonsterGuid();
	m_MonsterInfo = info;
	m_HP = m_HPMax = info->GetAttByType(ATTRIBUTE_HPMAX);
	return true;      
}

void Monster::Attack(int64 guid)
{
	//if (guid <= 0)
	//{
	//	Vec2 newPosition =  m_map->Tile2Pos(m_MonsterBirthPos);
	//	m_map->ObjectChangePosition(this, newPosition);
	//	//返回出生点并通知客户端
	//	MsgMonster_Goto send;
	//	send.MonsterGuid = m_GUID;
	//	send.posX = GetPosition().x;
	//	send.posY = GetPosition().y;
	//	BroadcastMsg(&send, false);
	//	return;
	//}
	//m_AttTargteGuid = guid;
	//Player* player = g_MapManager->GetPlayerByID(guid);
	//if (!player || player->m_Attribute[ATTRIBUTE_HP].GetValueI() <= 0)
	//{
	//	Vec2 newPosition =  m_map->Tile2Pos(m_MonsterBirthPos);
	//	m_map->ObjectChangePosition(this, newPosition);
	//	//返回出生点并通知客户端
	//	MsgMonster_Goto send;
	//	send.MonsterGuid = m_GUID;
	//	send.posX = GetPosition().x;
	//	send.posY = GetPosition().y;
	//	BroadcastMsg(&send, false);

	//	m_AttTargteGuid = 0;
	//	return;
	//}

	////离出生点太远了
 //   Vec2 MyTile = GetTile();
	//if (TileDistance(MyTile, m_MonsterBirthPos) >= m_MonsterInfo->iTrackingArea)
	//{
	//	m_AttTargteGuid = 0;

 //       Vec2 newPosition =  m_map->Tile2Pos(m_MonsterBirthPos);
 //       m_map->ObjectChangePosition(this, newPosition);

	//	//返回出生点并通知客户端
 //       MsgMonster_Goto send;
 //       send.MonsterGuid = m_GUID;
 //       send.posX = GetPosition().x;
 //       send.posY = GetPosition().y;
 //       BroadcastMsg(&send, false);
	//	return;
	//}
	////!技能选择
	//const SkillInfo *skill = g_CSVFileManager->GetSkillByID(m_MonsterInfo->iSkill[0]);
	//if (!skill)
	//{
	//	return;
	//}
	////判断攻击距离
 //   Vec2 TargetTile = player->GetTile();
	//if (TileDistance(TargetTile, MyTile) >= skill->iUserDistance)
	//{
 //       
	//	//直线还是斜线移动
	//	float time = STRAIGHT_MOVE_SPEED;
	//	if (m_TargteTile.x == MyTile.x || m_TargteTile.y == MyTile.y)
	//	{
	//		//time = STRAIGHT_MOVE_SPEED;
 //           time = 64.0 / (UNIT_MOVE_SPEED * m_Speed);
	//	}
	//	else
	//	{
	//		//time = SLASH_MOVE_SPEED;
 //           time = (64.0 * 1.41421356f) / (UNIT_MOVE_SPEED * m_Speed);
	//	}

	//	//移动目标点
	//	if(TargetTile.x > MyTile.x)
	//	{
	//		m_TargteTile.x = MyTile.x + 1;
	//	}
	//	else if(TargetTile.x < MyTile.x)
	//	{
	//		m_TargteTile.x = MyTile.x - 1;
	//	}
	//	else
	//	{
	//		m_TargteTile.x = MyTile.x;
	//	}
	//	
	//	if(TargetTile.y > MyTile.y)
	//	{
	//		m_TargteTile.y = MyTile.y + 1;
	//	}
	//	else if(TargetTile.y < MyTile.y)
	//	{
	//		m_TargteTile.y = MyTile.y - 1;
	//	}
	//	else
	//	{
	//		m_TargteTile.y = MyTile.y;
	//	}

 //       bool appendPath = false;
 //       Vec2 NextTargetTile;
 //       if (TileDistance(TargetTile, m_TargteTile) >= skill->iUserDistance)
 //       {
 //           appendPath = true;
 //           if(TargetTile.x > m_TargteTile.x)
 //               NextTargetTile.x = m_TargteTile.x + 1;
 //           else if(TargetTile.x < m_TargteTile.x)
 //               NextTargetTile.x = m_TargteTile.x - 1;
 //           else
 //               NextTargetTile.x = m_TargteTile.x;

 //           if(TargetTile.y > m_TargteTile.y)
 //               NextTargetTile.y = m_TargteTile.y + 1;
 //           else if(TargetTile.y < m_TargteTile.y)
 //               NextTargetTile.y = m_TargteTile.y - 1;
 //           else
 //               NextTargetTile.y = m_TargteTile.y;
 //       }

	//	
	//	//移动
	//	SetTimer(time * 1000, new Callback<Monster>(this, &Monster::MoveCallback));
	//	
	//	//广播到客户端
	//	MsgMonster_Move send;
	//	send.MonsterGuid = m_GUID;
	//	send.PathNum = 1;
	//	send.MovePath = Tile2TileId(m_TargteTile);
 //       if (appendPath)
 //       {
 //           StackStream<> moveStream;
 //           send.PathNum = 2;
 //           moveStream.Wirte(&send, sizeof(send));
 //           int tileId = Tile2TileId(NextTargetTile);
 //           moveStream.Wirte(&tileId, sizeof(tileId));
 //           MsgMonster_Move* pMoveMsg = (MsgMonster_Move*)moveStream.Get();
 //           pMoveMsg->Len = moveStream.GetSize();
 //           BroadcastMsg(pMoveMsg, false);
 //       }
 //       else
 //       {
 //           BroadcastMsg(&send, false);
 //       }
	//	return;
	//}
	//
	////攻击
	//if (!m_IsSkillCD)
	//{
	//	g_BattleManager->UseSkill(this, skill->id, player);
	//	if (player->GetState(State_Die))
	//	{
	//		Vec2 newPosition =  m_map->Tile2Pos(m_MonsterBirthPos);
	//		m_map->ObjectChangePosition(this, newPosition);
	//		//返回出生点并通知客户端
	//		MsgMonster_Goto send;
	//		send.MonsterGuid = m_GUID;
	//		send.posX = GetPosition().x;
	//		send.posY = GetPosition().y;
	//		BroadcastMsg(&send, false);
	//		m_AttTargteGuid = 0;
	//		return;
	//	}
	//	m_IsSkillCD = true;
	//	SetTimer(2000, new Callback<Monster>(this, &Monster::AttackCallback));
	//}
	//else
	//{
	//	SetTimer(750, new Callback<Monster>(this, &Monster::AttackCallback));
	//}

	
}

void Monster::MoveCallback(int type)
{
 //   m_map->ObjectChangePosition(this, m_map->Tile2Pos(m_TargteTile));
	//m_IsSkillCD = false;
	//Attack(m_AttTargteGuid);
}

void Monster::AttackCallback(int type)
{
	//m_IsSkillCD = false;
	//Attack(m_AttTargteGuid);
}


void Monster::OnEnterMap()
{

}

void Monster::OnLeaveMap()
{

}

bool Monster::AddHp(const AddHpMode& mode)
{
    bool br = Object::AddHp(mode);
    if (br || mode.mode == DropHpMode::WuDiMianYiBuff || mode.mode == DropHpMode::Dodge || DropHpMode::HuDunXiShouBuff)
    {
        if (m_MonsterInfo->iType == MONSTER_BOSS)
        {
            MsgObject_AddHp addHpMsg;
            addHpMsg.OwnerID = m_GUID;
            addHpMsg.OwnerType = OBJECT_MONSTER;
            addHpMsg.LastHp = (int32)m_Attribute[ATTRIBUTE_HP].GetBase();
            addHpMsg.Mode = mode;
            BroadcastMsg(&addHpMsg);
        }
        else
        {
            if (!m_SelectedMePlayers.empty())
            {
                MsgObject_AddHp addHpMsg;
                addHpMsg.OwnerID = m_GUID;
                addHpMsg.OwnerType = OBJECT_MONSTER;
                addHpMsg.LastHp = (int32)m_Attribute[ATTRIBUTE_HP].GetBase();
                addHpMsg.Mode = mode;

                auto it = m_SelectedMePlayers.begin();
                for (auto end = m_SelectedMePlayers.end(); it != end; ++it)
                {
                    addHpMsg.OBJID = *it;
                    g_GameServer->SendToClient(&addHpMsg);
                }
            }
        }
    }
    return br;
}

bool Monster::DropHP(const DropHpMode& mode)
{
    bool br = Object::DropHP(mode);
    if (br)
    {
       if (m_MonsterInfo->iType == MONSTER_BOSS)
       {
           MsgObject_DropHp dorpHpMsg;
           dorpHpMsg.OwnerID = m_GUID;
           dorpHpMsg.OwnerType = OBJECT_MONSTER;
           dorpHpMsg.LastHp = (int32)m_Attribute[ATTRIBUTE_HP].GetBase();
           dorpHpMsg.Mode = mode;
           BroadcastMsg(&dorpHpMsg);
       }
       else
       {
           if (!m_SelectedMePlayers.empty())
           {
               MsgObject_DropHp dropHpMsg;
               dropHpMsg.OwnerID = m_GUID;
               dropHpMsg.OwnerType = OBJECT_MONSTER;
               dropHpMsg.LastHp = (int32)m_Attribute[ATTRIBUTE_HP].GetBase();
               dropHpMsg.Mode = mode;

               auto it = m_SelectedMePlayers.begin();
               for (auto end = m_SelectedMePlayers.end(); it != end; ++it)
               {
                   dropHpMsg.OBJID = *it;
                   g_GameServer->SendToClient(&dropHpMsg);
               }
           }
       }
    }
    return br;
}

void Monster::Idle()
{
    Object::Idle();
}

void Monster::Die()
{
    Object::Die();
    ////通知任务管理
    //if (obj->type == OBJECT_PLAYER)
    //{
    //	((Player*)obj)->ChangePlayerAttribute(ATTRIBUTE_EXP, m_MonsterInfo->iExp);
    //	//((Player*)obj)->m_Attribute[ATTRIBUTE_EXP] += m_MonsterInfo->iExp;
    //	////通知客户端属性改变
    //	//MsgPlayer_Property send;
    //	//send.Property = ATTRIBUTE_EXP;
    //	//send.Value = ((Player*)obj)->m_Attribute[ATTRIBUTE_EXP];
    //	//send.OBJID = m_GUID;
    //	//g_GameServer->SendToClient(&send);	
    //	g_TaskManager->OnMonsterKill(this, (Player*)obj);
    //	const MapInfo* mapInfo = g_CSVFileManager->GetMapByID(obj->m_map->GetMapID());
    //	if(mapInfo->iMapType == 3)
    //	{
    //		g_CopyManager->updateCopyProgress((Player*)obj,this->m_MonsterInfo->id,0);
    //	}
    //}

    //m_TargteTile = Vec2(0,0);
    //m_AttTargteGuid = 0;
    //if (m_MonsterInfo->iDealRefreshTime > 0)
    //{
    //	//设置怪物复活
    //	SetTimer(m_MonsterInfo->iDealRefreshTime*1000, new Callback<Monster>(this, &Monster::RefreshCallback));
    //	//移除怪物
    //	m_map->RemoveMonster(this);	
    //}
    //else
    //{
    //	//移除怪物
    //	m_map->RemoveMonster(this);			
    //}
    ////广播客户端
    //MsgMonster_Del send;
    //send.MonsterGuid = m_GUID;
    //BroadcastMsg(&send, false);
}

void Monster::RefreshCallback(int type)
{
	//Init(m_MonsterInfo);
	//m_map->AddMonster(this);
	//if (m_map)
	//{
	//	std::vector<Object*> viewObjSet;
	//	m_map->GetRangeSet(PLAYER_VIEW_RANGE, this, viewObjSet);
	//	//>广播消息
	//	Object* obj;
	//	Player* player;
	//	size_t count = viewObjSet.size();
	//	for (size_t i = 0; i < count; ++i)
	//	{
	//		obj = viewObjSet[i];
	//		if (obj->type == OBJECT_PLAYER)
	//		{
	//			player = static_cast<Player*>(obj);
	//			player->OnObjEnterView(this);     //>通知别人,自己进入其视野
	//		}
	//	}		
	//}
}

float Monster::CalcMonsterOddsAttribute(AttributeType attribute,AttributeType oddss)
{
	////ODDS初始参数
	//float oddsInit = 0.0f;
	////ODDS等级参数
	//float oddsLevel = 0.0f;
	////ODDS值系数
	//float oddsValue = 0.0f;
	////ODDS调节参数
	//float oddsAdjust =0.0f;
	//switch (oddss)
	//{
	//case ATTRIBUTE_HIT_ODDS:
	//	{
	//		oddsInit = g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_INITHIT];
	//		oddsLevel =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_LEVELHIT];
	//		oddsValue =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_HIT];
	//		oddsAdjust =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_ADJUSTHIT];
	//	}
	//	break;
	//case ATTRIBUTE_DODGE_ODDS:
	//	{
	//		oddsInit = g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_INITDODGE];
	//		oddsLevel =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_LEVELDODGE];
	//		oddsValue =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_DODGE];
	//		oddsAdjust =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_ADJUSTDODGE];
	//	}
	//	break;
	//case ATTRIBUTE_CRIT_ODDS:
	//	{
	//		oddsInit = g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_INITCRIT];
	//		oddsLevel =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_LEVELCRIT];
	//		oddsValue =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_CRIT];
	//		oddsAdjust =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_ADJUSTCRIT];
	//	}		
	//	break;
	//case ATTRIBUTE_SUNDER_ODDS:
	//	{
	//		oddsInit = g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_INITSUNDER];
	//		oddsLevel =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_LEVELSUNDER];
	//		oddsValue =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_SUNDER];
	//		oddsAdjust =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_ADJUSTSUNDER];
	//	}		
	//	break;
	//case ATTRIBUTE_PENETRATE_ODDS:
	//	{
	//		oddsInit = g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_INITPENETRATE];
	//		oddsLevel =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_LEVELPENETRATE];
	//		oddsValue =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_PENETRATE];
	//		oddsAdjust =  g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_ADJUSTPENETRATE];
	//	}		
	//	break;
	//default:
	//	break;
	//}
	//float Attribute = 0.0f;
	//for(int i = 0;i < 11;i++)
	//{
	//	if(m_MonsterInfo->iAtt[i] == attribute)
	//	{
	//		{
	//			Attribute = m_MonsterInfo->iValue[i];
	//		}
	//	}
	//}
	//float Odds = 0.0f;
	//for(int i = 0;i < 11;i++)
	//{
	//	if(m_MonsterInfo->iAtt[i] == oddss)
	//	{
	//		{
	//			Odds = m_MonsterInfo->iValue[i];
	//		}
	//	}
	//}
	//float numConvertOdds = oddsInit + Attribute/(m_MonsterInfo->iLevel * oddsLevel + Attribute * oddsValue) * oddsAdjust;
	//
	//

	//return numConvertOdds + Odds;
    return 0;
}


void Monster::OnMoveByTarget(const Vec2& moveTarget)
{
    m_IsMoveing = true;
}

void Monster::OnMoveByPath(std::vector<int>& paths, const Vec2& moveTarget)
{
    m_IsMoveing = true;
}

void Monster::OnMoveEnd()
{
    m_IsMoveing = false;
}

bool Monster::IsNormalSkill(int skillId)
{
    return false;
}

bool Monster::HasSkill(int skillId)
{
    return false;
}

void Monster::OnUseSkill(int skillId, Object* target)
{
   
}

void Monster::GetSkillsForInterestNormalSkill(std::vector<const SkillInfo*>& skills)
{

}

void Monster::GetSkillsForInterestBeHurt(std::vector<const SkillInfo*>& skills)
{

}

void Monster::GetSkillsForInterestHpPct(std::vector<const SkillInfo*>& skills)
{

}

void Monster::GetSkillsForInterestAddBuff(std::vector<const SkillInfo*>& skills)
{

}

