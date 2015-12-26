#include "Hero.h"
USING_NS_CC;

#include "CSVFileManager.h"
#include "LogicManager.h"
#include "CCArmatureDataManager.h"
#include "CCArmature.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "UIMain.h"
#include "ResManager.h"
#include "SceneManager.h"
#include "BaseFunction.h"
#include "ArmatureExtend.h"
#include "ArmatureExPhantom.h"
#include "AStar.h"
#include "NetManager.h"
#include "StackStream.h"

#include "UITextAtlas.h"
#include "UIImageView.h"

#include "CCActionInstant.h"
#include "deprecated/CCString.h"

using namespace cc2dui;

Hero::Hero()
{		
}

Hero::~Hero()
{
}

bool Hero::Init()
{
	m_iFindPathClickNpc = 0;

	//公会ID
	m_isocietyID = 0;

	m_Name = "来点我头像";
	//m_SkillList[0] = 1001001;
	//m_SkillList[1] = 1002001;
	//m_SkillList[2] = 1003001;
	//m_SkillList[3] = 1004001;
	//m_SkillList[4] = 1005001;
	MagicWeaponData data;
	data.id = 1;
	data.level = 3;
	data.exp = 10;
	data.Mountid = 100001;
	data.Growth = 1.5f;
	m_MagicWeaponIDList[1001] = data;
	data.id = 2;
	data.level = 5;
	data.exp =20;
	m_MagicWeaponIDList[1002] = data;
	data.id = 3;
	data.level = 9;
	data.exp = 30;
	m_MagicWeaponIDList[1003] = data;
	data.id = 4;
	data.level = 11;
	data.exp = 40;
	m_MagicWeaponIDList[1004] = data;
	data.id = 4;
	data.level = 11;
	data.exp = 40;
	m_MagicWeaponIDList[1005] = data;
	data.id = 3;
	data.level = 9;
	data.exp = 30;
	m_MagicWeaponIDList[1006] = data;
	m_CurrentMagicWeaponID = 1003;

	m_Type = OBJECT_PLAYER;

    m_VIPLevel = 0;
    m_SelTarget = NULL;

    memset(&m_State, 0, sizeof(m_State));
    memset(&m_Attribute, 0, sizeof(m_Attribute));

    //>初始化背包
    std::vector<CliItemInfo*> empty;
    m_Bag.Init(empty);

	return Player::Init(g_LogicManager->m_BaseInfo.playerID,
                        g_LogicManager->m_BaseInfo.sex,
                        g_LogicManager->m_BaseInfo.playerName,
                        NULL);
}

void Hero::Update(float delta)
{
    if (!m_SkillCDMap.empty())
    {//>更新cd
        auto it = m_SkillCDMap.begin();
        for (auto end = m_SkillCDMap.end(); it != end; ++it)
        {
            if (it->second > 0.0f)
            {
                it->second -= delta;
            }
        }
    }

    Player::Update(delta);
}

bool Hero::FindPath(const Vec2 &tagPos)
{
    if (IsDead())
        return false;
    
    Vec2 from = getPosition();
    from.y = g_SceneManager->m_MainLayer->m_TileMap->getMapSize().height * 64.0f - from.y;
    Vec2 to = tagPos;
    to.y = g_SceneManager->m_MainLayer->m_TileMap->getMapSize().height * 64.0f - to.y;
    if (g_astar.CanLinearMotion(from, to))
    {
        MsgObject_Move msg;
        msg.SysID = ID_C2G;
        msg.MoverID = m_GUID;
        msg.MoverType = m_Type;
        msg.MoverPosX = getPosition().x;
        msg.MoverPosY = getPosition().y;
        msg.TargetPosX = tagPos.x;
        msg.TargetPosY = tagPos.y;
        msg.PathNum = 0;
        g_NetManager->SendMsg(msg);
        return true;
    }
    else
    {
        Vec2 startPos = g_SceneManager->m_MainLayer->Pos2Tile(getPosition());
        Vec2 endPos = g_SceneManager->m_MainLayer->Pos2Tile(tagPos);
        std::vector<cocos2d::Vec2> paths;
        if (g_astar.FindPath(startPos.x,startPos.y, endPos.x,endPos.y, paths))
        {
            CCASSERT(!paths.empty(), "");
            MsgObject_Move msg;
            msg.SysID = ID_C2G;
            msg.MoverID = m_GUID;
            msg.MoverType = m_Type;
            msg.MoverPosX = getPosition().x;
            msg.MoverPosY = getPosition().y;
            msg.TargetPosX = tagPos.x;
            msg.TargetPosY = tagPos.y;
            msg.PathNum = paths.size();
            StackStream<> moveStream;
            moveStream.Wirte(&msg, sizeof(msg));    //>写入固定消息部分
            int posId;
            for (size_t i = 0; i < paths.size(); i++)
            {
                posId = g_SceneManager->m_MainLayer->Tile2TileId(paths[i]); //>写入变长消息部分
                moveStream.Wirte(&posId, sizeof(posId));
            }
            MsgObject_Move* pMsg = (MsgObject_Move*)moveStream.Get();
            pMsg->Len = moveStream.GetSize();   //>重新赋值长度
            CCASSERT(pMsg->Len == (sizeof(MsgObject_Move) + (pMsg->PathNum) * sizeof(int)), "");
            g_NetManager->SendMsg(*pMsg);
            return true;
        }
    }
    return false;
}


