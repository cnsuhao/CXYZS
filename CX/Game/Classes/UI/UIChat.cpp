#include "UIChat.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "CtrlRichText.h"
#include "BaseFunction.h"
#include "ChatSystem.h"
#include "Hero.h"
#include "LogicManager.h"

UIChat::UIChat()
{
    pCheckBoxWorld = nullptr;
    pCheckBoxSingleChat = nullptr;
    pCheckBoxNear = nullptr;
    pCheckBoxGuild = nullptr;
    pCheckBoxFriend = nullptr;
    pCheckBoxSystem = nullptr;
    pCheckBoxTeam = nullptr;
    pCheckBoxAll = nullptr;
    SendBtn = nullptr;
    InputField = nullptr;
    _RichText = nullptr;
}

UIChat::~UIChat()
{
}

bool UIChat::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(!UIBase::init());
        m_chatPanel = CSLoader::createNode("UI/Chat.csb");	
        addChild(m_chatPanel);

        // 关闭按钮
        Button* pBtn = static_cast<Button*>(m_chatPanel->getChildByName("bt_close"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            g_UIManager->HideUI(UI_CHAT);
        });

        pCheckBoxWorld = static_cast<CheckBox*>(m_chatPanel->getChildByName("cb_world"));
        if (pCheckBoxWorld)
        {
            pCheckBoxWorld->setTag(Channel_World);
            AddCheckBoxItem(pCheckBoxWorld);
        }
        pCheckBoxGuild = static_cast<CheckBox*>(m_chatPanel->getChildByName("cb_gang"));
        if (pCheckBoxGuild)
        {
            pCheckBoxGuild->setTag(Channel_Guild);
            AddCheckBoxItem(pCheckBoxGuild);
        }
        pCheckBoxTeam = static_cast<CheckBox*>(m_chatPanel->getChildByName("cb_team"));
        if (pCheckBoxTeam)
        {
            pCheckBoxTeam->setTag(Channel_Team);
            AddCheckBoxItem(pCheckBoxTeam);
        }
        pCheckBoxNear = static_cast<CheckBox*>(m_chatPanel->getChildByName("cb_near"));
        if (pCheckBoxNear)
        {
            pCheckBoxNear->setTag(Channel_Near);
            AddCheckBoxItem(pCheckBoxNear);
        }
        pCheckBoxSingleChat = static_cast<CheckBox*>(m_chatPanel->getChildByName("cb_secret"));
        if (pCheckBoxSingleChat)
        {
            pCheckBoxSingleChat->setTag(Channel_SingleChat);
            AddCheckBoxItem(pCheckBoxSingleChat);
        }
        pCheckBoxSystem = static_cast<CheckBox*>(m_chatPanel->getChildByName("cb_system"));
        if (pCheckBoxSystem)
        {
            pCheckBoxSystem->setTag(Channel_System);
            AddCheckBoxItem(pCheckBoxSystem);
        }
        pCheckBoxAll = static_cast<CheckBox*>(m_chatPanel->getChildByName("cb_all"));
        if (pCheckBoxAll)
        {
            pCheckBoxAll->setTag(Channel_System + 1);
            AddCheckBoxItem(pCheckBoxAll);
        }

        SendBtn = static_cast<Button*>(m_chatPanel->getChildByName("bt_send"));
        if (SendBtn)
        {
            SendBtn->addClickEventListener([this](Ref* _sender)
            {
                OnSendMessage();
            });
        }

        InputField = static_cast<TextField*>(m_chatPanel->getChildByName("input_message"));

		Node* bg_panel = m_chatPanel->getChildByName("bg_panel");
		if (bg_panel)
		{
			Point p = bg_panel->getPosition();
			Size chat_bg_size = bg_panel->getContentSize();
			bg_panel->setVisible(false);

			_RichText = CtrlRichText::create(Size(chat_bg_size.width - 10, chat_bg_size.height - 10), p);
            if (_RichText)
            {
                _RichText->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
                _RichText->setDirection(extension::ScrollView::Direction::VERTICAL);
            }
		}
        
        addChild(_RichText);

        Channel = Channel_Near;

        bRet = true;
    } while (0);

    return bRet;
}

