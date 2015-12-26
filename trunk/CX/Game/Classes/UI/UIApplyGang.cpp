#include "UIApplyGang.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "CSVFileManager.h"
#include "NetManager.h"

using namespace cocostudio;

UIApplyGang::UIApplyGang()
{
	m_mapGangListBt.clear();
}

UIApplyGang::~UIApplyGang()
{
}

bool UIApplyGang::init()
{
	bool bRet = false;
	do 
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		CC_BREAK_IF(!UIBase::init());
		m_applyGangPanel = CSLoader::createNode("UI/ApplyGang.csb");	
		addChild(m_applyGangPanel);

		m_CreateGangPanel = static_cast<Layout*>(m_applyGangPanel->getChildByName("Panel_3"));
		m_CreateGangPanel->setVisible(false);

		Button *btn;
		btn	= static_cast<Button*>(m_applyGangPanel->getChildByName("Button_1"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref*)
			{
				g_UIManager->HideUI(UI_APPLYGANG);
			});
		}

		btn	= static_cast<Button*>(m_CreateGangPanel->getChildByName("Button_4"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref*)
			{
				g_UIManager->HideUI(UI_APPLYGANG);
			});
		}

		btn	= static_cast<Button*>(m_applyGangPanel->getChildByName("Button_2"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref*)
			{
				(static_cast<ImageView*>(m_applyGangPanel->getChildByName("Image_1")))->setVisible(false);
				(static_cast<ImageView*>(m_applyGangPanel->getChildByName("Image_2")))->setVisible(false);
				(static_cast<ImageView*>(m_applyGangPanel->getChildByName("Image_3")))->setVisible(false);
				(static_cast<ImageView*>(m_applyGangPanel->getChildByName("Image_4")))->setVisible(false);
				(static_cast<Layout*>(m_applyGangPanel->getChildByName("Panel_2")))->setVisible(false);
				(static_cast<Button*>(m_applyGangPanel->getChildByName("Button_1")))->setVisible(false);
				(static_cast<Button*>(m_applyGangPanel->getChildByName("Button_2")))->setVisible(false);
				(static_cast<Text*>(m_applyGangPanel->getChildByName("Text_1")))->setVisible(false);
				(static_cast<Layout*>(m_applyGangPanel->getChildByName("ScrollView_1")))->setVisible(false);
				m_CreateGangPanel->setVisible(true);
			});
		}

		btn	= static_cast<Button*>(m_CreateGangPanel->getChildByName("Button_5"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref*)
			{
				CrateGang();
			});
		}

		InputField = static_cast<TextField*>(m_CreateGangPanel->getChildByName("TextField_1"));

		//载入帮派列表
		InitGangList();

		bRet = true;
	} while (0);

	return bRet;
}

void UIApplyGang::CrateGang()
{
	const std::string& content = InputField->getString();

	string strTemp;
	strTemp = BaseFunction::u2a(content.c_str());

	//判断是否有帮派
	if (g_LogicManager->m_Hero->m_isocietyID > 0)
	{
		const TextInfo *TempText = g_CSVFileManager->GetTextByID(800);
		g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		g_UIManager->HideUI(UI_APPLYGANG);
		return;
	}
	////判断等级
	//if (g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL] < 30)
	//{
	//	const TextInfo *TempText = g_CSVFileManager->GetTextByID(801);
	//	g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
	//	g_UIManager->HideUI(UI_APPLYGANG);
	//	return;
	//}

	//判断输入是否为空
	if (content.empty())
	{
		const TextInfo *TempText = g_CSVFileManager->GetTextByID(803);
		g_UIManager->Notify(BaseFunction::a2u(TempText->strContent),Color3B(255,255,255));
		return;
	}

	MsgGang Msg(g_LogicManager->m_Hero->m_GUID,0,0,strTemp.c_str());
	Msg.MsgID = Msg_Create_Gang;
	g_NetManager->SendMsg(Msg);

	g_UIManager->HideUI(UI_APPLYGANG);
}

void UIApplyGang::InitGangList()
{
	//模板
	m_gangTemplate = static_cast<Layout*>(m_applyGangPanel->getChildByName("Panel_2"));
	m_gangTemplate->setVisible(false);

	//滚动
	m_gangSV = static_cast<cc2dui::ScrollView*>(m_applyGangPanel->getChildByName("ScrollView_1"));
	m_gangSV ->setSwallowTouches(true);

	
}

void UIApplyGang::GangAddItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,GangSimpleInfo *ganginfo)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	char szTemp[50];
	memset(szTemp,0,50);
	sprintf(szTemp,"%s lv.%d",ganginfo->szName,ganginfo->iLevel);
	(static_cast<Text*>(newLayout->getChildByName("Text_2")))->setString(BaseFunction::a2u(szTemp));
	sprintf(szTemp,"%d",ganginfo->iFight);
	(static_cast<Text*>(newLayout->getChildByName("Text_3")))->setString(BaseFunction::a2u(szTemp));
	sprintf(szTemp,"%s",ganginfo->szHeaderName);
	(static_cast<Text*>(newLayout->getChildByName("Text_4")))->setString(BaseFunction::a2u(szTemp));
	sprintf(szTemp,"%d/50",ganginfo->iGangNum);
	(static_cast<Text*>(newLayout->getChildByName("Text_4_Copy")))->setString(BaseFunction::a2u(szTemp));

	Button *btn;
	btn = static_cast<Button*>(newLayout->getChildByName("Button_3"));
	m_mapGangListBt.insert(pair<int64,Button*>(ganginfo->id,btn));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			map<int64,Button*>::iterator itt = m_mapGangListBt.begin();
			for (; itt != m_mapGangListBt.end() ; ++itt)
			{
				if (itt->second == static_cast<Button*>(_sender))
				{
					if (g_LogicManager->m_Hero->m_isocietyID <= 0)
					{
						MsgGang Msg(g_LogicManager->m_Hero->m_GUID,itt->first,0,"");
						Msg.MsgID = Msg_Apply_Gang;
						g_NetManager->SendMsg(Msg);
					}
					break;
				}
			}

		});
	}
	sv->addChild(newLayout);
}

void UIApplyGang::UpdateList()
{
	m_mapGangListBt.clear();
	m_gangSV->removeAllChildrenWithCleanup(true);
	vector<GangSimpleInfo>::iterator itt = g_LogicManager->m_vecGangList.begin();
	for ( ; itt != g_LogicManager->m_vecGangList.end(); itt ++)
	{
		GangAddItem(m_gangSV,m_gangTemplate->clone(),&(*itt));
	}

	m_gangSV->setLayoutType(Layout::Type::VERTICAL);
	m_gangSV->forceDoLayout();
	Size gangSize = m_gangSV->getInnerContainerSize();
	gangSize.height = m_gangSV->getChildrenCount() * m_gangTemplate->getContentSize().height;
	m_gangSV->setInnerContainerSize(gangSize);
}