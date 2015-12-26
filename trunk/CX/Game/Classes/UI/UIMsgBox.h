/********************************************************************
created:	2014/12/5
author:		LL
desc:		消息提示框UI：在MessageBox的基础上增加了按钮回调，通过MsgBoxEventType来做不同的回调响应
*********************************************************************/
#ifndef __UI_MSGBOX_H__
#define __UI_MSGBOX_H__

#include "UIBase.h"


//按钮样式
enum MsgBoxStyle
{	
	MSGBOX_STYLE_NULL,				//没按钮，不由玩家主动关闭，需要手动调用Release
	MSGBOX_STYLE_CONFIRM,			//有确定按钮,点确定按钮会执行相应事件并自动释放
	MSGBOX_STYLE_CONFIRMANDCANCEL,	//有确定和取消按钮,点确定和取消按钮会执行相应事件并自动释放
};

//事件类型
enum MsgBoxEventType
{
	MSGBOX_EVENT_NORMAL,	//没有回调事件
	MSGBOX_EVENT_LINK,		//重新连接服务器
	MSGBOX_EVENT_EXIT,		//退出游戏确认
	MSGBOX_EVENT_DELMAIL,	//删除邮件确认
	MSGBOX_EVENT_RETURNLOGIN,	//服务器断开连接，需返回登陆界面
	MSGBOX_EVENT_CREATEROLEOK,	//创建角色成功确认，点确定直接进游戏
	MSGBOX_EVENT_ADDFRIEND,		//接到被添加为好友时，是否添加好友
	MSGBOX_EVENT_RELIVE,		//原地复活
    MSGBOX_EVENT_TEAM_APPLAY,   //队伍申请
};

class UIMsgBox :public UIBase
{
public:
	UIMsgBox(); 
	~UIMsgBox();
	
	//参数:事件类型，内容文本，标题文本,按钮样式,事件对应数值
	static UIMsgBox * create(const char* msg, bool isutf8 = true, MsgBoxEventType eventtype = MSGBOX_EVENT_NORMAL, MsgBoxStyle style = MSGBOX_STYLE_CONFIRM, long long value = -1);	
    void Release();	//释放UIMsgBox本身

	bool init();
private:
	void ConfirmCallback(cc2d::Ref *pSender, cc2dui::TouchEventType type);	//确定按钮回调
	void CancelCallback(cc2d::Ref *pSender, cc2dui::TouchEventType type);	//取消按钮回调	
	
	cc2dui::Text *msg_text;			//msg内容文本
	//Button *confirm_button;	//确定按钮
	//Button *cancel_button;	//取消按钮

	MsgBoxStyle box_style;		//按钮样式
	MsgBoxEventType event_type;	//事件类型
	long long m_Value;	//MsgBoxEventType事件对应的值，默认为-1
};

#endif	//__UI_MSGBOX_H__