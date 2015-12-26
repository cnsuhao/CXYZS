#include "UIManager.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "BaseFunction.h"
#include "SceneManager.h"
#include "ResManager.h"
#include "CSVFileManager.h"
#include "CCSGUIReader.h"
#include "UILayout.h"
#include "UILogin.h"
#include "UIMain.h"
#include "UIFaBao.h"
#include "UIPlayerInfo.h"
#include "UIMenu.h"
#include "UIBag.h"
#include "UIChat.h"
#include "UIGuild.h"
#include "UIUpgradeEquip.h"
#include "UIFriends.h"
#include "UITeam.h"
#include "UIEmail.h"
#include "UITask.h"
#include "UIAuction.h"
#include "UISkill.h"
#include "UIShop.h"
#include "UISetting.h"
#include "UIDefaultServer.h"
#include "UICreateRole.h"
#include "CtrlJoystick.h"
#include "UIState.h"
#include "UIWing.h"
#include "UIList.h"
#include "UIConsign.h"
#include "UIHttpUpdate.h"
#include "UIWaitting.h"
#include "UIItemInfo.h"
#include "UITalk.h"
#include "UIReceiveTask.h"
#include "UIChest.h"
#include "UIChestItem.h"
#include "UIApplyGang.h"

USING_NS_CC;
//#include "cocos-ext.h"
//USING_NS_CC_EXT;

UIManager *g_UIManager = NULL;
UIManager::UIManager()
{	
}

UIManager::~UIManager()
{
	Release();
}

void UIManager::Init()
{	
	CSLoader::getInstance()->setRecordProtocolBuffersPath(true);
	for (int i = 0; i < UI_MAX; i++)
	{
		m_UIList[i] = NULL;
	}

    m_Start.x = 512.0f; m_Start.y = 370.0f;
    m_FontSize = 24;
    m_ElapseTime = 0.0f;
    m_DelayTime = 0.2f;

}

void UIManager::Release()
{	
}

void UIManager::Update(float delta)
{
    m_ElapseTime += delta;
    if (m_ElapseTime > m_DelayTime)
    {
        if (!m_NoticeLabels.empty())
        {
            NoticeInfo* pNotice = m_NoticeLabels.front();
            Notify(pNotice->content, pNotice->color);
            delete pNotice;
            m_NoticeLabels.pop();
        }
    }

    UIMain* pUIMain = static_cast<UIMain*>( GetUI(UI_MAIN) );
    if (pUIMain)
    {
        pUIMain->Update(delta);
    }
}

void UIManager::OpenUI(UIType type)
{
	if (!m_UIList[type])
	{
		CreateUI(type);
        m_UIList[type]->OnOpen();
	}
	ShowUI(type);
	m_CurrUI = type;

    if (((UIMain*)g_UIManager->GetUI(UI_MAIN)) != NULL)
    {
        ((UIMain*)g_UIManager->GetUI(UI_MAIN))->HidePlayerMenu();   //Òþ²Øµ¯³ö²Ëµ¥
    }
}


