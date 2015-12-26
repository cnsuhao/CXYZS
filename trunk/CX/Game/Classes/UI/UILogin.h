/********************************************************************
created:	2014/12/6
author:		LSH
desc:		登陆面板
*********************************************************************/

#ifndef __UILOGIN_H__
#define __UILOGIN_H__


#include "ClientDef.h"
#include "UIBase.h"
#include "UIScrollView.h"
#include "UITextField.h"

class UILogin : public UIBase
{
public:
	UILogin();
	~UILogin();	
	virtual bool init();
	CREATE_FUNC(UILogin);

    const char* GetUserName() const;
    const char* GetPassword() const;
    const char* GetRegisterUserName() const;
    const char* GetRegisterPassword() const;
    const char* GetRegsiterRePassword() const;

    void SetUserName(const char* name);

    void RecordNewRegisterUserName(const char* newUser, const char* pw); // 保存新注册的账号名
    void DeleteRegisterUserName(const std::string& user); // 删除保存账号
protected:
    void OnLogin();
    void OnConfirmRegister();

    void ReplaceUI(cc2d::Node* oldUI, cc2d::Node* newUI);
private:
    void AddAcountItem(cc2dui::ScrollView* sv, cc2d::ui::Widget* layout, const char* itemName);
    void PlaySwitchAnim();
private:
    cc2d::Node* loginUI;
    cc2d::Node* registerUI;

    cc2dui::TextField* tf_userName;
    cc2dui::TextField* tf_password;
    cc2dui::TextField* tf_regUserName;
    cc2dui::TextField* tf_regPassword;
    cc2dui::TextField* tf_regRePassword;

    cc2d::Label* helpLable;
    cc2d::LayerColor* lc_cursor;
    cc2d::Action* cursorAction;
    cc2d::Sprite* sp_canvas;

    cc2d::ui::Layout* panelLogin;
    cc2d::ui::Layout* panelAccountRecord;
    cc2d::ui::Layout* accountTemplate;
    cc2d::ui::ScrollView* accountSV;
public:
	//切换注册窗口与登录窗口
	void switchWindows(bool bReg);
};

#endif	//__UILOGIN_H__