#pragma once

/********************************************************************
created:	2015/5/15
author:		lishihai
desc:	    滤镜的基类
*********************************************************************/

#include "ClientDef.h"

// 滤镜的基类.帮助完成shader注册,本身并无效果
class FilterBase : public cc2d::Ref
{
protected:
    FilterBase(void);
public:
    virtual bool Init(); 
    
    virtual ~FilterBase(void);

    static FilterBase* create();

    virtual const std::string& GetProgramKey() const;

    virtual cc2d::GLProgram* CreateProgram();

    cc2d::GLProgramState* GetGLProgramState();

    virtual void Setup(class SpriteWithFilter* spr);
protected:
    cc2d::GLProgramState* _state;
};

