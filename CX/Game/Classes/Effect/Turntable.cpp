#include "Turntable.h"
#include "ccMacros.h"
#include "BaseFunction.h"

using namespace cocos2d;

Turntable::Turntable(void)
{
    m_IsSetup = false;
    OnInitSetPosition = nullptr;
    OnUpdatePosition = nullptr;
    OnWillShow = nullptr;
    OnWillHide = nullptr;
    m_Data = nullptr;
    m_Step = 0;
    m_StepTime = 0;
    m_Time = 0;
    m_Clockwise = false;
    m_Anim = false;
    m_StartAng = 0;
}


Turntable::~Turntable(void)
{
    if (m_Data)
        delete[] m_Data;
}

void Turntable::Setup(int numWidget, int showNumWidget, float radius, Mode mode)
{
    if (numWidget <= 0 || showNumWidget > numWidget)
        return;

    m_NumWidget = numWidget;
    m_ShowWidget = showNumWidget;
    m_Radius = radius;
    m_Mode = mode;
    m_Step = 0;

    if (m_Data)
        delete[] m_Data;
    m_Data = new MyStruct[numWidget];

    Vec2 startPos;
    switch (m_Mode)
    {
    case Turntable::LeftUpCorner:
        startPos.x = 0.0f;
        startPos.y = -radius;
        m_StartAng = 90.0f;
        break;
    case Turntable::LeftDownCorner:
        startPos.x = radius;
        startPos.y = 0.0f;
        m_StartAng = 90.0f;
        break;
    case Turntable::RightUpCorner:
        startPos.x = -radius;
        startPos.y = 0.0f;
        m_StartAng = 90.0f;
        break;
    case Turntable::RightDownCorner:
        startPos.x = 0.0f;
        startPos.y = radius;
        m_StartAng = 90.0f;
        break;
    case Turntable::LeftBorder:
        startPos.x = 0.0f;
        startPos.y = -radius;
        m_StartAng = 180.0f;
        break;
    case Turntable::RightBorder:
        startPos.x = 0.0f;
        startPos.y = radius;
        m_StartAng = 180.0f;
        break;
    case Turntable::TopBorder:
        startPos.x = -radius;
        startPos.y = 0.0f;
        m_StartAng = 180.0f;
        break;
    case Turntable::BottomtBorder:
        startPos.x = radius;
        startPos.y = 0.0f;
        m_StartAng = 180.0f;
        break;
    case Turntable::CenterAny:
        startPos.x = 0.0f;
        startPos.y = radius;
        m_StartAng = (360.0f / m_ShowWidget) * (m_ShowWidget - 1.0f);
        break;
    default:
        CCASSERT(0, "");
        break;
    }

    for (int i = 0; i < numWidget; ++i)
    {
        if (i < m_ShowWidget)
        {
            if (OnWillShow != nullptr)
                OnWillShow(i);

            m_Data[i].hide = false;
        }
        else
        {
            if(OnWillHide != nullptr)
                OnWillHide(i);

            m_Data[i].hide = true;
        }
    }

    m_InteravlAng = M_PI/180.0f * (m_StartAng / (m_ShowWidget - 1.0f));
    Vec2 newPos;
    for (int i = m_ShowWidget - 1, j = 0; i >= 0; --i, ++j)
    {
        float theta = m_InteravlAng * i;
        newPos.x = startPos.x * cosf(theta) - startPos.y * sinf(theta);
        newPos.y = startPos.y * cosf(theta) + startPos.x * sinf(theta);
        
        m_Data[j].positions = newPos;
        m_Data[j].angle = theta;
        if (i == 0){
            m_StartPos = newPos;
            m_StartAng = theta;
        }

        if (OnInitSetPosition != nullptr)
        {
            OnInitSetPosition(j, newPos);
        }
    }

    m_IsSetup = true;
}

void Turntable::DoClockwise(int numStep, float stepTime)
{
    if (numStep > 0)
    {
        m_StepCount = numStep;
        DoClockwise(stepTime / (m_StepCount * 1.0f));
        --m_StepCount;
    }
}

void Turntable::DoAnticlockwise(int numStep, float stepTime)
{
    if (numStep > 0)
    {
        m_StepCount = numStep;
        DoAnticlockwise(stepTime / (m_StepCount * 1.0f));
        --m_StepCount;
    }
}

