#pragma once

/********************************************************************
created:	2015/5/15
author:		lishihai
desc:	    SpriteWithFilter基类
*********************************************************************/

#include "ClientDef.h"
#include "FilterBase.h"


// 使用滤镜的精灵
class SpriteWithFilter : public cc2d::Sprite
{
    SpriteWithFilter(void);
public:
    ~SpriteWithFilter(void);
// 重载所有sprite创建的接口.
    static SpriteWithFilter* create();

    static SpriteWithFilter* create(const std::string& filename);

    static SpriteWithFilter* create(const std::string& filename, const cc2d::Rect& rect);

    static SpriteWithFilter* createWithTexture(cc2d::Texture2D *texture);

    static SpriteWithFilter* createWithTexture(cc2d::Texture2D *texture, const cc2d::Rect& rect, bool rotated=false);

    static SpriteWithFilter* createWithSpriteFrame(cc2d::SpriteFrame *spriteFrame);

    static SpriteWithFilter* createWithSpriteFrameName(const std::string& spriteFrameName);

public:
    // 使用滤镜
    void UseFilter(FilterBase* filter);
public:
    FilterBase* _filter;
};

