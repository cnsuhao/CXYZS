/********************************************************************
created:	2015/11/26
author:		lishihai
desc:		移动组件
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
    Vec2                m_MoveTarget;   //>移动目标
    float               m_MoveTime;     //>移动时间
    Vec2                m_Dir;          //>方向,单位化
    std::vector<int>    m_Paths;	    //>移动路径
    
};

#endif	//__MOVECOMPONET_H__
