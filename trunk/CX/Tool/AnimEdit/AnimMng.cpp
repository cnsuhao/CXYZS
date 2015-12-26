#include "stdafx.h"
#include "AnimMng.h"
#include "SelectHelper.h"
#include "2d/CCNode.h"
#include "2d/CCTweenFunction.h"
#include "2d/CCActionInterval.h"
#include "2d/CCActionInstant.h"
#include "2d/CCActionEase.h"
USING_NS_CC;

AnimMng gAnimMng;

AnimMng::AnimMng(void)
{
    SetFps(1.0f/30.0f);
    m_lastEditingFrameId = 0;
}


AnimMng::~AnimMng(void)
{
}

void AnimMng::EnterEditNode(EditClass* eNode, int type)
{
    CCASSERT(eNode, "");
    m_currentNode = eNode;
    m_type = type;

    GetMyApp()->m_Frame.OnEditAnimNode(eNode, type);
    Node* node = eNode->GetNode();

    AnimFrame* frame = Find(node->getName());
    if (frame == NULL)
    {
        frame = AddFrame(node->getName());
    }

    if (frame)
    {
        SetFrameData(frame, int(FrameType::Position), (unsigned char)(tweenfunc::Linear), 0, node->getPositionX(), node->getPositionY());
        SetFrameData(frame, int(FrameType::Scale), (unsigned char)(tweenfunc::Linear), 0, node->getScaleX(), node->getScaleY());
        SetFrameData(frame, int(FrameType::Rotation), (unsigned char)(tweenfunc::Linear), 0, node->getRotationX(), node->getRotationY());
        SetFrameData(frame, int(FrameType::Opacity), (unsigned char)(tweenfunc::Linear),  0, node->getOpacity());

    }
}

void AnimMng::ExitEidtNode()
{
    ResetFrame();
    m_currentNode = NULL;

    GetMyApp()->m_Frame.OnExitAnim();
}

void AnimMng::OnPlay()
{
    if (m_currentNode)
    {
        Node* node = m_currentNode->GetNode();
        AnimFrame* frame = Find(node->getName());
        if (frame->m_position_array.size() < 2 && 
            frame->m_rotation_array.size() < 2 &&
            frame->m_scale_array.size() < 2 &&
            frame->m_opacity_array.size() < 2)
        {
            log("小于两帧,无法播放动画");
            return;
        }

        Vector<FiniteTimeAction*> actions;
        FiniteTimeAction* a1 = MakeTranslateAction(frame->m_position_array);
        if (a1)
        {
            //m_currentNode->runAction(a1);
            actions.pushBack(a1);
        }

        FiniteTimeAction* a2 = MakeScaleAction(frame->m_scale_array);
        if (a2)
        {
            //m_currentNode->runAction(a2);
            actions.pushBack(a2);
        }

        FiniteTimeAction* a3 = MakeRotationAction(frame->m_rotation_array);
        if (a3)
        {
            //m_currentNode->runAction(a3);
            actions.pushBack(a3);
        }

        FiniteTimeAction* a4 = MakeOpacityAction(frame->m_opacity_array);
        if (a4)
        {
            //m_currentNode->runAction(a4);
            actions.pushBack(a4);
        }
        if (actions.size() > 1)
        {
            node->runAction(Sequence::create(Spawn::create(actions), CallFuncN::create([this](Node* node){
                //ResetFrame();
            }), nullptr));
        }
        else{
            node->runAction(Sequence::create(actions.back(), CallFuncN::create([this](Node* node){
                //ResetFrame();
            }),nullptr));
        }
        
        ResetFrame();
    }
}

bool AnimMng::HasFrame(unsigned int frame)
{
    if (m_currentNode == NULL)
    {
        return false;
    }
    Node* node = m_currentNode->GetNode();
    return Find(node->getName()) != NULL;
}

