#include "UIReceiveTask.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UIReceiveTask::UIReceiveTask()
{

}

UIReceiveTask::~UIReceiveTask()
{
}

bool UIReceiveTask::init()
{
	bool bRet = false;
	do 
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		CC_BREAK_IF(!UIBase::init());
		m_receiveTaskPanel = CSLoader::createNode("UI/ReceiveTask.csb");	
		addChild(m_receiveTaskPanel);

		cc2dui::Layout *m_bg = static_cast<Layout*>(m_receiveTaskPanel->getChildByName("panel_bg"));
		if (m_bg)
		{
			m_bg->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_RECEIVETASK);
			});
		}

		bRet = true;
	} while (0);

	return bRet;
}