#include "Player.h"
#include "CSVFileManager.h"
#include "LogManage.h"
#include "Map.h"
#include "MapManager.h"
#include "MsgInfo.h"
#include "StackStream.h"
#include "GameServer.h"
#include "SkillTreeManager.h"
#include <algorithm>
#include "SocialManager.h"
#include "Team.h"
#include "Skill.h"
#include "Buff.h"
#include "Monster.h"
#include "MoveComponent.h"

//-----------------------------------------------------------------------------------------
//>两个帮助检测函数.

//>检测路点a和b是否连续相通的
inline bool CheckIsInterlinkedForTwoPos(const Vec2& a, const Vec2& b)
{
    return TileDistance(a, b) <= 1;
}

//>检测移动路劲是否连续相通的
bool CheckIsInterlinkedForMovePath(const std::vector<int>& movePath)
{
    Vec2 current;
    Vec2 next;
    size_t count = movePath.size();
    for (size_t i = 0; i < count - 1; ++i)
    {
        current = TileId2Tile(movePath[i]);
        next = TileId2Tile(movePath[i+1]);
        if (!CheckIsInterlinkedForTwoPos(current, next))
        {
            return false;
        }
    }
    return true;
}

MsgBase* GetMoveMsg(Object* obj, StackStream<>& moveStream)
{
    bool isMoving = false;
    if (obj->m_Type == OBJECT_PLAYER)
    {
        Player* player = static_cast<Player*>(obj);
        if (player->m_State[State_Moving])
            isMoving = true;
    }
    else
    {
        //do something.
        Monster* pMonster = static_cast<Monster*>(obj);
        if (pMonster->IsMoveing())
            isMoving = true;
    }

    if (isMoving)
    {
        MsgObject_Move moveMsg;
        moveMsg.SysID = ID_G2C;
        moveMsg.MoverID = obj->m_GUID;
        moveMsg.MoverType = obj->m_Type;
        moveMsg.MoverPosX = obj->GetPosition().x;
        moveMsg.MoverPosY = obj->GetPosition().y;
        moveMsg.TargetPosX = obj->m_MoveComp->GetTarget().x;
        moveMsg.TargetPosY = obj->m_MoveComp->GetTarget().y;
        const std::vector<int> paths = obj->m_MoveComp->GetPaths();
        moveMsg.PathNum = paths.size();
        moveStream.Wirte(&moveMsg, sizeof(moveMsg));
        int posId;
        for (size_t i = 0; i < moveMsg.PathNum; ++i)
        {
            posId = paths[i];
            moveStream.Wirte(&posId, sizeof(posId));
        }
        MsgBase* pMsg = static_cast<MsgBase*>(moveStream.Get());
        static_cast<MsgObject_Move*>(pMsg)->Len = moveStream.GetSize();
        return pMsg;
    }

    return NULL;
}

Player::Player()
{
    m_Type = OBJECT_PLAYER;

    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        m_Equip[i] = NULL;
    }

    m_teamId = 0;

    m_HpCooling = false;
    m_MpCooling = false;

    m_Tile4LastUpdateViwe.x = -1.0f;
    m_Tile4LastUpdateViwe.y = -1.0f;

    m_MoveComp = new MoveComponent(this, this);
    m_BattleComp = new BattleComponent(this, this);
}

Player::~Player()
{
    std::vector<SvrItemInfo*>::iterator end = m_BagList.end();
    for (std::vector<SvrItemInfo*>::iterator it = m_BagList.begin(); 
        it != end; ++it)
    {
        delete *it;
    }
    m_BagList.clear();
}

bool Player::Init(uint64 guid)
{
    m_GUID = guid;
	for (int i = 0; i < State_Max; i++)
	{
		m_State[i] = false;
	}
	for (int i = 0; i < ATTRIBUTE_MAX; i++)
	{
		m_Attribute[i].Clear(0.0f);
	}
	return true;
}


void Player::Update(float deltaTime)
{
    m_MoveComp->Update(deltaTime);

    m_BattleComp->Update(deltaTime);
}

void Player::OnChangeBlock(int oldBlock, int newBlock)
{
    UpdateViewObjectSet();
    m_Tile4LastUpdateViwe = GetTile();
}

void Player::OnChangeTile(const Vec2& oldTile, const Vec2& newTile)
{
    if (TileDistance(m_Tile4LastUpdateViwe, newTile) >= 2)
    {
        UpdateViewObjectSet();
    }
}

void Player::LoadBaseAttribute(BaseInfoIt& baseInfo)
{
    m_Attribute[ATTRIBUTE_MONEY].SetBase((float)baseInfo.money);
    m_Attribute[ATTRIBUTE_HP].SetBase((float)baseInfo.hp);
    m_Attribute[ATTRIBUTE_MP].SetBase((float)baseInfo.mp);
    m_Attribute[ATTRIBUTE_GOLD].SetBase((float)baseInfo.gold);
    m_Attribute[ATTRIBUTE_BINDGOLD].SetBase((float)baseInfo.bindGold);
    m_Attribute[ATTRIBUTE_ARENAMONEY].SetBase ((float)baseInfo.arenaMoney);
    m_Attribute[ATTRIBUTE_BATTLEFIELDMONEY].SetBase((float)baseInfo.battlefieldMoney);
    m_Attribute[ATTRIBUTE_XP].SetBase((float)baseInfo.xp);
	m_Attribute[ATTRIBUTE_LEVEL].SetBase((float)baseInfo.level);
    m_Attribute[ATTRIBUTE_EXP].SetBase((float)baseInfo.exp);
    m_Attribute[ATTRIBUTE_REALMLEVEL].SetBase((float)baseInfo.realmLevel);
    m_Attribute[ATTRIBUTE_REALMEXP].SetBase((float)baseInfo.realmExp);
    strncpy(m_Name,baseInfo.playerName, MAX_NAME_SIZE);
    m_HeadIconIndex = baseInfo.headID;
    m_Sex = baseInfo.sex;
    m_OfflineTime = baseInfo.offlineTime;
	m_activeSkillType = baseInfo.activeSkillType;
	m_SocietyID = baseInfo.societyID;
    
    LoadOtherAttributes();
}

