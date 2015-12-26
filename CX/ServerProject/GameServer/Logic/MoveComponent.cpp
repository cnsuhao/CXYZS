#include "MoveComponent.h"
#include "Object.h"
#include "Map.h"
#include "LogManage.h"
#include "Player.h"

MoveComponent::MoveComponent(Object* woner, Interface* listener)
{
    m_Woner = woner;
    m_Interface = listener;
}

bool MoveComponent::MoveByTarget(const Vec2& moveTarget)
{
    const Vec2& myPosition = m_Woner->GetPosition();
    float distance = myPosition.distance(moveTarget);
    if (IsFloatZero(distance))
        return false;

    StopMove();
    m_MoveTarget = moveTarget;
    m_MoveTime = distance / (UNIT_MOVE_SPEED * m_Woner->m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].GetValueF());
    m_Dir = m_MoveTarget - myPosition;
    m_Dir.Normalize();

    if (m_Interface)
    {
        m_Interface->OnMoveByTarget(moveTarget);
    }

    return true;
}

bool MoveComponent::MoveByPath(std::vector<int>& paths, const Vec2& moveTarget)
{
    if (paths.empty())
        return false;

    const Vec2& myPosition = m_Woner->GetPosition();
    int nextTileId = paths[paths.size() - 1];
    Vec2 nextPosition = m_Woner->m_Map->TileId2Pos(nextTileId);
    if (m_Paths.size() == 1) { nextPosition = moveTarget; }
    float distance = myPosition.distance(nextPosition);
    if (IsFloatZero(distance))
        return false;

    StopMove();
    m_Paths.resize(paths.size());
    std::copy(paths.begin(), paths.end(), m_Paths.begin());
    m_MoveTarget = moveTarget;
    m_MoveTime = distance / (UNIT_MOVE_SPEED * m_Woner->m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].GetValueF());
    m_Dir = nextPosition - myPosition;
    m_Dir.Normalize();
    gLog.Info("MoveBegin===>nextPosition(%f,%f,id=%d), m_position(%f,%f), Dir(%f,%f)",nextPosition.x, nextPosition.y, nextTileId, myPosition.x, myPosition.y, m_Dir.x, m_Dir.y);

    if (m_Interface)
    {
        m_Interface->OnMoveByPath(paths, moveTarget);
    }

    return true;
}

void MoveComponent::OnMoveEnd()
{
    if (m_Interface)
    {
        m_Interface->OnMoveEnd();
    }
}

void MoveComponent::StopMove()
{
    m_MoveTime = 0.0f;
    m_Paths.clear();
}

void MoveComponent::Update(float deltaTime)
{
    if (m_MoveTime > 0.0f)
    {
        m_Dir.Normalize();

        Vec2 newPosition;
        float speed = m_Woner->m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].GetValueF();
        const Vec2& myPosition = m_Woner->GetPosition();
        newPosition.x = myPosition.x + m_Dir.x * UNIT_MOVE_SPEED * speed * deltaTime;
        newPosition.y = myPosition.y + m_Dir.y * UNIT_MOVE_SPEED * speed * deltaTime;

        m_Woner->m_Map->ObjectChangePosition(m_Woner, newPosition);

        m_MoveTime -= deltaTime;
        if (m_MoveTime <= 0.0f) 
        {
            if(!m_Paths.empty())
                m_Paths.pop_back();

            const Vec2& myPosition = m_Woner->GetPosition();
            if (m_Paths.size() > 0)
            {
                int nextTileId = m_Paths[m_Paths.size() - 1];
                Vec2 nextPosition = m_Woner->m_Map->TileId2Pos(nextTileId);
                if (m_Paths.size() == 1) { nextPosition = m_MoveTarget; }
                float distance = myPosition.distance(nextPosition);
                m_MoveTime = distance / (UNIT_MOVE_SPEED * speed);
                m_Dir = nextPosition - myPosition;
                m_Dir.Normalize();

                gLog.Info("===>nextPosition(%f,%f), m_position(%f,%f), Dir(%f,%f)",nextPosition.x, nextPosition.y, myPosition.x, myPosition.y, m_Dir.x, m_Dir.y);
            }
            else
            {
                m_Woner->m_Map->ObjectChangePosition(m_Woner, m_MoveTarget);
                OnMoveEnd();
            }
        }
    }
}
