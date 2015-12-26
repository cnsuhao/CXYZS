#include "UIMenu.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "Hero.h"
#include "LogicManager.h"
#include "NetManager.h"
#include "BaseFunction.h"
#include "CSVFileManager.h"
#include "UIPlayerInfo.h"

UIMenu::UIMenu()
{

}

UIMenu::~UIMenu()
{
}

bool UIMenu::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!UIBase::init());
		cc2d::Node* ui = CSLoader::createNode("UI/Menu.csb");	
		addChild(ui);
		
        // 关闭按钮
        Button* pBtn = static_cast<Button*>(ui->getChildByName("bt_return"));
        pBtn->addClickEventListener([this](Ref* _sender)
		{
            g_UIManager->HideUI(UI_MENU);
		});

        // 角色信息
        pBtn = static_cast<Button*>(ui->getChildByName("bt_hero"));
        pBtn->addClickEventListener([this](Ref* _sender)
		{
            UIPlayerInfo* pPlayerInofUI = static_cast<UIPlayerInfo*>(g_UIManager->GetUI(UI_PLAYERINFO));
            if (pPlayerInofUI == NULL)
                g_UIManager->CreatePlayerInfo();

            pPlayerInofUI = static_cast<UIPlayerInfo*>(g_UIManager->GetUI(UI_PLAYERINFO));
            if (pPlayerInofUI)
            {
                pPlayerInofUI->SetInfo(g_LogicManager->m_Hero);
                g_UIManager->OpenUI(UI_PLAYERINFO);
                g_UIManager->HideUI(UI_MENU);
            }
		});

        // 背包按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_bag"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            g_UIManager->OpenUI(UI_BAG);
            g_UIManager->HideUI(UI_MENU);

        });

        // 法宝按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_weapon"));
        pBtn->addClickEventListener([this](Ref* _sender)
		{
            const TextInfo *pText = g_CSVFileManager->GetTextByID(672);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //此功能尚未开启.
            //g_UIManager->OpenUI(UI_FABAO);
            //g_UIManager->HideUI(UI_MENU);
		});

        // 帮会按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_gang"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
			if (g_LogicManager->m_Hero->m_isocietyID <= 0)
			{
				//获得工会列表
				MsgGang Msg(g_LogicManager->m_Hero->m_GUID,0,0,"");	
				Msg.MsgID = Msg_Get_GangList;
				Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
				g_NetManager->SendMsg(Msg);

				g_UIManager->OpenUI(UI_APPLYGANG);
			}
			else
			{
				//获得工会信息
				MsgGang Msg(g_LogicManager->m_Hero->m_GUID,g_LogicManager->m_Hero->m_isocietyID,0,"");	
				Msg.MsgID = Msg_Get_GangInfo;
				Msg.OBJID = g_LogicManager->m_Hero->m_GUID;
				g_NetManager->SendMsg(Msg);

				g_UIManager->OpenUI(UI_GUILD);
			}
			g_UIManager->HideUI(UI_MENU);

        });

        // 装备进阶
        pBtn = static_cast<Button*>(ui->getChildByName("bt_upgrade"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            //const TextInfo *pText = g_CSVFileManager->GetTextByID(672);
            //g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //此功能尚未开启.
            g_UIManager->OpenUI(UI_EQUIP_UPGRADE);
            g_UIManager->HideUI(UI_MENU);

        });

        // 好友按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_social"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
			MsgFriends Msg(g_LogicManager->m_Hero->m_GUID,0, Relation_Friends,"");	
			Msg.MsgID = Msg_Get_FriendsList;
			g_NetManager->SendMsg(Msg);
			
            g_UIManager->OpenUI(UI_FRIENDS);
            g_UIManager->HideUI(UI_MENU);

        });

        // 队伍按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_list"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(672);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //此功能尚未开启.
            //g_UIManager->OpenUI(UI_LIST);
            //g_UIManager->HideUI(UI_MENU);

        });

        // 邮件按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_mail"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {

            const TextInfo *pText = g_CSVFileManager->GetTextByID(651);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //功法功能尚未开启.
            //g_UIManager->OpenUI(UI_EMAIL);
            //g_UIManager->HideUI(UI_MENU);

        });

        // 任务按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_task"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            g_UIManager->OpenUI(UI_TASK);
            g_UIManager->HideUI(UI_MENU);
        });

        // 寄售按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_consign"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(672);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //此功能尚未开启.
            //g_UIManager->OpenUI(UI_CONSIGN);
            //g_UIManager->HideUI(UI_MENU);

        });

        // 功法按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_method"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            if (g_LogicManager->m_Hero->m_SkillList.empty())
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(651);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //功法功能尚未开启.
                return;
            }
            else
            {
                g_UIManager->OpenUI(UI_GONGFA);
                g_UIManager->HideUI(UI_MENU);
            }
        });

        // 商店按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_medicine"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            g_UIManager->OpenUI(UI_SHOP);
            g_UIManager->HideUI(UI_MENU);

        });

		// 境界按钮
		pBtn = static_cast<Button*>(ui->getChildByName("bt_state"));
		pBtn->addClickEventListener([this](Ref* _sender)
		{
            const TextInfo *pText = g_CSVFileManager->GetTextByID(672);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //此功能尚未开启.
			//g_UIManager->OpenUI(UI_STATE);
			//g_UIManager->HideUI(UI_MENU);
		});

		// 翅膀按钮
		pBtn = static_cast<Button*>(ui->getChildByName("bt_wings"));
		pBtn->addClickEventListener([this](Ref* _sender)
		{
            const TextInfo *pText = g_CSVFileManager->GetTextByID(672);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //此功能尚未开启.
			//g_UIManager->OpenUI(UI_WING);
			//g_UIManager->HideUI(UI_MENU);

		});

        // 设置按钮
        pBtn = static_cast<Button*>(ui->getChildByName("bt_set"));
        pBtn->addClickEventListener([this](Ref* _sender)
        {
            g_UIManager->OpenUI(UI_SETTING);
            g_UIManager->HideUI(UI_MENU);

        });


		bRet = true;	
	} while (0);


	return bRet;
}

//void UIMenu::OnShow()
//{
//    UIBase::OnShow();
//    g_UIManager->HideUI(UI_MAIN);
//}
//
//void UIMenu::OnHide()
//{
//    UIBase::OnHide();
//    g_UIManager->OpenUI(UI_MAIN);
//}