void Player::LoadOtherAttributes()
{
	//等级基础属性
	const LevelInfo* levelInfo = g_CSVFileManager->GetLevelByID( (int)m_Attribute[ATTRIBUTE_LEVEL].GetBase());
	if(levelInfo)
	{
		for(int i = 0;i < 11;i++)
		{
			if(levelInfo->iAtt[i] != 0 && levelInfo->iAtt[i] < ATTRIBUTE_MAX)
			{
                m_Attribute[levelInfo->iAtt[i]].SetBase((float)levelInfo->iValue[i]);
			}
		}
	}
    else
    {
        assert(0);
        gLog.Warning("can't find level date in level table.");
    }

    //玩家属性系数初始值
    m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].SetBase( g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_MOVESPEED_FACTOR] );
    m_Attribute[ATTRIBUTE_CRIT_FACTOR].SetBase( g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_CRIT_FACTOR] );
    m_Attribute[ATTRIBUTE_HUR_FACTOR].SetBase( g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_HUR_FACTOR] );
    m_Attribute[ATTRIBUTE_CURE_FACTOR].SetBase( g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_CURE_FACTOR] );
    m_Attribute[ATTRIBUTE_CRIT_FACTOR].SetBase( g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_CRIT_FACTOR] );
    m_Attribute[ATTRIBUTE_HIT_PROB].SetBase( g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_HIT_PROB] );
    m_Attribute[ATTRIBUTE_DODGE_PROB].SetBase( g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_DODGE_PROB] );

    if (m_Attribute[ATTRIBUTE_EXP].GetValueI() == 0)
    {//>如果经验值为零,初始化角色hp
        m_Attribute[ATTRIBUTE_HP].SetBase(m_Attribute[ATTRIBUTE_HPMAX].GetValueF());
        m_Attribute[ATTRIBUTE_MP].SetBase(m_Attribute[ATTRIBUTE_MPMAX].GetValueF());
    }
}



void Player::CalcPlayerOddsAttribute(AttributeType attributeOdds,AttributeType oddsValues)
{
	// m_Attribute[attributeOdds].SetBaseValue( 0.0f);

	////ODDS初始参数
	//float oddsInit = 0.0f;
	////ODDS等级参数
	//float oddsLevel = 0.0f;
	////ODDS值系数
	//float oddsValue = 0.0f;
	////ODDS调节参数
	//float oddsAdjust =0.0f;
	//switch (attributeOdds)
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
	// float numConvertOdds = oddsInit + m_Attribute[oddsValues].GetValueAndBuffF()/(m_Attribute[ATTRIBUTE_LEVEL].GetValueAndBuffF() * oddsLevel + m_Attribute[oddsValues].GetValueAndBuffF() * oddsValue) * oddsAdjust;
	// 

	// m_Attribute[attributeOdds].SetBaseValue(numConvertOdds) ;
	

}

void Player::LoadPlayerOtherAttribute()
{
	//CalcPlayerOtherAttribute( ATTRIBUTE_HPMAX );
	//CalcPlayerOtherAttribute( ATTRIBUTE_MPMAX );
	//CalcPlayerOtherAttribute( ATTRIBUTE_ATTACKHURT );
	//CalcPlayerOtherAttribute( ATTRIBUTE_DEFENSE );
	//CalcPlayerOtherAttribute( ATTRIBUTE_MAGICATTACK );
	//CalcPlayerOtherAttribute( ATTRIBUTE_MAGICDEFENSE );
	//CalcPlayerOtherAttribute( ATTRIBUTE_HIT );
	//CalcPlayerOtherAttribute( ATTRIBUTE_DODGE );
	//CalcPlayerOtherAttribute( ATTRIBUTE_CRIT );
	//CalcPlayerOtherAttribute( ATTRIBUTE_SUNDER );
	//CalcPlayerOtherAttribute( ATTRIBUTE_PENETRATE );
 //   CalcPlayerOtherAttribute( ATTRIBUTE_MOVESPEED);
	//CalcPlayerOddsAttribute ( ATTRIBUTE_HIT_ODDS,ATTRIBUTE_HIT );
	//CalcPlayerOddsAttribute ( ATTRIBUTE_DODGE_ODDS,ATTRIBUTE_DODGE );
	//CalcPlayerOddsAttribute ( ATTRIBUTE_CRIT_ODDS,ATTRIBUTE_CRIT );
	//CalcPlayerOddsAttribute ( ATTRIBUTE_SUNDER_ODDS,ATTRIBUTE_SUNDER );
	//CalcPlayerOddsAttribute ( ATTRIBUTE_PENETRATE_ODDS,ATTRIBUTE_PENETRATE );
	//CalcPlayerAbility();
 //   CalcSuitAttribute();        //>计算套装属性
	//SendOtherAttribute();
}

void Player::CalcPlayerBattlePower()
{
	//计算HP增加的战斗力
	float hpAbility = 0.0f;
	hpAbility = g_CSVFileManager->GetAttributeByID(ATTRIBUTE_HPMAX)->iEffect * m_Attribute[ATTRIBUTE_HPMAX].GetValueF();

	//计算MP增加的战斗力
	float mpAbility = 0.0f;
	mpAbility = g_CSVFileManager->GetAttributeByID(ATTRIBUTE_MPMAX)->iEffect * m_Attribute[ATTRIBUTE_MPMAX].GetValueF();

	//计算攻击强度增加的战斗力
	float attackHurtAbility = 0.0f;
	attackHurtAbility = g_CSVFileManager->GetAttributeByID(ATTRIBUTE_ATTACK)->iEffect * m_Attribute[ATTRIBUTE_ATTACK].GetValueF();

	//计算防御增加的战斗力
	float defenseAbility = 0.0f;
	defenseAbility = g_CSVFileManager->GetAttributeByID(ATTRIBUTE_DEFENSE)->iEffect * m_Attribute[ATTRIBUTE_DEFENSE].GetValueF();

	//计算命中值增加的战斗力
	float hitAbility = 0.0f;
	hitAbility = g_CSVFileManager->GetAttributeByID(ATTRIBUTE_HIT)->iEffect * m_Attribute[ATTRIBUTE_HIT].GetValueF();

	//计算闪避值增加的战斗力
	float dodgeAbility = 0.0f;
	dodgeAbility = g_CSVFileManager->GetAttributeByID(ATTRIBUTE_DODGE)->iEffect * m_Attribute[ATTRIBUTE_DODGE].GetValueF();
	
	//计算暴击值增加的战斗力
	float critAbility = 0.0f;
	critAbility = g_CSVFileManager->GetAttributeByID(ATTRIBUTE_CRIT)->iEffect * m_Attribute[ATTRIBUTE_CRIT].GetValueF();


	m_BattlePower = (int)(hpAbility + mpAbility + attackHurtAbility + defenseAbility + hitAbility + dodgeAbility + critAbility);
}

void Player::CalcSuitAttribute()
{
    //>给自己添加套装属性
    std::vector<int32> suitIds;
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if (m_Equip[i] != NULL && m_Equip[i]->pCfg->iSuitID != 0)
        {
            suitIds.push_back(m_Equip[i]->pCfg->iSuitID);
        }
    }

    if (!suitIds.empty())
    {
        std::sort(suitIds.begin(), suitIds.end(), std::less<int32>());

        int32 currentSuitId = 0;
        int32 count = 0;
        std::vector<int32>::iterator it = suitIds.begin();
        for (std::vector<int32>::iterator end = suitIds.end(); 1; ++it)
        {
            if (it == end || currentSuitId != *it)
            {
                if (currentSuitId != 0)
                {
                    const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(currentSuitId);
                    assert(pInfo);
                    assert(count <= 8);
                    count = MyMin(count, 8);
                    for (int i = 0; i < count / 2; ++i)
                    {
                        AttributeType aType = (AttributeType)pInfo->iSuitAttribute[i];
                        float aValue = pInfo->fSuitValue[i];
                        ChangePlayerAttribute(aType, aValue);
                    }
                }

                if (it == end)
                    break;

                currentSuitId = *it;
                count = 1;
            }
            else
            {
                ++count;
            }
        }
    }
}

