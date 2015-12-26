#include "LaserRayFilter.h"
#include "SpriteWithFilter.h"

#include "CCGLProgram.h"

USING_NS_CC;

LaserRayFilter::LaserRayFilter(void)
{
    time = 0.0f;
    use = true;
    exitPos = Vec2::ANCHOR_MIDDLE;
    numSample = 40;
    weight = 0.1f;
}


LaserRayFilter::~LaserRayFilter(void)
{
}

LaserRayFilter* LaserRayFilter::create()
{
    LaserRayFilter *filter = new (std::nothrow) LaserRayFilter();
    if (filter)
    {
        filter->autorelease();
        return filter;
    }
    CC_SAFE_DELETE(filter);
    return nullptr;
}

const std::string& LaserRayFilter::GetProgramKey() const
{
    static std::string key("LaserRayFilterShader");
    return key;
}

cc2d::GLProgram* LaserRayFilter::CreateProgram()
{
    return GLProgram::createWithFilenames("shader/LaserRay.vsh", "shader/LaserRay.fsh");
}

void LaserRayFilter::Setup(class SpriteWithFilter* spr)
{
    if (_state == NULL)
        return;

    _state->setUniformTexture("u_texture", spr->getTexture());
    SetUseParamPos(false);
    SetParamPos(exitPos);
    SetNumSample(numSample);
    SetWeight(weight);
}

void LaserRayFilter::SetTime(float t)
{
    static float _time = 0.0f;
    _time += 0.02f;

    time = t;
    
    _state->setUniformFloat("u_time", _time);
}

void LaserRayFilter::SetUseParamPos(bool u)
{
    use = u;
    if (_state)
    {
        _state->setUniformInt("u_useParamPos", use ? 1 : 0);
    }
    
}

void LaserRayFilter::SetParamPos(cc2d::Vec2& pos)
{
    exitPos = pos;
    if (_state)
    {
        _state->setUniformVec2("u_localPos", Vec2(0.5, 0.5));
    }
}

void LaserRayFilter::SetNumSample(int num)
{
    numSample = num;
    if (_state)
    {
        _state->setUniformInt("u_num_sample", num);
    }
}

void LaserRayFilter::SetWeight(float w)
{
    weight = w;
    if (_state)
    {
        _state->setUniformFloat("u_Weight", 0.1f);
    }
}

