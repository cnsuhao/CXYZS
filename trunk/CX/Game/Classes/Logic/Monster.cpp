#include "Monster.h"

USING_NS_CC;
using namespace cc2dui;

#include "LogicManager.h"
#include "ResManager.h"
#include "CSVFileManager.h"
#include "BaseFunction.h"
#include "CCActionInstant.h"


Monster::Monster():m_MonsterInfo(NULL)
{
}

Monster::~Monster()
{
    delete m_AnimCompoenet;
    delete m_MoveCompoenet;
}

bool Monster::Init(int objid)
{	
	m_Type = OBJECT_MONSTER;
	m_MonsterInfo = g_CSVFileManager->GetMonsterByID(objid);
    assert(m_MonsterInfo);

	if (m_MonsterInfo)
	{		
		//查找血量属性
		for (int i = 0 ; i < Monter_Att_Num ; i++)
		{
			if(m_MonsterInfo->iAtt[i] == ATTRIBUTE_HPMAX)
			{
				//InitHP(m_MonsterInfo->iValue[i]);
			}
		}
		
		m_HeadIcon = m_MonsterInfo->strHeadIcon;
		g_ResManager->CreatePlayerPartAsync(m_MonsterInfo->strModleid);


        NpcAnimComp* pAmimComp = new NpcAnimComp(this, this);
        assert(pAmimComp);
        if (pAmimComp)
        {
            m_AnimCompoenet = pAmimComp;
            pAmimComp->LoadResouce(true);
            cc2d::Node* pBody = pAmimComp->GetBody();
            if (pBody)
            {
                pBody->setAnchorPoint(Vec2(m_MonsterInfo->fAnchorX, m_MonsterInfo->fAnchorY));
                pBody->setScale(m_MonsterInfo->fScale);
            }

            pAmimComp->ChangeDirection((DirectionType)BaseFunction::Random(0, DIRECTION_MAX - 1));
            Idle();
            pAmimComp->EnableShadow(true);
        }

        m_MoveCompoenet = new MoveComponent(this, this);
        
        const NameColor *nameColor = g_CSVFileManager->GetNameColorByID(6);
        assert(nameColor);
        SetName(m_MonsterInfo->strName, (float)m_MonsterInfo->iNameHeight, Color3B(nameColor->iR, nameColor->iG, nameColor->iB));

        // 注册模型响应触摸事件
        schedule(CC_SCHEDULE_SELECTOR(Monster::Update), 0.25f);
        return true;   
	}
    return false;
}

void Monster::Update(float delta)
{
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->Update(delta);
    }
}

const char* Monster::GetModelName()
{
    return m_MonsterInfo->strModleid.c_str();
}

void Monster::OnAttcakAnimEnd()
{
    if (m_AnimCompoenet)
    {
        if (!IsDead())
        {
            m_AnimCompoenet->ChangeAction(ACTION_IDLE);
        }
    }
}

float Monster::MC_GetMoveSpeed()
{
    return 1.0f;
}

void Monster::OnChangeDirection(DirectionType direction)
{
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->ChangeDirection(direction);
    }
}

void Monster::OnMoveByTarget(const cc2d::Vec2 &tagPos)
{
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->ChangeAction(ACTION_MOVE);
    }
}

void Monster::OnMoveByPath(std::vector<cc2d::Vec2>& paths, const cc2d::Vec2 &tagPos)
{
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->ChangeAction(ACTION_MOVE);
    }
}

void Monster::OnMoveEnd()
{
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->ChangeAction(ACTION_IDLE);
    }
}

void Monster::Idle()
{
    MapObj::Idle();
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->ChangeAction(ACTION_IDLE);
    }
}

void Monster::Die()
{
    MapObj::Die();
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->ChangeAction(ACTION_DEAD);
    }
}