void Player::SendDetailAttribute()
{
	if(m_Attribute[ATTRIBUTE_HP].GetValueF() > m_Attribute[ATTRIBUTE_HPMAX].GetValueF())
	{
		m_Attribute[ATTRIBUTE_HP].SetBase(m_Attribute[ATTRIBUTE_HPMAX].GetValueF());
	}
	//主角其他信息发送给客户端
	MsgRole_DetailAttribute msg(0);
	msg.SysID = ID_G2C;
	msg.OBJID = m_GUID;
	msg.Attribute.hpMax = m_Attribute[ATTRIBUTE_HPMAX].GetValueI();
	msg.Attribute.mpMax = m_Attribute[ATTRIBUTE_MPMAX].GetValueI();
	msg.Attribute.attack= m_Attribute[ATTRIBUTE_ATTACK].GetValueI();
	msg.Attribute.defense = m_Attribute[ATTRIBUTE_DEFENSE].GetValueI();
	msg.Attribute.hit = m_Attribute[ATTRIBUTE_HIT].GetValueI();
	msg.Attribute.dodge = m_Attribute[ATTRIBUTE_DODGE].GetValueI();
	msg.Attribute.crit = m_Attribute[ATTRIBUTE_CRIT].GetValueI();
    msg.Attribute.tenacity = m_Attribute[ATTRIBUTE_TENACITY].GetValueI();
    msg.Attribute.hitProb = m_Attribute[ATTRIBUTE_HIT_PROB].GetValueF();
    msg.Attribute.critProb = m_Attribute[ATTRIBUTE_CRIT_PROB].GetValueF();
    msg.Attribute.moveSpeedFactor = m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].GetValueI();
   
    CalcPlayerBattlePower();
	msg.Attribute.battlePower = m_BattlePower;

	g_GameServer->SendToClient(&msg);

}

void Player::SendBaseAttribute()
{
    //>主角信息发送给客户端.
    MsgRole_BaseInfo msg(0);
    msg.SysID = ID_G2C;
    msg.OBJID = m_GUID;
    msg.BaseInfo.playerID = m_GUID;
    strncpy(msg.BaseInfo.playerName, m_Name, MAX_NAME_SIZE);
    msg.BaseInfo.sex = m_Sex;
    msg.BaseInfo.headID = m_HeadIconIndex;
    msg.BaseInfo.offlineTime = m_OfflineTime;
    msg.BaseInfo.level = m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
    msg.BaseInfo.exp = m_Attribute[ATTRIBUTE_EXP].GetValueI();
    msg.BaseInfo.money = m_Attribute[ATTRIBUTE_MONEY].GetValueI();
    msg.BaseInfo.gold = m_Attribute[ATTRIBUTE_GOLD].GetValueI();

    msg.BaseInfo.bindGold = m_Attribute[ATTRIBUTE_BINDGOLD].GetValueI();
    msg.BaseInfo.rechargGoldSum = 0; //??? 
    msg.BaseInfo.arenaScore = 0; //???
    msg.BaseInfo.arenaMoney = m_Attribute[ATTRIBUTE_ARENAMONEY].GetValueI();
    msg.BaseInfo.battlefieldScore = 0; //???
    msg.BaseInfo.battlefieldMoney = m_Attribute[ATTRIBUTE_BATTLEFIELDMONEY].GetValueI();
    
    msg.BaseInfo.mapID = m_Map->GetMapID();
    msg.BaseInfo.posx = (int)GetPosition().x;
    msg.BaseInfo.posy = (int)GetPosition().y;
    msg.BaseInfo.hp   = m_Attribute[ATTRIBUTE_HP].GetValueI();
    msg.BaseInfo.hpMax = m_Attribute[ATTRIBUTE_HPMAX].GetValueI();
    msg.BaseInfo.mp   = m_Attribute[ATTRIBUTE_MP].GetValueI();
    msg.BaseInfo.mpMax = m_Attribute[ATTRIBUTE_MPMAX].GetValueI();
    msg.BaseInfo.xp   = m_Attribute[ATTRIBUTE_XP].GetValueI();
    msg.BaseInfo.xpMax = m_Attribute[ATTRIBUTE_XPMAX].GetValueI(); 
        
    msg.BaseInfo.societyID = m_SocietyID;
    msg.BaseInfo.realmLevel = (int)m_Attribute[ATTRIBUTE_REALMLEVEL].GetValueI();
    msg.BaseInfo.realmExp = (int)m_Attribute[ATTRIBUTE_REALMEXP].GetValueI();

    g_GameServer->SendToClient(&msg);
}

void Player::SendUdpAttributeToDB(AttributeType attributeType)
{
	MsgUpdate_BaseInfo udpBaseInfo(0);
	udpBaseInfo.OBJID = m_GUID;
	udpBaseInfo.attributeType = attributeType;
	udpBaseInfo.attributeValue = m_Attribute[attributeType].GetValueF();
	g_GameServer->SendToDB(&udpBaseInfo);
}

