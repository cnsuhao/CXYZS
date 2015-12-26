#pragma once

/********************************************************************
created:	2015/5/16
author:		lishihai
desc:	    �һ��˾���
*********************************************************************/

#include "FilterBase.h"

// �һ��˾�Ч��.
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

