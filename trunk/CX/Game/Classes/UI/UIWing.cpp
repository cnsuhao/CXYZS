#include "UIWing.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UIWing::UIWing()
{

}

UIWing::~UIWing()
{
}

bool UIWing::init()
{
	bool bRet = false;
	do 
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_wingPanel = CSLoader::createNode("UI/Wing.csb");
		if (m_wingPanel == NULL)	return false;
		addChild(m_wingPanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_wingPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_WING);
			});
		}

		bRet = true;
	} while (0);

	return bRet;
}