void Player::ChangePlayerAttribute(AttributeType attributeType,float attributeValue)
{
//	m_Attribute[attributeType].setAddValue(attributeValue);
//
//	switch (attributeType)
//	{
//	case ATTRIBUTE_MONEY:
//		break;
//	case ATTRIBUTE_GOLD:
//		break;
//	case ATTRIBUTE_HP:
//		{
//			if (m_Attribute[ATTRIBUTE_HP].GetValueAndBuffI() < 0)
//			{
//				m_Attribute[ATTRIBUTE_HP].SetBaseValue(0);
//			}
//			else if (m_Attribute[ATTRIBUTE_HP].GetValueAndBuffI() > m_Attribute[ATTRIBUTE_HPMAX].GetValueAndBuffI())
//			{
//				m_Attribute[ATTRIBUTE_HP].SetBaseValue(m_Attribute[ATTRIBUTE_HPMAX].GetValueAndBuffF());
//			}
//		}
//		break;
//	case ATTRIBUTE_MP:
//		{
//			if (m_Attribute[ATTRIBUTE_MP].GetValueAndBuffI() < 0)
//			{
//				m_Attribute[ATTRIBUTE_MP].SetBaseValue(0);
//			}
//			else if (m_Attribute[ATTRIBUTE_MP].GetValueAndBuffI() > m_Attribute[ATTRIBUTE_MPMAX].GetValueAndBuffI())
//			{
//				m_Attribute[ATTRIBUTE_MP].SetBaseValue(m_Attribute[ATTRIBUTE_MPMAX].GetValueAndBuffF());
//			}
//		}
//		break;
//	case ATTRIBUTE_BINDGOLD:
//		break;
//	case ATTRIBUTE_ARENAMONEY:
//		break;
//	case ATTRIBUTE_BATTLEFIELDMONEY:
//		break;
//	case ATTRIBUTE_EXPMULTIPLE:
//		break;
//	case ATTRIBUTE_SKILLEXPMULTIPLE:
//		break;
//	case ATTRIBUTE_DAMAGEABSORPTION:
//		break;
//	case ATTRIBUTE_SILENCE:
//		break;
//	case ATTRIBUTE_NOMOVE:
//		break;
//	case ATTRIBUTE_VERTIGO:
//		break;
//	case ATTRIBUTE_INVINCIBLE:
//		break;
//	case ATTRIBUTE_XPMAX:
//		break;
//	case ATTRIBUTE_XP:
//		break;
//	case ATTRIBUTE_LEVEL:
//		{
//			LoadPlayerOtherAttribute();
//			ChangePlayerAttribute(ATTRIBUTE_HP, m_Attribute[ATTRIBUTE_HPMAX].GetValueAndBuffF());
//			const LevelInfo* levelInfo = g_CSVFileManager->GetLevelByID((int)m_Attribute[ATTRIBUTE_LEVEL].GetValueI()-1);
//			m_Attribute[ATTRIBUTE_EXP].SetBaseValue(m_Attribute[ATTRIBUTE_EXP].GetValueI() - levelInfo->iExpValue);
//			ChangePlayerAttribute(ATTRIBUTE_EXP,0);
//		}
//		
//		break;
//	case ATTRIBUTE_EXP:
//		{
//			const LevelInfo* levelInfo = g_CSVFileManager->GetLevelByID(m_Attribute[ATTRIBUTE_LEVEL].GetValueI());
//			if(levelInfo && m_Attribute[ATTRIBUTE_EXP].GetValueI() >= levelInfo->iExpValue)
//			{
//				ChangePlayerAttribute(ATTRIBUTE_LEVEL,1);
//			}
//		}
//		break;
//	case ATTRIBUTE_REALMLEVEL:
//		{
//			LoadPlayerOtherAttribute();
//		//	m_Attribute[ATTRIBUTE_REALMEXP] = 0.0f;
//		}
//		
//		break;
//	//!暂时不做
//	case ATTRIBUTE_REALMEXP:
//		{
//			
//		}
//		break;
//	default:
//		break;
//	}
//	MsgPlayer_Property send;
//	send.OBJID = m_GUID;
//	send.Property = attributeType;
//	send.Value = m_Attribute[attributeType].GetValueAndBuffF();
//	g_GameServer->SendToClient(&send);
//	SendUdpAttributeToDB(attributeType);
}

void Player::SendSkillList()
{
    MsgSkill_List skillListMsg;
    skillListMsg.SysID = ID_G2C;
    skillListMsg.OBJID = m_GUID;

    //////////////////////////////////////////////////////////////////////////
    //>添加技能,临时代码
    if (m_SkillList.empty())
    {
        AddSkill(101);
        AddSkill(113);
        AddSkill(115);
    }
    //>
    //////////////////////////////////////////////////////////////////////////

    if (m_activeSkillType == 0 && m_SkillList.size() >= 1)
    {
        m_activeSkillType = m_SkillList[0].data.skillType;
    }

    skillListMsg.numberSkillTree = m_SkillList.size();
    
    StackStream<> skillListStream;
    skillListStream.Wirte(&skillListMsg, sizeof(skillListMsg));
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        SkillInfoIt& skillInfo = (*it).data;

        skillListStream.Wirte(&skillInfo.skillType, sizeof(skillInfo.skillType));
        skillListStream.Wirte(&skillInfo.level, sizeof(skillInfo.level));
    }

    MsgSkill_List* pSkillListMsg = (MsgSkill_List*)skillListStream.Get();
    pSkillListMsg->Len = skillListStream.GetSize();

    g_GameServer->SendToClient(pSkillListMsg);
}

void Player::Relive(int8 type)
{
	////原地
	//if (type == 1)
	//{
	//}
    CancelState(State_Die);
    MsgG2CPlayer_Relive send;
    send.OBJID = m_GUID;
    send.SysID = ID_G2C;
    send.mapid = m_Map->GetMapID();
    send.posid = Tile2TileId(GetTile());
    g_GameServer->SendToClient(&send);

    //>重置hp
    AddHp(AddHpMode(m_Attribute[ATTRIBUTE_HPMAX].GetValueI()/2,AddHpMode::Relive));
    Idle();
}

void Player::UpdateViewObjectSet()
{   
    if (m_Map)
    {
        std::vector<Object*> viewObjSet;
        m_Map->GetRangeSet(PLAYER_VIEW_RANGE, this, viewObjSet);
    
        //>广播消息
        Object* obj;
        Player* player;
        size_t count = viewObjSet.size();
        //>删除离开视野的对象
        for (std::list<Object*>::iterator j = m_ViewObjSet.begin(); 
            j != m_ViewObjSet.end(); )
        {
            obj = *j++;
            bool findIt = false;
            for (size_t i = 0; i < count; ++i)
            {
                if (viewObjSet[i] == obj)
                {
                    findIt = true;
                }
            }

            if (false == findIt)
            {
                OnObjLeaveView(obj);           //>通知自己
                if (obj->m_Type == OBJECT_PLAYER)
                {
                    player = static_cast<Player*>(obj);
                    player->OnObjLeaveView(this);     //>通知别人
                }
            }
            
        }

        //>添加进入视野的对象
        for (size_t i = 0; i < count; ++i)
        {
            obj = viewObjSet[i];
            player = static_cast<Player*>(obj);
            bool findIt = false;
            for (std::list<Object*>::iterator j = m_ViewObjSet.begin(); 
                j != m_ViewObjSet.end(); )
            {
                if (*j++ == obj)
                {
                    findIt = true;
                }
            }

            if (false == findIt)
            {
                OnObjEnterView(obj);           //>通知自己
                if (obj->m_Type == OBJECT_PLAYER)
                {
                    player = static_cast<Player*>(obj);
                    player->OnObjEnterView(this);     //>通知别人
                }
            }
        }
    }
}

void Player::ActivateSkill(int skillType)
{
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        if((*it).data.skillType == skillType)
        {
            m_activeSkillType = skillType;

            //>发送到数据库
            MsgSkill_Activate activeMsg;
            activeMsg.SysID = ID_G2D;
            activeMsg.OBJID = m_GUID;
            activeMsg.ActivateSkillType = m_activeSkillType;
            g_GameServer->SendToDB(&activeMsg);

            return;
        }
    }
    gLog.Warning("can't find skill tree type[%d] when call ActivateSkillTree.", skillType);
}

