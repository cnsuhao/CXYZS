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
//����UI�ؼ����־���
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
//����UI�ؼ����ֿ���
void UIBase::SetLayoutTop(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	//pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(pWidget->getAnchorPoint().x, 1.0));
	pWidget->setPosition(Point(pWidget->getPositionX(), size.height));
}
//����UI�ؼ����ֿ���
void UIBase::SetLayoutBottom(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	//pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(pWidget->getAnchorPoint().x, 0));
	pWidget->setPosition(Point(pWidget->getPositionX(), 0));
}
//����UI�ؼ����ֿ���
void UIBase::SetLayoutRight(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(0.5,pWidget->getAnchorPoint().y));
	pWidget->setPosition(Point(size.width-pWidget->getContentSize().width/2-20,pWidget->getPosition().y));
}
//����UI�ؼ����ֿ���
void UIBase::SetLayoutLeft(Widget *pWidget)
{
	Size size = Director::getInstance()->getWinSize();
	pWidget->ignoreAnchorPointForPosition(false);
	pWidget->setAnchorPoint(Point(0.5,pWidget->getAnchorPoint().y));
	pWidget->setPosition(Point(pWidget->getContentSize().width/2+20,pWidget->getPosition().y));
}
//��ȡ�е�
Point UIBase::GetLayoutCenter()
{
	Size size = Director::getInstance()->getWinSize();
	return Point(size.width/2,size.height/2);
}
//��ȡ�ұ߽�������
float UIBase::GetLayoutRightX()
{
	Size size = Director::getInstance()->getWinSize();
	return size.width;
}