void UIChat::OnShow()
{
    UIBase::OnShow();
    SetChannel(Channel);
}

void UIChat::SetChannel(Chat_Channel channel)
{
    static const int Number = 8;
    static CheckBox* arrayChechBox[Number] = { NULL, pCheckBoxWorld, pCheckBoxSingleChat, pCheckBoxNear, pCheckBoxGuild, pCheckBoxTeam, pCheckBoxSystem, pCheckBoxAll  };

    int channelIdx = (int)channel;
    if (channelIdx >= Number)
        return;

    for (int c = 0; c < Number; c++)
    {
        if (arrayChechBox[c])
            arrayChechBox[c]->setSelected(false);
    }

    if (arrayChechBox[channelIdx])
    {
        arrayChechBox[channelIdx]->setSelected(true);
    }

    Channel = channel;


    InputField->setPlaceHolder("");

    if(Channel == Channel_System)
    {
        InputField->setPlaceHolder(BaseFunction::a2u("系统频道不能发言!"));
    }
    else if (Channel == Channel_Team)
    {
        if (!g_LogicManager->m_Hero->HasTeam())
        {
            InputField->setPlaceHolder(BaseFunction::a2u("你当前没有加入队伍!"));
        }
    }
    else if (Channel == Channel_Guild)
    {
        if (g_LogicManager->m_Hero->m_isocietyID == 0)
        {
            InputField->setPlaceHolder(BaseFunction::a2u("你当前没有加入帮派!"));
        }
    }

    Reload();
}

void UIChat::OnAddMsg(const ChatMessage* chatMessage)
{
    if (g_UIManager->IsOpened(UI_CHAT))
    {
        Reload();
    }
}

void UIChat::OnSendMessage()
{
    if (InputField)
    {
        std::string content = InputField->getString();
        if (!content.empty())
        {
            //>清空输入区域
            InputField->setString("");

            if (Channel == Channel_Team)
            {
                if (!g_LogicManager->m_Hero->HasTeam())
                {
                    g_UIManager->Notify(BaseFunction::a2u("发送失败,你当前没有加入队伍!"), Color3B::RED);
                    return;
                }
            }

            if (Channel == Channel_Guild)
            {
                if (g_LogicManager->m_Hero->m_isocietyID == 0)
                {
                    g_UIManager->Notify(BaseFunction::a2u("发送失败,你当前没有加入帮派!"), Color3B::RED);
                    return;
                }
            }

            if (Channel == Channel_System)
            {
                g_UIManager->Notify(BaseFunction::a2u("发送失败,系统频道不能发言!"), Color3B::RED);
                return;
            }

            g_ChatSystem.SendChatMsg(content, Channel, NULL);

            
            /*
            ChatMessage* msg = new ChatMessage();
            msg->channel = Channel_World;
            msg->senderId = g_LogicManager->m_Hero->m_GUID;
            msg->senderName = g_LogicManager->m_Hero->m_Name;
            msg->senderName.append(":");
            msg->msg.assign(BaseFunction::u2a(content));

			static string token("|");
			size_t pos = msg->msg.find(token, 0);
			while (pos != string::npos)
			{
				msg->appendix.push_back(pos);
				pos = msg->msg.find(token, pos + 1);
			}

			g_ChatSystem.AddMessage(msg);

            Reload();
			//>更新至界面
			UIMain *ui = (UIMain*)g_UIManager->GetUI(UI_MAIN);
			ui->UpdateChat(msg->channel,msg->senderName,msg->msg);

            */
        }
    }
}


void UIChat::Reload()
{
    if (_RichText)
    {
        _RichText->ReloadData();
        if(_RichText->getContentSize().height > _RichText->getViewSize().height)
        {
            _RichText->setContentOffset(Vec2(0,0));
        }
        else
        {
            _RichText->setContentOffset(Vec2(0,_RichText->minContainerOffset().y));
        }
    }
}

void UIChat::AddCheckBoxItem(CheckBox *pcb)
{
	if (pcb)
	{
		m_Checkboxs.pushBack(pcb);
		pcb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				tcb->setSelected(true);
                SetChannel((Chat_Channel)tcb->getTag());
			}
		});
	}
}

