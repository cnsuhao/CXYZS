/********************************************************************
created:	2014/12/6
author:		LSH
desc:		邮件面板
*********************************************************************/

#ifndef __UIEMAIL_H__
#define __UIEMAIL_H__

#include "UIBase.h"

class UIEmail : public UIBase
{
public:
	UIEmail();
	~UIEmail();	
	virtual bool init();
	CREATE_FUNC(UIEmail);
	virtual bool DisabledJoystickWhenSelfShow() { return true; }

private:
	//主面板
	cc2d::Node *m_emailPanel;

	cc2dui::CheckBox *m_inbox;
	cc2dui::CheckBox *m_unread;

	//模板
	cc2dui::Layout *m_Template;

	cc2dui::ScrollView *m_emailSV;
	void AddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout);
};

#endif	//__UIEMAIL_H__