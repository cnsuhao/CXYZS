#include "UIChestItem.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "NetManager.h"
#include "CSVFileManager.h"
using namespace cocostudio;

UIChestItem::UIChestItem()
{

}

UIChestItem::~UIChestItem()
{
}

bool UIChestItem::init()
{
	bool bRet = false;
	m_num = 0;
	do 
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		//加载主面板
		m_wingPanel = CSLoader::createNode("UI/ChestItem.csb");
		if (m_wingPanel == NULL)	return false;
		addChild(m_wingPanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_wingPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_UICHESTITEM);
				m_num = 0;
			});
		}
		btn = static_cast<Button*>(m_wingPanel->getChildByName("Button_1"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				MsgChest send(ID_C2G, g_LogicManager->ListChestType, g_LogicManager->ListChestNum);
				g_NetManager->SendMsg(send);
				g_UIManager->HideUI(UI_UICHESTITEM);
				m_num = 0;
			});
		}
		btn = static_cast<Button*>(m_wingPanel->getChildByName("Button_2"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_UICHESTITEM);
				m_num = 0;
			});
		}		
		bRet = true;
	} while (0);

	return bRet;
}

void UIChestItem::AddChestItem(CliItemInfo* pItem)
{
	//钱
	ImageView* temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_3"));
	if (temp)
	{
		cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_1"));
		if (Money)
		{
			Money->setString(StringUtils::format("%d", (int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY]));
		}
	}
	//元宝
	temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_4"));
	if (temp)
	{
		cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_1_Copy"));
		if (Money)
		{
			Money->setString(StringUtils::format("%d", (int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_GOLD]));
		}
	}
	//再抽一次
	const ChestInfo *info = g_CSVFileManager->GetChestInfo(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL], g_LogicManager->ListChestType, g_LogicManager->ListChestNum);
	if (info)
	{	
		temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_5"));
		if (temp)
		{
			if (g_LogicManager->ListChestType == ATTRIBUTE_MONEY)
			{
				ImageView* pIcon = static_cast<ImageView*>(temp->getChildByName("Image_4_0"));	
				if (pIcon)
				{
					pIcon->setVisible(true);
				}
				pIcon = static_cast<ImageView*>(temp->getChildByName("Image_4_1"));	
				if (pIcon)
				{
					pIcon->setVisible(false);
				}
				cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_1_Copy"));
				if (Money)
				{
					Money->setString(StringUtils::format("%d", info->iCurrencyNum));
				}
			} 
			else
			{
				ImageView* pIcon = static_cast<ImageView*>(temp->getChildByName("Image_4_0"));	
				if (pIcon)
				{
					pIcon->setVisible(false);
				}
				pIcon = static_cast<ImageView*>(temp->getChildByName("Image_4_1"));	
				if (pIcon)
				{
					pIcon->setVisible(true);
				}
				cc2dui::Text* Money = static_cast<Text*>(temp->getChildByName("Text_1_Copy"));
				if (Money)
				{
					Money->setString(StringUtils::format("%d", info->iCurrencyNum));
				}
			}
		
		}
	}
	temp = static_cast<ImageView*>(m_wingPanel->getChildByName("Image_2"));
	ImageView* view = NULL;
	if (g_LogicManager->ListChestNum > 1)
	{
		view = static_cast<ImageView*>(temp->getChildByName("Image_1"));	
		if (view)
		{
			view->setVisible(false);
		}
		view = static_cast<ImageView*>(temp->getChildByName("Image_1_Copy"));	
		if (view)
		{
			view->setVisible(true);
		}
	}
	else
	{
		view = static_cast<ImageView*>(temp->getChildByName("Image_1_Copy"));	
		if (view)
		{
			view->setVisible(false);
		}
		view = static_cast<ImageView*>(temp->getChildByName("Image_1"));	
		if (view)
		{
			view->setVisible(true);
		}

	}
	if (view)
	{
		string icon = StringUtils::format("Image_icon_%d",m_num);
		string name = StringUtils::format("Text_name_%d",m_num);		
	
		if (pItem)
		{
			ImageView* pIcon = static_cast<ImageView*>(view->getChildByName(StringUtils::format("Image_icon_%d",m_num)));
			pIcon->loadTexture(pItem->pCfg->strIcon);
			cc2dui::Text* pText = static_cast<cc2dui::Text*>(view->getChildByName( StringUtils::format("Text_name_%d",m_num)));
			pText->setText(BaseFunction::a2u(pItem->pCfg->strName));
			pText->setColor(pItem->GetQualityColor());
			m_num++;
		}
	}	
}