void AnimMng::SetFrameData(AnimFrame* frame, int type, unsigned char interpolation, unsigned int frameId, float newValue1, float newValue2, float newValue3)
{

    CCASSERT(m_currentNode, "");
    m_lastEditingFrameId = frameId;
    switch (type)
    {
    case FrameType::Position:
        {
            PositionData* data = frame->GetPositionData(frameId);
            data->position.x = newValue1;
            data->position.y = newValue2;
            data->tween = interpolation;
        }break;
    case FrameType::Scale:
        {
            ScaleData* data = frame->GetScaleData(frameId);
            data->scale.x = newValue1;
            data->scale.y = newValue2;
            data->tween = interpolation;
        }break;
    case FrameType::Rotation:
        {
            RotateData* data = frame->GetRotateData(frameId);
            data->dstAngle = newValue1;
            data->tween = interpolation;
        }break;
    case FrameType::Opacity:
        {
            OpacityData* data = frame->GetOpacityData(frameId);
            data->opacity = newValue1;
            data->tween = interpolation;
        }break;
    default:
        break;
    }
}

void AnimMng::SetFrameData(const std::string& name, int type, unsigned char interpolation, unsigned int frameId, float newValue1, float newValue2 /*= 0.0f*/, float newValue3 /*= 0.0f*/)
{
    AnimFrame* frame = Find(name);
    if (frame == NULL)
    {
        frame = AddFrame(name);
    }

    if (frame)
    {
        SetFrameData(frame, type, interpolation, frameId, newValue1, newValue2, newValue3);
    }
}

void AnimMng::SetCurrentFrameData(int type, unsigned char interpolation, unsigned int frameId, float newValue1, float newValue2 /*= 0.0f*/, float newValue3 /*= 0.0f*/)
{
    if (m_currentNode)
    {
        Node* node = m_currentNode->GetNode();
        SetFrameData(node->getName(), type, interpolation, frameId, newValue1, newValue2, newValue3);
    }
}

void AnimMng::SetCurrentFrameData(int type, unsigned char interpolation, unsigned int frameId)
{
    if (m_currentNode)
    {
        Node* node = m_currentNode->GetNode();
        AnimFrame* frame = Find(node->getName());
        if (frame)
        {
            m_lastEditingFrameId = frameId;
            switch (type)
            {
            case FrameType::Position:
                {
                    PositionData* data = frame->GetPositionData(frameId);
                    data->tween = interpolation;
                }break;
            case FrameType::Scale:
                {
                    ScaleData* data = frame->GetScaleData(frameId);
                    data->tween = interpolation;
                }break;
            case FrameType::Rotation:
                {
                    RotateData* data = frame->GetRotateData(frameId);
                    data->tween = interpolation;
                }break;
            case FrameType::Opacity:
                {
                    OpacityData* data = frame->GetOpacityData(frameId);
                    data->tween = interpolation;
                }break;
            default:
                break;
            }
        }
    }
}

void AnimMng::DelCurrentFrameData(int type, unsigned int frameId)
{
    Node* node = m_currentNode->GetNode();
    AnimFrame* frame = Find(node->getName());
    if (frame == NULL)
    {
        return;
    }
    if (frameId > 0)
    {
        switch (type)
        {
        case FrameType::Position:
            {
                frame->DelPositionData(frameId);
            }break;
        case FrameType::Scale:
            {
                frame->DelScaleData(frameId);
            }break;
        case FrameType::Rotation:
            {
                frame->DelRotateData(frameId);
            }break;
        case FrameType::Opacity:
            {
                frame->DelOpacityData(frameId);
            }break;
        default:
            break;
        }
    }
}

