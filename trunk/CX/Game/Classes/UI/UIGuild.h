/********************************************************************
created:	2014/12/6
author:		LSH
desc:		帮派面板
*********************************************************************/

#ifndef __UIGUILD_H__
#define __UIGUILD_H__

#include "UIBase.h"
#include "UIGrid.h"

struct GangLogInfo;

struct tagBt
{
	cc2dui::Button* RefusedBT;
	cc2dui::Button*	AgreeBT;
};

class UIGuild : public UIBase
{
public:
	UIGuild();
	~UIGuild();	
	virtual bool init();
	CREATE_FUNC(UIGuild);
	virtual bool DisabledJoystickWhenSelfShow() { return true; }
	//virtual void OnShow();      //>显示回调,子类重载记得调用父类方法
private:
	//主面板
	cc2d::Node *m_gangPanel;

	//添加CHECKBOX
	bool InitCheckBox();

	//CheckBox
	cc2d::Vector<cc2dui::CheckBox*> m_vecGang;
	void ShowCheckBoxUI(Ref* _sender, cc2dui::CheckBox::EventType type);

	//帮会信息
	cc2dui::Layout *m_gangInfoPanel;
	cc2dui::Layout *m_gangAppPanel;

	//小窗口
	cc2dui::Layout *m_gangListPanel;
	cc2dui::Layout *m_gangContriPanel;
	cc2dui::Layout *m_gangAnnounPanel;

	//初始化帮会信息,帮会信息模板
	void InitGangInfo();
	cc2dui::Layout *m_gangInfoTemplate;
	cc2dui::ScrollView *m_gangInfoSV;
	void GangInfoAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangMember *gangMemberTemp);
	void EditAnnounce();
	cc2dui::TextField* m_InputField;
	cc2dui::Layout *m_gangInfoAnnounce;
	map<int64, cc2dui::CheckBox*> m_gangMemberInfo;
	cc2dui::Layout *m_editMemberInfo;
	void EditMemberInfo();
	int64 m_gangInfoSelectid;
	int8 m_selectFun;

	//初始化帮会日志，帮会申请列表
	void InitGangApp();
	cc2dui::Layout *m_ganglogTemplate;
	cc2dui::ScrollView *m_ganglogSV;
	void GanglogAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangLogInfo *gangLog);
	cc2dui::Layout *m_gangAppTemplate;
	cc2dui::ScrollView *m_gangAppSV;
	void GangAppAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangMember *gangMemberTemp);
	map<int64,tagBt> m_mapBT;
	//virtual void Reload(unsigned int rowIndex,  cc2d::ui::Widget* cellTemplate);

public:
	void UpdateList();
};

#endif	//__UIGUILD_H__