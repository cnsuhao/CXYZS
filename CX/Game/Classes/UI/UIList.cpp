#include "UIList.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UIList::UIList()
{

}

UIList::~UIList()
{
}

bool UIList::init()
{
	bool bRet = false;
	do
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_listPanel = CSLoader::createNode("UI/List.csb");
		if (m_listPanel == NULL)	return false;
		addChild(m_listPanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_listPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_LIST);
			});
		}

		

		bRet = true;
	} while (0);

	return bRet;
}

