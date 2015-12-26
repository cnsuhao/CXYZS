/********************************************************************
created:	2014/12/6
author:		LSH
desc:		任务面板
*********************************************************************/

#ifndef __UITASK_H__
#define __UITASK_H__

#include "UIBase.h"

class UITask : public UIBase
{
public:
	UITask();
	~UITask();	
	virtual bool init();
	CREATE_FUNC(UITask);

	virtual bool DisabledJoystickWhenSelfShow() { return true; }

	void UpdateTaskList();			//更新任务列表

private:
	//主面板
	cc2d::Node *m_taskPanel;

	cc2dui::CheckBox *m_haveTask;
	cc2dui::CheckBox *m_acceptTask;

	//任务列表模板
	cc2dui::Layout *m_Template;

	//加载各种面板
	bool LoadNode();

	//面板左边任务列表及模本
	cc2dui::ScrollView *m_taskSV;
	void AddTaskItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,int pid);
	map<int32,cc2dui::CheckBox*> m_mapChecks;

	//当前选择的任务
	int32	m_iCurrSelectTask;
	//当前选择任务的状态
	TaskState	m_SelectTaskState;
	//右边显示任务详细
	void	ShowTaskDetailed();

	//按钮事件
	void	BtClick();

	//当前面板选择，0为已接任务，1为可接任务
	bool	m_currModelSelect;

	bool	m_bCurrSelect;
};

#endif	//__UITASK_H__