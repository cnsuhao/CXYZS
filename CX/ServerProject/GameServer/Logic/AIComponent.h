/********************************************************************
created:	2015/11/26
author:		lishihai
desc:		AI组件
*********************************************************************/

#ifndef __AICOMPONET_H__
#define __AICOMPONET_H__

#include "GameDef.h"
#include "Vec2.h"
#include <unordered_map>

class Object;
class AIComponent
{
public:
    class Interface
    {
    public:
        virtual void    OnBeHurt(int64 attacker, int damage) {}
    };
public:
    AIComponent(Object* woner, Interface* interfece);


    void ClearGrudge();     //>清除仇恨
private:
    Object*             m_Woner;
    Interface*          m_Interfece;

    std::unordered_map<int64, int32> m_GrudgeTable; //>仇恨列表
};

#endif	//__AICOMPONET_H__
