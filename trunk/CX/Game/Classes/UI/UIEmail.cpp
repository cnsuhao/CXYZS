#include "UIEmail.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UIEmail::UIEmail()
{

}

UIEmail::~UIEmail()
{
}

bool UIEmail::init()
{
    bool bRet = false;
    do
    {
        Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_emailPanel = CSLoader::createNode("UI/Email.csb");
		if (m_emailPanel == NULL)	return false;
		addChild(m_emailPanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_emailPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_EMAIL);
			});
		}

		m_inbox =  static_cast<cc2dui::CheckBox*>(m_emailPanel->getChildByName("cb_inbox"));
		if (m_inbox)
		{
			m_inbox->setSelected(false);
			m_inbox->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				m_inbox->setSelected(true);
				m_unread->setSelected(false);
			});
		}

		m_unread =  static_cast<cc2dui::CheckBox*>(m_emailPanel->getChildByName("cb_unread"));
		if (m_unread)
		{
			m_unread->setSelected(false);
			m_unread->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				m_inbox->setSelected(false);
				m_unread->setSelected(true);
			});
		}

		//模板
		m_Template = static_cast<Layout*>(m_emailPanel->getChildByName("panel_model"));
		m_Template->setVisible(false);

		//滚动条
		m_emailSV = static_cast<cc2dui::ScrollView*>(m_emailPanel->getChildByName("sv_data"));
		m_emailSV ->setSwallowTouches(true); 

		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());
		AddItem(m_emailSV,m_Template->clone());

		m_emailSV->setLayoutType(Layout::Type::VERTICAL);
		m_emailSV->forceDoLayout();
		Size TempSize = m_emailSV->getInnerContainerSize();
		TempSize.height = m_emailSV->getChildrenCount() * m_Template->getContentSize().height;
		m_emailSV->setInnerContainerSize(TempSize);

        bRet = true;
    } while (0);

    return bRet;
}

void UIEmail::AddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	(static_cast<cc2dui::CheckBox*>(m_Template->getChildByName("cb_item")))->setSelected(false);

	sv->addChild(newLayout);
}
