/********************************************************************
created:	2015/10/23
author:		zp
desc:		����������
*********************************************************************/

#ifndef __UIAPPLYGANG_H__
#define __UIAPPLYGANG_H__

#include "UIBase.h"

struct GangSimpleInfo;

class UIApplyGang : public UIBase
{
public:
	UIApplyGang();
	~UIApplyGang();	
	virtual bool init();
	CREATE_FUNC(UIApplyGang);

	virtual bool DisabledJoystickWhenSelfShow() { return true; }

	cc2d::Node *m_applyGangPanel;		//�����
	cc2d::Node *m_CreateGangPanel;		//�������

	//����б�
	void InitGangList();
	cc2dui::Layout *m_gangTemplate;
	cc2dui::ScrollView *m_gangSV;
	void GangAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangSimpleInfo *ganginfo);
	map<int64, cc2dui::Button*> m_mapGangListBt;

	void CrateGang();
	cc2dui::TextField* InputField;

	void UpdateList();
};

#endif	//__UIAPPLYGANG_H__