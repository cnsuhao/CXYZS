/********************************************************************
created:	2014/12/6
author:		LL
desc:		UI基类：所有的ui界面都必须继承自此类，用于设计ui的公共功能。
*********************************************************************/
#ifndef __UIBASE_H__
#define __UIBASE_H__

#include "ClientDef.h"
#include "CCLayer.h"
#include "UILayout.h"

#include "UIText.h"
#include "UIButton.h"
#include "UIImageView.h"
#include "UICheckBox.h"
#include "UIScrollView.h"
#include "UITextField.h"

using namespace std;

template <class T>
class Smemory
 {
public:
	void mput(T x);
	T* CreateUI(){return T::create();};
 };

class UIBase : public cc2dui::Layout
{
public:
	UIBase();
	~UIBase();

	virtual void OnOpen();      //>初次开启回调,子类重载记得调用父类方法(UIBase::OnOpen),否则可能有问题
	virtual void OnShow();      //>显示回调,子类重载记得调用父类方法
	virtual void OnHide();      //>隐藏回调,子类重载记得调用父类方法
	virtual void OnRemove();    //>移除回调,子类重载记得调用父类方法
    virtual bool DisabledJoystickWhenSelfShow() { return false; } // UI开启时是否关闭摇杆功能,默认不关闭,派生类如果需要关闭摇杆功能需重载此函数
    virtual bool TouchJoystickInRange(cc2d::Touch *touch) { return true; } // 如果UI开启允许摇杆功能,需要进一步判断范围能够才能开启摇杆功能.
    virtual bool HideMainPanelWhenSelfShow() { return true; }   // UI开启是否隐藏主界面

	static void SetLayoutCenter(Widget * pObject, bool scale = false);
	static void SetLayoutBottom(Widget *pObject);
	static void SetLayoutTop(Widget *pObject);
	static void SetLayoutRight(Widget *pObject);
	static void SetLayoutLeft(Widget *pObject);
	static float GetLayoutRightX();
	static cc2d::Point GetLayoutCenter();

	virtual bool init();
	CREATE_FUNC(UIBase);
};

#endif	//__UIBASE_H__