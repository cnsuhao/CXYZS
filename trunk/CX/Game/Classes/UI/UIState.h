/********************************************************************
created:	2015/07/14
author:		zp
desc:		����
*********************************************************************/

#ifndef __UISTATE_H__
#define __UISTATE_H__

#include "UIBase.h"

class UIState : public UIBase
{
public:
	UIState();
	~UIState();	
	virtual bool init();
	CREATE_FUNC(UIState);

    virtual bool DisabledJoystickWhenSelfShow() { return true; }
private:
	//�����
	cc2d::Node *m_statePanel;
};

#endif	//__UISTATE_H__