bool AnimMng::HasCurrentFrameData(int type, unsigned int frameId)
{
    Node* node = m_currentNode->GetNode();
    AnimFrame* frame = Find(node->getName());
    if (frame == NULL || frameId == 0)
    {
        return false;
    }

    switch (type)
    {
    case FrameType::Position:
        {
            return NULL != frame->GetPositionData(frameId, false);
        }break;
    case FrameType::Scale:
        {
            return NULL != frame->GetScaleData(frameId, false);
        }break;
    case FrameType::Rotation:
        {
            return NULL != frame->GetRotateData(frameId, false);
        }break;
    case FrameType::Opacity:
        {
            return NULL !=  frame->GetOpacityData(frameId, false);
        }break;
    default:
        break;
    }
    return false;
}

AnimFrame* AnimMng::Find(const std::string& name)
{
    std::map<std::string, AnimFrame*>::iterator it = m_frames.find(name);
    if (it != m_frames.end())
    {
        return it->second;
    }
    return NULL;
}

AnimFrame* AnimMng::AddFrame(const std::string& name)
{
    CCASSERT(Find(name) == NULL, "");

    AnimFrame* frame = new AnimFrame(); 
    m_frames.insert(std::pair<std::string, AnimFrame*>(name,frame));
    return frame;
}

void AnimMng::RemoveFrame(const std::string& name)
{
    std::map<std::string, AnimFrame*>::iterator it = m_frames.find(name);
    if (it != m_frames.end())
    {
        m_frames.erase(it);
    }
}

void AnimMng::ResetFrame()
{
    if (NULL == m_currentNode)
        return;
    
    Node* node = m_currentNode->GetNode();
    AnimFrame* frame = Find(node->getName());
    if (frame)
    {
        if (!frame->m_position_array.empty())
        {
            const cc2d::Vec2& p = (*(frame->m_position_array.begin()))->position;
            node->setPosition(p.x, p.y);
            gSelectHepler.NodeMoveUpdate();
        }
        if (!frame->m_scale_array.empty())
        {
            const cc2d::Vec2& s = (*(frame->m_scale_array.begin()))->scale;
            node->setScale(s.x, s.y);
        }
        if (!frame->m_rotation_array.empty())
        {
            float dstAngle = (*(frame->m_rotation_array.begin()))->dstAngle;
            node->setRotation(dstAngle);
        }
        if (!frame->m_opacity_array.empty())
        {
            float opacity = (*(frame->m_opacity_array.begin()))->opacity;
            node->setOpacity(opacity);
        }
    }
}

FiniteTimeAction* AnimMng::GetInterpolation(unsigned char interpolation, ActionInterval* action)
{
    switch (interpolation)
    {
    case tweenfunc::Linear: break;
    case tweenfunc::Sine_EaseIn: 
        return EaseSineIn::create(action);
        break;
    case tweenfunc::Sine_EaseOut:
        return EaseSineOut::create(action);
    case tweenfunc::Sine_EaseInOut:
        return EaseQuadraticActionIn::create(action);
    case tweenfunc::Quad_EaseIn:
        return EaseQuadraticActionIn::create(action);
    case tweenfunc::Quad_EaseOut:
        return EaseQuadraticActionOut::create(action);
    case tweenfunc::Quad_EaseInOut:
        return EaseQuadraticActionInOut::create(action);
    case tweenfunc::Cubic_EaseIn:
        return EaseCubicActionIn::create(action);
    case tweenfunc::Cubic_EaseOut:
        return EaseCubicActionOut::create(action);
    case tweenfunc::Cubic_EaseInOut:
        return EaseCubicActionInOut::create(action);
    case tweenfunc::Quart_EaseIn:
        return EaseQuarticActionIn::create(action);
    case tweenfunc::Quart_EaseOut:
        return EaseQuarticActionOut::create(action);
    case tweenfunc::Quart_EaseInOut:
        return EaseQuarticActionInOut::create(action);
    case tweenfunc::Quint_EaseIn:
        return EaseQuinticActionIn::create(action);
    case tweenfunc::Quint_EaseOut:
        return EaseQuinticActionOut::create(action);
    case tweenfunc::Quint_EaseInOut:
        return EaseQuinticActionInOut::create(action);
    case tweenfunc::Expo_EaseIn:
        return EaseExponentialIn::create(action);
    case tweenfunc::Expo_EaseOut:
        return EaseExponentialOut::create(action);
    case tweenfunc::Expo_EaseInOut:
        return EaseExponentialInOut::create(action);
    case tweenfunc::Circ_EaseIn:
        return EaseCircleActionIn::create(action);
    case tweenfunc::Circ_EaseOut:
        return EaseCircleActionOut::create(action);
    case tweenfunc::Circ_EaseInOut:
        return EaseCircleActionInOut::create(action);
    case tweenfunc::Elastic_EaseIn:
        return EaseElasticIn::create(action);
    case tweenfunc::Elastic_EaseOut:
        return EaseElasticOut::create(action);
    case tweenfunc::Elastic_EaseInOut:
        return EaseElasticInOut::create(action);
    case tweenfunc::Back_EaseIn:
        return EaseBackIn::create(action);
    case tweenfunc::Back_EaseOut:
        return EaseBackOut::create(action);
    case tweenfunc::Back_EaseInOut:
        return EaseBackInOut::create(action);
    case tweenfunc::Bounce_EaseIn:
        return EaseBounceIn::create(action);
    case tweenfunc::Bounce_EaseOut:
        return EaseBounceOut::create(action);
    case tweenfunc::Bounce_EaseInOut:
        return EaseBounceInOut::create(action);
    default:
        break;
    }
    return NULL;
}

