/********************************************************************
created:	2015/9/24
author:		ZP
desc:		�����������
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

	cc2d::Node *m_receiveTaskPanel;		//�����
};

#endif	//__UIRECEIVETASK_H__