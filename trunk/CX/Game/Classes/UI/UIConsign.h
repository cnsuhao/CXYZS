/********************************************************************
created:	2015/07/24	
author:		zp
desc:		���������
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
	//�����
	cc2d::Node *m_consignPanel;

	void InitLeft();
	//ģ��
	cc2dui::Layout *m_leftTemplate;
	cc2dui::ScrollView *m_leftSV;
	void AddleftItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,bool bIsmain);

	void InitRight();
	//ģ��
	cc2dui::Layout *m_rightTemplate;
	cc2dui::ScrollView *m_rightSV;
	void AddrightItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout);
};

#endif	//__UICONSIGN_H__