#pragma once

/********************************************************************
created:	2015/5/15
author:		lishihai
desc:	    �˾��Ļ���
*********************************************************************/

#include "ClientDef.h"

// �˾��Ļ���.�������shaderע��,������Ч��
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