void Player::SetChoseSkill(int32 index, int32 skillId)
{
    if (0 <= index && index < MAX_CHOSESKILL)
    {
        int oldIndex = -1;
        for (int i = 0; i < MAX_CHOSESKILL; ++i)
        {
            if (m_ChoseSkill[i] == skillId)
            {
                oldIndex = i;
                break;
            }
        }
       
        if (oldIndex >= 0)
        {
            m_ChoseSkill[oldIndex] = m_ChoseSkill[index];
        }

        m_ChoseSkill[index] = skillId;

        //>发送到数据库
        MsgSkill_ChoseSkill choseMsg;
        choseMsg.SysID = ID_G2D;
        choseMsg.OBJID = m_GUID;
        choseMsg.setIndex = index;
        choseMsg.skillId = skillId;
        g_GameServer->SendToDB(&choseMsg);
    }
}

void Player::Chest(AttributeType type, int num)
{
	//const ChestInfo *info = g_CSVFileManager->GetChestInfo(m_Attribute[ATTRIBUTE_LEVEL].GetValueI(), type, num);
	//if (info)
	//{
	//	if (m_Attribute[type].GetValueI() >= info->iCurrencyNum)
	//	{
	//		ChangePlayerAttribute(type, -info->iCurrencyNum);
	//		g_ItemManager->AddItem(this, ItemOperate_BuyChest, info->iBuyItemsid, 1);
	//	}
	//	else
	//	{
	//		if (type == ATTRIBUTE_MONEY)
	//		{
	//			MsgNotify notify(ID_G2C, Notify_S2C_No_Money);
	//			notify.OBJID = m_GUID;
	//			g_GameServer->SendToClient(&notify);
	//		} 
	//		else
	//		{
	//			MsgNotify notify(ID_G2C, Notify_S2C_No_Gold);
	//			notify.OBJID = m_GUID;
	//			g_GameServer->SendToClient(&notify);
	//		}
	//		return;
	//	}
	//	if (info->iSpecialDropid > 0)
	//	{
	//		g_ItemManager->DropItems(this, info->iSpecialDropid, ItemOperate_Chest);
	//	}
	//	if (info->iDropid > 0)
	//	{
	//		for (int i = 0; i < num; i++)
	//		{
	//			g_ItemManager->DropItems(this, info->iDropid, ItemOperate_Chest);
	//		}
	//	}
	//}
}

bool Player::AddHp(const AddHpMode& mode)
{
    bool br = Object::AddHp(mode);
    if (br)
    {
        MsgObject_AddHp addHpMsg;
        addHpMsg.OwnerID = m_GUID;
        addHpMsg.OwnerType = OBJECT_PLAYER;
        addHpMsg.LastHp = (int32)m_Attribute[ATTRIBUTE_HP].GetBase();
        addHpMsg.Mode = mode;
        BroadcastMsg(&addHpMsg);
    }

    if (br)
        SendUdpAttributeToDB(ATTRIBUTE_HP);

    return br;
}

bool Player::DropHP(const DropHpMode& mode)
{
    bool br = Object::DropHP(mode);
    if (br || mode.mode == DropHpMode::WuDiMianYiBuff || mode.mode == DropHpMode::Dodge || DropHpMode::HuDunXiShouBuff)
    {
        MsgObject_DropHp dropHpMsg;
        dropHpMsg.OwnerID = m_GUID;
        dropHpMsg.OwnerType = OBJECT_PLAYER;
        dropHpMsg.LastHp = (int32)m_Attribute[ATTRIBUTE_HP].GetBase();
        dropHpMsg.Mode = mode;
        BroadcastMsg(&dropHpMsg);
    }

    if (br)
        SendUdpAttributeToDB(ATTRIBUTE_HP);

    return br;
}

void Player::Idle()
{
    Object::Idle();

    //MsgObject_SyncAttribute hpChangeMsg;
    //hpChangeMsg.Property = ATTRIBUTE_HP;
    //hpChangeMsg.OwnerID = m_GUID;
    //hpChangeMsg.Value = m_Attribute[ATTRIBUTE_HP].GetValueF();
    //SendUdpAttributeToDB(ATTRIBUTE_HP);
    //BroadcastMsg(&hpChangeMsg);
}

void Player::Die()
{
    //if (m_map->m_MapInfo->iResMapid == 0)
    //{
    //	MsgPlayer_ReliveData send;
    //	send.OBJID = m_GUID;
    //	send.SysID = ID_G2C;
    //	//send.
    //	g_GameServer->SendToClient(&send);
    //}
    //else
    //{

    //}
    //!判断当前地图能否原地复活
    Object::Die();

    MsgPlayer_ReliveData send;
    send.OBJID = m_GUID;
    send.SysID = ID_G2C;
    //send.
    g_GameServer->SendToClient(&send);
}

void Player::OnOnline()
{

}

void Player::OnOffline()
{
    Team* pTeam = g_SocailManager->GetTeam(m_teamId);
    if(pTeam){
        pTeam->OnPlayerExit(m_GUID);
    }

    gLog.Info("player[%llu] off line.", m_GUID);
}

void Player::OnEnterMap()
{  
    //>发送基础数据到客户端

    SendBaseAttribute();
	SendDetailAttribute();

    if (m_Map)
    {
        std::vector<Object*> viewObjSet;
        m_Map->GetRangeSet(PLAYER_VIEW_RANGE, this, viewObjSet);
        //>广播消息
        Object* obj;
        Player* player;
        size_t count = viewObjSet.size();
        for (size_t i = 0; i < count; ++i)
        {
            obj = viewObjSet[i];
            if (obj->m_Type == OBJECT_PLAYER)
            {
                player = static_cast<Player*>(obj);
                player->OnObjEnterView(this);     //>通知别人,自己进入其视野
            }
        }

        for (size_t i = 0; i < count; ++i)
        {
            obj = viewObjSet[i];
            OnObjEnterView(obj);              //>把别人加入自己显示列表
        }
    }
}

void Player::OnLeaveMap()
{
    Object* obj;
    Player* player;
    std::list<Object*>::iterator it = m_ViewObjSet.begin();
    for (std::list<Object*>::iterator end = m_ViewObjSet.end(); 
        it != end; ++it)
    {
        obj = *it;
        if (obj->m_Type == OBJECT_PLAYER)
        {
            player = static_cast<Player*>(obj);
            player->OnObjLeaveView(this);     //>通知别人,自己离开其视野
        }
    }
}

void Player::OnObjEnterView(Object* obj)
{
    //>通知客户端 创建Object;
    SyncCreate(obj);
    //>通知客户端 同步移动状态
    SyncMove(obj);
    //>通知客户端buff
    SyncBuff(obj);

    m_ViewObjSet.push_back(obj);
}

void Player::OnObjLeaveView(Object* obj)
{
    //>通知客户端删除Object
    SyncDelete(obj);

    std::list<Object*>::iterator it = m_ViewObjSet.begin();
    for (std::list<Object*>::iterator end = m_ViewObjSet.end(); 
         it != end; ++it)
    {
        if (obj == *it)
        {
            m_ViewObjSet.erase(it);
            break;
        }
    }
}

