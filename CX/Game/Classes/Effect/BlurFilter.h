#pragma once

#include "FilterBase.h"

// Ä£ºýÂË¾µÐ§¹û.
class BlurFilter : public FilterBase
{
protected:
    BlurFilter(void);
public:
    
    ~BlurFilter(void);

    static BlurFilter* create();

    virtual const std::string& GetProgramKey() const override;

    virtual cc2d::GLProgram* CreateProgram();

    void SetResolution(const cc2d::Size& size);

    void SetBlurRadius(float radius);

    void SetSampleNum(float num);

protected:
    cc2d::Size _resolution;
    float _blurRadius;
    float _blurSampleNum;
};

