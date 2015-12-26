#include "UIMsgBox.h"


USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "SceneManager.h"
#include "LogicManager.h"
#include "NetManager.h"
#include "CCSGUIReader.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "Hero.h"

UIMsgBox::UIMsgBox()
{	
}

UIMsgBox::~UIMsgBox()
{
}

UIMsgBox * UIMsgBox::create(const char* msg, bool isutf8, MsgBoxEventType eventtype, MsgBoxStyle style, long long value)
{
	UIMsgBox* widget = new UIMsgBox();
	if (!widget)
	{
		return NULL;
	}
	widget->box_style = style;
	widget->event_type = eventtype;	
	widget->m_Value = value;
	if (widget->init())
	{
		if (isutf8)
		{
			widget->msg_text->setString(msg);
		}
		else
		{
			widget->msg_text->setString(BaseFunction::a2u(msg).c_str());
		}
		
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return NULL;
}

void UIMsgBox::Release()
{	
	this->removeFromParentAndCleanup(true);	
}


bool UIMsgBox::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!UIBase::init());
		setContentSize(Size(g_ScreenSize));
		setVisible(true);		
		setColor(Color3B(20,20,20));
		setTouchEnabled(true);
		//载入		
		cc2d::Node* ui = CSLoader::createNode("UI/MsgBox.csb");	
		addChild(ui);		
		Button* confirm_button1 = dynamic_cast<Button*>(ui->getChildByName("Button_2"));
		Button* confirm_button = dynamic_cast<Button*>(ui->getChildByName("Button_1"));
		Button* cancel_button = dynamic_cast<Button*>(ui->getChildByName("Button_3"));
		msg_text = dynamic_cast<Text*>(ui->getChildByName("Text_1"));		
		switch (box_style)
		{
		case MSGBOX_STYLE_NULL:
			{
				confirm_button1->setVisible(false);	
				confirm_button->setVisible(false);	
				cancel_button->setVisible(false);
			}
			break;
		case MSGBOX_STYLE_CONFIRM:
			{
				confirm_button1->setVisible(true);	
				confirm_button->setVisible(false);	
				cancel_button->setVisible(false);
				confirm_button1->addTouchEventListener(this, SEL_TouchEvent(&UIMsgBox::ConfirmCallback));
				
			}
			break;
		case MSGBOX_STYLE_CONFIRMANDCANCEL:
			{
				confirm_button1->setVisible(false);	
				confirm_button->setVisible(true);	
				cancel_button->setVisible(true);
				confirm_button->addTouchEventListener(this, SEL_TouchEvent(&UIMsgBox::ConfirmCallback));
				cancel_button->addTouchEventListener(this, SEL_TouchEvent(&UIMsgBox::CancelCallback));
			}
			break;
		default:
			break;
		} 		
		
		bRet = true;
	} while (0);

	return bRet;
}

void UIMsgBox::ConfirmCallback(Ref *pSender, ui::TouchEventType type)
{
	if (type != ui::TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}

	switch (event_type)
	{
	
	case MSGBOX_EVENT_EXIT:
		Director::getInstance()->end();
		break;
	case MSGBOX_EVENT_LINK:
		{
			g_NetManager->Connect();
			g_NetManager->m_CutTip = NULL;
		
		}
		break;		
	case MSGBOX_EVENT_RETURNLOGIN:
		{
			if (g_SceneManager->m_CurrLayer == g_SceneManager->m_LoginLayer)
			{
				g_UIManager->RemoveAllUI();
				g_UIManager->OpenUI(UI_LOGIN);
			}
			if (g_SceneManager->m_CurrLayer == g_SceneManager->m_MainLayer)
			{
				g_SceneManager->Main2Login();
			}
			
			g_NetManager->m_ReturnLoginTip = NULL;
		}
		break;
	case MSGBOX_EVENT_CREATEROLEOK:
		{
            MsgRole_EnterMap enterMap;
            g_NetManager->SendMsg(enterMap);
		}
		break;
	case MSGBOX_EVENT_ADDFRIEND:
		{
			//自己的ID，对方ID
			MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,m_Value, Relation_Friends,"");	
			Msg.MsgID = Msg_Add_Friends;
			g_NetManager->SendMsg(Msg);
		}
		break;
	case MSGBOX_EVENT_RELIVE:
		{
			MsgC2GPlayer_Relive send;
			send.OBJID = g_LogicManager->m_Hero->m_GUID;
			send.relive_type = 1;
			g_NetManager->SendMsg(send);
		}
		break;
    case MSGBOX_EVENT_TEAM_APPLAY :
        {
            struct MsgTeam_Apply* pApplyMsg = g_NetManager->m_LastTeamInfo;
            if (pApplyMsg->iRequesterId)
            {
                MsgTeam_ReplyApply replyMsg(pApplyMsg->iRequesterId, pApplyMsg->szRequesterName, pApplyMsg->iPartnerId, "unknwon", 0);
                replyMsg.SysID = ID_C2G;
                replyMsg.OBJID = g_LogicManager->m_Hero->m_GUID;
                g_NetManager->SendMsg(replyMsg);
                g_NetManager->m_LastApplyMsgBox = NULL;
            }

        }break;
	default:
		break;
	}

	Release();
}

void UIMsgBox::CancelCallback(Ref *pSender, ui::TouchEventType type)
{
	if (type != ui::TouchEventType::TOUCH_EVENT_ENDED)
	{
		return;
	}
	switch (event_type)
	{
	case MSGBOX_EVENT_NORMAL:	
		break;	
	default:
		break;
	}	
	Release();

    if (g_NetManager->m_LastApplyMsgBox == this)
        g_NetManager->m_LastApplyMsgBox = NULL;
}
