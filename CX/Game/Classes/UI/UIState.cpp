#include "UIState.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UIState::UIState()
{

}

UIState::~UIState()
{
}

bool UIState::init()
{
	bool bRet = false;
	do 
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_statePanel = CSLoader::createNode("UI/State.csb");
		if (m_statePanel == NULL)	return false;
		addChild(m_statePanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_statePanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_STATE);
			});
		}

		bRet = true;
	} while (0);

	return bRet;
}
