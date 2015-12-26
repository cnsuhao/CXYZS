#pragma once

/********************************************************************
created:	2015/10/15
author:		lishihai
desc:	    ����ʶ��
*********************************************************************/

#include "ClientDef.h"
#include <functional>

//>����ʶ��
class GestureDetector
{
public:
    GestureDetector(void);
    ~GestureDetector(void);

    enum MoveType
    {
        None,                   //>����ʶ��
        TopToBottom,            //>���ϵ��»���
        BottomToTop,            //>���µ��ϻ���
        LeftToRight,            //>�����һ���
        RightToLeft,            //>���ҵ��󻬶�
        LeftTopToRightDown,     //>�����ϵ����»���
        RightDownToLeftTop,     //>�����µ����ϻ���
        RightTopToLeftDown,     //>�����ϵ����»���
        LeftDownToRightTop,     //>�����µ����ϻ���
    };

    inline void SetSize(const cocos2d::Vec2& size) { m_TouchSize = size; }

    void BeginTouch(cocos2d::Vec2& point);

    void EndTouch(cocos2d::Vec2& point);

public:
    std::function<void(GestureDetector::MoveType moveType)> OnDetectReslut;

private:
    cocos2d::Vec2   m_TouchSize;
    cocos2d::Vec2   m_BeginPoint;
    cocos2d::Vec2   m_EndPoint;
};

