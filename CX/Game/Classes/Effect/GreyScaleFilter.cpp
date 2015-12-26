#include "GreyScaleFilter.h"
USING_NS_CC;
#include "CCFileUtils.h"
#include "CCGLProgram.h"
#include "ccShaders.h"

GreyScaleFilter::GreyScaleFilter(void)
{
}


GreyScaleFilter::~GreyScaleFilter(void)
{
}

GreyScaleFilter* GreyScaleFilter::create()
{
    GreyScaleFilter *filter = new (std::nothrow) GreyScaleFilter();
    if (filter  && filter->Init())
    {
        filter->autorelease();
        return filter;
    }
    CC_SAFE_DELETE(filter);
    return nullptr;
}

const std::string& GreyScaleFilter::GetProgramKey() const
{
    static std::string key("GreyScaleFilterShader");
    return key;
}

cc2d::GLProgram* GreyScaleFilter::CreateProgram()
{
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename("shader/example_greyScale.fsh");
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    return GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
}

