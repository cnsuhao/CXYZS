#include "UIGuild.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "NetManager.h"



using namespace cocostudio;

UIGuild::UIGuild()
{
	m_gangInfoSelectid = 0;
	m_mapBT.clear();
}

UIGuild::~UIGuild()
{
}

bool UIGuild::init()
{
    bool bRet = false;
    do 
    {
        Size winSize = Director::getInstance()->getVisibleSize();

		if (!InitCheckBox())
			return false;

        bRet = true;
    } while (0);

    return bRet;
}

void UIGuild::ShowCheckBoxUI(Ref* _sender,CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
	{
		CheckBox* tcb =  static_cast<CheckBox*>(_sender);
		cc2d::Vector<CheckBox*>::iterator it =  m_vecGang.begin();
		for (; it != m_vecGang.end(); it++)
		{
			(*it)->setSelected(false);
		}
		
		m_gangInfoPanel->setVisible(false);
		m_gangAppPanel->setVisible(false);

		tcb->setSelected(true);

		if (tcb == static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_other")))
		{
			m_gangInfoPanel->setVisible(true);
		}
		else if (tcb == static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_property")))
		{
			m_gangAppPanel->setVisible(true);
		}

	}
}

bool UIGuild::InitCheckBox()
{
	//加载主面板
	m_gangPanel = CSLoader::createNode("UI/Gang.csb");
	if (m_gangPanel == NULL)	return false;
	addChild(m_gangPanel);

	Button *btn = nullptr;
	//关闭按钮
	btn = static_cast<Button*>(m_gangPanel->getChildByName("bt_close"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			g_UIManager->HideUI(UI_GUILD);
		});
	}

	CheckBox *cb;
	cb =  static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_other"));
	if (cb)
	{
		m_vecGang.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	cb =  static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_property"));
	if (cb)
	{
		m_vecGang.pushBack(cb);
		cb->setSelected(false);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	cb =  static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_welfare"));
	if (cb)
	{
		cb->setVisible(false);
		(static_cast<cc2dui::Text*>(m_gangPanel->getChildByName("tx_welfare")))->setVisible(false);
		m_vecGang.pushBack(cb);
		cb->setSelected(false);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	cb =  static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_task"));
	if (cb)
	{
		cb->setVisible(false);
		(static_cast<cc2dui::Text*>(m_gangPanel->getChildByName("tx_task")))->setVisible(false);
		m_vecGang.pushBack(cb);
		cb->setSelected(false);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	cb =  static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_fight"));
	if (cb)
	{
		cb->setVisible(false);
		(static_cast<cc2dui::Text*>(m_gangPanel->getChildByName("tx_fight")))->setVisible(false);
		m_vecGang.pushBack(cb);
		cb->setSelected(false);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	cb =  static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_fight_Copy"));
	if (cb)
	{
		cb->setVisible(false);
		(static_cast<cc2dui::Text*>(m_gangPanel->getChildByName("tx_fight_Copy")))->setVisible(false);
		m_vecGang.pushBack(cb);
		cb->setSelected(false);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	cb =  static_cast<cc2dui::CheckBox*>(m_gangPanel->getChildByName("cb_fight_Copy_Copy"));
	if (cb)
	{
		cb->setVisible(false);
		(static_cast<cc2dui::Text*>(m_gangPanel->getChildByName("tx_fight_Copy_Copy")))->setVisible(false);
		m_vecGang.pushBack(cb);
		cb->setSelected(false);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	//初始化帮会信息
	m_gangInfoPanel = static_cast<cc2dui::Layout*>(m_gangPanel->getChildByName("Panel_3"));
	m_gangInfoPanel->setVisible(true);
	InitGangInfo();

	//初始化帮会审批
	m_gangAppPanel = static_cast<cc2dui::Layout*>(m_gangPanel->getChildByName("Panel_7"));
	m_gangAppPanel->setVisible(false);
	InitGangApp();

	m_gangListPanel = static_cast<cc2dui::Layout*>(m_gangPanel->getChildByName("Panel_12"));
	m_gangListPanel->setVisible(false);
	m_gangContriPanel = static_cast<cc2dui::Layout*>(m_gangPanel->getChildByName("Panel_13"));
	m_gangContriPanel->setVisible(false);
	m_gangAnnounPanel = static_cast<cc2dui::Layout*>(m_gangPanel->getChildByName("Panel_14"));
	m_gangAnnounPanel->setVisible(false);

	return true;
}

void UIGuild::InitGangInfo()
{
	//模板
	m_gangInfoTemplate = static_cast<Layout*>(m_gangInfoPanel->getChildByName("Panel_4"));
	m_gangInfoTemplate->setVisible(false);

	//滚动
	m_gangInfoSV = static_cast<cc2dui::ScrollView*>(m_gangInfoPanel->getChildByName("ScrollView_2"));
	m_gangInfoSV ->setSwallowTouches(true);

	//成员信息编辑
	m_editMemberInfo = static_cast<Layout*>(m_gangPanel->getChildByName("Panel_12"));

	//编辑公告
	EditAnnounce();
}

void UIGuild::GangInfoAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangMember *gangMemberTemp)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	CheckBox *cb = static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("CheckBox_9"));
	if (cb)
	{
		cb->setSelected(false);
		m_gangMemberInfo.insert(pair<int64,CheckBox*>(gangMemberTemp->id,cb));
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				map<int64,CheckBox*>::iterator it = m_gangMemberInfo.begin();
				for (; it != m_gangMemberInfo.end(); it++)
				{
					if (tcb == it->second)
					{
						m_gangInfoSelectid = it->first;
					}
					it->second->setSelected(false);
				}
				tcb->setSelected(true);
			}
			EditMemberInfo();
		});
	}

	(static_cast<cc2dui::Text*>(newLayout->getChildByName("Text_41")))->setString(BaseFunction::a2u(gangMemberTemp->szName));
	char szTemp[50];
	memset(szTemp,0,50);
	sprintf(szTemp,"%d",gangMemberTemp->iLevel);
	(static_cast<cc2dui::Text*>(newLayout->getChildByName("Text_41_Copy")))->setString(szTemp);

	if (gangMemberTemp->id  == g_LogicManager->m_Hero->m_GangInfo.m_iHeaderid)
	{
		(static_cast<cc2dui::Text*>(newLayout->getChildByName("Text_41_Copy_Copy")))->setString(BaseFunction::a2u("帮主"));
	}
	else
	{
		(static_cast<cc2dui::Text*>(newLayout->getChildByName("Text_41_Copy_Copy")))->setString(BaseFunction::a2u("成员"));
	}

	sv->addChild(newLayout);
}

void UIGuild::InitGangApp()
{
	//日志
	m_ganglogTemplate = static_cast<Layout*>(m_gangAppPanel->getChildByName("Panel_9"));
	m_ganglogTemplate->setVisible(false);
	m_ganglogSV = static_cast<cc2dui::ScrollView*>(m_gangAppPanel->getChildByName("ScrollView_4"));
	m_ganglogSV ->setSwallowTouches(true);

	//申请
	m_gangAppTemplate = static_cast<Layout*>(m_gangAppPanel->getChildByName("Panel_11"));
	m_gangAppTemplate->setVisible(false);
	m_gangAppSV = static_cast<cc2dui::ScrollView*>(m_gangAppPanel->getChildByName("ScrollView_5"));
	m_gangAppSV ->setSwallowTouches(true);

	
}

void UIGuild::GangAppAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangMember *gangMemberTemp)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	(static_cast<Text*>(newLayout->getChildByName("Text_59")))->setString(gangMemberTemp->szName);

	char szTemp[50];
	memset(szTemp,0,50);
	sprintf(szTemp,"%d",gangMemberTemp->iLevel);
	(static_cast<Text*>(newLayout->getChildByName("Text_59_Copy")))->setString(BaseFunction::a2u(szTemp));

	tagBt tagBtTemp;
	Button *btn;
	btn	= static_cast<Button*>(newLayout->getChildByName("Button_13"));
	tagBtTemp.RefusedBT = btn;
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			int64 iTemp = 0;
			map<int64,tagBt>::iterator itt = m_mapBT.begin();
			for (; itt != m_mapBT.end() ; ++itt)
			{
				if (itt->second.RefusedBT == static_cast<Button*>(_sender))
				{
					iTemp = itt->first;
					MsgGang Msg(g_LogicManager->m_Hero->m_isocietyID,itt->first,0,"");
					Msg.MsgID = Msg_Reply_ApplyGang;
					Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
					g_NetManager->SendMsg(Msg);
					break;
				}
			}

			map<int64,GangMember>::iterator itEx = g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.find(iTemp);
			if(itEx != g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.end())
			{
				g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.erase(itEx);
			}
			UpdateList();
		});
	}

	btn = static_cast<Button*>(newLayout->getChildByName("Button_13_Copy"));
	tagBtTemp.AgreeBT = btn;
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			int64 iTemp = 0;
			map<int64,tagBt>::iterator itt = m_mapBT.begin();
			for (; itt != m_mapBT.end() ; ++itt)
			{
				if (itt->second.AgreeBT == static_cast<Button*>(_sender))
				{
					iTemp = itt->first;
					MsgGang Msg(g_LogicManager->m_Hero->m_isocietyID,itt->first,1,"");
					Msg.MsgID = Msg_Reply_ApplyGang;
					Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
					g_NetManager->SendMsg(Msg);
					break;
				}
			}
			map<int64,GangMember>::iterator itEx = g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.find(iTemp);
			if(itEx != g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.end())
			{
				g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.erase(itEx);
			}
			UpdateList();
		});
	}
	
	m_mapBT.insert(pair<int64,tagBt>(gangMemberTemp->id,tagBtTemp));
	sv->addChild(newLayout);
}

