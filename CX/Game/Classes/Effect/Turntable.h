#pragma once

/********************************************************************
created:	2015/9/23
author:		lishihai
desc:	    转盘类
*********************************************************************/

#include "math/vec2.h"
#include <functional>

//>可旋转的转盘
class Turntable
{
public:
    Turntable(void);
    ~Turntable(void);

    enum Mode
    {
        LeftUpCorner,       //>左上角.显示90度
        LeftDownCorner,     //>左下角.显示90度
        RightUpCorner,      //>右上角.显示90度
        RightDownCorner,    //>右上角.显示90度
        
        LeftBorder,         //>左边,显示180度
        RightBorder,        //>右边,显示180度
        TopBorder,          //>左边,显示180度
        BottomtBorder,      //>右边,显示180度

        CenterAny,          //>中间任意.显示360度
    };
    
    //>设置.
    void Setup(int numWidget, int showNumWidget, float radius, Mode mode);

    void DoClockwise(int numStep, float stepTime);
    void DoAnticlockwise(int numStep, float stepTime);

    //>做顺时针
    void DoClockwise(float stepTime); 
    //>做逆时针
    void DoAnticlockwise(float stepTime);
    //>更新
    void Update(float deltaTime);
public:
    //>设置初始化位置
    std::function<void(int widgetIndex, cocos2d::Vec2& newPosition)> OnInitSetPosition;
    //>设置更新的位置
    std::function<void(int widgetIndex, cocos2d::Vec2& newPosition)> OnUpdatePosition;
    //>设置显示
    std::function<void(int widgetIndex)> OnWillShow;
    //>设置隐藏
    std::function<void(int widgetIndex)> OnWillHide;
private:
    bool        m_IsSetup;
    int         m_NumWidget;
    int         m_ShowWidget;
    float       m_Radius;
    float       m_InteravlAng;
    Mode        m_Mode;
    int         m_Step;
    float       m_StepTime;
    float       m_Time;
    bool        m_Clockwise;
    bool        m_Anim;
    cocos2d::Vec2 m_StartPos;
    float       m_StartAng;
    int         m_StepCount;

    struct MyStruct
    {
        cocos2d::Vec2   positions;
        float           angle;
        bool            hide;
    };
    MyStruct*   m_Data;
};

