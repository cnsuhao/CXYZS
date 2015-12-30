#include "FollowEx.h"
#include "CCDirector.h"

USING_NS_CC;

FollowEx::FollowEx(void)
{
    _scale = 1.0f;
    _newScale = false;
}


FollowEx::~FollowEx(void)
{
}

FollowEx* FollowEx::create(cc2d::Node *followedNode, const cc2d::Rect& rect /*= Rect::ZERO*/)
{
    FollowEx *follow = new (std::nothrow) FollowEx();
    if (follow && follow->initWithTarget(followedNode, rect))
    {
        follow->autorelease();
        return follow;
    }
    CC_SAFE_DELETE(follow);
    return nullptr;
}

bool FollowEx::initWithTarget(Node *followedNode, const Rect& rect /*= Rect::ZERO*/)
{
    CCASSERT(followedNode != nullptr, "");

    followedNode->retain();
    _followedNode = followedNode;
    _worldRect = rect;
    if (rect.equals(Rect::ZERO))
    {
        _boundarySet = false;
    }
    else
    {
        _boundarySet = true;
    }

    _boundaryFullyCovered = false;

    Size winSize = Director::getInstance()->getWinSize();
    _fullScreenSize = Vec2(winSize.width, winSize.height);
    _halfScreenSize = _fullScreenSize * 0.5f;

    if (_boundarySet)
    {
        _leftBoundary = -((rect.origin.x+rect.size.width) - _fullScreenSize.x);
        _rightBoundary = -rect.origin.x ;
        _topBoundary = -rect.origin.y;
        _bottomBoundary = -((rect.origin.y+rect.size.height) - _fullScreenSize.y);

        if(_rightBoundary < _leftBoundary)
        {
            // screen width is larger than world's boundary width
            //set both in the middle of the world
            _rightBoundary = _leftBoundary = (_leftBoundary + _rightBoundary) / 2;
        }
        if(_topBoundary < _bottomBoundary)
        {
            // screen width is larger than world's boundary width
            //set both in the middle of the world
            _topBoundary = _bottomBoundary = (_topBoundary + _bottomBoundary) / 2;
        }
			
        if( (_topBoundary == _bottomBoundary) && (_leftBoundary == _rightBoundary) )
        {
            _boundaryFullyCovered = true;
        }
    }

    return true;
}

Follow* FollowEx::clone() const 
{
    // no copy constructor
    auto a = new (std::nothrow) FollowEx();
    a->initWithTarget(_followedNode, _worldRect);
    a->autorelease();
    return a;
}

Follow* FollowEx::reverse() const 
{
    return clone();
}

void FollowEx::step(float dt)
{
    CC_UNUSED_PARAM(dt);

    if(_boundarySet)
    {
        // whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
        if(_boundaryFullyCovered)
            return;

        float invScale = 1.0f / _scale;
        Vec2 tempPos = _halfScreenSize * invScale - _followedNode->getPosition();
        
        tempPos.x = clampf(tempPos.x, _leftBoundary + (1.0f - _scale) * _fullScreenSize.x * invScale, _rightBoundary);
        tempPos.y = clampf(tempPos.y, _bottomBoundary + (1.0f - _scale) * _fullScreenSize.y * invScale, _topBoundary);

        if (_newScale){
            _target->setPosition(tempPos);
            _newScale = false;
            return;
        }

        Vec2 nowPos = _target->getPosition();
        Vec2 direction = tempPos - nowPos;
        float distance = direction.length();
        if (distance > 15.0f)
        {
            if (distance > (Director::getInstance()->getVisibleSize().height * 0.5f))
            {
                _target->setPosition(tempPos);
            }
            else
            {
                direction.normalize();
                _target->setPosition(nowPos + direction * distance * 0.04f);
            }
        }
    }
    else
    {
        _target->setPosition(_halfScreenSize - _followedNode->getPosition());
    }
}

void FollowEx::SetRectScale(float scale)
{
    _scale = scale;
    _newScale = true;
}

