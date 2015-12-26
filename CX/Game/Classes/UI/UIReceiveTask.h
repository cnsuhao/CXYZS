/********************************************************************
created:	2015/9/24
author:		ZP
desc:		接受任务面板
*********************************************************************/

#ifndef __UIRECEIVETASK_H__
#define __UIRECEIVETASK_H__

#include "UIBase.h"

class UIReceiveTask : public UIBase
{
public:
	UIReceiveTask();
	~UIReceiveTask();	
	virtual bool init();
	CREATE_FUNC(UIReceiveTask);

	cc2d::Node *m_receiveTaskPanel;		//主面板
};

#endif	//__UIRECEIVETASK_H__