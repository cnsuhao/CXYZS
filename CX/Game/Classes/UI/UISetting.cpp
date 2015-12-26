#include "UISetting.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UISetting::UISetting()
{

}

UISetting::~UISetting()
{
}

bool UISetting::init()
{
    bool bRet = false;
    do 
    {
        Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_systemPanel = CSLoader::createNode("UI/Setting.csb");
		if (m_systemPanel == NULL)	return false;
		addChild(m_systemPanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_systemPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_SETTING);
			});
		}

		m_panelGangup = static_cast<Layout*>(m_systemPanel->getChildByName("panel_gangup"));
		m_panelGangup->setVisible(true);

		m_panelGame = static_cast<Layout*>(m_systemPanel->getChildByName("panel_game"));
		m_panelGame->setVisible(false);
		m_panelMusic = static_cast<Layout*>(m_systemPanel->getChildByName("panel_music"));
		m_panelMusic->setVisible(false);
		m_panelAccount = static_cast<Layout*>(m_systemPanel->getChildByName("panel_account"));
		m_panelAccount->setVisible(false);

		CheckBox *cb;
		cb =  static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_gangup"));
		if (cb)
		{
			cb->setSelected(true);
			cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				m_panelGangup->setVisible(true);
				m_panelGame->setVisible(false);
				m_panelMusic->setVisible(false);
				m_panelAccount->setVisible(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_gangup")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_game")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_music")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_account")))->setSelected(false);
				(static_cast<CheckBox*>(_sender))->setSelected(true);
			});
		}
		cb =  static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_game"));
		if (cb)
		{
			cb->setSelected(false);
			cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				m_panelGangup->setVisible(false);
				m_panelGame->setVisible(true);
				m_panelMusic->setVisible(false);
				m_panelAccount->setVisible(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_gangup")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_game")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_music")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_account")))->setSelected(false);
				(static_cast<CheckBox*>(_sender))->setSelected(true);
			});
		}
		cb =  static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_music"));
		if (cb)
		{
			cb->setSelected(false);
			cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				m_panelGangup->setVisible(false);
				m_panelGame->setVisible(false);
				m_panelMusic->setVisible(true);
				m_panelAccount->setVisible(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_gangup")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_game")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_music")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_account")))->setSelected(false);
				(static_cast<CheckBox*>(_sender))->setSelected(true);
			});
		}
		cb =  static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_account"));
		if (cb)
		{
			cb->setSelected(false);
			cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				m_panelGangup->setVisible(false);
				m_panelGame->setVisible(false);
				m_panelMusic->setVisible(false);
				m_panelAccount->setVisible(true);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_gangup")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_game")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_music")))->setSelected(false);
				(static_cast<cc2dui::CheckBox*>(m_systemPanel->getChildByName("cb_account")))->setSelected(false);
				(static_cast<CheckBox*>(_sender))->setSelected(true);
			});
		}


        bRet = true;
    } while (0);

    return bRet;
}
