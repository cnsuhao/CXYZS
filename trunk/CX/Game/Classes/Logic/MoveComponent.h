/********************************************************************
created:	2015/11/30
author:		lishihai
desc:		移动类
*********************************************************************/

#ifndef __MOVECOMPOENT_H__
#define __MOVECOMPOENT_H__

#include "ClientDef.h"

#include "math/Vec2.h"

namespace cc2d { class Sprite; }
class MapObj;

class MoveComponent
{
public:
    class Interface
    {
    public:
        virtual void OnChangeDirection(DirectionType direction) = 0;
        virtual void OnMoveByTarget(const cc2d::Vec2 &tagPos) = 0;
        virtual void OnMoveByPath(std::vector<cc2d::Vec2>& paths, const cc2d::Vec2 &tagPos) = 0;
        virtual void OnMoveEnd() = 0;
        virtual float MC_GetMoveSpeed() = 0;
    };
public:
	MoveComponent(MapObj* Owner, Interface*  pInterfece);
	virtual ~MoveComponent();


    void MoveByTarget(const cc2d::Vec2 &tagPos);	//目标移动
    void MoveByPath(std::vector<int>& paths, const cc2d::Vec2 &tagPos); //路径移动
    void MoveNext();								//移动中
    void MoveNextCallBack();                        //移动中回调
    void MoveEnd();			                        //移动结束
    void MoveStop();                                //停止移动                        
protected:
    void InnerMoveEnd();

    MapObj* m_Owner;
    Interface*  m_Interfece;    //接口
    std::vector<cc2d::Vec2> m_Paths;	//移动路径
    cc2d::Vec2        m_MoveTarget;
    bool              m_LineMove;        
};

#endif	//__MOVECOMPOENT_H__