bool UIManager::CreateUI(UIType type)
{
    if (type == UI_LOGIN)
    {
        Smemory<UILogin> mo1;
        m_UIList[UI_LOGIN] = mo1.CreateUI();
        m_UIList[UI_LOGIN]->retain();
    }
    else if (type == UI_MAIN)
	{
		Smemory<UIMain> mo1;
		m_UIList[UI_MAIN] = mo1.CreateUI();
        m_UIList[UI_MAIN]->retain();
		//m_UIList[UI_MAIN] = UIMain::create();
        //addChild(m_UIList[UI_MAIN], LAYER_UI);
	}
    else if (type == UI_FABAO)
    {
        Smemory<UIFaBao> mo1;
		m_UIList[UI_FABAO] = mo1.CreateUI();
        m_UIList[UI_FABAO]->retain();
    }
    else if (type == UI_PLAYERINFO)
    {
        Smemory<UIPlayerInfo> mo1;
		m_UIList[UI_PLAYERINFO] = mo1.CreateUI();
        m_UIList[UI_PLAYERINFO]->retain();
    }
    else if (type == UI_MENU)
    {
        Smemory<UIMenu> mo1;
		m_UIList[UI_MENU] = mo1.CreateUI();
        m_UIList[UI_MENU]->retain();
    }
    else if (type == UI_CHAT)
    {
        Smemory<UIChat> mo1;
		m_UIList[UI_CHAT] = mo1.CreateUI();
        m_UIList[UI_CHAT]->retain();
    }
    else if (type == UI_BAG)
    {
        Smemory<UIBag> mo1;
		m_UIList[UI_BAG] = mo1.CreateUI();
        m_UIList[UI_BAG]->retain();
    }
    else if (type == UI_GUILD)
    {
        Smemory<UIGuild> mo1;
		m_UIList[UI_GUILD] = mo1.CreateUI();
        m_UIList[UI_GUILD]->retain();
    }
    else if (type == UI_EQUIP_UPGRADE)
    {
        Smemory<UIUpgradeEquip> mo1;
		m_UIList[UI_EQUIP_UPGRADE] = mo1.CreateUI();
        m_UIList[UI_EQUIP_UPGRADE]->retain();
    }
    else if (type == UI_FRIENDS)
    {
        Smemory<UIFriends> mo1;
		m_UIList[UI_FRIENDS] = mo1.CreateUI();
        m_UIList[UI_FRIENDS]->retain();
    }
    else if (type == UI_TEAM)
    {
        Smemory<UITeam> mo1;
		m_UIList[UI_TEAM] = mo1.CreateUI();
        m_UIList[UI_TEAM]->retain();
    }
    else if (type == UI_EMAIL)
    {
        Smemory<UIEmail> mo1;
		m_UIList[UI_EMAIL] = mo1.CreateUI();
        m_UIList[UI_EMAIL]->retain();
    }
    else if (type == UI_TASK)
    {
        Smemory<UITask> mo1;
		m_UIList[UI_TASK] = mo1.CreateUI();
        m_UIList[UI_TASK]->retain();
    }
    else if (type == UI_AUCTION)
    {
        Smemory<UIAuction> mo1;
		m_UIList[UI_AUCTION] = mo1.CreateUI();
        m_UIList[UI_AUCTION]->retain();
    }
    else if (type == UI_GONGFA)
    {
        Smemory<UISkill> mo1;
		m_UIList[UI_GONGFA] = mo1.CreateUI();
        m_UIList[UI_GONGFA]->retain();
    }
    else if (type == UI_SHOP)
    {
        Smemory<UIShop> mo1;
		m_UIList[UI_SHOP] = mo1.CreateUI();
        m_UIList[UI_SHOP]->retain();
    }
    else if (type == UI_SETTING)
    {
        Smemory<UISetting> mo1;
		m_UIList[UI_SETTING] = mo1.CreateUI();
        m_UIList[UI_SETTING]->retain();
    }
	else if(type == UI_DEFAULTSERVER)
	{
		Smemory<UIDefaultServer> mo1;
		m_UIList[UI_DEFAULTSERVER] = mo1.CreateUI();
        m_UIList[UI_DEFAULTSERVER]->retain();
	}
    else if (type == UI_ROLECREATE)
    {
        Smemory<UICreateRole> mo1;
        m_UIList[UI_ROLECREATE] = mo1.CreateUI();
        m_UIList[UI_ROLECREATE]->retain();
    }
	else if (type == UI_STATE)
	{
		Smemory<UIState> mo1;
		m_UIList[UI_STATE] = mo1.CreateUI();
		m_UIList[UI_STATE]->retain();
	}
	else if (type == UI_WING)
	{
		Smemory<UIWing> mo1;
		m_UIList[UI_WING] = mo1.CreateUI();
		m_UIList[UI_WING]->retain();
	}
	else if (type == UI_LIST)
	{
		Smemory<UIList> mo1;
		m_UIList[UI_LIST] = mo1.CreateUI();
		m_UIList[UI_LIST]->retain();
	}
	else if (type == UI_CONSIGN)
	{
		Smemory<UIConsign> mo1;
		m_UIList[UI_CONSIGN] = mo1.CreateUI();
		m_UIList[UI_CONSIGN]->retain();
	}
    else if(type == UI_ITEM_INFO)
    {
        Smemory<UIItemInfo> mo1;
        m_UIList[UI_ITEM_INFO] = mo1.CreateUI();
        m_UIList[UI_ITEM_INFO]->retain();
    }
    else if (type == UI_WAITTING)
    {
        Smemory<UIWaitting> mo1;
        m_UIList[UI_WAITTING] = mo1.CreateUI();
        m_UIList[UI_WAITTING]->retain();
    }
    else if (type == UI_HTTP_UPDATE)
    {
        Smemory<UIHttpUpdate> mo1;
        m_UIList[UI_HTTP_UPDATE] = mo1.CreateUI();
        m_UIList[UI_HTTP_UPDATE]->retain();
    }
	else if (type == UI_TALK)
	{
		Smemory<UITalk> mo1;
		m_UIList[UI_TALK] = mo1.CreateUI();
		m_UIList[UI_TALK]->retain();
	}
	else if (type == UI_RECEIVETASK)
	{
		Smemory<UIReceiveTask> mo1;
		m_UIList[UI_RECEIVETASK] = mo1.CreateUI();
		m_UIList[UI_RECEIVETASK]->retain();
	}
	else if (type == UI_UICHEST)
	{
		Smemory<UIChest> mo1;
		m_UIList[UI_UICHEST] = mo1.CreateUI();
		m_UIList[UI_UICHEST]->retain();
	}
	else if (type == UI_UICHESTITEM)
	{
		Smemory<UIChestItem> mo1;
		m_UIList[UI_UICHESTITEM] = mo1.CreateUI();
		m_UIList[UI_UICHESTITEM]->retain();
	}
	else if (type == UI_APPLYGANG)
	{
		Smemory<UIApplyGang> mo1;
		m_UIList[UI_APPLYGANG] = mo1.CreateUI();
		m_UIList[UI_APPLYGANG]->retain();
	}

	return true;
}

