/********************************************************************
created:	2015/10/31
author:		LL
desc:		������
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
	virtual void ScaleCallBack(cc2d::Node *sender, long data); //�ƶ��лص�
private:
	//�����
	cc2d::Node *m_wingPanel;
};

#endif	//__UICHEST_H__