void UIGuild::GanglogAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangLogInfo *gangLog)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	static_cast<Text*>(newLayout->getChildByName("Text_54"))->setString(BaseFunction::a2u(gangLog->recordContent));
	static_cast<Text*>(newLayout->getChildByName("Text_55"))->setString(BaseFunction::a2u(gangLog->recordTime));

	sv->addChild(newLayout);
}

void UIGuild::UpdateList()
{
	//申请信息
	m_mapBT.clear();
	m_gangAppSV->removeAllChildrenWithCleanup(true);
	map<int64,GangMember>::iterator itt = g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.begin();
	for (; itt != g_LogicManager->m_Hero->m_GangInfo.m_mapApplys.end(); ++itt)
	{
		GangAppAddItem(m_gangAppSV,m_gangAppTemplate->clone(),&itt->second);
	}

	m_gangAppSV->setLayoutType(Layout::Type::VERTICAL);
	m_gangAppSV->forceDoLayout();
	Size gangSizeEx = m_gangAppSV->getInnerContainerSize();
	gangSizeEx.height = m_gangAppSV->getChildrenCount() * m_gangAppTemplate->getContentSize().height;
	m_gangAppSV->setInnerContainerSize(gangSizeEx);

	//帮派人物
	m_gangInfoSelectid = 0;
	m_gangMemberInfo.clear();
	m_gangInfoSV->removeAllChildrenWithCleanup(true);
	map<int64,GangMember>::iterator ittex = g_LogicManager->m_Hero->m_GangInfo.m_mapMembers.begin();
	for (; ittex != g_LogicManager->m_Hero->m_GangInfo.m_mapMembers.end(); ++ittex)
	{
		GangInfoAddItem(m_gangInfoSV,m_gangInfoTemplate->clone(),&ittex->second);
	}

	m_gangInfoSV->setLayoutType(Layout::Type::VERTICAL);
	m_gangInfoSV->forceDoLayout();
	Size gangSize = m_gangInfoSV->getInnerContainerSize();
	gangSize.height = m_gangInfoSV->getChildrenCount() * m_gangInfoTemplate->getContentSize().height;
	m_gangInfoSV->setInnerContainerSize(gangSize);

	//日志信息
	m_ganglogSV->removeAllChildrenWithCleanup(true);
	vector<GangLogInfo>::iterator itlog = g_LogicManager->m_Hero->m_GangInfo.m_vecGangLog.begin();
	for (; itlog != g_LogicManager->m_Hero->m_GangInfo.m_vecGangLog.end(); ++itlog)
	{
		GanglogAddItem(m_ganglogSV,m_ganglogTemplate->clone(),&(*itlog));
	}

	m_ganglogSV->setLayoutType(Layout::Type::VERTICAL);
	m_ganglogSV->forceDoLayout();
	Size gangSizelog = m_ganglogSV->getInnerContainerSize();
	gangSizelog.height = m_ganglogSV->getChildrenCount() * m_ganglogTemplate->getContentSize().height;
	m_ganglogSV->setInnerContainerSize(gangSizelog);


	//帮派的基本情况
	char szTemp[50];
	memset(szTemp,0,50);
	//帮派公告
	(static_cast<cc2dui::Text*>(m_gangInfoPanel->getChildByName("Text_35")))->setString(g_LogicManager->m_Hero->m_GangInfo.m_szAccounce);
	//隐藏转让帮主按钮
	(static_cast<cc2dui::Button*>(m_gangInfoPanel->getChildByName("Button_3")))->setVisible(false);
	//帮派名称
	(static_cast<cc2dui::Text*>(m_gangInfoPanel->getChildByName("Text_26")))->setString(BaseFunction::a2u(g_LogicManager->m_Hero->m_GangInfo.m_szName));
	//帮派等级
	sprintf(szTemp,"%d",g_LogicManager->m_Hero->m_GangInfo.m_iLevel);
	(static_cast<cc2dui::Text*>(m_gangInfoPanel->getChildByName("Text_26_Copy")))->setString(szTemp);
	//帮主名称
	(static_cast<cc2dui::Text*>(m_gangInfoPanel->getChildByName("Text_26_Copy_1")))->setString(BaseFunction::a2u(g_LogicManager->m_Hero->m_GangInfo.m_szHeaderName));
	//帮派公告
	(static_cast<cc2dui::Text*>(m_gangInfoPanel->getChildByName("Text_35")))->setString(BaseFunction::a2u(g_LogicManager->m_Hero->m_GangInfo.m_szAccounce));
}