void Player::OnChangeEquipment()
{
    MsgEquip_ShowUpdate equipUpdateMsg;
    equipUpdateMsg.whoGuid = m_GUID;
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if (m_Equip[i] != NULL)
            equipUpdateMsg.equip[i] = m_Equip[i]->pCfg->id;
        else
            equipUpdateMsg.equip[i] = 0;
    }
    BroadcastMsg(&equipUpdateMsg);

    //>重新计算角色属性
    LoadPlayerOtherAttribute();
}

void Player::OnEquipmentUpstart(SvrItemInfo* pItem)
{
    //>身上装备升星会影响到角色属性
    LoadPlayerOtherAttribute();
}

void Player::OnEquipmentCompose(SvrItemInfo* pItem)
{
    //>身上装备参与合成会影响到角色属性
    LoadPlayerOtherAttribute();
}

void Player::OnEquipmentInherit()
{
    //>身上装备参与传承会影响到角色属性
    LoadPlayerOtherAttribute();
}

void Player::SyncCreate(Object* obj)
{
    //>同步给自己,将要创建一个obj
    if (obj->m_Type == OBJECT_PLAYER)
    {
        Player* otherPlayer = static_cast<Player*>(obj);
        MsgOtherRole_BaseInfo msg(m_GUID);
        msg.SysID = ID_G2C;
        msg.OBJID = m_GUID;
        strncpy(msg.playerName, otherPlayer->m_Name, MAX_NAME_SIZE);
        msg.guid = otherPlayer->m_GUID;
        msg.sex = otherPlayer->m_Sex;
        for (int i = 0; i < EQUIPTYPE_MAX; ++i)
        {
            if (otherPlayer->m_Equip[i] != NULL)
                msg.equip[i] = otherPlayer->m_Equip[i]->pCfg->id;
            else
                msg.equip[i] = 0;
        }

        msg.posX = otherPlayer->GetPosition().x;
        msg.posY = otherPlayer->GetPosition().y;
        msg.hp = otherPlayer->m_Attribute[ATTRIBUTE_HP].GetValueI();
        msg.maxHp = otherPlayer->m_Attribute[ATTRIBUTE_HPMAX].GetValueI();
        msg.level = otherPlayer->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
        msg.headIconIndex = otherPlayer->m_HeadIconIndex;
        gLog.AddLog(LOG_LEVEL_INFO, "send to player[%llu] create a player[%llu] at(%f,%f)", m_GUID, otherPlayer->m_GUID, msg.posX, msg.posY);
        g_GameServer->SendToClient(&msg);
    }
    else if (obj->m_Type == OBJECT_MONSTER)
    {
        Monster* monster = static_cast<Monster*>(obj);
        MsgMonster_Baseinfo msg;
        msg.SysID = ID_G2C;
        msg.OBJID = m_GUID;
        
        msg.MonsterGuid = monster->m_GUID;
        msg.MonsterId   = monster->m_MonsterInfo->id;
        msg.hp          = monster->m_HP;
        msg.hp_max      = monster->m_HPMax;
        msg.CanAttack   = monster->m_MonsterInfo->bIsAttack;
        msg.posX       = monster->GetPosition().x;
        msg.posY       = monster->GetPosition().y;
        gLog.AddLog(LOG_LEVEL_INFO, "send to player[%llu] create monster[%llu] at(%f,%f)", m_GUID, monster->m_GUID, monster->GetPosition().x, monster->GetPosition().y);
        g_GameServer->SendToClient(&msg);
    }
}

void Player::SyncDelete(Object* obj)
{
    //>同步自己要删除一个obj
    if (obj->m_Type == OBJECT_PLAYER)
    {
        Player* player = static_cast<Player*>(obj);
        MsgOtherRole_Leave msg(m_GUID);
        msg.SysID = ID_G2C;
        msg.OBJID = m_GUID;
        msg.LeaverID = player->m_GUID;

        gLog.AddLog(LOG_LEVEL_INFO, "send to player[%llu] leave a player[%llu] from map.", m_GUID, player->m_GUID);
        g_GameServer->SendToClient(&msg);
    }
    else if(obj->m_Type == OBJECT_MONSTER)
    {
        Monster* monster = static_cast<Monster*>(obj);
        MsgMonster_Del msg;
        msg.SysID = ID_G2C;
        msg.OBJID = m_GUID;
        msg.MonsterGuid = monster->m_GUID;

        gLog.AddLog(LOG_LEVEL_INFO, "send to player[%llu] delete monster=%d from map.", m_GUID, monster->m_GUID);
        g_GameServer->SendToClient(&msg);
    }
}


void Player::SyncMove(Object* obj)
{
    StackStream<> moveStream;
    MsgBase* pMsg = GetMoveMsg(obj, moveStream);
    if (pMsg)
    {
        pMsg->OBJID = m_GUID;
        g_GameServer->SendToClient(pMsg);
    }
}

void Player::SyncBuff(Object* obj)
{
    if (obj->m_BattleComp)
    {
        Buff* pBuff = NULL;
        auto buffList = obj->m_BattleComp->GetBuffList();
        if (!buffList.empty())
        {
            MsgObject_BuffList buffListMsg;
            buffListMsg.OBJID = m_GUID;
            buffListMsg.OwnerID = obj->m_GUID;
            buffListMsg.OwnerType = obj->m_Type;
            buffListMsg.buffNumber = (int)buffList.size();

            int buffId;
            int buffInstId;
            float buffLastTime;
            StackStream<> buffListStream;
            buffListStream.Wirte(&buffListMsg, sizeof(buffListMsg));
            auto it = buffList.begin();
            for (auto end = buffList.end(); it != end; ++it )
            {
                pBuff = *it;
                buffId          = pBuff->GetInfo()->id;
                buffInstId      = pBuff->GetInstanceId();
                buffLastTime    = pBuff->GetTime();

                buffListStream.Wirte(&buffId, sizeof(buffId));
                buffListStream.Wirte(&buffInstId, sizeof(buffInstId));
                buffListStream.Wirte(&buffLastTime, sizeof(buffLastTime));
            }

            MsgObject_BuffList* pListMsg = (MsgObject_BuffList*)buffListStream.Get();
            pListMsg->Len = buffListStream.GetSize();
            g_GameServer->SendToClient(pListMsg);
        }
    }
}

/*
传入参数：操作的枚举
返回值：是否允许进行该操作
用法：做某种操作之前，调用此函数进行判断
if( !玩家指针->CheckOperInState(具体操作) )
{
如果返回值为false，表明在人物的当前状态下，不允许进行这样的操作
}o
*/
bool Player::CheckOperInState(PlayerState_Operation _operate)
{
	if( _operate < 0 || _operate >= Oper_Max)
	{
		return false;
	}
	for(int i = 0;i < State_Max; i++)
	{		
		if( m_State[i] && !g_CSVFileManager->m_StateMutex[i][_operate] )
		{
			return false;
		}
	}
	return true;
}

