#include "GestureDetector.h"

using namespace cocos2d;

GestureDetector::GestureDetector(void)
{
}


GestureDetector::~GestureDetector(void)
{
}

void GestureDetector::BeginTouch(cocos2d::Vec2& point)
{
    m_BeginPoint = Vec2(-1.0f, -1.0f);

    if (point.x < 0.0f || point.y < 0.0f)
        return;

    if (point.x > m_TouchSize.x || point.y > m_TouchSize.y)
        return;


    m_BeginPoint = point;
}

void GestureDetector::EndTouch(cocos2d::Vec2& point)
{

#define cos22dot5 0.92387f      // cos( 22.5')
#define cos67dot5 0.38268f      // cos( 67.5')
#define cos112dot5 -0.38268f    // cos(112.5')
#define cos157dot5 -0.92387f    // cos(157.5')
    if (OnDetectReslut != nullptr)
    {
        if (m_BeginPoint.x < 0.0f || m_BeginPoint.y < 0.0f){
            OnDetectReslut(None);
            return;
        }

        if (point.x < 0.0f || point.y < 0.0f){
            OnDetectReslut(None);
            return;
        }

        if (point.x > m_TouchSize.x || point.y > m_TouchSize.y){
            OnDetectReslut(None);
            return;
        }

        m_EndPoint = point;

        if((m_EndPoint - m_BeginPoint).length() < 0.25f* m_TouchSize.length())
        {
            OnDetectReslut(None);
            return;
        }

        static const Vec2 vleft = Vec2(1.0f, 0.0f);
        Vec2 pn = m_EndPoint - m_BeginPoint;
        pn.normalize();
        float dot = vleft.dot(pn);
        if (dot > cos22dot5) // cos(22.5');
        {
            OnDetectReslut(LeftToRight);
        }
        else if (dot > cos67dot5) // cos(67.5')
        {
            if (pn.y > 0.0f)
            {
                OnDetectReslut(LeftDownToRightTop);
            }else
            {
                OnDetectReslut(LeftTopToRightDown);
            }
        }else if (dot > cos112dot5) // cos(112.5')
        {
            if (pn.y > 0.0f)
            {
                OnDetectReslut(BottomToTop);
            }else
            {
                OnDetectReslut(TopToBottom);
            }
        }else if (dot > cos157dot5) // cos(157.5')
        {
            if (pn.y > 0.0f)
            {
                OnDetectReslut(RightDownToLeftTop);
            }else
            {
                OnDetectReslut(RightTopToLeftDown);
            }
        }else
        {
            OnDetectReslut(RightToLeft);
        }
    }
}
