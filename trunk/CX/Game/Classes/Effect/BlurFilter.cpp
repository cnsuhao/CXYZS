#include "BlurFilter.h"

USING_NS_CC;

#include "SceneManager.h"
#include "CCFileUtils.h"
#include "CCShaders.h"
#include "CCGLProgram.h"

BlurFilter::BlurFilter(void)
{
    _resolution = g_ScreenSize;
    _blurRadius = 10.0f;
    _blurSampleNum = 5.0f;
}

BlurFilter::~BlurFilter(void)
{
}

BlurFilter* BlurFilter::create()
{
    BlurFilter *filter = new (std::nothrow) BlurFilter();
    if (filter  && filter->Init())
    {
        filter->autorelease();
        return filter;
    }
    CC_SAFE_DELETE(filter);
    return nullptr;
}

const std::string& BlurFilter::GetProgramKey() const 
{
    static std::string key("BlurFilterShader");
    return key;
}

cc2d::GLProgram* BlurFilter::CreateProgram()
{
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename("shader/example_Blur.fsh");
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    return GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
}

void BlurFilter::SetResolution(const cc2d::Size& size)
{
    _resolution = size;
    if (_state)
    {
        _state->setUniformVec2("resolution", _resolution);
    }
}

void BlurFilter::SetBlurRadius(float radius)
{
    _blurRadius = radius;
    if (_state)
    {
        _state->setUniformFloat("blurRadius", _blurRadius);
    }
}

void BlurFilter::SetSampleNum(float num)
{
    _blurSampleNum = num;
    if (_state)
    {
        _state->setUniformFloat("sampleNum", _blurSampleNum);
    }
}
/*
// Ä£ºýÔË¶¯
class BlurAction : public ActionInterval
{
    BlurAction(PostRenderNode* renderNode)
    {
        _renderNode = renderNode;
        _filter = BlurFilter::create();
        _filter->SetResolution(Size(g_ScreenSize));
        _filter->SetBlurRadius(2.0f);
        _renderNode->UseFilter(_filter);
    }
public:
    static BlurAction* create(float duration, PostRenderNode* renderNode)
    {
        BlurAction *ret = new (std::nothrow) BlurAction(renderNode);
        if (ret && ret->initWithDuration(duration))
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    //
    // Override
    //
    virtual BlurAction* clone() const override
    {
        return create(getDuration(), _renderNode);
    }
    virtual BlurAction* reverse() const override
    {
        return clone();
    }

    bool BlurAction::isDone() const
    {
        bool r = ActionInterval::isDone();
        if (r){
            _renderNode->UseFilter(NULL);
        }
        return r;
    }

    virtual void update(float time) override
    {
        float number = -4.0f * time + 5.0f;
        _filter->SetSampleNum(number);
    }
public:
    PostRenderNode* _renderNode;
    BlurFilter* _filter;
};
*/