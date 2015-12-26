//////////////////////////////////////////////////////////////////////////
//    File Name:      UIDefaultServer.h
//    Description:    选择服务器页面
//    Creator:        zp
//    Version:        2015.04.21
//////////////////////////////////////////////////////////////////////////  


#include "UIBase.h"
#include "RequestServerList.h"


#ifndef __UIDEFAULTSERVER_H__
#define __UIDEFAULTSERVER_H__

class UIDefaultServer : public UIBase
{
public:
	UIDefaultServer();
	~UIDefaultServer();

	virtual bool init();
	CREATE_FUNC(UIDefaultServer);

protected:
	//默认服务器界面中方法
	void OnBtEnterGame();
	void OnBtChangeServer();
	//选择服务器界面中方法
	void OnTxSelectServer();
	//选择角色界面中方法
	void OnBtSelectRole();
	//返回到默认选择服务器界面
	void OnBtSelectServertoDefault();

	void ReplaceUI(cc2d::Node* oldUI, cc2d::Node* newUI);
	//左边服务器列表添加数据
	void AddAcountItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,  std::string itemName, int iPos);
	//右边服务器列表添加数据
	void AddServerName(cc2dui::Layout* sv, cc2dui::Widget* layout, tagServer& serverLeft,tagServer& serverRight);
	//右边服务器角色添加数据
	void AddServerRole(cc2dui::Layout* sv, cc2dui::Widget* layout);

	//更新默认登录界面的服务器名
	void UpdateServerName();
	//更新选择服务器界面的服务器名
	void UpdateSelectServerName();

private:
	//默认服务器UI与选择服务器UI
    cc2d::Node* defaultServerUI;
	cc2d::Node* selectServerUI;
	
	//左边的服务器列表及模板
	cc2dui::ScrollView* accountSV;
	cc2dui::Layout* accountTemplate;

	//右边的服务器列表及模板
	cc2dui::Layout* selectServer;
	cc2dui::Layout* selectServerTemplate;
	//右边的角色列表及模板
	cc2dui::Layout* selectRole;
	cc2dui::Layout* selectRoleTemplate;

	//背景层
	cc2d::Sprite* sp_canvas;
	//选择服务器界面左边的CHECKBOX
	map<int, cc2dui::CheckBox*> m_mapcheckboxs; 

	//当前选择的服务器
	tagServer m_currServer;
	vecServerList m_vecServerList;

	//服务器名的状态提示
	char m_szSerStateOld[15];			//老服
	char m_szSerStateNew[15];			//新服
	char m_szSerStateAnd[15];			//合服
	char m_szSerStateClose[15];			//关服

	char m_szSerRec[15];				//推荐
	char m_szSer1to10[15];				//1-10区
	char m_szSer11to20[15];				//11-20区
};

#endif	//__UIDEFAULTSERVER_H__