#pragma once

/********************************************************************
created:	2015/9/23
author:		lishihai
desc:	    ת����
*********************************************************************/

#include "math/vec2.h"
#include <functional>

//>����ת��ת��
class Turntable
{
public:
    Turntable(void);
    ~Turntable(void);

    enum Mode
    {
        LeftUpCorner,       //>���Ͻ�.��ʾ90��
        LeftDownCorner,     //>���½�.��ʾ90��
        RightUpCorner,      //>���Ͻ�.��ʾ90��
        RightDownCorner,    //>���Ͻ�.��ʾ90��
        
        LeftBorder,         //>���,��ʾ180��
        RightBorder,        //>�ұ�,��ʾ180��
        TopBorder,          //>���,��ʾ180��
        BottomtBorder,      //>�ұ�,��ʾ180��

        CenterAny,          //>�м�����.��ʾ360��
    };
    
    //>����.
    void Setup(int numWidget, int showNumWidget, float radius, Mode mode);

    void DoClockwise(int numStep, float stepTime);
    void DoAnticlockwise(int numStep, float stepTime);

    //>��˳ʱ��
    void DoClockwise(float stepTime); 
    //>����ʱ��
    void DoAnticlockwise(float stepTime);
    //>����
    void Update(float deltaTime);
public:
    //>���ó�ʼ��λ��
    std::function<void(int widgetIndex, cocos2d::Vec2& newPosition)> OnInitSetPosition;
    //>���ø��µ�λ��
    std::function<void(int widgetIndex, cocos2d::Vec2& newPosition)> OnUpdatePosition;
    //>������ʾ
    std::function<void(int widgetIndex)> OnWillShow;
    //>��������
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