/*
传入参数：操作的枚举
返回值：状态是否有改动
用法：当CheckOperInState成功后执行某种操作之前调用这个函数
*/
bool Player::ChangeState(PlayerState_Operation _oper)
{
	//!这段代码用于捡查之前是否漏掉CheckOperInState 所有状态操作加完后可考虑删除！！！
	if( _oper >= Oper_Max )
	{
		gLog.AddLog(LOG_LEVEL_DEBUG, "状态检查有遗漏～操作ID为%d", _oper);
		return false;
	}
	for(int iS = 0;iS < State_Max;iS++)
	{		
		if(m_State[iS] && !g_CSVFileManager->m_StateMutex[iS][_oper])
		{
			gLog.AddLog(LOG_LEVEL_DEBUG, "状态检查有遗漏～操作ID为%d", _oper);
			return false;
		}
	}


	bool isChange = false;		//是否修改状态表的标志
	for (int i = 0; i < State_Max; i++)
	{
		switch (g_CSVFileManager->m_StateSet[i][_oper])
		{
		case 1:
			{
				if (!m_State[i])
				{
					AddState((PlayerState_States)i);
					isChange = true;
				}
			}
			break;
		case -1:
			{
				if (m_State[i])
				{
					CancelState((PlayerState_States)i);
					isChange = true;
				}
			}
			break;
		}
	}
	return isChange;
}

/*
传入参数：状态的枚举
返回值：玩家当前是否拥有传入的状态
用法：做某种操作之前，可酌情调用此函数进行判断
*/
bool Player::GetState(PlayerState_States _state)
{
	if(_state < 0 || _state >= State_Max )
	{
		return false;
	}
	return m_State[_state];
}

/*
传入参数：状态的枚举
无返回值
用法：这个函数是个备用函数
注意：这个函数是仅仅只是增加一种状态，没有状态间的互斥判断，所以请慎用！！
*/
void Player::AddState(PlayerState_States _state)
{
	if (_state > 0 && _state < State_Max && !m_State[_state])
	{		
		//!状态对应的玩家属性改变
		switch (_state)
		{
		case State_Moving:
			break;
		case State_ChangeMap:
			break;
		case State_Die:
			break;
		case State_Fight:
			break;
		case State_Riding:
			break;
		case State_Team:
			break;
		case State_BattlefieldLine:
			break;
		case State_CopyLine:
			break;
		case State_Collect:
			break;
		case State_Hang:
			break;
		case State_Arena:
			break;
		case State_Battlefield:
			break;
		case State_Copy:
			break;
		case State_Silence:
			break;
		case State_Nomove:
			break;
		case State_Vertigo:
			break;
		case State_Invincible:
			break;
		case State_Max:
			break;
		default:
			break;
		}
		m_State[_state] = true;
	}
}

/*
传入参数：状态的枚举
无返回值
用法：这个函数是个备用函数
注意：这个函数是仅仅只是取消一种状态，没有状态间的互斥判断，所以请慎用！！
*/
void Player::CancelState(PlayerState_States _state)
{
	if (_state >= 0 && _state < State_Max && m_State[_state])
	{		
		//!状态对应的玩家属性改变
		switch (_state)
		{
		case State_Moving:
			break;
		case State_ChangeMap:
			break;
		case State_Die:
			break;
		case State_Fight:
			break;
		case State_Riding:
			break;
		case State_Team:
			break;
		case State_BattlefieldLine:
			break;
		case State_CopyLine:
			break;
		case State_Collect:
			break;
		case State_Hang:
			break;
		case State_Arena:
			break;
		case State_Battlefield:
			break;
		case State_Copy:
			break;
		case State_Silence:
			break;
		case State_Nomove:
			break;
		case State_Vertigo:
			break;
		case State_Invincible:
			break;
		case State_Max:
			break;
		default:
			break;
		}
		m_State[_state] = false;
	}
}

void Player::OnMoveByTarget(const Vec2& moveTarget)
{
    m_State[State_Moving] = true;

    //>广播移动
    StackStream<> moveStream;
    MsgBase* pMsg = GetMoveMsg(this, moveStream);
    if (pMsg)
    {
        BroadcastMsg(pMsg);
    }
}

void Player::OnMoveByPath(std::vector<int>& paths, const Vec2& moveTarget)
{
    m_State[State_Moving] = true;

    //>广播移动
    StackStream<> moveStream;
    MsgBase* pMsg = GetMoveMsg(this, moveStream);
    if (pMsg)
    {
        BroadcastMsg(pMsg);
    }
}

void Player::OnMoveEnd()
{
    m_State[State_Moving] = false;
}

void Player::BroadcastMsg(MsgBase* msg, bool isBroadcastSelf)
{	
	std::list<Object*>::iterator it = m_ViewObjSet.begin();
	for (std::list<Object*>::iterator end = m_ViewObjSet.end(); 
		it != end; ++it)
	{
		if ((*it)->m_Type == OBJECT_PLAYER)
		{
			msg->OBJID = (*it)->m_GUID;	
			g_GameServer->SendToClient(msg);
		}
	}

	if (isBroadcastSelf)
	{
		msg->OBJID = m_GUID;
		g_GameServer->SendToClient(msg);
	}	
}


bool Player::IsTeamMembers(int64 guid)
{
    if (m_teamId)
    {
        Team* pTeam = g_SocailManager->GetTeam(m_teamId);
        assert(pTeam);
        return pTeam->HasMember(guid);
        
    }
	return false;
}


void Player::StartHpCD()
{
	g_TimerManage->AddTimer(1, (uint32)(g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_HP_PUBLIC_CD] * 1000), new Callback<Player>(this, &Player::HpCDCallback));
    m_HpCooling = true;  
}

void Player::StartMpCD()
{
	g_TimerManage->AddTimer(1, (uint32)(g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_MP_PUBLIC_CD] * 1000), new Callback<Player>(this, &Player::MpCDCallback));
    m_MpCooling = true;   
}

bool Player::IsHpCDCooled()
{
    return m_HpCooling == false;
}

bool Player::IsMpCDCooled()
{
    return m_MpCooling == false;
}

void Player::HpCDCallback(int t)
{
    m_HpCooling = false;
}

void Player::MpCDCallback(int t)
{
    m_MpCooling = false;
}

void Player::AddExp(int exp)
{
    //level full ?

    if (exp > 0)
    {
        int nowExp = (int)(m_Attribute[ATTRIBUTE_EXP].GetBase());

        //>查表. 查看是否达到升级经验
        int levelUpgradeExpCost = 0;
        if (nowExp + exp > levelUpgradeExpCost)
        {
            Upgrade();
            exp = nowExp + exp - levelUpgradeExpCost;
            AddExp(exp);
        }
        else
        {
            m_Attribute[ATTRIBUTE_EXP].AddBase((float)exp);
        }
    }
}

