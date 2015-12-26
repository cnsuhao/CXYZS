#include "UIConsign.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UIConsign::UIConsign()
{

}

UIConsign::~UIConsign()
{
}

bool UIConsign::init()
{
	bool bRet = false;
	do
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_consignPanel = CSLoader::createNode("UI/Consign.csb");
		if (m_consignPanel == NULL)	return false;
		addChild(m_consignPanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_consignPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_CONSIGN);
			});
		}

		InitLeft();
		InitRight();

		bRet = true;
	} while (0);

	return bRet;
}

void UIConsign::InitLeft()
{
	//模板
	m_leftTemplate = static_cast<Layout*>(m_consignPanel->getChildByName("panel_leftmodel"));
	m_leftTemplate->setVisible(false);

	//滚动条
	m_leftSV = static_cast<cc2dui::ScrollView*>(m_consignPanel->getChildByName("sv_leftdata"));
	m_leftSV ->setSwallowTouches(true); 

	AddleftItem(m_leftSV,m_leftTemplate->clone(),true);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),true);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),false);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),false);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),true);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),false);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),false);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),false);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),true);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),true);
	AddleftItem(m_leftSV,m_leftTemplate->clone(),true);

	m_leftSV->setLayoutType(Layout::Type::VERTICAL);
	m_leftSV->forceDoLayout();
	Size TempSize = m_leftSV->getInnerContainerSize();
	TempSize.height = m_leftSV->getChildrenCount() * m_leftTemplate->getContentSize().height;
	m_leftSV->setInnerContainerSize(TempSize);
}

void UIConsign::AddleftItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,bool bIsmain)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	if (bIsmain)
	{
		(static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("cb_2")))->setVisible(false);
		(static_cast<cc2dui::ImageView*>(newLayout->getChildByName("line_2")))->setVisible(false);
		(static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_2")))->setVisible(false);
	}
	else
	{
		(static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("cb_1")))->setVisible(false);
		(static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_1")))->setVisible(false);
	}

	sv->addChild(newLayout);
}

void UIConsign::InitRight()
{
	//模板
	m_rightTemplate = static_cast<Layout*>(m_consignPanel->getChildByName("panel_rightmodel"));
	m_rightTemplate->setVisible(false);

	//滚动条
	m_rightSV = static_cast<cc2dui::ScrollView*>(m_consignPanel->getChildByName("sv_rightdata"));
	m_rightSV ->setSwallowTouches(true); 

	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());
	AddrightItem(m_rightSV,m_rightTemplate->clone());

	m_rightSV->setLayoutType(Layout::Type::VERTICAL);
	m_rightSV->forceDoLayout();
	Size TempSize = m_rightSV->getInnerContainerSize();
	TempSize.height = m_rightSV->getChildrenCount() * m_rightTemplate->getContentSize().height;
	m_rightSV->setInnerContainerSize(TempSize);
}

void UIConsign::AddrightItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	sv->addChild(newLayout);
}



