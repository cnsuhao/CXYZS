/********************************************************************
created:	2014/12/6
author:		LL
desc:		UI���ࣺ���е�ui���涼����̳��Դ��࣬�������ui�Ĺ������ܡ�
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

	virtual void OnOpen();      //>���ο����ص�,�������ؼǵõ��ø��෽��(UIBase::OnOpen),�������������
	virtual void OnShow();      //>��ʾ�ص�,�������ؼǵõ��ø��෽��
	virtual void OnHide();      //>���ػص�,�������ؼǵõ��ø��෽��
	virtual void OnRemove();    //>�Ƴ��ص�,�������ؼǵõ��ø��෽��
    virtual bool DisabledJoystickWhenSelfShow() { return false; } // UI����ʱ�Ƿ�ر�ҡ�˹���,Ĭ�ϲ��ر�,�����������Ҫ�ر�ҡ�˹��������ش˺���
    virtual bool TouchJoystickInRange(cc2d::Touch *touch) { return true; } // ���UI��������ҡ�˹���,��Ҫ��һ���жϷ�Χ�ܹ����ܿ���ҡ�˹���.
    virtual bool HideMainPanelWhenSelfShow() { return true; }   // UI�����Ƿ�����������

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