bool Hero::FindPathByDirection(DirectionType direction)
{
    if(IsDead())
        return false;

    Vec2 tempTile = g_SceneManager->m_MainLayer->Pos2Tile(getPosition());
    std::vector<cocos2d::Vec2> paths;
    for (int i = 0; i < 64; i++)
    {
        switch (direction)
        {
        case DIRECTION_UP:
            tempTile.y -= 1;
            break;
        case DIRECTION_RIGHT_UP:
            tempTile.x += 1;
            tempTile.y -= 1;
            break;
        case DIRECTION_RIGHT:
            tempTile.x += 1;
            break;
        case DIRECTION_RIGHT_DOWN:
            tempTile.x += 1;
            tempTile.y += 1;
            break;
        case DIRECTION_DOWN:
            tempTile.y += 1;
            break;
        case DIRECTION_LEFT_DOWN:
            tempTile.x -= 1;
            tempTile.y += 1;
            break;
        case DIRECTION_LEFT:
            tempTile.x -= 1;
            break;
        case DIRECTION_LEFT_UP:
            tempTile.x -= 1;
            tempTile.y -= 1;
            break;
        default:
            return false;
            break;
        }

        if (!g_astar.IsNoZD(tempTile.x, tempTile.y))
        {
            break;
        }
        paths.push_back(tempTile);		
    }

    if (paths.size() > 0)
    {
        std::reverse( std::begin(paths), std::end(paths) );

        MsgObject_Move msg;
        msg.SysID = ID_C2G;
        msg.MoverID = m_GUID;
        msg.MoverType = m_Type;
        msg.MoverPosX =getPosition().x;
        msg.MoverPosY =getPosition().y;
        Vec2 targetPos = g_SceneManager->m_MainLayer->Tile2Pos(*(paths.begin()));
        msg.TargetPosX = targetPos.x;
        msg.TargetPosY = targetPos.y;
        msg.PathNum = paths.size();
        StackStream<> moveStream;
        moveStream.Wirte(&msg, sizeof(msg));    //>写入固定消息部分
        int posId;
        for (size_t i = 0; i < paths.size(); i++)
        {
            posId = g_SceneManager->m_MainLayer->Tile2TileId(paths[i]); //>写入变长消息部分
            moveStream.Wirte(&posId, sizeof(posId));
        }
        MsgObject_Move* pMsg = (MsgObject_Move*)moveStream.Get();
        pMsg->Len = moveStream.GetSize();   //>重新赋值长度
        g_NetManager->SendMsg(*pMsg);

        return true;		
    }

    return false;
}

void Hero::StopMove()
{
    MsgObject_Move_Stop send;
    send.SysID = ID_C2G;
    send.MoverID = m_GUID;
    send.MoverType = m_Type;
    send.MoverPosX = getPosition().x;
    send.MoverPosY = getPosition().y;
    g_NetManager->SendMsg(send);	
}

