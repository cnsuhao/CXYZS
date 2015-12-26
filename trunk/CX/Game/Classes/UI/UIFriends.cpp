#include "UIFriends.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "NetManager.h"
#include "StackStream.h"
#include "CSVFileManager.h"
#include "UITextField.h"

using namespace cocostudio;

UIFriends::UIFriends()
{

}

UIFriends::~UIFriends()
{
}

bool UIFriends::init()
{
bool bRet = false;
    do 
    {
		Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_friendPanel = CSLoader::createNode("UI/Friends.csb");
		if (m_friendPanel == NULL)	return false;
		addChild(m_friendPanel);

		InitLoad();

		//置空选择的对象
		m_iSelectRole = 0;

        bRet = true;
    } while (0);

    return bRet;
}

//加载各种面板及按钮
void UIFriends::InitLoad()
{
	m_friendlist_foot = static_cast<Layout*>(m_friendPanel->getChildByName("panel_friendlist_foot"));
	m_friendlist_foot->setVisible(true);
	m_blacklist_foot = static_cast<Layout*>(m_friendPanel->getChildByName("panel_blacklist_foot"));
	m_blacklist_foot->setVisible(false);
	m_search = static_cast<Layout*>(m_friendPanel->getChildByName("panel_search"));
	m_search->setVisible(false);
	m_search_ex = static_cast<Layout*>(m_search->getChildByName("panel_search_contente"));
	m_friendlist = static_cast<Layout*>(m_friendPanel->getChildByName("panel_list"));
	m_friendlist->setVisible(true);
	//模板
	m_Template = static_cast<Layout*>(m_friendPanel->getChildByName("panel_temp"));
	m_Template->setVisible(false);

	//列表
	m_friendSV = static_cast<cc2dui::ScrollView*>(m_friendlist->getChildByName("sv_list"));
	m_friendSV ->setSwallowTouches(true);

	//搜索列表
	m_FindfriendSV = static_cast<cc2dui::ScrollView*>(m_search_ex->getChildByName("sv_list_Copy"));
	m_FindfriendSV ->setSwallowTouches(true);

	Button* btn;

	//关闭按钮
	btn = static_cast<Button*>(m_friendPanel->getChildByName("bt_close"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			g_UIManager->HideUI(UI_FRIENDS);
		});
	}

	btn = static_cast<Button*>(m_search->getChildByName("bt_search"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			const std::string& content = static_cast<cc2dui::TextField*>(m_search->getChildByName("tx_edit"))->getString();
			char szName[MAX_NAME_SIZE];
			sprintf(szName,"%s",content.c_str());
			SearchRole(szName);
		});
	}

	btn = static_cast<Button*>(m_search->getChildByName("bt_addfri"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			AddFrinds();
		});
	}

	btn = static_cast<Button*>(m_search->getChildByName("bt_addblacklist"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			AddBlackList();
		});
	}

	btn = static_cast<Button*>(m_friendlist_foot->getChildByName("bt_del"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			DelFriends(Relation_Friends);
		});
	}

	btn = static_cast<Button*>(m_blacklist_foot->getChildByName("bt_del"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			DelFriends(Relation_BlackList);
		});
	}



	CheckBox *cb;
	//好友名单
	cb =  static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_friendlist"));
	if (cb)
	{
		m_mainChecks.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
			MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,0, Relation_Friends,"");	
			Msg.MsgID = Msg_Get_FriendsList;
			g_NetManager->SendMsg(Msg);
		});
	}
	//黑名单
	cb =  static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_blacklist"));
	if (cb)
	{
		m_mainChecks.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
			MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,0, Relation_BlackList,"");	
			Msg.MsgID = Msg_Get_FriendsList;
			g_NetManager->SendMsg(Msg);
		});
	}
	//玩家搜索
	cb =  static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_search"));
	if (cb)
	{
		m_mainChecks.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	(static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_friendlist")))->setSelected(true);
	(static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_blacklist")))->setSelected(false);
	(static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_search")))->setSelected(false);
}

void UIFriends::ShowCheckBoxUI(Ref* _sender,CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
	{
		CheckBox* tcb =  static_cast<CheckBox*>(_sender);
		cc2d::Vector<CheckBox*>::iterator it =  m_mainChecks.begin();
		for (; it != m_mainChecks.end(); it++)
		{
			(*it)->setSelected(false);
		}
		tcb->setSelected(true);

		if (tcb == static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_friendlist")))
		{
			m_friendlist_foot->setVisible(true);
			m_blacklist_foot->setVisible(false);
			m_search->setVisible(false);
			m_friendlist->setVisible(true);
		}
		if (tcb == static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_blacklist")))
		{
			m_friendlist_foot->setVisible(false);
			m_blacklist_foot->setVisible(true);
			m_search->setVisible(false);
			m_friendlist->setVisible(true);
		}
		if (tcb == static_cast<cc2dui::CheckBox*>(m_friendPanel->getChildByName("cb_search")))
		{
			m_friendlist_foot->setVisible(false);
			m_blacklist_foot->setVisible(false);
			m_search->setVisible(true);
			m_friendlist->setVisible(false);
		}
	}
}

void UIFriends::AddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout, friendInfo parTag)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	char szTemp[30];
	sprintf(szTemp,"%d",parTag.iLevel);
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_level"))->setString(szTemp);
	sprintf(szTemp,"%d",parTag.iFight);
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_fight"))->setString(szTemp);

	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_name"))->setString(BaseFunction::a2u(parTag.szName));

	if (parTag.bIsLine)
	{
        const TextInfo *pText = g_CSVFileManager->GetTextByID(668);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setString(BaseFunction::a2u(pText->strContent)); //在线

		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_level"))->setColor(Color3B(0,255,0));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_fight"))->setColor(Color3B(0,255,0));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_name"))->setColor(Color3B(0,255,0));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setColor(Color3B(0,255,0));
	}
	else
	{
        const TextInfo *pText = g_CSVFileManager->GetTextByID(669);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setString(BaseFunction::a2u(pText->strContent)); //离线

		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_level"))->setColor(Color3B(166,140,125));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_fight"))->setColor(Color3B(166,140,125));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_name"))->setColor(Color3B(166,140,125));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setColor(Color3B(166,140,125));
	}

	CheckBox *cb;
	cb =  static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("cb_project"));
	if (cb)
	{
		cb->setSelected(false);
		m_mapFriendList.insert(pair<int64,CheckBox*>(parTag.iRoleID,cb));
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				map<int64,CheckBox*>::iterator it =  m_mapFriendList.begin();
				for (; it != m_mapFriendList.end(); it++)
				{
					it->second->setSelected(false);

					if (it->second == tcb)
					{
						m_iSelectRole = it->first;
					}
				}
				tcb->setSelected(true);
			}
		});
	}

	sv->addChild(newLayout);
}

