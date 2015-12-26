/********************************************************************
created:	2015/10/31
author:		LL
desc:		抽箱子
*********************************************************************/

#ifndef __UICHEST_H__
#define __UICHEST_H__

#include "UIBase.h"

class UIChest : public UIBase
{
public:
	UIChest();
	~UIChest();	
	virtual bool init();
	CREATE_FUNC(UIChest);
	virtual void OnShow();
    void UpdateMoneyShow();
    virtual bool DisabledJoystickWhenSelfShow() { return true; }
	virtual void ScaleCallBack(cc2d::Node *sender, long data); //移动中回调
private:
	//主面板
	cc2d::Node *m_wingPanel;
};

#endif	//__UICHEST_H__