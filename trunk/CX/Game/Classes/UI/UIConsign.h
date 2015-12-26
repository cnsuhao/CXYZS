/********************************************************************
created:	2015/07/24	
author:		zp
desc:		寄售行面板
*********************************************************************/

#ifndef __UICONSIGN_H__
#define __UICONSIGN_H__

#include "UIBase.h"

class UIConsign : public UIBase
{
public:
	UIConsign();
	~UIConsign();	
	virtual bool init();
	CREATE_FUNC(UIConsign);
	virtual bool DisabledJoystickWhenSelfShow() { return true; }

private:
	//主面板
	cc2d::Node *m_consignPanel;

	void InitLeft();
	//模板
	cc2dui::Layout *m_leftTemplate;
	cc2dui::ScrollView *m_leftSV;
	void AddleftItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,bool bIsmain);

	void InitRight();
	//模板
	cc2dui::Layout *m_rightTemplate;
	cc2dui::ScrollView *m_rightSV;
	void AddrightItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout);
};

#endif	//__UICONSIGN_H__