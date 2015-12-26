#include "UITeam.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"

using namespace cocostudio;

UITeam::UITeam()
{

}

UITeam::~UITeam()
{
}

bool UITeam::init()
{
    bool bRet = false;
    do 
    {
        Size winSize = Director::getInstance()->getVisibleSize();

        ImageView* img = ImageView::create("UI/Team/Team.png");
        if (img)
        {
            img->setPositionX(winSize.width / 2);
            img->setPositionY(winSize.height / 2);
            addChild(img);
        }

        Size bgSize = img->getContentSize();
        Button* btn = Button::create("UI/PublicButton/window_close_0.png", "UI/PublicButton/window_close_1.png", "");
        if (btn)
        {
            btn->setPositionX(img->getPositionX() + bgSize.width / 2 - 20);
            btn->setPositionY(img->getPositionY() + bgSize.height / 2 - 10);
            btn->addClickEventListener([this](Ref*)
            {
                g_UIManager->HideUI(UI_TEAM);
            });
            addChild(btn);
        }

        bRet = true;
    } while (0);

    return bRet;
}
