/********************************************************************
created:	2015/9/24
author:		ZP
desc:		对话面板
*********************************************************************/

#ifndef __UITALK_H__
#define __UITALK_H__

#include "CSVFileManager.h"

#include "UIBase.h"

class UITalk : public UIBase
{
public:
	UITalk();
	~UITalk();	
	virtual bool init();
	CREATE_FUNC(UITalk);
	virtual bool DisabledJoystickWhenSelfShow() { return true; }

	cc2d::Node *m_talkPanel;		//主面板

	cc2dui::Text* m_txNPC;
	cc2dui::ImageView* m_imgNPC;
	cc2dui::Text* m_txNPCname;
	cc2dui::Text* m_txHERO;
	cc2dui::ImageView* m_imgHERO;
	cc2dui::Text* m_txHEROname;

	int32 m_iCurrProgress;
	int32 m_iAllProgress;

	cc2dui::Layout *m_bg;
	cc2dui::Layout *m_receive;

	const TalkInfo* m_ProtalkInfo;
	const TalkInfo* m_talkInfo;

	int32	m_iClickNpc;

	bool	m_IsPre;				//是否为前置对话
	bool	m_bIsTalk;				//是否为对话
	int32	m_iTaskid;				//任务编号
	bool	m_bIsReceiveTask;		//是否为领取任务，true为领取任务，false为交接任务

	void InitProgress(int32 taskid,int64 npcid,bool bIsReceiveTask);
	void PageClick();
	void GetTaskClick();
};

#endif	//__UITALK_H__