cc2d::FiniteTimeAction* AnimMng::MakeTranslateAction(const std::list<PositionData*>& PosArray)
{
    if (PosArray.size() < 2)
    {
        return NULL;
    }

    std::list<PositionData*>::const_iterator it = PosArray.begin();
    Vector<FiniteTimeAction*> arrayOfActions;
    ++it;
    int lastId = 0;
    for ( ;it != PosArray.end(); ++it)
    {
        float time = float((*it)->frameId - lastId) * m_fps;
        lastId = (*it)->frameId;
        if ((*it)->tween == (unsigned char)(tweenfunc::Linear))
        {
            arrayOfActions.pushBack(MoveTo::create(time, (*it)->position));
        }
        else
        {
            arrayOfActions.pushBack(GetInterpolation((*it)->tween, MoveTo::create(time, (*it)->position)));
        }
    }
    return Sequence::create(arrayOfActions);
}

cc2d::FiniteTimeAction* AnimMng::MakeScaleAction(const std::list<ScaleData*>& ScaleArray)
{
    if (ScaleArray.size() < 2)
    {
        return NULL;
    }

    std::list<ScaleData*>::const_iterator it = ScaleArray.begin();
    Vector<FiniteTimeAction*> arrayOfActions;
    ++it;
    int lastId = 0;
    for ( ;it != ScaleArray.end(); ++it)
    {
        float time = float((*it)->frameId - lastId) * m_fps;
        lastId = (*it)->frameId;
        if ((*it)->tween == (unsigned char)(tweenfunc::Linear))
        {
            arrayOfActions.pushBack(ScaleTo::create(time, (*it)->scale.x, (*it)->scale.y));
        }else
        {
            arrayOfActions.pushBack(GetInterpolation((*it)->tween, ScaleTo::create(time, (*it)->scale.x, (*it)->scale.y)));
        }
    }
    return Sequence::create(arrayOfActions);
}

