#pragma once

/********************************************************************
created:	2015/7/16
author:		lishihai
desc:	    ½Úµã¸úËæ
*********************************************************************/

#include "CCNode.h"
#include "CCAction.h"

class FollowEx : public cc2d::Follow
{
protected:
    FollowEx(void);
public:
    ~FollowEx(void);

    static FollowEx* create(cc2d::Node *followedNode, const cc2d::Rect& rect = cc2d::Rect::ZERO);

    //
    // Override
    //
    virtual Follow* clone() const override;
    virtual Follow* reverse() const override;
    virtual void step(float dt) override;

    void SetRectScale(float scale);
protected:
    bool initWithTarget(cc2d::Node *followedNode, const cc2d::Rect& rect = cc2d::Rect::ZERO);
protected:
    float _scale;
    bool  _newScale;
private:
    CC_DISALLOW_COPY_AND_ASSIGN(FollowEx);
};

