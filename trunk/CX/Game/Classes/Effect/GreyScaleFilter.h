#pragma once

/********************************************************************
created:	2015/5/16
author:		lishihai
desc:	    灰化滤镜类
*********************************************************************/

#include "FilterBase.h"

// 灰化滤镜效果.
class GreyScaleFilter :  public FilterBase
{
protected:
    GreyScaleFilter(void);
public:
    
    ~GreyScaleFilter(void);

    static GreyScaleFilter* create();

    virtual const std::string& GetProgramKey() const override;

    virtual cc2d::GLProgram* CreateProgram();
};