cc2d::FiniteTimeAction* AnimMng::MakeRotationAction(const std::list<RotateData*>& RotationArray)
{
    if (RotationArray.size() < 2)
    {
        return NULL;
    }

    std::list<RotateData*>::const_iterator it = RotationArray.begin();
    Vector<FiniteTimeAction*> arrayOfActions;
    ++it;
    int lastId = 0;
    for ( ;it != RotationArray.end(); ++it)
    {
        float time = float((*it)->frameId - lastId) * m_fps;
        lastId = (*it)->frameId;
        if ((*it)->tween == (unsigned char)(tweenfunc::Linear))
        {
            arrayOfActions.pushBack(RotateTo::create(time, (*it)->dstAngle));
        }else
        {
            arrayOfActions.pushBack(GetInterpolation((*it)->tween, RotateTo::create(time, (*it)->dstAngle)));
        }
    }
    return Sequence::create(arrayOfActions);
}

cc2d::FiniteTimeAction* AnimMng::MakeOpacityAction(const std::list<OpacityData*>& OpacityArray)
{
    if (OpacityArray.size() < 2)
    {
        return NULL;
    }

    std::list<OpacityData*>::const_iterator it = OpacityArray.begin();
    Vector<FiniteTimeAction*> arrayOfActions;
    ++it;
    int lastId = 0;
    for ( ;it != OpacityArray.end(); ++it)
    {
        float time = float((*it)->frameId - lastId) * m_fps;
        lastId = (*it)->frameId;
        if ((*it)->tween == (unsigned char)(tweenfunc::Linear))
        {
            arrayOfActions.pushBack(FadeTo::create(time, (*it)->opacity));
        }
        else{
            arrayOfActions.pushBack(GetInterpolation((*it)->tween, FadeTo::create(time, (*it)->opacity)));
        }
    }
    return Sequence::create(arrayOfActions);
}

AnimFrame::AnimFrame()
{

}

AnimFrame::~AnimFrame()
{

}

template <class T>
T* AnimFrame::GetData(unsigned int frameId, std::list<T*>& container, bool create/* = true*/)
{
    std::list<T*>::iterator it = container.begin();
    for (; it != container.end();)
    {
        std::list<T*>::iterator lastIt = it;

        if ((*it)->frameId == frameId)
        {
            return *it;
        }

        ++it;
        if (create)
        {
            if (it == container.end())
            {
                T* data = new T();
                data->frameId = frameId;
                container.push_back(data);
                return container.back();
            }
            else if (frameId < (*it)->frameId)
            {
                T* data = new T();
                data->frameId = frameId;
                container.insert(it, data);
                return data;
            }
        }
    }

    if (container.empty() && create)
    {
        T* data = new T();
        data->frameId = frameId;
        container.push_back(data);
        return container.back();
    }
    return NULL;
}

PositionData* AnimFrame::GetPositionData(unsigned int frameId, bool create)
{
    return GetData<PositionData>(frameId, m_position_array, create);
}

RotateData* AnimFrame::GetRotateData(unsigned int frameId, bool create)
{
    return GetData<RotateData>(frameId, m_rotation_array, create);
}

ScaleData* AnimFrame::GetScaleData(unsigned int frameId, bool create)
{
    return GetData<ScaleData>(frameId, m_scale_array, create);
}

OpacityData* AnimFrame::GetOpacityData(unsigned int frameId, bool create)
{
    return GetData<OpacityData>(frameId, m_opacity_array, create);
}


template <class T>
void AnimFrame::DelData(unsigned int frameId, std::list<T*>& container)
{
    std::list<T*>::iterator it = container.begin();
    for (; it != container.end(); ++it)
    {
        if ((*it)->frameId == frameId)
        {
            delete *it;
            container.erase(it);
            return;
        }
    }
}


void AnimFrame::DelPositionData(unsigned int frameId)
{
    DelData<PositionData>(frameId, m_position_array);
}

void AnimFrame::DelRotateData(unsigned int frameId)
{
     DelData<RotateData>(frameId, m_rotation_array);
}

void AnimFrame::DelScaleData(unsigned int frameId)
{
     DelData<ScaleData>(frameId, m_scale_array);
}

void AnimFrame::DelOpacityData(unsigned int frameId)
{
     DelData<OpacityData>(frameId, m_opacity_array);
}
