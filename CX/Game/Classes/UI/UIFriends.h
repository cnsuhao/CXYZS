/********************************************************************
created:	2014/12/6
author:		LSH
desc:		好友面板
*********************************************************************/

#ifndef __UIFRIENDS_H__
#define __UIFRIENDS_H__

#include "UIBase.h"
#include "Hero.h"

class UIFriends : public UIBase
{
public:
	UIFriends();
	~UIFriends();	
	virtual bool init();
	CREATE_FUNC(UIFriends);

	virtual bool DisabledJoystickWhenSelfShow() { return true; }

	friendInfo m_FindFriend;
	void UpdateSearch();				//更新搜索界面

	void UpdateFriendList();			//更新好友列表

	void UpdateBlackList();				//更新黑名单列表
private:
	//主面板
	cc2d::Node *m_friendPanel;

	cc2d::Node *m_friendlist_foot;
	cc2d::Node *m_blacklist_foot;
	cc2d::Node *m_search;
	cc2d::Node *m_search_ex;
	cc2d::Node *m_friendlist;

	cc2dui::Layout *m_Template;

	//加载各种面板及按钮
	void InitLoad();

	//主面板的checkbox
	cc2d::Vector<cc2dui::CheckBox*> m_mainChecks;
	void ShowCheckBoxUI(Ref* _sender, cc2dui::CheckBox::EventType type);

	//模拟载入好友
	void LoadFriendList();

	//面板好友列表
	cc2dui::ScrollView *m_friendSV;
	map<int64, cc2dui::CheckBox*> m_mapFriendList;
	void AddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,friendInfo parTag);
	int64 m_iSelectRole;

	//搜索按钮
	void SearchRole(char *szPar);
	void AddItemFind(cc2dui::ScrollView* sv, cc2dui::Widget* layout,friendInfo parTag);
	//添加好友按钮
	void AddFrinds();
	//添加黑名单按钮
	void AddBlackList();
	//搜索好友列表
	cc2dui::ScrollView *m_FindfriendSV;

	//删除好友
	void DelFriends(Role_Relation pid);
	
};

#endif	//__UIFRIENDS_H__