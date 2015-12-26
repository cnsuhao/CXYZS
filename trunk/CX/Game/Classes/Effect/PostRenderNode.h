#pragma once

/********************************************************************
created:	2015/6/15
author:		lishihai
desc:	    ������
*********************************************************************/

#include "ClientDef.h"
#include "FilterBase.h"
#include "CCNodeGrid.h"
#include "CCGrid.h"

class PostRenderNode : public cc2d::NodeGrid
{
    PostRenderNode(void);
public:
    
    ~PostRenderNode(void);

    static PostRenderNode* create();

    void SetActive(bool active);

    // ʹ���˾�
    void UseFilter(FilterBase* filter);
protected:
    cc2d::GridBase* _quad;
    FilterBase* _filter;
};

