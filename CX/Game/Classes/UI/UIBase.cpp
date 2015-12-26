#include "UIBase.h"

USING_NS_CC;
using namespace cocos2d::ui;

#include "SceneManager.h"
#include "CCDirector.h"


UIBase::UIBase()
{	
}

UIBase::~UIBase()
{
}

bool UIBase::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!Layout::init());

		bRet = true;	
	} while (0);

	return bRet;
}

void UIBase::OnOpen()
{
	OnShow();
}

void UIBase::OnShow()
{
	setEnabled(true);
}

void UIBase::OnHide()
{
	setEnabled(false);
}

void UIBase::OnRemove()
{
	//this->removeFromParentAndCleanup(true);	
}


///////////////////////////////////////////////////
//设置UI控件布局居中
void UIBase::SetLayoutCenter(Widget * pWidget, bool scale)
{
	Size size = Director::getInstance()->getWinSize();
	//pWidget->ignoreAnchorPointForPosition(false);
	if (scale)
	{
		pWidget->setScale(size.height/g_ScreenSize.y);
	}
	pWidget->setAnchorPoint(Point(0.5,0.5));
	pWidget->setPosition(Point(size.width/2,size.height/2));
	
}
//设置UI控件布局靠上
void UIBase::SetLayoutTop(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	//pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(pWidget->getAnchorPoint().x, 1.0));
	pWidget->setPosition(Point(pWidget->getPositionX(), size.height));
}
//设置UI控件布局靠下
void UIBase::SetLayoutBottom(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	//pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(pWidget->getAnchorPoint().x, 0));
	pWidget->setPosition(Point(pWidget->getPositionX(), 0));
}
//设置UI控件布局靠右
void UIBase::SetLayoutRight(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(0.5,pWidget->getAnchorPoint().y));
	pWidget->setPosition(Point(size.width-pWidget->getContentSize().width/2-20,pWidget->getPosition().y));
}
//设置UI控件布局靠左
void UIBase::SetLayoutLeft(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(0.5,pWidget->getAnchorPoint().y));
	pWidget->setPosition(Point(pWidget->getContentSize().width/2+20,pWidget->getPosition().y));
}
//获取中点
Point UIBase::GetLayoutCenter()
{
	Size size = Director::getInstance()->getWinSize();
	return Point(size.width/2,size.height/2);
}
//获取右边界点横坐标
float UIBase::GetLayoutRightX()
{
	Size size = Director::getInstance()->getWinSize();
	return size.width;
}