void Hero::SetProperty(AttributeType property, float value)
{
	m_Attribute[property] = value;
	switch (property)
	{
	case ATTRIBUTE_MONEY:
		break;
	case ATTRIBUTE_GOLD:
		break;
	case ATTRIBUTE_HP:
		{
			if (m_Attribute[ATTRIBUTE_HP] <= 0.0f)
			{
				m_Attribute[ATTRIBUTE_HP] = 0.0f;
                Die();
			}
			else if (m_Attribute[ATTRIBUTE_HP] > m_Attribute[ATTRIBUTE_HPMAX])
			{
				m_Attribute[ATTRIBUTE_HP] = m_Attribute[ATTRIBUTE_HPMAX];
			}
			if (g_UIManager->GetUI(UI_MAIN))
			{
				((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeHero();
			}
            if (m_AnimCompoenet)
            {
                float percent = m_Attribute[ATTRIBUTE_HP] * 100.0f / m_Attribute[ATTRIBUTE_HPMAX];
                m_AnimCompoenet->SetHpBarPercent(percent);
            }
		}
		break;
	case ATTRIBUTE_MP:
		{
            if (g_UIManager->GetUI(UI_MAIN))
            {
                ((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeHero();
            }
		}
		break;
	case ATTRIBUTE_BINDGOLD:
		break;
	case ATTRIBUTE_ARENAMONEY:
		break;
	case ATTRIBUTE_BATTLEFIELDMONEY:
		break;
	case ATTRIBUTE_XPMAX:
		{
            if (g_UIManager->GetUI(UI_MAIN))
            {
                ((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeHero();
            }
		}
		break;
	case ATTRIBUTE_XP:
		{
            if (g_UIManager->GetUI(UI_MAIN))
            {
                ((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeHero();
            }
		}
		break;
	case ATTRIBUTE_LEVEL:
		{
			//m_Level = value;
			//播放特效，更新ui
			g_ResManager->ShowSprite(3, g_SceneManager->m_MainLayer, LAYER_EFFECT, g_ScreenHalfSize);
            if (g_UIManager->GetUI(UI_MAIN))
            {
			    ((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeHero();
            }
		}
		break;
	case ATTRIBUTE_EXP:
		{			
            if (g_UIManager->GetUI(UI_MAIN))
            {
                ((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeHero();
            }
		}
		break;
	case ATTRIBUTE_REALMLEVEL:
		{
			
			
		}
		break;	
	case ATTRIBUTE_REALMEXP:
		{

		}
		break;
	default:
		break;
	}
}


const SkillInfoIt* Hero::GetSkillByType(int32 type)
{
    auto it = m_SkillList.begin();
    for (auto end = m_SkillList.end(); it != end; ++it)
    {
        if ((*it).skillType == type)
        {
            return &(*it);
        }
    }

    return NULL;
}

void Hero::SetChoseSkill(int32 index, int32 skillId)
{
    if (0 <= index && index < MAX_CHOSESKILL)
    {
        int oldIndex = -1;
        for (int i = 0; i < MAX_CHOSESKILL; ++i)
        {
            if (m_ChoseSkillList[i] == skillId)
            {
                oldIndex = i;
                break;
            }
        }

        if (oldIndex >= 0)
        {
            m_ChoseSkillList[oldIndex] = m_ChoseSkillList[index];
        }

        m_ChoseSkillList[index] = skillId;
        //>发送消息
        MsgSkill_ChoseSkill choseMsg;
        choseMsg.setIndex = index;
        choseMsg.skillId = skillId;
        g_NetManager->SendMsg(choseMsg);
    }
}

bool Hero::CheckSkillCooling(int skillId)
{
    auto it = m_SkillCDMap.find(SKILL_ID_TO_TYPE(skillId));
    if (it != m_SkillCDMap.end())
    {
        if(it->second > 0.0f)
        {
            return false;
        }
    }
    return true;
}

float Hero::GetSkillLastCD(int skillId)
{
    auto it = m_SkillCDMap.find(SKILL_ID_TO_TYPE(skillId));
    if (it != m_SkillCDMap.end())
    {
        if(it->second > 0.0f)
        {
            return it->second;
        }
    }
    return 0.0f;
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
bool Hero::CheckOperInState(PlayerState_Operation _operate)
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
bool Hero::ChangeState(PlayerState_Operation _oper)
{
	//!这段代码用于捡查之前是否漏掉CheckOperInState 所有状态操作加完后可考虑删除！！！
	if( _oper >= Oper_Max )
	{
		log("状态检查有遗漏～操作ID为%d", _oper);
		return false;
	}
	for(int iS = 0;iS < State_Max;iS++)
	{		
		if(m_State[iS] && !g_CSVFileManager->m_StateMutex[iS][_oper])
		{
			log("状态检查有遗漏～操作ID为%d", _oper);
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
bool Hero::GetState(PlayerState_States _state)
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
void Hero::AddState(PlayerState_States _state)
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
void Hero::CancelState(PlayerState_States _state)
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

void Hero::LoadBaseAttribute(BaseInfoIt& baseInfo)
{
	m_Attribute[ATTRIBUTE_MONEY] = (float)baseInfo.money;
	m_Attribute[ATTRIBUTE_HP] = (float)baseInfo.hp;
    m_Attribute[ATTRIBUTE_HPMAX] = (float)baseInfo.hpMax;
	m_Attribute[ATTRIBUTE_MP] = (float)baseInfo.mp;
    m_Attribute[ATTRIBUTE_MPMAX] = (float)baseInfo.mpMax;
	m_Attribute[ATTRIBUTE_GOLD] = (float)baseInfo.gold;
	m_Attribute[ATTRIBUTE_BINDGOLD] = (float)baseInfo.bindGold;
	m_Attribute[ATTRIBUTE_ARENAMONEY] = (float)baseInfo.arenaMoney;
	m_Attribute[ATTRIBUTE_BATTLEFIELDMONEY] = (float)baseInfo.battlefieldMoney;
	m_Attribute[ATTRIBUTE_XP] = (float)baseInfo.xp;
    m_Attribute[ATTRIBUTE_XPMAX] = (float)baseInfo.xpMax;
	m_Attribute[ATTRIBUTE_LEVEL] = (float)baseInfo.level;
	m_Attribute[ATTRIBUTE_EXP] = (float)baseInfo.exp;
	m_Attribute[ATTRIBUTE_REALMLEVEL] = (float)baseInfo.realmLevel;
	m_Attribute[ATTRIBUTE_REALMEXP] = (float)baseInfo.realmExp;
	m_Name.assign(baseInfo.playerName);
	m_HeadIconIndex = baseInfo.headID;
	m_Sex = baseInfo.sex;
	m_OfflineTime = baseInfo.offlineTime;
	m_activeSkillType = baseInfo.activeSkillType;
	m_isocietyID = baseInfo.societyID;
}

void Hero::LoadDetailAttribute(DetailAttribute& otherInfo)
{
	m_Attribute[ATTRIBUTE_HPMAX] = otherInfo.hpMax;
	m_Attribute[ATTRIBUTE_MPMAX] = otherInfo.mpMax;
	m_Attribute[ATTRIBUTE_ATTACK] = otherInfo.attack;
	m_Attribute[ATTRIBUTE_DEFENSE] = otherInfo.defense;
	m_Attribute[ATTRIBUTE_HIT] = otherInfo.hit;
	m_Attribute[ATTRIBUTE_DODGE] = otherInfo.dodge;
	m_Attribute[ATTRIBUTE_CRIT] = otherInfo.crit;
    m_Attribute[ATTRIBUTE_TENACITY] = otherInfo.tenacity;
    m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR] = otherInfo.moveSpeedFactor;
	m_BattlePower = otherInfo.battlePower;
}

//根据taskid取得任务状态信息
TaskInfoIt* Hero::GetTaskInfoItByID(int32 id)
{
	if (id > 0)
	{
		Vector<TaskInfoIt>::iterator it;
		for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
		{
			if (it->id == id)
			{
				return &(*it);
			}
		}
	}
	return NULL;
}

float Hero::AC_GetMoveSpeed()
{
    return m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR];
}

float Hero::MC_GetMoveSpeed()
{
    return m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR];
}

void Hero::OnSkillBegin(int skillId, MapObj* target)
{
    const SkillInfo* pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
    assert(pSkillInfo);
    //>添加技能cd
    auto it = m_SkillCDMap.find(SKILL_ID_TO_TYPE(skillId));
    if (it != m_SkillCDMap.end())
    {
        it->second = pSkillInfo->CD;
    }
    else
    {
        if(pSkillInfo->CD > 0.0f)
            m_SkillCDMap.insert(std::pair<int32, float>(SKILL_ID_TO_TYPE(skillId), pSkillInfo->CD));
    }

    Player::OnSkillBegin(skillId, target);
}

bool Hero::IsTeamMember(uint64 guid)
{
    int count = m_team.m_Members.size();
    if (count > 0)
    {
        for (int i = 0; i < count; ++i)
        {
            if(m_team.m_Members[i].memberId == guid)
                return true;
        }
    }
    return false;
}

bool Hero::IsTeamLeader()
{
    int count = m_team.m_Members.size();
    if (count > 0)
    {
        for (int i = 0; i < count; ++i)
        {
            if(m_team.m_Members[i].attribute == Team::MA_Leaber)
            {
                if (m_team.m_Members[i].memberId == m_GUID)
                {
                    return true;
                }
                return false;
            }
        }
    }
    return false;
}