void UIGuild::EditAnnounce()
{
	m_gangInfoAnnounce = static_cast<Layout*>(m_gangPanel->getChildByName("Panel_14"));
	m_gangInfoAnnounce->setVisible(false);

	//输入框
	m_InputField = static_cast<TextField*>(m_gangInfoAnnounce->getChildByName("TextField_3"));

	Button *btn;
	//修改按钮
	btn = static_cast<cc2dui::Button*>(m_gangInfoPanel->getChildByName("Button_6"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			m_selectFun = 0;
			static_cast<cc2dui::Text*>(m_gangInfoAnnounce->getChildByName("Text_66"))->setString(BaseFunction::a2u("公告"));
			static_cast<cc2dui::TextField*>(m_gangInfoAnnounce->getChildByName("TextField_3"))->setString("");
			static_cast<cc2dui::Button*>(m_gangInfoAnnounce->getChildByName("Text_67"))->setVisible(false);
			m_gangInfoAnnounce->setVisible(true);
		});
	}

	//离开帮派
	btn = static_cast<cc2dui::Button*>(m_gangInfoPanel->getChildByName("Button_7"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			m_selectFun = 1;
			static_cast<cc2dui::Text*>(m_gangInfoAnnounce->getChildByName("Text_66"))->setString(BaseFunction::a2u("退出帮派"));
			static_cast<cc2dui::TextField*>(m_gangInfoAnnounce->getChildByName("TextField_3"))->setString(BaseFunction::a2u("确认要退出帮会吗？"));
			static_cast<cc2dui::Button*>(m_gangInfoAnnounce->getChildByName("Text_67"))->setVisible(true);
			m_gangInfoAnnounce->setVisible(true);
		});
	}

	//确定按钮
	btn = static_cast<cc2dui::Button*>(m_gangInfoAnnounce->getChildByName("Button_27"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			if (m_selectFun == 0)
			{
				const std::string& content = m_InputField->getString();
				string strTemp;
				strTemp = BaseFunction::u2a(content.c_str());

				MsgGang Msg(g_LogicManager->m_Hero->m_isocietyID,g_LogicManager->m_Hero->m_GUID,0,strTemp.c_str());
				Msg.MsgID = Msg_Edit_GangAnnounce;
				Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
				g_NetManager->SendMsg(Msg);
				m_gangInfoAnnounce->setVisible(false);
			}
			else if (m_selectFun == 1)
			{
				MsgGang Msg(g_LogicManager->m_Hero->m_isocietyID,g_LogicManager->m_Hero->m_GUID,0,"");
				Msg.MsgID = Msg_Leave_Gang;
				Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
				g_NetManager->SendMsg(Msg);

				m_gangInfoAnnounce->setVisible(false);
				g_UIManager->HideUI(UI_GUILD);
			}
			else if (m_selectFun == 2)
			{
				MsgGang Msg(g_LogicManager->m_Hero->m_isocietyID,m_gangInfoSelectid,g_LogicManager->m_Hero->m_GUID,"");
				Msg.MsgID = Msg_Transfer_GangHeader;
				Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
				g_NetManager->SendMsg(Msg);
				m_gangInfoAnnounce->setVisible(false);
			}
		});
	}

	//取消按钮
	btn = static_cast<cc2dui::Button*>(m_gangInfoAnnounce->getChildByName("Button_27_Copy"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			m_gangInfoAnnounce->setVisible(false);
		});
	}
}