void UIManager::ShowUI(UIType type)
{
	if (m_UIList[type] && m_UIList[type]->getParent() == NULL)
	{
        
       addChild(m_UIList[type], LAYER_UI);
       m_UIList[type]->OnShow();
		//m_UIList[type]->setEnabled(true);
       DisabledJoystick();
       CheckHideMainPannel();
	}
	g_ResManager->PlaySound(101);
}

void UIManager::HideUI(UIType type)
{
	if (m_UIList[type])
	{	
        removeChild(m_UIList[type]);
        m_UIList[type]->OnHide();
		//m_UIList[type]->setEnabled(false);
        EnabledJoystick();
        CheckShowMainPannel(m_UIList[type]);
	}
}

void UIManager::RemoveUI(UIType type)
{
	if (m_UIList[type])
	{
        m_UIList[type]->OnRemove();
		m_UIList[type]->removeAllChildrenWithCleanup(true);
		m_UIList[type]->release();
        m_UIList[type] = NULL;
	}
}

void UIManager::RemoveAllUI()
{
	for (int i = 0; i < UI_MAX; i++)
	{
		if (m_UIList[i])
		{
			m_UIList[i]->removeAllChildrenWithCleanup(true);
            m_UIList[i]->release();
			m_UIList[i] = NULL;
		}
	}
}

UIBase* UIManager::GetUI(UIType type)
{
	return m_UIList[type];
}

bool UIManager::IsOpened(UIType type)
{
    if (m_UIList[type])
    {
        return m_UIList[type]->getParent() != NULL;
    }
    return false;
}

void UIManager::DisabledJoystick()
{
    if (g_SceneManager->m_joystick)
    {
        for (int i = 0; i < UI_MAX; i++)
        {
            if (m_UIList[i] && m_UIList[i]->getParent() != NULL && m_UIList[i]->DisabledJoystickWhenSelfShow() )
            {
                g_SceneManager->m_joystick->SetEnabled(false);
                break;
            }
        }
    }
}

