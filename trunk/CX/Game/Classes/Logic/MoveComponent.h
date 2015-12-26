/********************************************************************
created:	2015/11/30
author:		lishihai
desc:		�ƶ���
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


    void MoveByTarget(const cc2d::Vec2 &tagPos);	//Ŀ���ƶ�
    void MoveByPath(std::vector<int>& paths, const cc2d::Vec2 &tagPos); //·���ƶ�
    void MoveNext();								//�ƶ���
    void MoveNextCallBack();                        //�ƶ��лص�
    void MoveEnd();			                        //�ƶ�����
    void MoveStop();                                //ֹͣ�ƶ�                        
protected:
    void InnerMoveEnd();

    MapObj* m_Owner;
    Interface*  m_Interfece;    //�ӿ�
    std::vector<cc2d::Vec2> m_Paths;	//�ƶ�·��
    cc2d::Vec2        m_MoveTarget;
    bool              m_LineMove;        
};

#endif	//__MOVECOMPOENT_H__