//搜索按钮
void UIFriends::SearchRole(char *szPar)
{
	//自己的ID，对方ID
	MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,0, Relation_Friends,szPar);	
	Msg.MsgID = Msg_Get_PlayInfo;

	g_NetManager->SendMsg(Msg);
}

//添加好友按钮
void UIFriends::AddFrinds()
{
	if (g_LogicManager->m_Hero->m_GUID != m_FindFriend.iRoleID)
	{
		//自己的ID，对方ID
		MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,m_FindFriend.iRoleID, Relation_Friends,"");	
		Msg.MsgID = Msg_Add_Friends;
		g_NetManager->SendMsg(Msg);
	}
}
//添加黑名单按钮
void UIFriends::AddBlackList()
{
	if (g_LogicManager->m_Hero->m_GUID != m_FindFriend.iRoleID)
	{
		MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,m_FindFriend.iRoleID, Relation_BlackList,"");	
		Msg.MsgID = Msg_Add_Friends;
		g_NetManager->SendMsg(Msg);
	}
}

//更新搜索界面
void UIFriends::UpdateSearch()
{
	m_FindfriendSV->removeAllChildrenWithCleanup(true);
	if (m_FindFriend.iRoleID != 0)
	{
		AddItemFind(m_FindfriendSV,m_Template->clone(),m_FindFriend);
	}
	m_FindfriendSV->setLayoutType(Layout::Type::VERTICAL);
	m_FindfriendSV->forceDoLayout();
	Size friendSize = m_FindfriendSV->getInnerContainerSize();
	friendSize.height = m_FindfriendSV->getChildrenCount() * m_Template->getContentSize().height;
	m_FindfriendSV->setInnerContainerSize(friendSize);
}

