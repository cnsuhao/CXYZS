/********************************************************************
created:	2014/12/6
author:		LSH
desc:		聊天界面
*********************************************************************/

#ifndef __UICHAT_H__
#define __UICHAT_H__

#include "UIBase.h"
#include "UITextField.h"

class UIChat : public UIBase
{
    friend class ChatSystem;
public:
	UIChat();
	~UIChat();
	virtual bool init();
	CREATE_FUNC(UIChat);
    virtual bool DisabledJoystickWhenSelfShow() { return true; }
    virtual void OnShow();

    void SetChannel(Chat_Channel channel);
    Chat_Channel GetSeletedChannel() const { return Channel; }
   
    void OnAddMsg(const struct ChatMessage* chatMessage);
private:
    void OnSendMessage();
    void Reload();
	
	cc2d::Node *m_chatPanel;		//主面板

	cc2d::Vector<cc2dui::CheckBox*> m_Checkboxs;	//频道
	void AddCheckBoxItem(cc2dui::CheckBox *pcb);	//添加checkbox项
	void Loadchannel(Ref* _sender);			//载入到频道

	//Vector<MessageInfo> m_vecWorld;

	//vector<MessageInfo*> m_vecWorld;
	//vector<MessageInfo*> m_vecGang;
	//vector<MessageInfo*> m_vecTeam;
	//vector<MessageInfo*> m_vecCurr;
	//vector<MessageInfo*> m_vecSecret;
	//vector<MessageInfo*> m_vecSystem;
	//vector<MessageInfo*> m_vecCom
	//MessageInfo m_tagMessage;

    Chat_Channel Channel;

    cc2dui::CheckBox* pCheckBoxWorld;
    cc2dui::CheckBox* pCheckBoxSingleChat;
    cc2dui::CheckBox* pCheckBoxNear;
    cc2dui::CheckBox* pCheckBoxGuild;
    cc2dui::CheckBox* pCheckBoxFriend;
    cc2dui::CheckBox* pCheckBoxSystem;
    cc2dui::CheckBox* pCheckBoxTeam;
    cc2dui::CheckBox* pCheckBoxAll;
    cc2dui::Button* SendBtn;
    cc2dui::TextField* InputField;

    class CtrlRichText* _RichText;
};

#endif	//__UICHAT_H__