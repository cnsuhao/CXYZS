/********************************************************************
created:	2014/12/6
author:		LSH
desc:		设置面板
*********************************************************************/

#ifndef __UISETTING_H__
#define __UISETTING_H__

#include "UIBase.h"

class UISetting : public UIBase
{
public:
	UISetting();
	~UISetting();	
	virtual bool init();
	CREATE_FUNC(UISetting);
	virtual bool DisabledJoystickWhenSelfShow() { return true; }
private:
	//主面板
	cc2d::Node *m_systemPanel;

	cc2dui::CheckBox *m_cbGangup;
	cc2dui::CheckBox *m_cbGame;
	cc2dui::CheckBox *m_cbMusic;
	cc2dui::CheckBox *m_cbAccount;

	cc2d::Node *m_panelGangup;
	cc2d::Node *m_panelGame;
	cc2d::Node *m_panelMusic;
	cc2d::Node *m_panelAccount;

};

#endif	//__UISETTING_H__