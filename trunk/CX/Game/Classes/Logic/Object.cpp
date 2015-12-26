#include "Object.h"
#include "SceneManager.h"
#include "math/Vec2.h"
#include "AStar.h"
#include "AnimComponent.h"
#include "UIText.h"
#include "BaseFunction.h"


USING_NS_CC;

using namespace cc2dui;

MapObj::MapObj()
{
    m_GUID = INVALID_ID;
    m_Name = "";
    m_HeadIcon = "";
    m_Type = OBJECT_NONE;

    m_NameText = NULL;
    m_AnimCompoenet = NULL;
    m_MoveCompoenet = NULL;
    m_BattleCompoent = NULL;

    m_IsDead = false;
}

MapObj::~MapObj()
{

}

void MapObj::SetTile(const cc2d::Vec2 &pos)
{
    setPosition(g_SceneManager->m_MainLayer->Tile2Pos(pos));
    ApplyMapTileAttribute();
}

cc2d::Vec2 MapObj::GetTile()
{
    return g_SceneManager->m_MainLayer->Pos2Tile(getPosition());
}

void MapObj::SetMapPosition(const cc2d::Vec2 &position)
{
    setPosition(position);
    ApplyMapTileAttribute();
}

void MapObj::ApplyMapTileAttribute()
{
    const Vec2& position = getPosition();
    Vec2 currentTile = g_SceneManager->m_MainLayer->Pos2Tile(position);
    if (g_astar.m_MapMaskList[(int)currentTile.y][(int)currentTile.x])
    {
        if (m_AnimCompoenet)
            m_AnimCompoenet->SetOpacityWhenEnterMapTile(1);
    }
    else
    {
        if (m_AnimCompoenet)
            m_AnimCompoenet->SetOpacityWhenEnterMapTile(0);
    }
    setLocalZOrder(10000 - (int)position.y);
}

void MapObj::SetName(const std::string& name, float posY, const cc2d::Color3B& color)
{
    m_Name = name;

    if (m_NameText == NULL)
    {
        m_NameText = Text::create(BaseFunction::a2u(name.c_str()), "", 16);
        addChild(m_NameText, 10);
    }
    
    if (m_NameText)
    {
        m_NameText->setColor(color);
        m_NameText->enableShadow(Color4B(0, 0, 0, 255), Size(1,-1));
        m_NameText->setPosition(Vec2(0, posY));
    }
}

void MapObj::Idle()
{
    m_IsDead = false;
}

void MapObj::Die()
{
    m_IsDead = true;
}