void Turntable::DoClockwise(float stepTime)
{
    if (m_IsSetup == false)
        return;
    if (m_Anim)
        return;
    m_StepTime = stepTime;
    m_Time = stepTime;
    m_Clockwise = true;
    m_Anim = false;
}

void Turntable::DoAnticlockwise(float stepTime)
{
    if (m_IsSetup == false)
        return;
    if (m_Anim)
        return;
    m_StepTime = stepTime;
    m_Time = stepTime;
    m_Clockwise = false;
    m_Anim = false;
}

void Turntable::Update(float deltaTime)
{
    if (m_Time > 0)
    {
        if (m_Anim == false)
        {
            if (m_Clockwise)
            {
                int idx = (m_Step + m_ShowWidget) % m_NumWidget;

                if (OnWillShow)
                    OnWillShow(idx);

                MyStruct& data = m_Data[idx];
                data.hide = false;
                data.angle = m_StartAng - m_InteravlAng;
                data.positions.x = m_StartPos.x * cosf(data.angle) - m_StartPos.y * sinf(data.angle);
                data.positions.y = m_StartPos.y * cosf(data.angle) + m_StartPos.y * sinf(data.angle);

                if(OnUpdatePosition){
                    OnUpdatePosition(idx, data.positions);
                }
            }
            else
            {
                m_Step = (--m_Step +  m_NumWidget) % m_NumWidget;

                if (OnWillShow){
                    OnWillShow(m_Step);
                }

                int idx = m_Step;
                MyStruct& data = m_Data[idx];
                data.hide = false;
                data.angle = m_StartAng + m_InteravlAng * m_ShowWidget;
                data.positions.x = m_StartPos.x * cosf(data.angle) - m_StartPos.y * sinf(data.angle);
                data.positions.y = m_StartPos.y * cosf(data.angle) + m_StartPos.y * sinf(data.angle);

                if(OnUpdatePosition){
                    OnUpdatePosition(idx, data.positions);
                }
            }
            m_Anim = true;
        }

        m_Time -= deltaTime;
        if (m_Time < 0)
        {
            if (m_Clockwise)
            {
                if (OnWillHide)
                    OnWillHide(m_Step);

                m_Data[m_Step].hide = true;

                m_Step = ++m_Step % m_NumWidget;
            }
            else
            {
                int idx = (m_Step + m_ShowWidget) % m_NumWidget;
                if (OnWillHide)
                    OnWillHide(idx);

                m_Data[idx].hide = true;
            }
            m_Anim = false;

            for (int i = 0; i < m_NumWidget; ++i)
            {
                MyStruct& data = m_Data[i];
                float& dataAngle = data.angle;
            
                float m2Pi = (float)M_PI * 2.0f;
                if (dataAngle >= m2Pi)
                    dataAngle -= m2Pi;
                else if (dataAngle <= -m2Pi)
                    dataAngle += m2Pi;

                if (m_Clockwise)
                    dataAngle += m_InteravlAng;
                else
                    dataAngle -= m_InteravlAng;

                data.positions.x = m_StartPos.x * cosf(dataAngle) - m_StartPos.y * sinf(dataAngle);
                data.positions.y = m_StartPos.y * cosf(dataAngle) + m_StartPos.x * sinf(dataAngle);

                if(OnUpdatePosition)
                    OnUpdatePosition(i, data.positions);
            }

            if (m_StepCount > 0)
            {
                if (m_Clockwise)
                    DoClockwise(m_StepTime);
                else
                    DoAnticlockwise(m_StepTime);

                --m_StepCount;
            }
        }
        else
        {
            for (int i = 0; i < m_NumWidget; ++i)
            {
                MyStruct& data = m_Data[i];
                float dataAngle = 0;
                if (data.hide == false)
                {
                    if (m_Clockwise)
                        dataAngle = BaseFunction::Lerp(data.angle + m_InteravlAng, data.angle, std::max(m_Time/m_StepTime, (float)0));
                    else
                        dataAngle = BaseFunction::Lerp(data.angle - m_InteravlAng, data.angle, std::max(m_Time/m_StepTime, (float)0));

                    data.positions.x = m_StartPos.x * cosf(dataAngle) - m_StartPos.y * sinf(dataAngle);
                    data.positions.y = m_StartPos.y * cosf(dataAngle) + m_StartPos.x * sinf(dataAngle);

                    if(OnUpdatePosition)
                        OnUpdatePosition(i, data.positions);
                }
            }
        }
    }
}
