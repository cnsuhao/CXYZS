#include "Monster.h"
#include "CSVFileManager.h"
#include "MapManager.h"
#include "Map.h"
#include "LogManage.h"
#include "GameServer.h"

Object::Object()
{
    m_GUID = INVALID_ID;
    m_Map = NULL;
    m_IsDead = false;

    m_BattleComp = NULL;
    m_MoveComp = NULL;
    m_AIComponet = NULL;
}

Object::~Object()
{

}

void Object::Update(float dt)
{
    
}

const Vec2& Object::GetTile()
{
    if (m_Map)
    {
        return m_Map->Pos2Tile(m_Position);
    }
    return Vec2::ZERO;
}

bool Object::AddHp(const AddHpMode& mode)
{
    if (mode.hp > 0.0f)
    {
        if (m_Attribute[ATTRIBUTE_HP].GetBase() >= m_Attribute[ATTRIBUTE_HPMAX].GetValueF())
            return false;

        m_Attribute[ATTRIBUTE_HP].AddBase((float)mode.hp);
        if (m_Attribute[ATTRIBUTE_HP].GetBase() > m_Attribute[ATTRIBUTE_HPMAX].GetValueF())
        {
            m_Attribute[ATTRIBUTE_HP].SetBase(m_Attribute[ATTRIBUTE_HPMAX].GetValueF());
        }
        return true;
    }
    return false;
}

bool Object::DropHP(const DropHpMode& mode)
{
    if(IsDead())
        return false;

    if (mode.hp > 0.0f && DropHpMode::Dodge != mode.mode && DropHpMode::WuDiMianYiBuff != mode.mode)
    {
        m_Attribute[ATTRIBUTE_HP].AddBase(mode.hp * -1.0f);
        gLog.Debug("Obj[%llu] dorp hp[%d] mode[%d]", m_GUID, mode.hp, mode.mode);
        if (m_Attribute[ATTRIBUTE_HP].GetBase() <= 0)
        {
            m_Attribute[ATTRIBUTE_HP].SetBase(0);
            Die();
        }

        return true;
    }

    return false;
}

void Object::Idle()
{
    m_IsDead = false; 
}

void Object::Die()
{
    m_IsDead = true;
}

void Object::BroadcastMsg(MsgBase* msg, bool isBroadcastSelf /*= true*/)
{
    std::vector<Object*> viewObjSet;
    m_Map->GetRangeSet(PLAYER_VIEW_RANGE, this, viewObjSet);

    std::vector<Object*>::iterator it = viewObjSet.begin();
    for (std::vector<Object*>::iterator end = viewObjSet.end(); 
        it != end; ++it)
    {
        if ((*it)->m_Type == OBJECT_PLAYER)
        {
            msg->OBJID = (*it)->m_GUID;	
            g_GameServer->SendToClient(msg);
        }
    }
}
