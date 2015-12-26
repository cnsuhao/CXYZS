/********************************************************************
created:	2015/11/30
author:		lishihai
desc:		Object��
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

    void SetTile(const cc2d::Vec2 &pos);			    //>���ø���λ��
    cc2d::Vec2 GetTile();                               //>��ø���λ��
    void SetMapPosition(const cc2d::Vec2 &position);    //>���õ�ͼ��λ��
    void ApplyMapTileAttribute();                       //>��Ӧ��������

    inline void  SetHeadIcon(const string& Icon);          
    inline const string& GetHeadIcon() const;           //>���ͷ��
    void SetName(const std::string& name, float posY, const cc2d::Color3B& color);  //>��������,�߶�,��ɫ
    inline bool IsDead() const;                         //>�Ƿ�����

    virtual void Idle();							    //>����
    virtual void Die();						            //>����

    class AnimComponent* m_AnimCompoenet;               //>�������
    class MoveComponent* m_MoveCompoenet;               //>�ƶ����
    class BattleComponent* m_BattleCompoent;            //>ս�����
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
