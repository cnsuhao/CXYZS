/********************************************************************
created:	2015/11/26
author:		lishihai
desc:		�ƶ����
*********************************************************************/

#ifndef __MOVECOMPONET_H__
#define __MOVECOMPONET_H__

#include "Vec2.h"
#include <vector>

class Object;
class MoveComponent
{
public:
    class Interface
    {
    public:
        virtual void    OnMoveByTarget(const Vec2& moveTarget) {}
        virtual void    OnMoveByPath(std::vector<int>& paths, const Vec2& moveTarget) {}
        virtual void    OnMoveEnd() {}
    };
public:
    MoveComponent(Object* woner, Interface* listener = NULL);


    bool    MoveByTarget(const Vec2& moveTarget);
    bool    MoveByPath(std::vector<int>& paths, const Vec2& moveTarget);
    void    StopMove();

    const Vec2& GetTarget() { return m_MoveTarget; }
    const std::vector<int>& GetPaths() { return m_Paths; }

    void    Update(float dt);
private:
    void    OnMoveEnd();

    Object*             m_Woner;
    Interface*          m_Interface;
    Vec2                m_MoveTarget;   //>�ƶ�Ŀ��
    float               m_MoveTime;     //>�ƶ�ʱ��
    Vec2                m_Dir;          //>����,��λ��
    std::vector<int>    m_Paths;	    //>�ƶ�·��
    
};

#endif	//__MOVECOMPONET_H__
