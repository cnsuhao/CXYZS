/********************************************************************
created:	2014/12/6
author:		LSH
desc:		二级菜单：所有二级面板按钮
*********************************************************************/

#ifndef __UIMENU_H__
#define __UIMENU_H__

#include "UIBase.h"

class UIMenu : public UIBase
{
public:
	UIMenu();
	~UIMenu();	
	virtual bool init();
	CREATE_FUNC(UIMenu);

    virtual bool DisabledJoystickWhenSelfShow() { return true; }

    //virtual void OnShow();
    //virtual void OnHide();
};

#endif	//__UIMENU_H__