void UIFriends::AddItemFind(cc2dui::ScrollView* sv, cc2dui::Widget* layout,friendInfo parTag)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	char szTemp[30];
	sprintf(szTemp,"%d",parTag.iLevel);
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_level"))->setString(szTemp);
	sprintf(szTemp,"%d",parTag.iFight);
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_fight"))->setString(szTemp);

	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_name"))->setString(BaseFunction::a2u(parTag.szName));

	if (parTag.bIsLine)
	{
        const TextInfo *pText = g_CSVFileManager->GetTextByID(668);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setString(BaseFunction::a2u(pText->strContent)); //在线

		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_level"))->setColor(Color3B(0,255,0));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_fight"))->setColor(Color3B(0,255,0));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_name"))->setColor(Color3B(0,255,0));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setColor(Color3B(0,255,0));
	}
	else
	{
        const TextInfo *pText = g_CSVFileManager->GetTextByID(669);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setString(BaseFunction::a2u(pText->strContent)); //离线

		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_level"))->setColor(Color3B(166,140,125));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_fight"))->setColor(Color3B(166,140,125));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_name"))->setColor(Color3B(166,140,125));
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_state"))->setColor(Color3B(166,140,125));
	}
	sv->addChild(newLayout);
}

//更新好友列表
void UIFriends::UpdateFriendList()
{
	m_mapFriendList.clear();
	m_friendSV->removeAllChildrenWithCleanup(true);
	//置空选择的对象
	m_iSelectRole = 0;

	map<int64,friendInfo>::iterator it;
	for(it=g_LogicManager->m_Hero->m_mapFriendsInfo.begin();it!=g_LogicManager->m_Hero->m_mapFriendsInfo.end();++it)
	{
		AddItem(m_friendSV,m_Template->clone(),it->second);
	}
	m_friendSV->setLayoutType(Layout::Type::VERTICAL);
	m_friendSV->forceDoLayout();
	Size friendSize = m_friendSV->getInnerContainerSize();
	friendSize.height = m_friendSV->getChildrenCount() * m_Template->getContentSize().height;
	m_friendSV->setInnerContainerSize(friendSize);
}

//更新黑名单列表
void UIFriends::UpdateBlackList()
{
	m_mapFriendList.clear();
	m_friendSV->removeAllChildrenWithCleanup(true);
	//置空选择的对象
	m_iSelectRole = 0;

	map<int64,friendInfo>::iterator it;
	for(it=g_LogicManager->m_Hero->m_mapBlackList.begin();it!=g_LogicManager->m_Hero->m_mapBlackList.end();++it)
	{
		AddItem(m_friendSV,m_Template->clone(),it->second);
	}
	m_friendSV->setLayoutType(Layout::Type::VERTICAL);
	m_friendSV->forceDoLayout();
	Size friendSize = m_friendSV->getInnerContainerSize();
	friendSize.height = m_friendSV->getChildrenCount() * m_Template->getContentSize().height;
	m_friendSV->setInnerContainerSize(friendSize);
}

//删除好友
void UIFriends::DelFriends(Role_Relation pid)
{
	if (m_iSelectRole == 0)
	{
		return;
	}
	MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,m_iSelectRole, pid,"");
	Msg.MsgID = Msg_Del_Friends;
	g_NetManager->SendMsg(Msg);
}