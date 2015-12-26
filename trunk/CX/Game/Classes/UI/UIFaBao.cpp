#include "UIFaBao.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "Hero.h"
#include "CSVFileManager.h"
#include "UILoadingBar.h"

using namespace cocostudio;
using namespace cocostudio::timeline;

UIFaBao::UIFaBao()
{

}

UIFaBao::~UIFaBao()
{
}

bool UIFaBao::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!UIBase::init());
        Size winSize = Director::getInstance()->getVisibleSize();

		//载入各种面板
		if (!LoadNode())	return false;
		//载入法宝列表
		LoadWeaponList();

        bRet = true;
	} while (0);

	return bRet;
}

//加载各种面板
bool UIFaBao::LoadNode()
{
	//加载主面板
	m_fabaoPanel = CSLoader::createNode("UI/FaBao.csb");
	if (m_fabaoPanel == NULL)
		return false;
	addChild(m_fabaoPanel);

	//模板
	m_Template = static_cast<Layout*>(m_fabaoPanel->getChildByName("Panel_model"));
	m_Template->setVisible(false);

	//法宝与洗练窗口
	m_weasonPanel = static_cast<Node*>(m_fabaoPanel->getChildByName("panel_weason"));
	m_weasonPanel->setVisible(true);
	//法宝子窗口
	m_weasonPanel_wea = static_cast<Node*>(m_weasonPanel->getChildByName("panel_foot"));
	m_weasonPanel_wea->setVisible(true);
	//洗练子窗口
	m_weasonPanel_wash = static_cast<Node*>(m_weasonPanel->getChildByName("panel_foot2"));
	m_weasonPanel_wash->setVisible(false);

	//吸收窗口
	m_absorbPanel = static_cast<Node*>(m_fabaoPanel->getChildByName("panel_absorb"));
	m_absorbPanel->setVisible(false);
	//器灵窗口
	m_implementPanel = static_cast<Node*>(m_fabaoPanel->getChildByName("Panel_implement"));
	m_implementPanel->setVisible(false);
	//法宝列表窗口
	m_weaponList = static_cast<Node*>(m_fabaoPanel->getChildByName("bg_weaponlist"));
	m_weaponList->setVisible(true);

	Button *btn = nullptr;
	//关闭按钮
	btn = static_cast<Button*>(m_fabaoPanel->getChildByName("bt_Close"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			g_UIManager->HideUI(UI_FABAO);
		});
	}
	//器灵的激活按钮
    const TextInfo *pText = g_CSVFileManager->GetTextByID(667);
	(static_cast<cc2dui::Button*>(m_implementPanel->getChildByName("bt_enlighten")))->setTitleText(BaseFunction::a2u(pText->strContent)); // 激活

	CheckBox *cb;
	//法宝
	cb =  static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Weapon"));
	if (cb)
	{
		m_mainChecks.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}
	//洗练
	cb =  static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Wash"));
	if (cb)
	{
		m_mainChecks.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}
	//吸收
	cb =  static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Absorb"));
	if (cb)
	{
		m_mainChecks.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}
	//器灵
	cb =  static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Implement"));
	if (cb)
	{
		m_mainChecks.pushBack(cb);
		cb->setSelected(true);
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			ShowCheckBoxUI(_sender,type);
		});
	}

	(static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Weapon")))->setSelected(true);
	(static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Wash")))->setSelected(false);
	(static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Absorb")))->setSelected(false);
	(static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Implement")))->setSelected(false);

	cc2d::Node *footPanel = static_cast<Node*>(m_weasonPanel->getChildByName("panel_foot"));
	Button *bt;
	//点化按钮
	bt =  static_cast<cc2dui::Button*>(footPanel->getChildByName("bt_enlighten"));
	if (bt)
	{
		bt->addClickEventListener([this](Ref* _sender)
		{

		});
	}
	//激活按钮
	bt =  static_cast<cc2dui::Button*>(footPanel->getChildByName("bt_act"));
	if (bt)
	{
        const TextInfo *pText = g_CSVFileManager->GetTextByID(667);
		bt->setTitleText(BaseFunction::a2u(pText->strContent)); //激活
		bt->addClickEventListener([this](Ref* _sender)
		{
			g_LogicManager->m_Hero->m_CurrentMagicWeaponID = m_iCurrSelect;

			m_mapWeasonChecks.clear();
			m_weasonSV->removeAllChildren();

			for (map<int,MagicWeaponData>::iterator iter = g_LogicManager->m_Hero->m_MagicWeaponIDList.begin();iter != g_LogicManager->m_Hero->m_MagicWeaponIDList.end(); ++iter )
			{
				MagicWeaponData tagTemp = iter->second;
				const MagicWeapon *tagMagicWeapon = g_CSVFileManager->GetMagicWeaponByID(tagTemp.id);
				if (tagMagicWeapon != NULL)
				{ 
					char szTemp[30];
					sprintf(szTemp,"lv.%d %s",tagTemp.level,tagMagicWeapon->strName.c_str());

					if (iter->first == g_LogicManager->m_Hero->m_CurrentMagicWeaponID)
						AddWeasonRightItem(m_weasonSV,m_Template->clone(),iter->first,&tagTemp,true);
					else
						AddWeasonRightItem(m_weasonSV,m_Template->clone(),iter->first,&tagTemp,false);
				}
			}
			m_weasonSV->setLayoutType(Layout::Type::VERTICAL);
			m_weasonSV->forceDoLayout();
			Size WeasonSize = m_weasonSV->getInnerContainerSize();
			WeasonSize.height = m_weasonSV->getChildrenCount() * m_Template->getContentSize().height;
			m_weasonSV->setInnerContainerSize(WeasonSize);

			SetAbsorb(0);
		});
	}
	
	return true;
}

void UIFaBao::ShowCheckBoxUI(Ref* _sender,CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
	{
		CheckBox* tcb =  static_cast<CheckBox*>(_sender);
		cc2d::Vector<CheckBox*>::iterator it =  m_mainChecks.begin();
		for (; it != m_mainChecks.end(); it++)
		{
			(*it)->setSelected(false);
		}
		tcb->setSelected(true);

		if (tcb == static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Weapon")))
		{
			m_weasonPanel->setVisible(true);
			m_weasonPanel_wea->setVisible(true);
			m_weasonPanel_wash->setVisible(false);
			m_absorbPanel->setVisible(false);
			m_implementPanel->setVisible(false);
			m_weaponList->setVisible(true);
		}
		if (tcb == static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Wash")))
		{
			m_weasonPanel->setVisible(true);
			m_weasonPanel_wea->setVisible(false);
			m_weasonPanel_wash->setVisible(true);
			m_absorbPanel->setVisible(false);
			m_implementPanel->setVisible(false);
			m_weaponList->setVisible(true);
		}
		if (tcb == static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Absorb")))
		{
			m_weasonPanel->setVisible(false);
			m_weasonPanel_wea->setVisible(false);
			m_weasonPanel_wash->setVisible(false);
			m_absorbPanel->setVisible(true);
			m_implementPanel->setVisible(false);
			m_weaponList->setVisible(true);
		}
		if (tcb == static_cast<cc2dui::CheckBox*>(m_fabaoPanel->getChildByName("cb_Implement")))
		{
			m_weasonPanel->setVisible(false);
			m_weasonPanel_wea->setVisible(false);
			m_weasonPanel_wash->setVisible(false);
			m_absorbPanel->setVisible(false);
			m_implementPanel->setVisible(true);
			m_weaponList->setVisible(false);
		}
	}
}

//法宝列表载入
void UIFaBao::LoadWeaponList()
{
	m_iCurrSelect = 0;

	//右边滚动条
	m_weasonSV = static_cast<cc2dui::ScrollView*>(m_weaponList->getChildByName("ScrollView_model"));
	m_weasonSV ->setSwallowTouches(true);

	for (map<int,MagicWeaponData>::iterator iter = g_LogicManager->m_Hero->m_MagicWeaponIDList.begin();iter != g_LogicManager->m_Hero->m_MagicWeaponIDList.end(); ++iter )
	{
		MagicWeaponData tagTemp = iter->second;
		const MagicWeapon *tagMagicWeapon = g_CSVFileManager->GetMagicWeaponByID(tagTemp.id);
		if (tagMagicWeapon != NULL)
		{ 
			char szTemp[30];
			sprintf(szTemp,"lv.%d %s",tagTemp.level,tagMagicWeapon->strName.c_str());

			if (iter->first == g_LogicManager->m_Hero->m_CurrentMagicWeaponID)
				AddWeasonRightItem(m_weasonSV,m_Template->clone(),iter->first,&tagTemp,true);
			else
				AddWeasonRightItem(m_weasonSV,m_Template->clone(),iter->first,&tagTemp,false);
		}
	}
	m_weasonSV->setLayoutType(Layout::Type::VERTICAL);
	m_weasonSV->forceDoLayout();
	Size WeasonSize = m_weasonSV->getInnerContainerSize();
	WeasonSize.height = m_weasonSV->getChildrenCount() * m_Template->getContentSize().height;
	m_weasonSV->setInnerContainerSize(WeasonSize);

	SetWeason(0);
	SetAbsorb(0);
}

void UIFaBao::AddWeasonRightItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout, int pid, MagicWeaponData *pMagicWD, bool bIsAct)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	const MagicWeapon *tagMagicWeapon = g_CSVFileManager->GetMagicWeaponByID(pMagicWD->id);
	char szTemp[30];
	sprintf(szTemp,"lv.%d %s",pMagicWD->level,tagMagicWeapon->strName.c_str());
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_weasonname"))->setString(BaseFunction::a2u(szTemp));

	static_cast<cc2dui::ImageView*>(newLayout->getChildByName("img_ico"))->loadTexture(tagMagicWeapon->strIco);

	CheckBox *cb;
	cb =  static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("cb_bg"));
	if (cb)
	{
		if (m_iCurrSelect == pid)
		{
			cb->setSelected(true);
		}
		m_mapWeasonChecks.insert(pair<int,CheckBox*>(pid,cb));
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				map<int,CheckBox*>::iterator it =  m_mapWeasonChecks.begin();
				for (; it != m_mapWeasonChecks.end(); it++)
				{
					(*it).second->setSelected(false);
					if (tcb == it->second)
					{
						m_iCurrSelect = it->first;
						SetWeason(it->first);
						SetAbsorb(it->first);
					}
				}
				tcb->setSelected(true);
			}
		});
	}

	if (!bIsAct)
	{
		static_cast<cc2dui::ImageView*>(newLayout->getChildByName("bg_act"))->setVisible(false);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_act"))->setVisible(false);
	}

	sv->addChild(newLayout);
}

