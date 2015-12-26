#pragma once

/********************************************************************
created:	2015/10/15
author:		lishihai
desc:	    手势识别
*********************************************************************/

#include "ClientDef.h"
#include <functional>

//>手势识别
class GestureDetector
{
public:
    GestureDetector(void);
    ~GestureDetector(void);

    enum MoveType
    {
        None,                   //>不能识别
        TopToBottom,            //>从上到下滑动
        BottomToTop,            //>从下到上滑动
        LeftToRight,            //>从左到右滑动
        RightToLeft,            //>从右到左滑动
        LeftTopToRightDown,     //>从左上到右下滑动
        RightDownToLeftTop,     //>从右下到左上滑动
        RightTopToLeftDown,     //>从右上到左下滑动
        LeftDownToRightTop,     //>从左下到右上滑动
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

