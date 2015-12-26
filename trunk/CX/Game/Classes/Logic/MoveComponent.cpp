#include "MoveComponent.h"
#include "SceneManager.h"
#include "AStar.h"
#include "CCActionInterval.h"
#include "CCActionInstant.h"
#include "CCActionNode.h"
#include "Object.h"

USING_NS_CC;

MoveComponent::MoveComponent(MapObj* Owner, Interface* pInterfece):m_Owner(Owner),m_Interfece(pInterfece)
{
    m_LineMove = false;
}

MoveComponent::~MoveComponent()
{

}

void MoveComponent::MoveByTarget(const cc2d::Vec2 &tagPos)
{
    InnerMoveEnd();
    m_MoveTarget = tagPos;
    m_LineMove = true;
    if (m_Interfece)
    {
        m_Interfece->OnMoveByTarget(tagPos);
    }
    MoveNext();
}


void MoveComponent::MoveByPath(std::vector<int>& paths, const cc2d::Vec2 &tagPos)
{
    InnerMoveEnd();
    m_MoveTarget = tagPos;
    int count = paths.size();
    for (int i = 0; i < count; i++)
    {
        m_Paths.push_back(g_SceneManager->m_MainLayer->TileId2Tile(paths[i]));           
    }
    if (m_Interfece)
    {
        m_Interfece->OnMoveByPath(m_Paths, tagPos);
    }
    MoveNext();
}

void MoveComponent::MoveNext()
{
    if (m_LineMove && m_Interfece && m_Owner)
    {
        Vec2 start = m_Owner->getPosition();
        Vec2 distance = start - m_MoveTarget;
        float moveSpeed = m_Interfece->MC_GetMoveSpeed();

#define cos22dot5 0.92387f      // cos( 22.5')
#define cos67dot5 0.38268f      // cos( 67.5')
#define cos112dot5 -0.38268f    // cos(112.5')
#define cos157dot5 -0.92387f    // cos(157.5')
        static const Vec2 vleft = Vec2(1.0f, 0.0f);
        Vec2 pn = m_MoveTarget - start;
        pn.normalize();
        float dot = vleft.dot(pn);
        if (dot > cos22dot5) // cos(22.5');
        {
            m_Interfece->OnChangeDirection(DIRECTION_RIGHT);
        }
        else if (dot > cos67dot5) // cos(67.5')
        {
            if (pn.y > 0.0f)
            {
                m_Interfece->OnChangeDirection(DIRECTION_RIGHT_UP);
            }else
            {
                m_Interfece->OnChangeDirection(DIRECTION_RIGHT_DOWN);
            }
        }else if (dot > cos112dot5) // cos(112.5')
        {
            if (pn.y > 0.0f)
            {
                m_Interfece->OnChangeDirection(DIRECTION_UP);
            }else
            {
                m_Interfece->OnChangeDirection(DIRECTION_DOWN);
            }
        }else if (dot > cos157dot5) // cos(157.5')
        {
            if (pn.y > 0.0f)
            {
                m_Interfece->OnChangeDirection(DIRECTION_LEFT_UP);
            }else
            {
                m_Interfece->OnChangeDirection(DIRECTION_LEFT_DOWN);
            }
        }else
        {
            m_Interfece->OnChangeDirection(DIRECTION_LEFT);
        }

        float time = distance.length() / (UNIT_MOVE_SPEED * moveSpeed);
        auto moveto = MoveTo::create(time, m_MoveTarget);
        m_Owner->runAction(Sequence::create(moveto, CallFunc::create(std::bind(&MoveComponent::MoveNextCallBack,this)), nullptr));
        m_LineMove = false;
        return;
    }

    if (m_Paths.size() > 0)
    {
        float moveSpeed = m_Interfece->MC_GetMoveSpeed();
        const Vec2& position = m_Owner->getPosition();
        Vec2 startTile = g_SceneManager->m_MainLayer->Pos2Tile(position);
        Vec2 tagTile = m_Paths[m_Paths.size()-1];

        m_Owner->setLocalZOrder(10000 - (int)position.y);
        if (startTile.x > tagTile.x)
        {
            if (startTile.y > tagTile.y)
            {
                m_Interfece->OnChangeDirection(DIRECTION_LEFT_UP);
            }
            else if (startTile.y < tagTile.y)
            {
                m_Interfece->OnChangeDirection(DIRECTION_LEFT_DOWN);
            } 
            else
            {
                m_Interfece->OnChangeDirection(DIRECTION_LEFT);
            }
        }
        else if (startTile.x < tagTile.x)
        {
            if (startTile.y > tagTile.y)
            {
                m_Interfece->OnChangeDirection(DIRECTION_RIGHT_UP);
            }
            else if (startTile.y < tagTile.y)
            {
                m_Interfece->OnChangeDirection(DIRECTION_RIGHT_DOWN);
            } 
            else
            {
                m_Interfece->OnChangeDirection(DIRECTION_RIGHT);
            }
        } 
        else
        {
            if (startTile.y > tagTile.y)
            {
                m_Interfece->OnChangeDirection(DIRECTION_UP);
            }
            else if (startTile.y < tagTile.y)
            {
                m_Interfece->OnChangeDirection(DIRECTION_DOWN);
            } 
        }

        //ÒÆ¶¯Ê±¼ä
        Vec2 target = g_SceneManager->m_MainLayer->Tile2Pos(tagTile);
        log("next target(%f,%f, id=%d)", target.x, target.y, g_SceneManager->m_MainLayer->Tile2TileId(tagTile));
        if (m_Paths.size() == 1) 
        {
            log("m_MoveTarget(%f,%f)", m_MoveTarget.x, m_MoveTarget.y);
            target = m_MoveTarget; 
        }
        Vec2 distance = position - target;
        float time = distance.length() / (UNIT_MOVE_SPEED * moveSpeed);
        auto moveto = MoveTo::create(time, target);
        m_Owner->runAction(Sequence::create(moveto, CallFunc::create(std::bind(&MoveComponent::MoveNextCallBack,this)), nullptr));
        m_Paths.pop_back();
    }
    else
    {
        MoveEnd();
    }
}

void MoveComponent::MoveNextCallBack()
{
    m_Owner->ApplyMapTileAttribute();

    if (m_Paths.size() > 0)
    {
        MoveNext();
    }
    else
    {
        MoveEnd();
    }
}

void MoveComponent::MoveEnd()
{
    InnerMoveEnd();

    if (m_Interfece)
    {
        m_Interfece->OnMoveEnd();
    }
}


void MoveComponent::MoveStop()
{
    InnerMoveEnd();
}

void MoveComponent::InnerMoveEnd()
{
    m_Paths.clear();
    m_LineMove = false;
    m_Owner->stopAllActions();
}