void Player::Upgrade()
{
    m_Attribute[ATTRIBUTE_EXP].SetBase(0);
}

int  Player::GetMoney() //>拥有金币
{
    return m_Attribute[ATTRIBUTE_MONEY].GetValueI();
}

void Player::AddMoney(int money) //>添加金币
{
    m_Attribute[ATTRIBUTE_MONEY].AddBase((float)money);
}

bool Player::SpendMoney(int money) //>消耗金币
{
    int hasMoney = GetMoney();
    if (hasMoney >= money)
    {
        m_Attribute[ATTRIBUTE_MONEY].AddBase(-1.0f * money);
        return true;
    }
    return false;
}

int  Player::GetGold() //>拥有元宝
{
    return m_Attribute[ATTRIBUTE_GOLD].GetValueI();
}

void Player::AddGold(int gold) //>添加元宝
{
    m_Attribute[ATTRIBUTE_GOLD].AddBase((float)gold);
}

bool Player::SpendGold(int gold) //>消耗元宝
{
    int hasGold = GetGold();
    if (hasGold >= gold)
    {
        m_Attribute[ATTRIBUTE_GOLD].AddBase(-1.0f * gold);
        return true;
    }
    return false;
}

int  Player::GetBindGold()   //>拥有绑定元宝
{
    return m_Attribute[ATTRIBUTE_BINDGOLD].GetValueI();
}

void Player::AddBindGold(int gold)  //>添加绑定元宝
{
    m_Attribute[ATTRIBUTE_BINDGOLD].AddBase((float)gold);
}

bool Player::SpendBindGold(int gold)       //>消耗绑定元宝
{
    int hasBindGold = GetBindGold();
    if (hasBindGold >= gold)
    {
        m_Attribute[ATTRIBUTE_BINDGOLD].AddBase(-1.0f * gold);
        return true;
    }
    return false;
}

int Player::GetArenaMoney()  //>拥有战场货币
{
    return m_Attribute[ATTRIBUTE_ARENAMONEY].GetValueI();
}

void Player::AddArenaMoney(int money)   //>添加战场货币
{
    m_Attribute[ATTRIBUTE_ARENAMONEY].AddBase((float)money);
}

bool Player::SpendArenaMoney(int money) //>消耗战场货币
{
    int hasArenaMoney = GetArenaMoney();
    if (hasArenaMoney >= money)
    {
        m_Attribute[ATTRIBUTE_ARENAMONEY].AddBase(-1.0f * money);
        return true;
    }
    return false;
}

int Player::GetBattlefieldMoney()   //>拥有竞技场货币
{
    return m_Attribute[ATTRIBUTE_BATTLEFIELDMONEY].GetValueI();
}

void Player::AddBattlefieldMoney(int money)//>添加竞技场货币
{
    m_Attribute[ATTRIBUTE_BATTLEFIELDMONEY].AddBase((float)money);
}

bool Player::SpendBattlefieldMoney(int money)//>消耗竞技场货币
{
    int hasBattlefieldMoney = GetBattlefieldMoney();
    if (hasBattlefieldMoney >= money)
    {
        m_Attribute[ATTRIBUTE_BATTLEFIELDMONEY].AddBase(-1.0f * money);
        return true;
    }
    return false;
}

bool Player::AddSkill(int skillType)
{
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        if (it->data.skillType == skillType)
        {
            return false;
        }
    }

    size_t index = m_SkillList.size();
    m_SkillList.resize(m_SkillList.size() + 1);
    SvrSkillInfo& skill = m_SkillList[index];
    skill.data.skillType = skillType;
    skill.data.level = 1;
    const SkillInfo* pInfo = g_CSVFileManager->GetSkillByID(SKILL_TYPE_AND_LEVEL_TO_ID(skill.data.skillType, skill.data.level));
    assert(pInfo);
    if (pInfo)
    {
        skill.pCfg = pInfo;
    }
    //>发送数据库更新

    return true;
}

bool Player::UpgradeSkill(int skillType)
{
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        if (it->data.skillType == skillType)
        {
            SvrSkillInfo& skill = *it;
            const SkillInfo* pInfo = g_CSVFileManager->GetSkillByID(SKILL_TYPE_AND_LEVEL_TO_ID(skill.data.skillType, skill.data.level + 1));
            if (pInfo)
            {
                skill.data.level += 1;
                skill.pCfg = pInfo;
                //>发送数据库更新

                return true;
            }
            break;
        }
    }
    return false;
}

bool Player::IsNormalSkill(int skillId)
{
    if (skillId == m_SkillList[0].pCfg->id)
    {
        return true;
    }
    return false;
}

bool Player::HasSkill(int skillId)
{
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        if(it->pCfg->id == skillId)
        {
            return true;
        }
    }
    return false;
}

void Player::OnUseSkill(int skillId, Object* target)
{
    if (m_State[State_Moving])
    {
        m_MoveComp->StopMove();
    }

    MsgG2CPlayer_UseSkill useSkillMsg;
    useSkillMsg.SkillID = skillId;
    useSkillMsg.attacker = m_GUID;
    useSkillMsg.attackerType = m_Type;
    useSkillMsg.target = target->m_GUID;
    useSkillMsg.targetType = target->m_Type;
    BroadcastMsg(&useSkillMsg);
}

void Player::OnAddBuff(Buff* pBuff)
{
    MsgObject_AddBuff addBuffMsg;
    addBuffMsg.OwnerID = m_GUID;
    addBuffMsg.OwnerType = m_Type;
    addBuffMsg.buffId = pBuff->GetInfo()->id;
    addBuffMsg.buffInstanceId = pBuff->GetInstanceId();
    addBuffMsg.lastTime = pBuff->GetTime();
    BroadcastMsg(&addBuffMsg);
}

void Player::OnRemoveBuff(Buff* pBuff)
{
    MsgObject_DelBuff delBuffMsg;
    delBuffMsg.OwnerID = m_GUID;
    delBuffMsg.OwnerType = m_Type;
    delBuffMsg.buffInstanceId = pBuff->GetInstanceId();
    BroadcastMsg(&delBuffMsg);
}

void Player::GetSkillsForInterestNormalSkill(std::vector<const SkillInfo*>& skills)
{
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        if(it->pCfg->triggerType  == SklTrg_NormalAttack)
        {
            skills.push_back(it->pCfg);
        }
    }
}

void Player::GetSkillsForInterestBeHurt(std::vector<const SkillInfo*>& skills)
{
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        if(it->pCfg->triggerType == SklTrg_BeHurt)
        {
            skills.push_back(it->pCfg);
        }
    }
}

void Player::GetSkillsForInterestHpPct(std::vector<const SkillInfo*>& skills)
{

}

void Player::GetSkillsForInterestAddBuff(std::vector<const SkillInfo*>& skills)
{

}