void UIFaBao::SetWeason(int pid)
{
	cc2d::Node *TempPanel = static_cast<Node*>(m_weasonPanel->getChildByName("panel_head"));

	if (pid != 0)
	{
		for (map<int,MagicWeaponData>::iterator iter = g_LogicManager->m_Hero->m_MagicWeaponIDList.begin();iter != g_LogicManager->m_Hero->m_MagicWeaponIDList.end(); ++iter )
		{
			if (iter->first == pid)
			{
				MagicWeaponData tagTemp = iter->second;
				const MagicWeapon *tagMagicWeapon = g_CSVFileManager->GetMagicWeaponByID(tagTemp.id);

				if (tagMagicWeapon != NULL)
				{ 
					//ico
					static_cast<cc2dui::ImageView*>(TempPanel->getChildByName("img_tmp1"))->setVisible(true);
					static_cast<cc2dui::ImageView*>(TempPanel->getChildByName("img_tmp1"))->loadTexture(tagMagicWeapon->strIco);
					char szTemp[30];
					//等级与成长
					sprintf(szTemp,"%d",tagTemp.level);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_curlevel"))->setString(szTemp);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_maxlevel"))->setString("/100");
					sprintf(szTemp,"%d",tagMagicWeapon->iGrowthMin);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_grow"))->setString(szTemp);
					//属性1
					const ForceForAttribute *attTemp = g_CSVFileManager->GetAttributeByID(tagMagicWeapon->iAttribute[0]);
					sprintf(szTemp,"%s:",attTemp->strAttName.c_str());
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("tx_physicsattack"))->setString(BaseFunction::a2u(szTemp));
					sprintf(szTemp,"%d",tagMagicWeapon->iValue[0]);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_physicsattack"))->setString(BaseFunction::a2u(szTemp));
					//属性2
					attTemp = g_CSVFileManager->GetAttributeByID(tagMagicWeapon->iAttribute[1]);
					sprintf(szTemp,"%s:",attTemp->strAttName.c_str());
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("tx_physicsdefense"))->setString(BaseFunction::a2u(szTemp));
					sprintf(szTemp,"%d",tagMagicWeapon->iValue[1]);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_physicsdefense"))->setString(BaseFunction::a2u(szTemp));
					//属性3
					attTemp = g_CSVFileManager->GetAttributeByID(tagMagicWeapon->iAttribute[2]);
					sprintf(szTemp,"%s:",attTemp->strAttName.c_str());
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("tx_magicattack"))->setString(BaseFunction::a2u(szTemp));
					sprintf(szTemp,"%d",tagMagicWeapon->iValue[2]);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_magicattack"))->setString(BaseFunction::a2u(szTemp));
					//属性4
					attTemp = g_CSVFileManager->GetAttributeByID(tagMagicWeapon->iAttribute[3]);
					sprintf(szTemp,"%s:",attTemp->strAttName.c_str());
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("tx_magicdefense"))->setString(BaseFunction::a2u(szTemp));
					sprintf(szTemp,"%d",tagMagicWeapon->iValue[3]);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_magicdefense"))->setString(BaseFunction::a2u(szTemp));
					//属性5
					attTemp = g_CSVFileManager->GetAttributeByID(tagMagicWeapon->iAttribute[4]);
					sprintf(szTemp,"%s:",attTemp->strAttName.c_str());
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("tx_critchance"))->setString(BaseFunction::a2u(szTemp));
					sprintf(szTemp,"%d",tagMagicWeapon->iValue[4]);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_critchance"))->setString(BaseFunction::a2u(szTemp));
					//属性6
					attTemp = g_CSVFileManager->GetAttributeByID(tagMagicWeapon->iAttribute[5]);
					sprintf(szTemp,"%s:",attTemp->strAttName.c_str());
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("tx_crithurt"))->setString(BaseFunction::a2u(szTemp));
					sprintf(szTemp,"%d",tagMagicWeapon->iValue[5]);
					static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_crithurt"))->setString(BaseFunction::a2u(szTemp));
				}
				break;
			}
		}
	}
	else
	{
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_curlevel"))->setString("0");
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_maxlevel"))->setString("/0");
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_grow"))->setString("0");
		static_cast<cc2dui::LoadingBar*>(TempPanel->getChildByName("LoadingBar_exp"))->setPercent(0);

		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_physicsattack"))->setString("0");
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_physicsdefense"))->setString("0");
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_magicattack"))->setString("0");
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_magicdefense"))->setString("0");
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_critchance"))->setString("0%");
		static_cast<cc2dui::Text*>(TempPanel->getChildByName("value_crithurt"))->setString("0");
		static_cast<cc2dui::ImageView*>(TempPanel->getChildByName("img_tmp1"))->setVisible(false);

		cc2d::Node *footPanel = static_cast<Node*>(m_weasonPanel->getChildByName("panel_foot"));

		static_cast<cc2dui::Text*>(footPanel->getChildByName("value_hold"))->setString("0%");
		static_cast<cc2dui::Text*>(footPanel->getChildByName("value_expend"))->setString("0/0");

		cc2d::Node *foot2Panel = static_cast<Node*>(m_weasonPanel->getChildByName("panel_foot2"));

		static_cast<cc2dui::Text*>(foot2Panel->getChildByName("value_hold"))->setString("0%");
		static_cast<cc2dui::Text*>(foot2Panel->getChildByName("value_expend"))->setString("0/0");
	}
}

