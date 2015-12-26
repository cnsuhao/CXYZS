/********************************************************************
created:	2014/12/6
author:		LSH
desc:		法宝二级界面：法宝，灵气关联界面
*********************************************************************/

#ifndef __UIFABAO_H__
#define __UIFABAO_H__

#include "UIBase.h"

struct MagicWeaponData;
struct MagicWeapon;

class UIFaBao : public UIBase
{
public:
	UIFaBao();
	~UIFaBao();	
	virtual bool init();
	CREATE_FUNC(UIFaBao);

	virtual bool DisabledJoystickWhenSelfShow() { return true; }
private:
	//主面板
	cc2d::Node *m_fabaoPanel;

	//4个子面板
	cc2d::Node *m_weasonPanel;
	cc2d::Node *m_absorbPanel;
	cc2d::Node *m_implementPanel;
	cc2d::Node *m_weaponList;
	cc2d::Node *m_weasonPanel_wea;
	cc2d::Node *m_weasonPanel_wash;

	//法宝列表模板
	cc2dui::Layout *m_Template;

	//主面板的checkbox
	cc2d::Vector<cc2dui::CheckBox*> m_mainChecks;
	void ShowCheckBoxUI(cc2d::Ref* _sender,cc2dui::CheckBox::EventType type);

	//加载各种面板
	bool LoadNode();
	//法宝列表载入
	void LoadWeaponList();

	//面板右边法宝列表及模本
	cc2dui::ScrollView *m_weasonSV;
	void AddWeasonRightItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,int pid,  MagicWeaponData *pMagicWD, bool bIsAct);
	map<int,cc2dui::CheckBox*> m_mapWeasonChecks;
	
	int m_iCurrSelect;
	//设置法宝及洗练面板
	void SetWeason(int pid);
	//设置吸收面板
	void SetAbsorb(int pid);
};

#endif	//__UIFABAO_H__