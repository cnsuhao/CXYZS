/********************************************************************
created:	2015/07/23	
author:		zp
desc:		���а����
*********************************************************************/

#ifndef __UILIST_H__
#define __UILIST_H__

#include "UIBase.h"

class UIList : public UIBase
{
public:
	UIList();
	~UIList();	
	virtual bool init();
	CREATE_FUNC(UIList);
	virtual bool DisabledJoystickWhenSelfShow() { return true; }

private:
	//�����
	cc2d::Node *m_listPanel;

//	CheckBox *m_inbox;
//	CheckBox *m_unread;
//
//	//ģ��
//	cc2dui::Layout *m_Template;
//
//	cc2dui::ScrollView *m_emailSV;
//	void AddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout);
};

#endif	//__UILIST_H__