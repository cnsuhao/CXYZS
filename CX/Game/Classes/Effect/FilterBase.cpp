#include "FilterBase.h"
#include "SpriteWithFilter.h"
#include "CCGLProgram.h"
#include "CCGLProgramCache.h"

USING_NS_CC;

FilterBase* FilterBase::create()
{
    FilterBase *filter = new (std::nothrow) FilterBase();
    if (filter && filter->Init())
    {
        filter->autorelease();
        return filter;
    }
    CC_SAFE_DELETE(filter);
    return nullptr;
}

FilterBase::FilterBase(void):_state(NULL)
{

}


FilterBase::~FilterBase(void)
{
}

const std::string& FilterBase::GetProgramKey() const
{

    static std::string key("FilterBase");
    return key;
}

GLProgram* FilterBase::CreateProgram()
{
    return NULL;
}

GLProgramState* FilterBase::GetGLProgramState()
{
    if (_state == NULL)
    {
        _state = GLProgramState::getOrCreateWithGLProgramName(GetProgramKey());
    }
    return _state;
}

void FilterBase::Setup(class SpriteWithFilter* spr)
{

}

bool FilterBase::Init()
{
    if (nullptr == GLProgramCache::getInstance()->getGLProgram(GetProgramKey()))
    {
        GLProgramCache::getInstance()->addGLProgram(CreateProgram(), GetProgramKey());
    }
    
    return GetGLProgramState() != NULL;
}