void UIManager::EnabledJoystick()
{
    if (g_SceneManager->m_joystick)
    {
        g_SceneManager->m_joystick->SetEnabled(true);
        DisabledJoystick();
    }
}

bool UIManager::TouchJoystickInRange(cc2d::Touch *touch)
{
    if (g_SceneManager->m_joystick)
    {
        for (int i = 0; i < UI_MAX; i++)
        {
            if (m_UIList[i] && m_UIList[i]->getParent() != NULL)
            {
                if(false == m_UIList[i]->TouchJoystickInRange(touch))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void UIManager::CheckHideMainPannel()
{
    for (int i = 0; i < UI_MAX; i++)
    {
        if (m_UIList[i] && m_UIList[i]->getParent() != NULL)
        {
            if(m_UIList[i]->HideMainPanelWhenSelfShow())
            {
                HideUI(UI_MAIN);
                break;
            }
        }
    }
}

void UIManager::CheckShowMainPannel(UIBase* pannel)
{
    if (pannel->HideMainPanelWhenSelfShow())
    {
        if (m_UIList[UI_MAIN])
        {
            OpenUI(UI_MAIN);
            CheckHideMainPannel();
        }
    }
}

void UIManager::AddChestItem(CliItemInfo* pItem)
{
	UIChest* Chest = (UIChest*)GetUI(UI_UICHESTITEM);
	if (Chest && !Chest->isEnabled())
	{
		g_UIManager->HideUI(UI_UICHEST);
		g_ResManager->ShowSprite(24, g_SceneManager, LAYER_SCENE_EFFECT, g_ScreenHalfSize);
	}
	OpenUI(UI_UICHESTITEM);
	((UIChestItem*)GetUI(UI_UICHESTITEM))->AddChestItem(pItem);
	
}

void UIManager::Notify(const std::string& notice, const Color3B& color)
{
    if (m_ElapseTime > m_DelayTime)
    {
        Label* pLabel = NULL;
        if (m_FreeLabels.size() > 0)
        {
            pLabel = m_FreeLabels.front();
            m_FreeLabels.pop();
        }
        else
        {
            pLabel = Label::create();
            pLabel->setSystemFontSize(m_FontSize);
            pLabel->setVisible(false);
            pLabel->enableShadow();
            pLabel->setLocalZOrder(LAYER_SCENE_EFFECT);
            addChild(pLabel);
        }

        pLabel->setString(notice);
        pLabel->setColor(color);
        pLabel->setVisible(true);
        pLabel->setPosition(Vec2(m_Start.x, m_Start.y));
        pLabel->runAction(CCSequence::create(MoveBy::create(3.0f, Vec2(0.0, 350.0f)), CallFunc::create(std::bind(&UIManager::NotifyAnimaCallback,this, pLabel)), NULL));

        m_ElapseTime = 0.0f;
    }
    else
    {
        m_NoticeLabels.push(new NoticeInfo(notice, color));
    }
}

void UIManager::NotifyAnimaCallback(Label* pLabel)
{
    pLabel->setVisible(false);
    m_FreeLabels.push(pLabel);
}

UIMsgBox* UIManager::ShowMsgBox(const char* msg, bool isutf8, MsgBoxEventType eventtype, MsgBoxStyle style, long long value)
{
	UIMsgBox* temp = UIMsgBox::create(msg, isutf8, eventtype, style, value);
	if (temp)
	{
		//temp->setPosition(g_ScreenHalfSize);
		addChild(temp, LAYER_MSGBOX);
	}
	return temp;
}

UIMsgBox* UIManager::ShowMsgBox(int id, bool isutf8, MsgBoxEventType eventtype, MsgBoxStyle style, long long value)
{
	const TextInfo* textinfo = g_CSVFileManager->GetTextByID(id);

	return ShowMsgBox(textinfo->strContent.c_str(), isutf8, eventtype, style, value);
}
