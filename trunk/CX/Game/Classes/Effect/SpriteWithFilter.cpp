#include "SpriteWithFilter.h"
#include "FilterBase.h"
#include "CCSpriteFrameCache.h"

USING_NS_CC;

SpriteWithFilter::SpriteWithFilter(void):_filter(NULL)
{
}


SpriteWithFilter::~SpriteWithFilter(void)
{
}

SpriteWithFilter* SpriteWithFilter::create()
{
    SpriteWithFilter *sprite = new (std::nothrow) SpriteWithFilter();
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithFilter* SpriteWithFilter::create(const std::string& filename)
{
    SpriteWithFilter *sprite = new (std::nothrow) SpriteWithFilter();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithFilter* SpriteWithFilter::create(const std::string& filename, const cc2d::Rect& rect)
{
    SpriteWithFilter *sprite = new (std::nothrow) SpriteWithFilter();
    if (sprite && sprite->initWithFile(filename, rect))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithFilter* SpriteWithFilter::createWithTexture(cc2d::Texture2D *texture)
{
    SpriteWithFilter *sprite = new (std::nothrow) SpriteWithFilter();
    if (sprite && sprite->initWithTexture(texture))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithFilter* SpriteWithFilter::createWithTexture(cc2d::Texture2D *texture, const cc2d::Rect& rect, bool rotated/*=false*/)
{
    SpriteWithFilter *sprite = new (std::nothrow) SpriteWithFilter();
    if (sprite && sprite->initWithTexture(texture, rect, rotated))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithFilter* SpriteWithFilter::createWithSpriteFrame(cc2d::SpriteFrame *spriteFrame)
{
    SpriteWithFilter *sprite = new (std::nothrow) SpriteWithFilter();
    if (sprite && spriteFrame && sprite->initWithSpriteFrame(spriteFrame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithFilter* SpriteWithFilter::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
    CCASSERT(frame != nullptr, msg);
#endif

    return createWithSpriteFrame(frame);
}


void SpriteWithFilter::UseFilter(FilterBase* filter)
{
    if (_filter){
        _filter->release();
    }

    _filter = filter;
    if (_filter)
    {
        _filter->retain();
        setGLProgramState(_filter->GetGLProgramState());
        _filter->Setup(this);
    }
}