void UIGuild::EditMemberInfo()
{
	if (m_gangInfoSelectid != 0)
	{
		m_editMemberInfo->setVisible(true);

		Button *btn;
		//查看信息
		btn = static_cast<cc2dui::Button*>(m_editMemberInfo->getChildByName("Button_17"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				m_editMemberInfo->setVisible(false);
			});
		}
		//加为好友
		btn = static_cast<cc2dui::Button*>(m_editMemberInfo->getChildByName("Button_17_Copy"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				if (g_LogicManager->m_Hero->m_GUID != m_gangInfoSelectid)
				{
					//自己的ID，对方ID
					MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,m_gangInfoSelectid, Relation_Friends,"");	
					Msg.MsgID = Msg_Add_Friends;
					g_NetManager->SendMsg(Msg);
				}
				m_editMemberInfo->setVisible(false);
			});
		}
		//发起私聊
		btn = static_cast<cc2dui::Button*>(m_editMemberInfo->getChildByName("Button_17_Copy_0"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_GUILD);
				g_UIManager->OpenUI(UI_CHAT);

				m_editMemberInfo->setVisible(false);
			});
		}
		//提出帮派
		btn = static_cast<cc2dui::Button*>(m_editMemberInfo->getChildByName("Button_17_Copy_2"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				MsgGang Msg(g_LogicManager->m_Hero->m_isocietyID,m_gangInfoSelectid,g_LogicManager->m_Hero->m_GUID,"");
				Msg.MsgID = Msg_Put_Gang;
				Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
				g_NetManager->SendMsg(Msg);

				m_editMemberInfo->setVisible(false);
			});
		}
		//转让帮主
		btn = static_cast<cc2dui::Button*>(m_editMemberInfo->getChildByName("Button_17_Copy_3"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				if (m_gangInfoSelectid != g_LogicManager->m_Hero->m_GUID)
				{
					m_editMemberInfo->setVisible(false);

					m_selectFun = 2;
					static_cast<cc2dui::Text*>(m_gangInfoAnnounce->getChildByName("Text_66"))->setString(BaseFunction::a2u("转让帮主"));
					static_cast<cc2dui::TextField*>(m_gangInfoAnnounce->getChildByName("TextField_3"))->setString(BaseFunction::a2u("你确认要转让帮主？"));
					static_cast<cc2dui::Button*>(m_gangInfoAnnounce->getChildByName("Text_67"))->setVisible(false);
					m_gangInfoAnnounce->setVisible(true);
				}
			});
		}
		//取消
		btn = static_cast<cc2dui::Button*>(m_editMemberInfo->getChildByName("Button_17_Copy_4"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				m_editMemberInfo->setVisible(false);
			});
		}
	}
}