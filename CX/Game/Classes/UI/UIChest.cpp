#include "UIChest.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "CSVFileManager.h"
#include "Hero.h"
#include "NetManager.h"
#include "ResManager.h"
#include "SceneManager.h"

using namespace cocostudio;

UIChest::UIChest()
{

}

UIChest::~UIChest()
{
}

bool UIChest::init()
{
	bool bRet = false;
	do 
	{			
		Size winSize = Director::getInstance()->getVisibleSize();
	
		//¼ÓÔØÖ÷Ãæ°å
		m_wingPanel = CSLoader::createNode("UI/Chest.csb");
		if (m_wingPanel == NULL)	return false;
		addChild(m_wingPanel);
		
		Button *btn = nullptr;
		//¹Ø±Õ°´Å¥
		btn = static_cast<Button*>(m_wingPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_UICHEST);
			});
		}
		//Ç®
		ImageView* temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_44"));
		if (temp)
		{
			cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_1"));
			if (Money)
			{
				Money->setString(StringUtils::format("%d", (int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY]));
			}
		}
		//Ôª±¦
		temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_45"));
		if (temp)
		{
			cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_2"));
			if (Money)
			{
				Money->setString(StringUtils::format("%d", (int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_GOLD]));
			}
		}
		//Ç®1³é
		temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_68"));
		if (temp)
		{
			const ChestInfo *info = g_CSVFileManager->GetChestInfo(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL], ATTRIBUTE_MONEY, 1);
			if (!info)
			{
				return false;
			}
			ImageView* pIcon = static_cast<ImageView*>(temp->getChildByName("Image_84"));
			const ItemsInfo* item = g_CSVFileManager->GetItemsByID(info->iBuyItemsid);
			if (pIcon && item)
			{
				pIcon->loadTexture(item->strIcon);
			}
			ImageView* back = static_cast<ImageView*>(temp->getChildByName("Image_44_Copy"));
			if (back)
			{
				cc2dui::Text* Money = static_cast<cc2dui::Text*>(back->getChildByName("Text_1_Copy"));
				if (Money) 
				{
					Money->setString(StringUtils::format("%d", info->iCurrencyNum));
				}
			}	
			btn = static_cast<Button*>(temp->getChildByName("Button_24"));
			if (btn)
			{
				btn->addClickEventListener([this](Ref* _sender)
				{
					MsgChest send(ID_C2G, ATTRIBUTE_MONEY, 1);
					g_NetManager->SendMsg(send);
					g_LogicManager->ListChestType = ATTRIBUTE_MONEY;
					g_LogicManager->ListChestNum = 1;
				});
			}
		}
		//Ç®10³é
		temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_69"));
		if (temp)
		{
			const ChestInfo *info = g_CSVFileManager->GetChestInfo(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL], ATTRIBUTE_MONEY, 9);
			if (!info)
			{
				return false;
			}

			ImageView* pIcon = static_cast<ImageView*>(temp->getChildByName("Image_84_Copy"));
			const ItemsInfo* item = g_CSVFileManager->GetItemsByID(info->iBuyItemsid);
			if (pIcon && item)
			{
				pIcon->loadTexture(item->strIcon);
			}
			
			ImageView* back = static_cast<ImageView*>(temp->getChildByName("Image_44_Copy_Copy"));
			if (back)
			{
				cc2dui::Text* Money = static_cast<cc2dui::Text*>(back->getChildByName("Text_1_Copy_Copy"));
				if (Money) 
				{
					Money->setString(StringUtils::format("%d", info->iCurrencyNum));
				}
			}	
			btn = static_cast<Button*>(temp->getChildByName("Button_24_Copy"));
			if (btn)
			{
				btn->addClickEventListener([this](Ref* _sender)
				{
					MsgChest send(ID_C2G, ATTRIBUTE_MONEY, 9);
					g_NetManager->SendMsg(send);
					g_LogicManager->ListChestType = ATTRIBUTE_MONEY;
					g_LogicManager->ListChestNum = 9;
				});
			}
		}
		//Ôª±¦1³é
		temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_70"));
		if (temp)
		{
			const ChestInfo *info = g_CSVFileManager->GetChestInfo(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL], ATTRIBUTE_GOLD, 1);
			if (!info)
			{	
				return false;
			}
			ImageView* pIcon = static_cast<ImageView*>(temp->getChildByName("Image_84_Copy_Copy"));
			const ItemsInfo* item = g_CSVFileManager->GetItemsByID(info->iBuyItemsid);
			if (pIcon && item)
			{
				pIcon->loadTexture(item->strIcon);
			}
			ImageView* back = static_cast<ImageView*>(temp->getChildByName("Image_44_Copy_Copy_Copy"));
			if (back)
			{
				cc2dui::Text* Money = static_cast<cc2dui::Text*>(back->getChildByName("Text_1_Copy_Copy_Copy"));
				if (Money) 
				{
					Money->setString(StringUtils::format("%d", info->iCurrencyNum));
				}
			}	
			btn = static_cast<Button*>(temp->getChildByName("Button_24_Copy_Copy"));
			if (btn)
			{
				btn->addClickEventListener([this](Ref* _sender)
				{				
					MsgChest send(ID_C2G, ATTRIBUTE_GOLD, 1);
					g_NetManager->SendMsg(send);
					g_LogicManager->ListChestType = ATTRIBUTE_GOLD;
					g_LogicManager->ListChestNum = 1;
				});
			}
		}
		//Ôª±¦10³é
		temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_71"));
		if (temp)
		{
			const ChestInfo *info = g_CSVFileManager->GetChestInfo(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL], ATTRIBUTE_GOLD, 9);
			if (!info)
			{	
				return false;
			}
			ImageView* pIcon = static_cast<ImageView*>(temp->getChildByName("Image_84_Copy_Copy"));
			const ItemsInfo* item = g_CSVFileManager->GetItemsByID(info->iBuyItemsid);
			if (pIcon && item)
			{
				pIcon->loadTexture(item->strIcon);
			}
			ImageView* back = static_cast<ImageView*>(temp->getChildByName("Image_44_Copy_Copy_Copy"));
			if (back)
			{
				cc2dui::Text* Money = static_cast<cc2dui::Text*>(back->getChildByName("Text_1_Copy_Copy_Copy"));
				if (Money) 
				{
					Money->setString(StringUtils::format("%d", info->iCurrencyNum));
				}
			}	
			btn = static_cast<Button*>(temp->getChildByName("Button_24_Copy_Copy"));
			if (btn)
			{
				btn->addClickEventListener([this](Ref* _sender)
				{
					MsgChest send(ID_C2G, ATTRIBUTE_GOLD, 9);
					g_NetManager->SendMsg(send);
					g_LogicManager->ListChestType = ATTRIBUTE_GOLD;
					g_LogicManager->ListChestNum = 9;
				});
			}
		}
		
		bRet = true;
	} while (0);

	return bRet;
}

void UIChest::OnShow()
{
	setScale(1.0f);
	setPosition(Vec2(0,0));
	UpdateMoneyShow();
	UIBase::OnShow();
}

void UIChest::UpdateMoneyShow()
{
	
	//Ç®
	ImageView* temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_44"));
	if (temp)
	{
		cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_1"));
		if (Money)
		{
			Money->setString(StringUtils::format("%d", (int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY]));
		}
	}
	//Ôª±¦
	temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_45"));
	if (temp)
	{
		cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_2"));
		if (Money)
		{
			Money->setString(StringUtils::format("%d", (int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_GOLD]));
		}
	}
}

void UIChest::ScaleCallBack(Node *sender, long data)
{
	g_UIManager->HideUI(UI_UICHEST);
}