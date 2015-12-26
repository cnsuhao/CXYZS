#include "CtrlJoystick.h"

USING_NS_CC;

#include "SceneManager.h"
#include "UIManager.h"
#include "CCEventDispatcher.h"

CtrlJoystick::CtrlJoystick(void)
{
}


CtrlJoystick::~CtrlJoystick(void)
{
    unscheduleAllCallbacks();
}

CtrlJoystick* CtrlJoystick::create(const char* background, const char* touchImage)
{
    CtrlJoystick *pRet = new CtrlJoystick(); 
    if (pRet && pRet->Init(background, touchImage))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool CtrlJoystick::Init(const char* background, const char* touchImage)
{
    if(!init())
    {
        return false;
    }

	_joydir = -1;

    _listenter = NULL;

    _background = Sprite::create(background);
    _touchImage = Sprite::create(touchImage);
    _background->addChild(_touchImage);

    _touchListener = EventListenerTouchOneByOne::create();
    CC_SAFE_RETAIN(_touchListener);
    _touchListener->setSwallowTouches(false);
    _touchListener->onTouchBegan = CC_CALLBACK_2(CtrlJoystick::onTouchBegan, this);
    _touchListener->onTouchMoved = CC_CALLBACK_2(CtrlJoystick::onTouchMoved, this);
    _touchListener->onTouchEnded = CC_CALLBACK_2(CtrlJoystick::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithFixedPriority(_touchListener, -1);

    schedule(CC_SCHEDULE_SELECTOR(CtrlJoystick::Update), 0.02f);
    //setVisible(false);
    //SetDelayShow(1.0f);

    addChild(_background);
    _halfRange = (_background->getContentSize() - _touchImage->getContentSize()) * 0.5f;

 
    SetDelayShow(0.2f);
    _background->setPosition(Vec2(125.0f, 125.0f));
    _target = _background->getPosition();
    _touchImage->setPosition(_background->getAnchorPointInPoints());
    _realdyOnMoveBigin = false;

    SetEnabled(true);
    _target = Vec2::ZERO;
    _dampSpeed = 6.0f;
#ifdef _WIN32
    _touched = false;
#endif
    return true;
}

void CtrlJoystick::SetEnabled(bool enable)
{
    if (_enabled != enable)
    {
        _touchTime = 0.0f;
        _enabled = enable;
    }
    setVisible(enable);
}

bool CtrlJoystick::onTouchBegan(cc2d::Touch *touch, cc2d::Event *unusedEvent)
{
    if (_enabled){
        // 检测UI
        if (false == g_UIManager->TouchJoystickInRange(touch))
        {
            return false;
        }
        _touchTime = 0.0f;
#ifdef _WIN32
        _touchPosition = convertTouchToNodeSpace(touch);
        _touched = true;
#endif
    }
    return _enabled;
}

void CtrlJoystick::onTouchMoved(cc2d::Touch *touch, cc2d::Event *unusedEvent)
{
    if (_enabled)
    {
#ifdef _WIN32
        _touchPosition = convertTouchToNodeSpace(touch);
#endif
        if (_touchTime > GetDelayShow())
        {
            if (g_SceneManager->m_MainLayer->m_MuitiTouchCount > 0){
                _touchTime = 0.0f;
                return;
            }

            Vec2 p = convertTouchToNodeSpace(touch);

            if (_realdyOnMoveBigin == false)
            {
                _realdyOnMoveBigin = true;
                if (_listenter)
                    _listenter->OnMoveBegin();
            }

            //if (!isVisible())
            //{
            //    setVisible(true);
            //    _background->setPosition(p);
            //    _target = _background->getPosition();
            //    _touchImage->setPosition(_background->getAnchorPointInPoints());
            //    if (_listenter)
            //        _listenter->OnMoveBegin();
            //}
            //else
            {
                Vec2 delta = touch->getDelta();
                Vec2 p1 = _touchImage->getPosition() - _background->getAnchorPointInPoints() + delta;

                float radius = p1.length();
                if (radius > _halfRange.width * 1.5f)
                {
                    p1.normalize();
                    p1.scale(_halfRange.width * 1.5f);
                    _touchImage->setPosition(p1 + _background->getAnchorPointInPoints());
                }else
                {
                    _touchImage->setPosition(_touchImage->getPosition() + delta);
                }

                /*
                if (p1.x < -_halfRange.width || p1.x > _halfRange.width)
                {
                    _target.x = _target.x + delta.x;
                }
                else{
                     _touchImage->setPositionX(_touchImage->getPositionX() + delta.x);
                }
                if (p1.y < -_halfRange.height || p1.y > _halfRange.height)
                {
                    _target.y = _target.y + delta.y;
                }else
                {
                    _touchImage->setPositionY(_touchImage->getPositionY() + delta.y);
                }
                */
                NotityListener(Vec2(
                                    clampf(p1.x, -_halfRange.width, _halfRange.width)/_halfRange.width, 
                                    clampf(p1.y, -_halfRange.height, _halfRange.height)/_halfRange.height
                                    )
                              );
            }
        }
    }
}

void CtrlJoystick::onTouchEnded(cc2d::Touch *touch, cc2d::Event *unusedEvent)
{
#ifdef _WIN32
    _touched = false;
#endif
    if (_listenter && isVisible())
        _listenter->OnMoveEnd();

    //>还原到初始位置
    _realdyOnMoveBigin = false;
    _target = _background->getPosition();
    _touchImage->setPosition(_background->getAnchorPointInPoints());
    //setVisible(false);
	_joydir = -1;
}

void CtrlJoystick::Update(float dt)
{
    if (_enabled)
    {
        _touchTime += dt;

#ifdef _WIN32
        if (_touched && _touchTime > GetDelayShow())
        {

            if (_realdyOnMoveBigin == false)
            {
                _realdyOnMoveBigin = true;
                if (_listenter)
                    _listenter->OnMoveBegin();
            }

            //if (!isVisible())
            //{
            //    setVisible(true);
            //    _background->setPosition(_touchPosition);
            //    _target = _background->getPosition();
            //    _touchImage->setPosition(_background->getAnchorPointInPoints());
            //    if (_listenter)
            //        _listenter->OnMoveBegin();
            //}
            //else
            {
                Vec2 p1 = _touchImage->getPosition() - _background->getAnchorPointInPoints();
                NotityListener(Vec2(
                    clampf(p1.x, -_halfRange.width, _halfRange.width)/_halfRange.width, 
                    clampf(p1.y, -_halfRange.height, _halfRange.height)/_halfRange.height
                    )
                );
            }
        }
#endif

        //>取消跟踪手指移动
        //const Vec2& position =  _background->getPosition();
        //if (position != _target)
        //{
        //    Vec2 newPosition = position + (_target - position) / _dampSpeed;
        //    _background->setPosition(newPosition);
        //}
    }
}

void CtrlJoystick::NotityListener(const Vec2& dir)
{
#define cos22dot5 0.92387f      // cos( 22.5')
#define cos67dot5 0.38268f      // cos( 67.5')
#define cos112dot5 -0.38268f    // cos(112.5')
#define cos157dot5 -0.92387f    // cos(157.5')

    if (_listenter)
    {
        static const Vec2 vleft = Vec2(1.0f, 0.0f);
        Vec2 pn = dir;
        pn.normalize();
        float dot = vleft.dot(pn);
        if (dot > cos22dot5) // cos(22.5');
        {
            //log("joystick move right\n");
            _listenter->OnRight(dir);
        }
        else if (dot > cos67dot5) // cos(67.5')
        {
            if (pn.y > 0.0f)
            {
                //log("joystick move right up\n");
                _listenter->OnRightUp(dir);
            }else
            {
                //log("joystick move right down\n");
                _listenter->OnRightDown(dir);
            }
        }else if (dot > cos112dot5) // cos(112.5')
        {
            if (pn.y > 0.0f)
            {
                //log("joystick move up\n");
                _listenter->OnUp(dir);
            }else
            {
                //log("joystick move down\n");
                _listenter->OnDown(dir);
            }
        }else if (dot > cos157dot5) // cos(157.5')
        {
            if (pn.y > 0.0f)
            {
                //log("joystick move left up\n");
                _listenter->OnLeftUp(dir);
            }else
            {
                //log("joystick move left down\n");
                _listenter->OnLeftDown(dir);
            }
        }else
        {
            //log("joystick move left\n");
            _listenter->OnLeft(dir);
        }
        //log("dot--> %f, length-->%f\n", dot, dir.length());
    }
}
