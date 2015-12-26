/********************************************************************
created:	2015/07/14
author:		zp
desc:		³á°ò
*********************************************************************/

#ifndef __UIWING_H__
#define __UIWING_H__

#include "UIBase.h"

class UIWing : public UIBase
{
public:
	UIWing();
	~UIWing();	
	virtual bool init();
	CREATE_FUNC(UIWing);
    virtual bool DisabledJoystickWhenSelfShow() { return true; }
private:
	//Ö÷Ãæ°å
	cc2d::Node *m_wingPanel;
};

#endif	//__UIWING_H__