//设置吸收面板
void UIFaBao::SetAbsorb(int pid)
{
	//左边已经激活的数据
	for (map<int,MagicWeaponData>::iterator iter = g_LogicManager->m_Hero->m_MagicWeaponIDList.begin();iter != g_LogicManager->m_Hero->m_MagicWeaponIDList.end(); ++iter )
	{
		if (iter->first ==  g_LogicManager->m_Hero->m_CurrentMagicWeaponID)
		{
			MagicWeaponData tagTemp = iter->second;
			const MagicWeapon *tagMagicWeapon = g_CSVFileManager->GetMagicWeaponByID(tagTemp.id);

			if (tagMagicWeapon != NULL)
			{
				//ico
				static_cast<cc2dui::ImageView*>(m_absorbPanel->getChildByName("img_tmp1"))->setVisible(true);
				static_cast<cc2dui::ImageView*>(m_absorbPanel->getChildByName("img_tmp1"))->loadTexture(tagMagicWeapon->strIco);
				char szTemp[30];
				//等级与成长
				sprintf(szTemp,"%d",tagTemp.level);
				static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_curlevel_left"))->setString(szTemp);
				static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_maxlevel_left"))->setString("/100");
				sprintf(szTemp,"%d",tagMagicWeapon->iGrowthMin);
				static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_grow_left"))->setString(szTemp);
				//法宝名称
				sprintf(szTemp,"%s",tagMagicWeapon->strName.c_str());
				static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("tx_weason_left"))->setString(BaseFunction::a2u(szTemp));
				//经验
				static_cast<cc2dui::LoadingBar*>(m_absorbPanel->getChildByName("LoadingBar_exp_left"))->setPercent(0);
			}
			break;
		}
	}

	if (pid != 0)
	{
		for (map<int,MagicWeaponData>::iterator iter = g_LogicManager->m_Hero->m_MagicWeaponIDList.begin();iter != g_LogicManager->m_Hero->m_MagicWeaponIDList.end(); ++iter )
		{
			if (iter->first == pid)
			{
				MagicWeaponData tagTemp = iter->second;
				const MagicWeapon *tagMagicWeapon = g_CSVFileManager->GetMagicWeaponByID(tagTemp.id);

				if (tagMagicWeapon != NULL)
				{
					//ico
					static_cast<cc2dui::ImageView*>(m_absorbPanel->getChildByName("img_tmp2"))->setVisible(true);
					static_cast<cc2dui::ImageView*>(m_absorbPanel->getChildByName("img_tmp2"))->loadTexture(tagMagicWeapon->strIco);
					char szTemp[30];
					//等级与成长
					sprintf(szTemp,"%d",tagTemp.level);
					static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_curlevel_right"))->setString(szTemp);
					static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_maxlevel_right"))->setString("/100");
					sprintf(szTemp,"%d",tagMagicWeapon->iGrowthMin);
					static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_grow_right"))->setString(szTemp);

					sprintf(szTemp,"%s",tagMagicWeapon->strName.c_str());
					static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("tx_weason_right"))->setString(BaseFunction::a2u(szTemp));
				}
				break;
			}
		}
	}
	else
	{
		static_cast<cc2dui::ImageView*>(m_absorbPanel->getChildByName("img_tmp2"))->setVisible(false);

		static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_curlevel_right"))->setString("0");
		static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_maxlevel_right"))->setString("/0");
		static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("value_grow_right"))->setString("0");
		static_cast<cc2dui::LoadingBar*>(m_absorbPanel->getChildByName("LoadingBar_exp_right"))->setPercent(0);

		static_cast<cc2dui::Text*>(m_absorbPanel->getChildByName("tx_weason_right"))->setString("");
	}
}