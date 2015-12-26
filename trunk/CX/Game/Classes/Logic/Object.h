/********************************************************************
created:	2015/11/30
author:		lishihai
desc:		Object类
*********************************************************************/

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "ClientDef.h"

namespace cocos2d{ namespace ui {  class Text; } }

class MapObj : public cc2d::Node
{
public:
	MapObj();
	~MapObj();

	CREATE_FUNC(MapObj);

	bool Init(int ObjId);

    void SetTile(const cc2d::Vec2 &pos);			    //>设置格子位置
    cc2d::Vec2 GetTile();                               //>获得格子位置
    void SetMapPosition(const cc2d::Vec2 &position);    //>设置地图上位置
    void ApplyMapTileAttribute();                       //>适应格子属性

    inline void  SetHeadIcon(const string& Icon);          
    inline const string& GetHeadIcon() const;           //>获得头像
    void SetName(const std::string& name, float posY, const cc2d::Color3B& color);  //>设置名字,高度,颜色
    inline bool IsDead() const;                         //>是否死亡

    virtual void Idle();							    //>待机
    virtual void Die();						            //>死亡

    class AnimComponent* m_AnimCompoenet;               //>动作组件
    class MoveComponent* m_MoveCompoenet;               //>移动组件
    class BattleComponent* m_BattleCompoent;            //>战斗组件
private:  
    void setPosition(float x, float y) { Node::setPosition(x, y); }
    void setPosition(const cc2d::Vec2 &p)  { Node::setPosition(p); }

public:
    int64       m_GUID;
    ObjectType  m_Type;
    std::string m_Name;
    std::string m_HeadIcon;
    bool        m_IsDead;
    cc2dui::Text* m_NameText;
private:
    virtual void setName(const std::string& name) { Node::setName(name); }
};

inline const string& MapObj::GetHeadIcon() const
{
    return m_HeadIcon;
}

inline void MapObj::SetHeadIcon(const string& Icon)
{
    m_HeadIcon = Icon;
}

inline bool MapObj::IsDead() const
{
    return m_IsDead;
}

#endif	//__OBJECT_H__
