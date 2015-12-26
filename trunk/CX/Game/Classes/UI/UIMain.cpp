#include "UIMain.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "LogicManager.h"
#include "BaseFunction.h"
#include "Hero.h"
#include "CSVFileManager.h"
#include "CocoStudio.h"
#include "ResManager.h"
#include "CCArmatureDataManager.h"
#include "ArmatureExtend.h"
#include "UIMiniMap.h"
#include "SceneManager.h"
#include "NetManager.h"
#include "Monster.h"
#include "UITalk.h"
#include "UICreateRole.h"
#include "CSVFileManager.h"
using namespace cocostudio;
using namespace cocostudio::timeline;

const int TAG_GO_CONSTRUCT = 1;
const float CD_interval = 0.05f;

static const char* buffUiName[UIMain::MaxCountShowBuff] = { "img_buff1", "img_buff2", "img_buff3", "img_buff4", "img_buff5", "img_buff6" };

UIMain::UIMain():vip_text(NULL),Exp(NULL)
{
    m_comSkillIcon = nullptr;
    m_comSkillbtn = nullptr;
    m_xpSkillIcon = nullptr;
    m_xpSkillbtn = nullptr;
    for (int i = 0; i < 4; ++i)
    {
        m_SkillIcons[i] = nullptr;
        m_SkillFrames[i] = nullptr;
    }
    m_TouchPanel = nullptr;

    m_PlayerMenuPnl = nullptr;
    m_MenuImgBg = nullptr;
    m_MenuButton= nullptr;


    m_bRedDisable = false;
    m_fRedNum = 0;

    m_bBlueDisable = false;
    m_fBlueNum = 0;

    m_bSkill1Disable = false;
    m_fSkill1Num = 0;

    m_bSkill2Disable = false;
    m_fSkill2Num = 0; 

    m_bSkill3Disable = false;
    m_fSkill3Num = 0;

    m_bSkillxpDisable = false;
    m_fSkillxpNum = 0;

    m_CDInterval = 0.0f;
}

UIMain::~UIMain()
{
    if (m_MenuButton)
    {
        m_MenuButton->release();
    }
}

bool UIMain::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!UIBase::init());
		m_MainPanel = CSLoader::createNode("UI/Main.csb");	
		addChild(m_MainPanel);

		//初始化
		m_vecMainMessage.clear();

		//系统信息
		m_infobg = static_cast<ImageView*>(m_MainPanel->getChildByName("img_noticebg"));
		m_infobg->setVisible(false);
		m_infoContent = static_cast<Text*>(m_MainPanel->getChildByName("tx_notice"));
		m_infoContent->setVisible(false);

		//右上角BT
		m_BtPanel = m_MainPanel->getChildByName("panel_bt");
		m_BtPanel->setVisible(false);
		m_btCheck = static_cast<CheckBox*>(m_MainPanel->getChildByName("cb_show"));
		if (m_btCheck)
		{
			m_btCheck->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				if (type == CheckBox::EventType::SELECTED)
				{
					m_BtPanel->setVisible(true);
				}

				if (type == CheckBox::EventType::UNSELECTED)
				{
					m_BtPanel->setVisible(false);
				}
			});
		}
		Button *temp = static_cast<Button*>(m_BtPanel->getChildByName("bt_7"));
		if (temp)
		{
			temp->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->OpenUI(UI_UICHEST);
			});
		}

		//任务与组队栏
		m_taskTeamPanel = m_MainPanel->getChildByName("panel_task");
		m_taskTeamPanel->setVisible(false);

		//加载模板
		m_TaskTemplate = static_cast<Layout*>(m_taskTeamPanel->getChildByName("panel_modeltask"));
		m_TaskTemplate->setVisible(false);
		m_TeamTemplate = static_cast<Layout*>(m_taskTeamPanel->getChildByName("panel_modelteam"));
		m_TeamTemplate->setVisible(false);
		//加载滚动控件
		m_svPanel = static_cast<cc2dui::ScrollView*>(m_taskTeamPanel->getChildByName("sv_task"));
		m_svPanel->setSwallowTouches(true);

		m_cdTaskTeam = static_cast<CheckBox*>(m_MainPanel->getChildByName("cb_task"));
		m_cdTaskTeam->setPosition(Vec2(20,440));
		if (m_cdTaskTeam)
		{
			m_cdTaskTeam->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				if (type == CheckBox::EventType::SELECTED)
				{
					m_taskTeamPanel->setVisible(true);
					m_cdTaskTeam->setPosition(Vec2(230,440));
				}

				if (type == CheckBox::EventType::UNSELECTED)
				{
					m_taskTeamPanel->setVisible(false);
					m_cdTaskTeam->setPosition(Vec2(20,440));
				}
			});
		}

		TaskInit();

		//点击的物品采集
		m_iCollectItem = 0;
		//没有在采集
		m_bIsCollect = false;

		//队友checkbox点击效果
		m_csTeam = static_cast<CheckBox*>(m_taskTeamPanel->getChildByName("cb_team"));
		m_csTeam->setSelected(false);
		if (m_csTeam)
		{
			m_Checkboxs.pushBack(m_csTeam);
			m_csTeam->addEventListener([this](Ref* _sender,CheckBox::EventType type)
			{
				if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
				{
					CheckBox* tcb =  static_cast<CheckBox*>(_sender);
					cc2d::Vector<CheckBox*>::iterator it =  m_Checkboxs.begin();
					for (; it != m_Checkboxs.end(); it++)
					{
						(*it)->setSelected(false);
					}
					tcb->setSelected(true);

                    LoadTeamInfo();

					m_svPanel->setLayoutType(Layout::Type::VERTICAL);
					m_svPanel->forceDoLayout();
					Size ContainerSize = m_svPanel->getInnerContainerSize();
					ContainerSize.height = m_svPanel->getChildrenCount() * m_TeamTemplate->getContentSize().height;
					m_svPanel->setInnerContainerSize(ContainerSize);
				}
			});
		}
		
		//新装备提示框
		m_newEquipment = m_MainPanel->getChildByName("panel_equ");
		m_newEquipment->setVisible(false);
		m_neBtEquip = static_cast<Button*>(m_newEquipment->getChildByName("bt_equ_equ"));
		m_neBtClose = static_cast<Button*>(m_newEquipment->getChildByName("bt_equ_close"));
		if (m_neBtClose)
		{
			m_neBtClose->addClickEventListener([this](Ref* _sender)
			{
				m_newEquipment->setVisible(false);
			});
		}

		//提示框
		m_taskTipbg = static_cast<ImageView*>(m_MainPanel->getChildByName("img_infobg"));
		m_taskTipbg->setVisible(false);
		m_taskTipContent = static_cast<Text*>(m_MainPanel->getChildByName("tx_info"));
		m_taskTipContent->setVisible(false);

		//（开箱子）进度条
		m_barbg = static_cast<ImageView*>(m_MainPanel->getChildByName("img_barbg"));
		m_barbg->setVisible(false);
		m_barWord = static_cast<Text*>(m_MainPanel->getChildByName("Text_46"));
		m_barWord->setVisible(false);
		m_bar = static_cast<LoadingBar*>(m_MainPanel->getChildByName("img_bar"));
		m_bar->setPercent(88);
		m_bar->setVisible(false);

		//经验提示
		m_expInfo = m_MainPanel->getChildByName("panel_expinfo");
		m_expInfo->setVisible(false);

		//控制杆
		m_Control = m_MainPanel->getChildByName("panel_control");
		m_Control->setVisible(false);

		Button* btn;
		//聊天框
		m_chatPanel = m_MainPanel->getChildByName("panel_chat");
		btn = static_cast<Button*>(m_chatPanel->getChildByName("bt_chat"));	

		static_cast<Text*>(m_chatPanel->getChildByName("tx_channel1"))->setVisible(false);
		static_cast<Text*>(m_chatPanel->getChildByName("tx_content1"))->setVisible(false);
		static_cast<Text*>(m_chatPanel->getChildByName("tx_channel2"))->setVisible(false);
		static_cast<Text*>(m_chatPanel->getChildByName("tx_content2"))->setVisible(false);
		static_cast<Text*>(m_chatPanel->getChildByName("tx_channel3"))->setVisible(false);
		static_cast<Text*>(m_chatPanel->getChildByName("tx_content3"))->setVisible(false);

		btn->addClickEventListener([this](Ref* _sender)
		{
			//!打开聊天界面
		    g_UIManager->OpenUI(UI_CHAT);
		});
				
		//人物属性
		ProtagonistInit();
		TargetInit();		
		

		//技能CD遮罩
		m_sprSkillcd = static_cast<Sprite*>(m_MainPanel->getChildByName("spr_skillcd"));
		m_sprSkillcd->setVisible(false);

        InitSkillPanel();

		//红蓝CD遮罩
		m_disable = static_cast<Sprite*>(m_roleAtt->getChildByName("spr_disable"));
		m_disable->setVisible(false);

		//红CD
		btn = static_cast<Button*>(m_roleAtt->getChildByName("bt_redpoint"));	
		btn->addClickEventListener([this](Ref* _sender)
		{
			if (!m_bRedDisable)
			{
                Hero* pHero = g_LogicManager->m_Hero;
                if (pHero->m_Attribute[ATTRIBUTE_HP] >= pHero->m_Attribute[ATTRIBUTE_HPMAX])
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(657);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //血量已满!
                }else
                {
                    CliItemInfo* pItem = pHero->m_Bag.GetItemByTypeId(60200102);
                    if (pItem)
                    {
                        MsgItem_Use useMsg;
                        useMsg.operateType = ItemOperate_ItemUsed;
                        useMsg.useItemGuid = pItem->Data.guid;
                        g_NetManager->SendMsg(useMsg);

                       SetProgress(m_progressRed,g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_HP_PUBLIC_CD]);                      
                    }else
                    {
                        const TextInfo *pText = g_CSVFileManager->GetTextByID(658);
                        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //补血物品不足!
                    }
                }
			}else
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(659);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //CD尚未冷却!
            }
		});
		m_progressRed = ProgressTimer::create(m_disable);
		m_progressRed->setType(ProgressTimer::Type::RADIAL);
		m_roleAtt->addChild(m_progressRed);
		Vec2 tempPoint = static_cast<Button*>(m_roleAtt->getChildByName("bt_redpoint"))->getPosition();
		m_progressRed->setPosition(tempPoint);
		m_progressRed->setMidpoint(Vec2(0.5f, 0.5f));
		m_progressRed->setVisible(false);
		m_bRedDisable = false;

		//蓝CD
		btn = static_cast<Button*>(m_roleAtt->getChildByName("bt_bluepoint"));	
		btn->addClickEventListener([this](Ref* _sender)
		{
			if (!m_bBlueDisable)
			{
                Hero* pHero = g_LogicManager->m_Hero;
                if (pHero->m_Attribute[ATTRIBUTE_MP] >= pHero->m_Attribute[ATTRIBUTE_MPMAX])
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(660);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //魔法值已满!
                }else
                {
                    CliItemInfo* pItem = pHero->m_Bag.GetItemByTypeId(61200102);
                    if (pItem)
                    {
                        MsgItem_Use useMsg;
                        useMsg.operateType = ItemOperate_ItemUsed;
                        useMsg.useItemGuid = pItem->Data.guid;
                        g_NetManager->SendMsg(useMsg);

                         //>蓝公共CD
                        SetProgress(m_progressBlue,g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_MP_PUBLIC_CD]);              
                    }else
                    {
                        const TextInfo *pText = g_CSVFileManager->GetTextByID(661);
                        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //补蓝物品不足!
                    }
                }
			}else
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(659);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //CD尚未冷却!
            }
		});
		m_progressBlue = ProgressTimer::create(m_disable);
		m_progressBlue->setType(ProgressTimer::Type::RADIAL);
		m_roleAtt->addChild(m_progressBlue);
		Vec2 bluePoint = static_cast<Button*>(m_roleAtt->getChildByName("bt_bluepoint"))->getPosition();
		m_progressBlue->setPosition(bluePoint);
		m_progressBlue->setMidpoint(Vec2(0.5f, 0.5f));
		m_progressBlue->setVisible(false);
		m_bBlueDisable = false;

		//采集
		//进度条
		m_sprProcd = static_cast<Sprite*>(m_MainPanel->getChildByName("spr_procd"));
		m_sprProcd->setVisible(false);

		m_progressCollect = ProgressTimer::create(m_sprProcd);
		m_progressCollect->setType(kCCProgressTimerTypeBar);
		m_progressCollect->setReverseProgress(true);
		m_MainPanel->addChild(m_progressCollect);
		Vec2 proPoint = static_cast<ImageView*>(m_MainPanel->getChildByName("img_barbg"))->getPosition();
		m_progressCollect->setPosition(proPoint);
		m_progressCollect->setMidpoint(Point(1,0));
		m_progressCollect->setBarChangeRate(Point(1,0));
		m_progressCollect->setVisible(false);
		m_bCollectDisable = false;

	    //小地图
		Sprite* sprite = static_cast<Sprite*>(m_MainPanel->getChildByName("MapBack"));
		if (sprite)
		{
			m_pMiniMap = UIMiniMap::create();
			m_pMiniMap->setPosition(sprite->getPosition());
			addChild(m_pMiniMap, sprite->getLocalZOrder() - 1);
			if (g_SceneManager->m_MainLayer)
			{
				SetMiniMap(g_SceneManager->m_MainLayer->m_MapId);
			}
		}

        m_PlayerMenuPnl = m_MainPanel->getChildByName("Panel_PlayerMenu");
        m_MenuImgBg = (ImageView*)m_PlayerMenuPnl->getChildByName("Image_bg");
        m_MenuButton = (Button*)m_PlayerMenuPnl->getChildByName("Button_zd");
        CCASSERT(m_PlayerMenuPnl && m_MenuImgBg && m_MenuButton, "");

        m_MenuButton->removeFromParentAndCleanup(false);
        m_MenuButton->retain();

        m_MenuCallback = ([this](Ref* _sender)
        {
            Button* pBtn = static_cast<Button*>(_sender);
            MenuEnum menu = (MenuEnum)pBtn->getTag();
            switch (menu)
            {
            case UIMain::TeamJoin:
                OnTeamApply();
                break;
            case UIMain::TeamKick:
                OnTeamKick();
                break;
            case UIMain::TeamChangeLeader:
                OnTeamChangeLeader();
                break;
            case UIMain::TeamDimiss:
                OnTeamDimiss();
                break;
            case UIMain::TeamExit:
                OnTeamExit();
                break;
            case UIMain::SingleChat:
                OnSingleChat();
                break;
            case UIMain::SeeInfo:
                OnSeeInfo();
                break;
            default:
                break;
            }
            
            HidePlayerMenu();
        });

		ChangeHero();
		bRet = true;	
	} while (0);

	return bRet;
}

void UIMain::ChangeHero()
{
	if (!g_LogicManager->m_Hero)
	{
		return;
	}
    Hero* pHero = g_LogicManager->m_Hero;
	HeroIcon->loadTexture(UICreateRole::GetHeadIconPath((RoleSex)pHero->m_Sex, pHero->m_HeadIconIndex));
	HeroName->setString(BaseFunction::a2u(pHero->m_Name.c_str()));	
	vip_text->setProperty(__String::createWithFormat("%d",pHero->m_VIPLevel)->getCString(), "Public/vip_font.png", 20, 20, "0");
	
	if (pHero->m_Attribute[ATTRIBUTE_HP] > 0)
	{		
		HeroHP->setPercent(pHero->m_Attribute[ATTRIBUTE_HP]*100/pHero->m_Attribute[ATTRIBUTE_HPMAX]);		
	}
	else
	{
		g_LogicManager->m_Hero->m_SelTarget = NULL;
		m_targetAtt->setVisible(false);
		HeroHP->setPercent(0);
	}	
	if (pHero->m_Attribute[ATTRIBUTE_MP])
	{
		HeroMP->setPercent(pHero->m_Attribute[ATTRIBUTE_MP]*100/pHero->m_Attribute[ATTRIBUTE_MPMAX]);
	}
	else
	{
		HeroMP->setPercent(0);
	}

	//战斗力
	char szRoleAttTemp[50];
	TextAtlas* textAtlasFight;
	sprintf(szRoleAttTemp,"%d",g_LogicManager->m_Hero->m_BattlePower);
	textAtlasFight = TextAtlas::create(szRoleAttTemp, "Public/main_fight.png", 9, 16, "0");
	textAtlasFight->setPosition(Point(150,470));
	addChild(textAtlasFight);

	//等级tx_level
	sprintf(szRoleAttTemp,"%d",(int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL]);
	(static_cast<Text*>(m_roleAtt->getChildByName("tx_level")))->setText(szRoleAttTemp);

	if (pHero->m_Attribute[ATTRIBUTE_EXP] > 0 && g_CSVFileManager->GetLevelByID(pHero->m_Attribute[ATTRIBUTE_LEVEL]))
	{	
		Exp->setPercent(pHero->m_Attribute[ATTRIBUTE_EXP]*100/g_CSVFileManager->GetLevelByID(pHero->m_Attribute[ATTRIBUTE_LEVEL])->iExpValue);
		const LevelInfo* leveltemp = g_CSVFileManager->GetLevelByID(pHero->m_Attribute[ATTRIBUTE_LEVEL]);
		int32 currExp = pHero->m_Attribute[ATTRIBUTE_EXP];
		int32 maxExp = leveltemp->iExpValue;
		char szTemp[20];
		memset(szTemp,0,20);
		sprintf(szTemp,"%d/%d",currExp,maxExp);
		static_cast<Text*>(m_MainPanel->getChildByName("tx_exp"))->setString(szTemp);
	}
	else
	{
		Exp->setPercent(0);
	}

	//std::vector<BuffInfoIt>::iterator it = g_LogicManager->m_Hero->m_BuffList.begin();
	//int i = 0;
	//for (;  it != g_LogicManager->m_Hero->m_BuffList.end() && i < 4; ++it,i++)
	//{
	//	const BuffInfo *buff = g_CSVFileManager->GetBuffByID(it->id);
	//	if (buff)
	//	{
	//		m_HeroBuff[i]->loadTexture(buff->strIconPath);
	//		m_HeroBuff[i]->setVisible(true);
	//	}		
	//}
	//for (int j = i;  j < 4; j++)
	//{
	//	m_HeroBuff[i]->setVisible(false);
	//}
}

void UIMain::PointClickCollect(int32 parid, int32 Itemid)
{
	const ResInfo *resTemp = g_CSVFileManager->GetResByID(parid);

	if (resTemp)
	{
		if (!m_bCollectDisable)
		{
			m_iCollectItem = resTemp->id;
			if (m_iCollectItem == Itemid)
			{
				SetProgress(m_progressCollect,resTemp->iTime);
			}
		}
	}
	
}

void UIMain::ChangeSelTarget()
{
    if (!g_LogicManager->m_Hero->m_SelTarget || g_LogicManager->m_Hero->m_SelTarget->IsDead())
    {
        g_LogicManager->m_Hero->m_SelTarget = NULL;
        m_targetAtt->setVisible(false);
        return;
    }

	////!判断类型
	//if (g_LogicManager->m_Hero->m_SelTarget->m_ObjectType == OBJECT_MONSTER)
	//{
	//	m_targetAtt->setVisible(true);
	//	TargetLevel->setVisible(true);
	//	char szTemp[32];
	//	sprintf(szTemp,"%d",((Monster*)(g_LogicManager->m_Hero->m_SelTarget))->m_MonsterInfo->iLevel);
	//	TargetLevel->setString(szTemp);
	//	TargetIcon->loadTexture(((Monster*)(g_LogicManager->m_Hero->m_SelTarget))->m_MonsterInfo->strHeadIcon);
	//	TargetName->setString(BaseFunction::a2u(((Monster*)(g_LogicManager->m_Hero->m_SelTarget))->m_MonsterInfo->strName));
	//	TargetHP->setPercent(g_LogicManager->m_Hero->m_SelTarget->m_HP*100/g_LogicManager->m_Hero->m_SelTarget->m_MaxHP);
	//}
	if (g_LogicManager->m_Hero->m_SelTarget->m_Type == OBJECT_PLAYER)
	{
        Player* player = (Player*)(g_LogicManager->m_Hero->m_SelTarget);
		if (player != NULL)
		{
            m_targetAtt->setVisible(true);
            TargetLevel->setVisible(true);
		    char szTemp[32];
		    sprintf(szTemp,"%d",player->m_Level);
		    TargetLevel->setString(szTemp);
		    TargetIcon->loadTexture(UICreateRole::GetHeadIconPath((RoleSex)player->m_Sex, player->m_HeadIconIndex));
		    TargetName->setString(BaseFunction::a2u(player->m_Name));
		    TargetHP->setPercent(player->GetHp()*100/player->GetHpMax());
		}
	}
    else if (g_LogicManager->m_Hero->m_SelTarget->m_Type == OBJECT_NPC)
    {
        ClickNpc();
    }

	//std::vector<BuffInfoIt>::iterator it = g_LogicManager->m_Hero->m_SelTarget->m_BuffList.begin();
	//int i = 0;
	//for (;  it != g_LogicManager->m_Hero->m_SelTarget->m_BuffList.end() && i < 4; ++it,i++)
	//{
	//	const BuffInfo *buff = g_CSVFileManager->GetBuffByID(it->id);
	//	if (buff)
	//	{
	//		m_TargetBuff[i]->loadTexture(buff->strIconPath);
	//		m_TargetBuff[i]->setVisible(true);
	//	}		
	//}
	//for (int j = i;  j < 4; j++)
	//{
	//	m_TargetBuff[i]->setVisible(false);
	//}
}

 void UIMain::SetMiniMap(int mapid)
 {
    if (m_pMiniMap)
    {
        m_pMiniMap->SetMapId(mapid);
    }
 }

 void UIMain::OnShow()
 {
     UIBase::OnShow();
     setLocalZOrder(LAYER_UI - 1); //>调整显示级别比其他UI低.
 }

 bool UIMain::TouchJoystickInRange(cc2d::Touch *touch)
 {
     //Vec2 position = m_MainPanel->convertTouchToNodeSpace(touch);
     //Rect skillRect1(600.0, 0.0, 430.0, 120.0);
     //Rect skillRect2(824.0, 120.0, 200.0, 200.0);
     //if (skillRect1.containsPoint(position))
     //{
     //    return false;
     //}
     //if (skillRect2.containsPoint(position))
     //{
     //    return false;
     //}
     //return true;

     Vec2 position = m_MainPanel->convertTouchToNodeSpace(touch);
     Rect touchRect(20.0, 20.0, 210.0, 210.0);
     if (touchRect.containsPoint(position))
     {
         return true;
     }
     return false;
 }

 void UIMain::HeroIcon_callback(Ref* pSender , ui::TouchEventType type)
{
	if ( type == TOUCH_EVENT_ENDED )
	{	
		//g_SceneManager->Main2Fight();
		
	}
}

void UIMain::AddTeamItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,  std::string itemName,  int32 level, int32 hp, int32 maxHp, bool leader)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);
    newLayout->setTag(sv->getChildrenCount());
    newLayout->addClickEventListener([this](Ref* pSender)
    {
        cc2dui::Layout* layout = (cc2dui::Layout*)pSender;
        if(layout)
        {
            size_t index = layout->getTag();
            if (g_LogicManager->m_Hero->m_team.m_Members.size() > index)
            {
                PopPlayerMenu(layout->getWorldPosition() + layout->getContentSize() * 0.5f, g_LogicManager->m_Hero->m_team.m_Members[index].memberId);
            }
        }
    });

	Text* txName = static_cast<Text*>(newLayout->getChildByName("tx_name"));
    if (txName)
    {
        txName->setString(BaseFunction::a2u(itemName.c_str()));
    }

    Text* txLevel = static_cast<Text*>(newLayout->getChildByName("tx_level"));
    if (txLevel)
    {
        txLevel->setString(StringUtils::format("Lv.%d", level));
    }

    LoadingBar* barHp = static_cast<LoadingBar*>(newLayout->getChildByName("bar_red"));
    if (barHp)
    {
        barHp->setPercent(hp*100.0f/maxHp);
    }

    ImageView* pView = static_cast<ImageView*>(newLayout->getChildByName("Img_leader"));
    if (pView)
    {
        if (leader)
            pView->setVisible(true);
        else
            pView->setVisible(false);
    }

	sv->addChild(newLayout);
}


void UIMain::LoadTeamInfo()
{
    if (m_Checkboxs.at(0)->isSelected())
        return;

    m_svPanel->removeAllChildren();

    int memberNumber =  g_LogicManager->m_Hero->m_team.m_Members.size();
    
    if (memberNumber > 0)
        m_svPanel->setVisible(true);
    else
        m_svPanel->setVisible(false);

    for (int i = 0; i < memberNumber; ++i)
    {
        Hero::Team::Member& member = g_LogicManager->m_Hero->m_team.m_Members[i];
        AddTeamItem(m_svPanel,m_TeamTemplate->clone(), member.szName, member.memberLevel, member.Hp, member.MaxHp, member.attribute == Hero::Team::MA_Leaber);
    }
}

void UIMain::UpdateTeamHp()
{
    //Vector<Node*>& childs = m_svPanel->getChildren();
    //int count = childs.size();
    //for (int i = 0; i < count; ++i)
    //{
    //    Hero::Team::Member& member = g_LogicManager->m_Hero->m_team.m_Members[i];
    //    Node* pInfoPnl = childs.at(i);
    //    Player* player = g_LogicManager->GetPlayerByGUID(member.memberId);
    //    if (player)
    //    {
    //        Text* txLevel = static_cast<Text*>(pInfoPnl->getChildByName("tx_level"));
    //        if (txLevel)
    //        {
    //            txLevel->setString(StringUtils::format("Lv.%d", player->m_Level));
    //        }

    //        LoadingBar* barHp = static_cast<LoadingBar*>(pInfoPnl->getChildByName("bar_red"));
    //        if (barHp)
    //        {
    //            barHp->setPercent(player->m_HP*100.0f/player->m_MaxHP);
    //        }
    //    }

    //    if (member.memberId == g_LogicManager->m_Hero->m_GUID)
    //    {
    //        Text* txLevel = static_cast<Text*>(pInfoPnl->getChildByName("tx_level"));
    //        if (txLevel)
    //        {
    //            txLevel->setString(StringUtils::format("Lv.%d", (int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL]));
    //        }

    //        LoadingBar* barHp = static_cast<LoadingBar*>(pInfoPnl->getChildByName("bar_red"));
    //        if (barHp)
    //        {
    //            barHp->setPercent(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_HP]*100.0f/g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_HPMAX]);
    //        }
    //    }
    //}
}

void UIMain::RedCD(float dt)
{  
    if (m_fRedNum > 0)
    {
		m_progressRed->setPercentage(m_fRedNum);
		m_fRedNum = m_fRedNum - m_fRedLevel;
    }
	if (m_fRedNum == 0)
	{
		this->unschedule(schedule_selector(UIMain::RedCD)); 
		m_progressRed->setVisible(false);
		m_bRedDisable = false;
	}
} 

void UIMain::BlueCD(float dt)
{  
	if (m_fBlueNum > 0)
	{
		m_progressBlue->setPercentage(m_fBlueNum);
		m_fBlueNum = m_fBlueNum - m_fBlueLevel;
	}
	if (m_fBlueNum == 0)
	{
		this->unschedule(schedule_selector(UIMain::BlueCD)); 
		m_progressBlue->setVisible(false);
		m_bBlueDisable = false;
	}
} 

void UIMain::Skill1CD(float dt)
{
	if (m_fSkill1Num > 0)
	{
		m_progressSkill1->setPercentage(m_fSkill1Num);
		m_fSkill1Num = m_fSkill1Num - m_fSkill1Level;
	}
	if (m_fSkill1Num <= 0)
	{
		this->unschedule(schedule_selector(UIMain::Skill1CD)); 
		m_progressSkill1->setVisible(false);
		m_bSkill1Disable = false;
	}
}

void UIMain::Skill2CD(float dt)
{
	if (m_fSkill2Num > 0)
	{
		m_progressSkill2->setPercentage(m_fSkill2Num);
		m_fSkill2Num = m_fSkill2Num - m_fSkill2Level;
	}
	if (m_fSkill2Num == 0)
	{
		this->unschedule(schedule_selector(UIMain::Skill2CD)); 
		m_progressSkill2->setVisible(false);
		m_bSkill2Disable = false;
	}
}

void UIMain::Skill3CD(float dt)
{
	if (m_fSkill3Num > 0)
	{
		m_progressSkill3->setPercentage(m_fSkill3Num);
		m_fSkill3Num = m_fSkill3Num - m_fSkill3Level;
	}
	if (m_fSkill3Num <= 0)
	{
		this->unschedule(schedule_selector(UIMain::Skill3CD)); 
		m_progressSkill3->setVisible(false);
		m_bSkill3Disable = false;
	}
}

void UIMain::SkillxpCD(float dt)
{
	if (m_fSkillxpNum > 0)
	{
		m_progressSkillxp->setPercentage(m_fSkillxpNum);
		m_fSkillxpNum = m_fSkillxpNum - m_fSkillxpLevel;
	}
	if (m_fSkillxpNum <= 0)
	{
		this->unschedule(schedule_selector(UIMain::SkillxpCD)); 
		m_progressSkillxp->setVisible(false);
		m_bSkillxpDisable = false;

	}
}

void UIMain::CollectCD(float dt)
{
	if (m_fCollectNum > 0)
	{
		m_bIsCollect = true;
		m_progressCollect->setPercentage(m_fCollectNum);
		m_fCollectNum = m_fCollectNum - m_fCollectLevel;
	}
	if (m_fCollectNum <= 0)
	{
		m_bIsCollect = false;
		this->unschedule(schedule_selector(UIMain::CollectCD)); 
		m_progressCollect->setVisible(false);
		m_bCollectDisable = false;
		m_barbg->setVisible(false);
		m_barWord->setVisible(false);

		if (m_iCollectItem != 0)
		{
			MsgTask_MakeTask msg;
			msg.MsgID = Msg_Task_Collect;
			msg.OBJID = g_LogicManager->m_Hero->m_GUID;
			msg.taskId = m_iCollectItem;
			g_NetManager->SendMsg(msg);
			m_iCollectItem = 0;
		}
	}
}

void UIMain::StopCollectCD()
{
	if (m_bCollectDisable)
	{
		this->unschedule(schedule_selector(UIMain::CollectCD)); 
		m_barbg->setVisible(false);
		m_barWord->setVisible(false);
		m_progressCollect->setVisible(false);
		m_bCollectDisable = false;
	}
}

void UIMain::SetProgress(ProgressTimer* pPro,float fSec)
{
	if (pPro == m_progressSkill1)
	{
		m_progressSkill1->setVisible(true);
		m_bSkill1Disable = true;
		m_fSkill1Num = 100;
		m_fSkill1Level =  100 / ( fSec/CD_interval );
	}
	else if (pPro == m_progressSkill2)
	{
		m_progressSkill2->setVisible(true);
		m_bSkill2Disable = true;
		m_fSkill2Num = 100;
		m_fSkill2Level =  100 / ( fSec/CD_interval );
	}
	else if (pPro == m_progressSkill3)
	{
		m_progressSkill3->setVisible(true);
		m_bSkill3Disable = true;
		m_fSkill3Num = 100;
		m_fSkill3Level =  100 / ( fSec/CD_interval );
	}
	else if (pPro == m_progressSkillxp)
	{
		m_progressSkillxp->setVisible(true);
		m_bSkillxpDisable = true;
		m_fSkillxpNum = 100;
		m_fSkillxpLevel =  100 / ( fSec/CD_interval );
	}
	else if (pPro == m_progressRed)
	{
		m_progressRed->setVisible(true);
		m_bRedDisable = true;
		m_fRedNum = 100;
		m_fRedLevel =  100 / ( fSec/CD_interval );
	}
	else if (pPro == m_progressBlue)
	{
		m_progressBlue->setVisible(true);
		m_bBlueDisable = true;
		m_fBlueNum = 100;
		m_fBlueLevel =  100 / ( fSec/CD_interval );
	}
	else if (pPro == m_progressCollect)
	{
		m_progressCollect->setVisible(true);
		m_bCollectDisable = true;
		m_fCollectNum = 100;
		m_fCollectLevel =  100 / ( fSec/CD_interval );

		m_barbg->setVisible(true);
		m_barWord->setVisible(true);
	}
}

//主角人物初始化
void UIMain::ProtagonistInit()
{
	char szRoleAttTemp[50];
	m_roleAtt = static_cast<Node*>(m_MainPanel->getChildByName("Panel_HeadInfo"));
	//头像及功能
	HeroIcon = static_cast<ImageView*>(m_roleAtt->getChildByName("img_head"));
	HeroIcon->addClickEventListener([this](Ref* _sender)
	{
		if (!m_bIsCollect)
		{
			g_UIManager->OpenUI(UI_MENU);
		}
	});
	//名字及血量
	HeroName = static_cast<Text*>(m_roleAtt->getChildByName("tx_heroname"));
	HeroHP = static_cast<LoadingBar*>(m_roleAtt->getChildByName("bar_hp"));
	HeroMP = static_cast<LoadingBar*>(m_roleAtt->getChildByName("bar_mp"));	
	HeroName->setString(BaseFunction::a2u(g_LogicManager->m_Hero->m_Name.c_str()));

	//VIP等级
	vip_text = TextAtlas::create("8", "Public/main_vip.png", 14, 20, "0");
	vip_text->setPosition(Point(75,496));
	addChild(vip_text);	

	//经验条
	Exp = static_cast<LoadingBar*>(m_MainPanel->getChildByName("bar_exp"));
	if (g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_EXP] > 0 && g_CSVFileManager->GetLevelByID(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL]))
	{
		Exp->setPercent(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_EXP]*100/g_CSVFileManager->GetLevelByID(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL])->iExpValue);
		const LevelInfo* leveltemp = g_CSVFileManager->GetLevelByID(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL]);
		int32 currExp = g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_EXP];
		int32 maxExp = leveltemp->iExpValue;
		char szTemp[20];
		memset(szTemp,0,20);
		sprintf(szTemp,"%d/%d",currExp,maxExp);
		static_cast<Text*>(m_MainPanel->getChildByName("tx_exp"))->setString(szTemp);
	}
	else
	{
		Exp->setPercent(0);
	}

	//战斗力
	TextAtlas* textAtlasFight;
	sprintf(szRoleAttTemp,"%d",g_LogicManager->m_Hero->m_BattlePower);
	textAtlasFight = TextAtlas::create(szRoleAttTemp, "Public/main_fight.png", 9, 16, "0");
	textAtlasFight->setPosition(Point(150,470));
	addChild(textAtlasFight);

	//等级tx_level
	sprintf(szRoleAttTemp,"%d",(int)g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_LEVEL]);
	(static_cast<Text*>(m_roleAtt->getChildByName("tx_level")))->setText(szRoleAttTemp);


    for (int i = 0; i < MaxCountShowBuff; ++i)
    {
        m_HeroBuff[i] = static_cast<ImageView*>(m_roleAtt->getChildByName(buffUiName[i]));
        m_HeroBuff[i]->setVisible(false);
    }

    
}

//目标初始化
void UIMain::TargetInit()
{
	//目标属性
	m_targetAtt = m_MainPanel->getChildByName("TargetBack");
	m_targetAtt->setVisible(false);
	TargetIcon = static_cast<ImageView*>(m_targetAtt->getChildByName("img_target"));
    TargetIcon->setTouchEnabled(true);
    TargetIcon->addClickEventListener([this](Ref* _sender)
    {
        ImageView* img = (ImageView*)_sender;
        Vec2 position = img->convertToWorldSpace(img->getAnchorPointInPoints());
        if (g_LogicManager->m_Hero->m_SelTarget && g_LogicManager->m_Hero->m_SelTarget->m_Type == ObjectType::OBJECT_PLAYER)
        {
            PopPlayerMenu(position, g_LogicManager->m_Hero->m_SelTarget->m_GUID);
        }
    });

	TargetName = static_cast<Text*>(m_targetAtt->getChildByName("tx_targetname"));
	TargetHP = static_cast<LoadingBar*>(m_targetAtt->getChildByName("bar_hp"));
	TargetLevel = static_cast<Text*>(m_targetAtt->getChildByName("tx_targetlevel"));
	

    for (int i = 0; i < MaxCountShowBuff; ++i)
    {
        m_TargetBuff[i] = static_cast<ImageView*>(m_targetAtt->getChildByName(buffUiName[i]));
        m_TargetBuff[i]->setVisible(false);
    }
}


//主界面的任务栏
void UIMain::TaskInit()
{
    /*
	UpdateTask();

	//任务checkbox点击效果
	m_cdTask = static_cast<CheckBox*>(m_taskTeamPanel->getChildByName("cb_task"));
	m_cdTask->setSelected(true);
	if (m_cdTask)
	{
		m_Checkboxs.pushBack(m_cdTask);
		m_cdTask->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				cc2d::Vector<CheckBox*>::iterator it =  m_Checkboxs.begin();
				for (; it != m_Checkboxs.end(); it++)
				{
					(*it)->setSelected(false);
				}
				tcb->setSelected(true);

				UpdateTask();
			}
		});
	}
    */
}

void UIMain::LoadHeroBuffIcon()
{
    auto& buffList = g_LogicManager->m_Hero->m_BattleCompoent->GetBuffDatas();
    int buffCount = buffList.size();
    for (int i = 0; i < MaxCountShowBuff; ++i)
    {
        if (i < buffCount)
        {
            m_HeroBuff[i]->setVisible(true);
            Sprite* spr = static_cast<Sprite*>(m_HeroBuff[i]->getChildByName("Sprite_Icon"));
            if (spr)
            {
                spr->setTexture(buffList[i].pBuffInfo->strIconPath);
            }
        }
        else
        {
            m_HeroBuff[i]->setVisible(false);
        }
    }
}

void UIMain::LoadTargetBuffIcon()
{
    if (g_LogicManager->m_Hero->m_SelTarget && g_LogicManager->m_Hero->m_SelTarget->m_BattleCompoent)
    {
        auto& buffList = g_LogicManager->m_Hero->m_SelTarget->m_BattleCompoent->GetBuffDatas();
        int buffCount = buffList.size();
        for (int i = 0; i < MaxCountShowBuff; ++i)
        {
            if (i < buffCount)
            {
                m_TargetBuff[i]->setVisible(true);
                Sprite* spr = static_cast<Sprite*>(m_TargetBuff[i]->getChildByName("Sprite_Icon"));
                if (spr)
                {
                    spr->setTexture(buffList[i].pBuffInfo->strIconPath);
                }
            }
            else
            {
                m_TargetBuff[i]->setVisible(false);
            }
        }
    }
}

void UIMain::AddTaskItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,   int32 taskid)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	char szTemp[50];
	memset(szTemp,0,50);
	const TaskInfo* taskTemp = g_CSVFileManager->GetTaskByID(taskid);
	TaskInfoIt* taskState = g_LogicManager->m_Hero->GetTaskInfoItByID(taskid);
	if (taskTemp)
	{
		const TextInfo *textTemp;
		if (taskTemp->iType == 0)
			textTemp = g_CSVFileManager->GetTextByID(720);
		else if (taskTemp->iType == 1)
			textTemp = g_CSVFileManager->GetTextByID(721);
		else if (taskTemp->iType == 2)
			textTemp = g_CSVFileManager->GetTextByID(722);
		else if (taskTemp->iType == 3)
			textTemp = g_CSVFileManager->GetTextByID(723);
		sprintf(szTemp,"%s%s",textTemp->strContent.c_str(),taskTemp->strName.c_str());
		(static_cast<Text*>(newLayout->getChildByName("tx_taskname")))->setString(BaseFunction::a2u(szTemp));

		bool bIsComplete = false;

		if (taskState)
		{
			if (taskState->state == TASK_STATE_NOTASK)
			{
				sprintf(szTemp,"%s(可领取)",taskTemp->strTargetDesc.c_str());
			}
			else if (taskState->state == TASK_STATE_SUBMITTASK)
			{
				bIsComplete = true;
				sprintf(szTemp,"%s(可交付)",taskTemp->strTargetDesc.c_str());
			}
			else if (taskState->state == TASK_STATE_HAVETASK)
			{
				sprintf(szTemp,"%s(%d/%d)",taskTemp->strTargetDesc.c_str(),taskState->progress,taskTemp->iTermsParam[1]);
			}
		}
		else
		{
			sprintf(szTemp,"%s(可领取)",taskTemp->strTargetDesc.c_str());
		}

		if (!bIsComplete)
		{
			(static_cast<Text*>(newLayout->getChildByName("tx_content")))->setColor(Color3B(255,255,255));
		}
		else
		{
			(static_cast<Text*>(newLayout->getChildByName("tx_content")))->setColor(Color3B(0,255,0));
		}
		(static_cast<Text*>(newLayout->getChildByName("tx_content")))->setString(BaseFunction::a2u(szTemp));
	}

	//点击任务栏
	cc2dui::Layout *taskbgclick = static_cast<Layout*>(newLayout->getChildByName("panel_click"));
	if (taskbgclick)
	{
		m_mapTaskLayout.insert(pair<int32,Layout*>(taskid,taskbgclick));
		taskbgclick->addClickEventListener([this](Ref* _sender)
		{
			int32 taskid = 0;
			Layout *lo = static_cast<Layout*>(_sender);
			map<int32,Layout*>::iterator it =  m_mapTaskLayout.begin();
			for (; it != m_mapTaskLayout.end(); it++)
			{
				if (it->second == lo)
					taskid = it->first;
			}
			if (taskid != 0)
			{
				TaskClick(taskid);
			}
		});
	}
	

	sv->addChild(newLayout);
}

////经验条
//void UIMain::ExpInit()
//{
//	const LevelInfo* leveltemp = g_CSVFileManager->GetLevelByID(g_LogicManager->m_Hero->m_Level);
//
//	int32 currExp = g_LogicManager->m_Hero->m_Level;
//	int32 maxExp = leveltemp->iExpValue;
//
//	char szTemp[20];
//	memset(szTemp,0,20);
//	sprintf(szTemp,"%d/%d",currExp,maxExp);
//
//	static_cast<Text*>(m_MainPanel->getChildByName("tx_exp"))->setString(szTemp);
//	Exp->setPercent(currExp*100/maxExp);
//}

//更新聊天框
void UIMain::UpdateChat(int32 iChannel,string strName,string strContent)
{
	char szChannel[32];
    Color3B channelColor;

	if (iChannel == Channel_World){
        const TextInfo* info = g_CSVFileManager->GetTextByID(15); //>世界
        if (info)
            sprintf(szChannel, "%s",info->strContent.c_str());

        channelColor = Color3B::YELLOW;
    }
	else if (iChannel == Channel_SingleChat){
        const TextInfo* info = g_CSVFileManager->GetTextByID(17); //>私聊
        if (info)
            sprintf(szChannel, "%s",info->strContent.c_str());

        channelColor = Color3B::MAGENTA;
    }
	else if (iChannel == Channel_Near){
        const TextInfo* info = g_CSVFileManager->GetTextByID(20); //>当前
        if (info)
            sprintf(szChannel, "%s",info->strContent.c_str());

        channelColor = Color3B::WHITE;
    }
	else if (iChannel == Channel_Guild){
        const TextInfo* info = g_CSVFileManager->GetTextByID(18); //>帮派
        if (info)
            sprintf(szChannel, "%s",info->strContent.c_str());

        channelColor = Color3B::ORANGE;
    }
	else if (iChannel == Channel_Team){
        const TextInfo* info = g_CSVFileManager->GetTextByID(19); //>队伍
        if (info)
            sprintf(szChannel, "%s",info->strContent.c_str());

        channelColor = Color3B::BLUE;
    }
	else if (iChannel == Channel_System){
        const TextInfo* info = g_CSVFileManager->GetTextByID(16); //>系统
        if (info)
            sprintf(szChannel, "%s",info->strContent.c_str());

        channelColor = Color3B::RED;
    }

	MainMessage tempMessage;
	tempMessage.strChannel = szChannel;
	tempMessage.strContent = strName + strContent;
    tempMessage.channelColor = channelColor;
	if (m_vecMainMessage.size() < 3)
	{
		m_vecMainMessage.push_back(tempMessage);
	}
	else
	{
		m_vecMainMessage[0] = m_vecMainMessage[1];
		m_vecMainMessage[1] = m_vecMainMessage[2];
		m_vecMainMessage[2] = tempMessage;
	}

	if (m_vecMainMessage.size() != 0)
	{
		if (m_vecMainMessage.size() > 0)
		{
			Text* channel1 = static_cast<Text*>(m_chatPanel->getChildByName("tx_channel1"));
            if (channel1)
            {
                channel1->setVisible(true);
                channel1->setString(BaseFunction::a2u(m_vecMainMessage[0].strChannel));
                channel1->setColor(m_vecMainMessage[0].channelColor);
            }
            Text* contect1 = static_cast<Text*>(m_chatPanel->getChildByName("tx_content1"));
            if (contect1)
            {
                contect1->setVisible(true);
                contect1->setString(BaseFunction::a2u(m_vecMainMessage[0].strContent));
            }
		}

		if (m_vecMainMessage.size() > 1)
		{
			Text* channel2 = static_cast<Text*>(m_chatPanel->getChildByName("tx_channel2"));
            if (channel2)
            {
                channel2->setVisible(true);
                channel2->setString(BaseFunction::a2u(m_vecMainMessage[1].strChannel));
                channel2->setColor(m_vecMainMessage[1].channelColor);
            }

            Text* contect2 = static_cast<Text*>(m_chatPanel->getChildByName("tx_content2"));
            if (contect2)
            {
                contect2->setVisible(true);
                contect2->setString(BaseFunction::a2u(m_vecMainMessage[1].strContent));
            }
		}
		if (m_vecMainMessage.size() > 2)
		{
            Text* channel3 = static_cast<Text*>(m_chatPanel->getChildByName("tx_channel3"));
            if (channel3)
            {
                channel3->setVisible(true);
                channel3->setString(BaseFunction::a2u(m_vecMainMessage[2].strChannel));
                channel3->setColor(m_vecMainMessage[2].channelColor);
            }

            Text* contect3 = static_cast<Text*>(m_chatPanel->getChildByName("tx_content3"));
            if (contect3)
            {
                contect3->setVisible(true);
                contect3->setString(BaseFunction::a2u(m_vecMainMessage[2].strContent));
            };
		}
	}
}

void UIMain::HidePlayerMenu()
{
    if (m_PlayerMenuPnl)
    {
        m_PlayerMenuPnl->setVisible(false);
    }
}

struct MenuData
{
    UIMain::MenuEnum flag;
    int32 textId;

    MenuData(UIMain::MenuEnum flag_, int32 textId_):flag(flag_),textId(textId_)
    {}
};

void UIMain::PopPlayerMenu(const Vec2& position, uint64 SelTagGuid)
{
    Hero* pHero = g_LogicManager->m_Hero;
    if (pHero)
    {
        vector<MenuData> menus;
        if (m_PlayerMenuPnl)
        {
            m_PlayerMenuPnl->setPosition(position);

            if (pHero->HasTeam())
            {
                if (pHero->IsTeamLeader())
                {
                    if (pHero->IsTeamMember(SelTagGuid))
                    {
                        if (SelTagGuid != pHero->m_GUID)
                        {
                            MenuData data1(TeamKick, 619); //>踢人
                            menus.push_back(data1);
                        
                            MenuData data2(TeamChangeLeader, 620); //>队长转让
                            menus.push_back(data2);
                        }
                        else
                        {
                            MenuData data2(TeamDimiss, 621); //>队长解散
                            menus.push_back(data2);
                        }
                    }
                    else
                    {
                        if (pHero->m_team.m_Members.size() < 5)
                        {
                            //>组队
                            MenuData data(TeamJoin, 618);
                            menus.push_back(data);
                        }
                    }
                }
                else
                {
                    if (pHero->m_team.m_Members.size() < 5 && !pHero->IsTeamMember(SelTagGuid))
                    {
                        //>组队
                        MenuData data(TeamJoin, 618);
                        menus.push_back(data);
                    }
                }
            }
            else
            {
                if (pHero->m_team.m_Members.size() < 5 && SelTagGuid != pHero->m_GUID)
                {
                    //>组队
                    MenuData data(TeamJoin, 618);
                    menus.push_back(data);
                }
            }

            if (SelTagGuid != pHero->m_GUID)
            {
                MenuData chatdata(SingleChat, 622); //>私聊
                menus.push_back(chatdata);

                MenuData seeInfo(SeeInfo, 623); //>查看信息
                menus.push_back(seeInfo);
            }
            else
            {
                MenuData chatdata(TeamExit, 624); //>退出队伍
                menus.push_back(chatdata);
            }

            if (m_MenuImgBg && m_MenuButton)
            {

                Node* pPannel = m_PlayerMenuPnl->getChildByName("Panel_buttons");
                pPannel->removeAllChildren();

                size_t count = menus.size();
                for (size_t i = 0; i < count; ++i)
                {
                    Button* btn = (Button*)m_MenuButton->clone();
                    pPannel->addChild(btn);
                    btn->setPositionY(m_MenuButton->getPositionY() - i * 50);
                    btn->setTag(menus[i].flag);
                    btn->addClickEventListener(m_MenuCallback);
                    const TextInfo* pText = g_CSVFileManager->GetTextByID(menus[i].textId);
                    if (pText)
                        btn->setTitleText(BaseFunction::a2u(pText->strContent));
                }

                if (m_MenuImgBg)
                {
                    m_MenuImgBg->setContentSize(Size(m_MenuImgBg->getContentSize().width, 66 + (count - 1) * 50));
                }
                m_PlayerMenuPnl->setVisible(count > 0);

                m_PopMenuTagGuid = SelTagGuid;
            }
        }
    }
}

void UIMain::OnTeamApply()
{
    Hero* pHero = g_LogicManager->m_Hero;
    if (pHero->m_SelTarget)
    {
        if (pHero->IsTeamMember(pHero->m_SelTarget->m_GUID))
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(647);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //对方已是队伍成员!
            return;
        }

        if (pHero->HasTeam() && !pHero->IsTeamLeader())
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(648);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //只有队长才能邀请组队!
            return;
        }


        MsgTeam_Apply applyMsg(pHero->m_GUID, pHero->m_Name.c_str(),(int)pHero->m_Attribute[ATTRIBUTE_LEVEL], pHero->m_SelTarget->m_GUID);
        g_NetManager->SendMsg(applyMsg);
    }
}

void UIMain::OnTeamKick()
{
    MsgTeam_KickMemberReq kickMsg; 
    kickMsg.badGuyId = m_PopMenuTagGuid;
    g_NetManager->SendMsg(kickMsg);
}

void UIMain::OnTeamChangeLeader()
{
    MsgTeam_ChangeLeader changeMsg;
    changeMsg.SysID = ID_C2G;
    changeMsg.memberId = m_PopMenuTagGuid;
    g_NetManager->SendMsg(changeMsg);
}

void UIMain::OnTeamDimiss()
{
    MsgTeam_KickMemberReq kickMsg; 
    kickMsg.badGuyId = g_LogicManager->m_Hero->m_GUID;
    g_NetManager->SendMsg(kickMsg);
}

void UIMain::OnTeamExit()
{
    MsgTeam_MemberExit exitMsg;
    exitMsg.SysID = ID_C2G;
    exitMsg.memberId = g_LogicManager->m_Hero->m_GUID;
    g_NetManager->SendMsg(exitMsg);
}

void UIMain::OnSingleChat()
{

}

void UIMain::OnSeeInfo()
{
    MsgOtherRole_DetailInfoQuery queryMsg;
    queryMsg.queryGuid = m_PopMenuTagGuid;
    if (m_PopMenuTagGuid != g_LogicManager->m_Hero->m_GUID)
    {
        queryMsg.OBJID = g_LogicManager->m_Hero->m_GUID;
        g_NetManager->SendMsg(queryMsg);
    }
}

void UIMain::InitSkillPanel()
{
    Layout* pSkillPnl = static_cast<Layout*>(m_MainPanel->getChildByName("Panel_Skill"));	
    if (pSkillPnl)
    {
        //普通攻击
        m_comSkillbtn = static_cast<Button*>(pSkillPnl->getChildByName("img_attack"));
        m_comSkillbtn->addClickEventListener([this](Ref* _sender)
        {
            //g_LogicManager->m_Hero->m_AttackIndex = 0;
            int skillID = SKILL_TYPE_AND_LEVEL_TO_ID(g_LogicManager->m_Hero->m_SkillList[0].skillType, g_LogicManager->m_Hero->m_SkillList[0].level);
            //g_LogicManager->m_Hero->UseSkill(skillID);
            if (g_LogicManager->m_Hero->m_SelTarget != NULL)
            {
                MsgC2GPlayer_UseSkill send;
                send.ObjType = OBJECT_PLAYER;
                send.SkillID = skillID;
                send.target = g_LogicManager->m_Hero->m_SelTarget->m_GUID;
                g_NetManager->SendMsg(send);
            }
        });
        m_comSkillIcon = static_cast<ImageView*>(pSkillPnl->getChildByName("img_attack_bg"));

        //xp技能
        m_xpSkillbtn = static_cast<Button*>(pSkillPnl->getChildByName("XPSkill"));	
        m_xpSkillbtn->addClickEventListener([this](Ref* _sender)
        {
            if (!m_bSkillxpDisable)
            {
                //g_LogicManager->m_Hero->m_AttackIndex = 0;
                ////g_LogicManager->m_Hero->UseSkill(g_LogicManager->m_Hero->m_SkillList[MAX_CHOSESKILL - 1]);

                //SetProgress(m_progressSkillxp,4);
            }
        });
        m_xpSkillIcon = static_cast<ImageView*>(pSkillPnl->getChildByName("xpskill_bg"));

        m_progressSkillxp = ProgressTimer::create(m_sprSkillcd);
        m_progressSkillxp->setType(ProgressTimer::Type::RADIAL);
        addChild(m_progressSkillxp);
        Vec2 skillxpPoint = static_cast<Button*>(pSkillPnl->getChildByName("XPSkill"))->getPosition();
        m_progressSkillxp->setPosition(skillxpPoint);
        m_progressSkillxp->setMidpoint(Vec2(0.5f, 0.5f));
        m_progressSkillxp->setVisible(false);
        m_bSkillxpDisable = false;


        Button* pBtn = static_cast<Button*>(pSkillPnl->getChildByName("img_ride"));	
        pBtn->addClickEventListener([this](Ref* _sender)
        {			
            //！上下马
            g_LogicManager->m_Hero->Ride(!g_LogicManager->m_Hero->GetRide());
        });

        m_GetstureDetector.OnDetectReslut = ([this](GestureDetector::MoveType moveType)
        {
            switch (moveType)
            {
            case GestureDetector::None:
                break;
            case GestureDetector::BottomToTop:
            case GestureDetector::LeftToRight:
            case GestureDetector::LeftTopToRightDown:
            case GestureDetector::LeftDownToRightTop:
                m_Turntable.DoAnticlockwise(m_nSkillCount - 3, 0.2f);
                break;
            case GestureDetector::TopToBottom:
            case GestureDetector::RightToLeft:
            case GestureDetector::RightDownToLeftTop:
            case GestureDetector::RightTopToLeftDown:
                m_Turntable.DoClockwise(m_nSkillCount - 3, 0.2f);
                break;

            default:
                break;
            }
        });

        EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(false);
        listener->onTouchBegan = [this](Touch* touch, Event* event)
        {
            //>小于3个技能,不滑动
            if (m_nSkillCount <= 3)
                return false;

            Node* node = m_TouchPanel;
            Vec2 nsp = node->convertTouchToNodeSpace(touch);
            Size size = node->getContentSize();
            Rect rect(0,0,size.width, size.height);
            if (rect.containsPoint(nsp))
            {
                m_GetstureDetector.BeginTouch(nsp);
                return true;
            }
            return false;
        };

        listener->onTouchEnded = [this](Touch* touch, Event* event)
        {
            Node* node = m_TouchPanel;
            Vec2 nsp = node->convertTouchToNodeSpace(touch);
            Size size = node->getContentSize();
            Rect rect(0,0,size.width, size.height);
            if (rect.containsPoint(nsp))
            {
                m_GetstureDetector.EndTouch(nsp);
            }
        };
        m_TouchPanel = static_cast<Layout*>(pSkillPnl->getChildByName("Panel_touch"));	
        if (m_TouchPanel)
        {
            m_TouchPanel->setTouchEnabled(false);
            m_TouchPanel->setVisible(false);
            m_GetstureDetector.SetSize(Vec2(m_TouchPanel->getContentSize()));
            Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, -1);
        }

        m_SkillIcons[0] = static_cast<ImageView*>(pSkillPnl->getChildByName("img_skill1_bg"));
        m_SkillFrames[0] = static_cast<Button*>(pSkillPnl->getChildByName("img_skill1"));

        m_SkillIcons[1] = static_cast<ImageView*>(pSkillPnl->getChildByName("img_skill2_bg"));
        m_SkillFrames[1] = static_cast<Button*>(pSkillPnl->getChildByName("img_skill2"));
        
        m_SkillIcons[2] = static_cast<ImageView*>(pSkillPnl->getChildByName("img_skill3_bg"));
        m_SkillFrames[2] = static_cast<Button*>(pSkillPnl->getChildByName("img_skill3"));

        m_SkillIcons[3] = static_cast<ImageView*>(m_SkillIcons[0]->clone());
        m_SkillFrames[3] = static_cast<Button*>(m_SkillFrames[0]->clone());

        /*
        m_progressSkill2 = ProgressTimer::create(m_sprSkillcd);
        m_progressSkill2->setType(ProgressTimer::Type::RADIAL);
        addChild(m_progressSkill2);
        Vec2 skill2Point = static_cast<Button*>(m_MainPanel->getChildByName("img_skill2"))->getPosition();
        m_progressSkill2->setPosition(skill2Point);
        m_progressSkill2->setMidpoint(Vec2(0.5f, 0.5f));
        m_progressSkill2->setVisible(false);
        m_bSkill2Disable = false;

        m_progressSkill3 = ProgressTimer::create(m_sprSkillcd);
        m_progressSkill3->setType(ProgressTimer::Type::RADIAL);
        addChild(m_progressSkill3);
        Vec2 skill3Point = static_cast<Button*>(m_MainPanel->getChildByName("img_skill3"))->getPosition();
        m_progressSkill3->setPosition(skill3Point);
        m_progressSkill3->setMidpoint(Vec2(0.5f, 0.5f));
        m_progressSkill3->setVisible(false);
        m_bSkill3Disable = false;
        */


        Widget::ccWidgetClickCallback clickcallback = ([this](Ref* _sender)
        {
            Button* pBtn = (Button*)(_sender);
            //g_LogicManager->m_Hero->m_AttackIndex = pBtn->getTag();
            //g_LogicManager->m_Hero->UseSkill(g_LogicManager->m_Hero->m_SkillList[pBtn->getTag()]);
        });

        for (int i = 0; i < 4; ++i)
        {
            m_SkillIcons[i]->setVisible(false);
            m_SkillFrames[i]->setVisible(false);
            m_SkillFrames[i]->setTag(i);
            m_SkillFrames[i]->addClickEventListener(clickcallback);
        }

        pSkillPnl->addChild(m_SkillIcons[3]);
        pSkillPnl->addChild(m_SkillFrames[3]);

        SetupTurntable();

        ReloadSkill();
    }
}

//>获得英雄第n个游戏技能索引
inline int GetHeroSkillIndex(Hero* pHero, int n)
{
    int count = 0;
    for (int i = 1; i < MAX_CHOSESKILL - 1; ++i)
    {
        if (pHero->m_ChoseSkillList[i] != 0)
        {
            if (count == n)
                return i;

            ++count;
        }
    }

    return -1;
}

void UIMain::SetupTurntable()
{
    m_Turntable.OnInitSetPosition = ([this](int widgetIndex, Vec2& newPosition)
    {
        Hero* pHero = g_LogicManager->m_Hero;
        m_SkillIcons[widgetIndex]->setTag(widgetIndex);
        m_SkillIcons[widgetIndex]->setPosition(newPosition);
        m_SkillFrames[widgetIndex]->setPosition(newPosition);
        int skillIndex = GetHeroSkillIndex(pHero, widgetIndex);
        m_SkillFrames[widgetIndex]->setTag(skillIndex);
        //const SkillInfo* pSkill = g_CSVFileManager->GetSkillByID(g_LogicManager->m_Hero->m_SkillList[skillIndex]);
        //if (pSkill)
        //    m_SkillIcons[widgetIndex]->loadTexture(pSkill->strSkillIcon);

    });

    m_Turntable.OnUpdatePosition = ([this](int widgetIndex, cocos2d::Vec2& newPosition)
    {
        for (int i = 0; i < 4; ++i)
        {
            if(m_SkillIcons[i]->getTag() == widgetIndex)
            {
                m_SkillIcons[i]->setPosition(newPosition);
                m_SkillFrames[i]->setPosition(newPosition);
            }
        }
    });


    m_Turntable.OnWillShow = ([this](int widgetIndex)
    {
        Hero* pHero = g_LogicManager->m_Hero;
        for (int i = 0; i < 4; ++i)
        {
            if(!m_SkillIcons[i]->isVisible())
            {
                int skillIndex = GetHeroSkillIndex(pHero, widgetIndex);
                assert(skillIndex > 0);
                m_SkillIcons[i]->setTag(widgetIndex);
                m_SkillIcons[i]->setVisible(true);
                m_SkillFrames[i]->setVisible(true);

                m_SkillFrames[i]->setTag(skillIndex);
                //const SkillInfo* pSkill = g_CSVFileManager->GetSkillByID(g_LogicManager->m_Hero->m_SkillList[skillIndex]);
                //if (pSkill)
                //    m_SkillIcons[i]->loadTexture(pSkill->strSkillIcon);
                break;
            }
        }
    });

    m_Turntable.OnWillHide = ([this](int widgetIndex)
    {
        for (int i = 0; i < 4; ++i)
        {
            if(m_SkillIcons[i]->getTag() == widgetIndex){
                m_SkillIcons[i]->setVisible(false);
                m_SkillFrames[i]->setVisible(false);
                break;
            }
        }
    });
}

void UIMain::ReloadSkill()
{
    /*
    Hero* pHero = g_LogicManager->m_Hero;
    //>普通攻击
    int comSkillId = pHero->m_SkillList[0];
    if (comSkillId == 0)
    {
        m_comSkillbtn->setVisible(false);
        m_comSkillIcon->setVisible(false);
    }
    else
    {
        const SkillInfo* pSkill = g_CSVFileManager->GetSkillByID(comSkillId);
        if (pSkill)
        {
            m_comSkillbtn->setVisible(true);
            m_comSkillIcon->setVisible(true);
            //m_comSkillIcon->loadTexture(pSkill->strSkillIcon);
        }
    }
    //>xp技能
    int xpSkillId = pHero->m_SkillList[MAX_CHOSESKILL - 1];
    if (xpSkillId == 0)
    {
        m_xpSkillbtn->setVisible(false);
        m_xpSkillIcon->setVisible(false);
    }
    else
    {
        const SkillInfo* pSkill = g_CSVFileManager->GetSkillByID(xpSkillId);
        if (pSkill)
        {
            m_xpSkillbtn->setVisible(true);
            m_xpSkillIcon->setVisible(true);
            //m_xpSkillIcon->loadTexture(pSkill->strSkillIcon);
        }
    }

    m_nSkillCount = 0;
    for (int i = 1; i < MAX_CHOSESKILL - 1; ++i)
    {
        if(pHero->m_SkillList[i] != 0)
        {
            ++m_nSkillCount;
        }
    }

    if (m_nSkillCount <= 3)
    {
        for (int i = 1, j = 0; i < MAX_CHOSESKILL - 1; ++i, ++j)
        {
            if (pHero->m_SkillList[i] != 0)
            {
                const SkillInfo* pSkill = g_CSVFileManager->GetSkillByID(pHero->m_SkillList[i]);
                if (pSkill)
                {
                    m_SkillIcons[j]->setVisible(true);
                    //m_SkillIcons[j]->loadTexture(pSkill->strSkillIcon);
                    m_SkillFrames[j]->setTag(i);
                    m_SkillFrames[j]->setVisible(true);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; ++i){
            m_SkillIcons[i]->setTag(i);
            m_SkillIcons[i]->setVisible(false);
        }
        m_Turntable.Setup(m_nSkillCount, 3, 128.0f, Turntable::RightDownCorner);
        m_TouchPanel->setVisible(true);
    }
    */
}

void UIMain::Update(float deltaTime)
{
    m_Turntable.Update(deltaTime);

    if (m_pMiniMap)
    {
        m_pMiniMap->Refresh(deltaTime);
    }

    UpdateCD(deltaTime);
}

void UIMain::TaskClick(int32 taskid)
{
//	const TaskInfo* taskTemp = g_CSVFileManager->GetTaskByID(taskid);
//	int32 iTaskState;
//	Vector<TaskInfoIt>::iterator it;
//	//遍历所有任务
//	bool bIsNull = true;
//	TaskInfoIt *taskinfoitTemp = g_LogicManager->m_Hero->GetTaskInfoItByID(taskid);
//	if (taskinfoitTemp)
//	{
//		iTaskState = taskinfoitTemp->state;
//	}
//	else
//	{
//		iTaskState = TASK_STATE_NOTASK;
//	}
//
//	Vec2 startPos = g_SceneManager->m_MainLayer->Pos2Tile(g_LogicManager->m_Hero->getPosition());
//	switch (iTaskState)
//	{
//	case TASK_STATE_NOTASK:
//		{
//			//可接任务
//			g_LogicManager->m_Hero->m_iFindPathClickNpc = taskTemp->iTaskNpcID;
//			const NPCInfo *npcTemp = g_CSVFileManager->GetNPCInfoByID(taskTemp->iTaskNpcID);
//
//			//先判断是否需要寻路
//			if (g_LogicManager->m_CurrMapId == npcTemp->iMapid && startPos.x == npcTemp->pPos.x && startPos.y == npcTemp->pPos.y)
//			{
//				if (g_LogicManager->m_Hero->m_iFindPathClickNpc != 0)
//				{
//					NPC *tempNpc = g_LogicManager->GetNPCByID(g_LogicManager->m_Hero->m_iFindPathClickNpc);
//					if (tempNpc)
//					{
//						g_LogicManager->m_Hero->m_SelTarget = tempNpc;
//						((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
//					}
//					g_LogicManager->m_Hero->m_iFindPathClickNpc = 0;
//				}
//			}
//
//			g_LogicManager->m_Hero->RealFindPath(g_SceneManager->m_MainLayer->Tile2Pos(Vec2(npcTemp->pPos.x,npcTemp->pPos.y)));
//		}
//		break;
//	case TASK_STATE_HAVETASK:
//		{
//			//开始任务
//			g_LogicManager->m_Hero->m_iFindPathClickNpc = 0;
//			g_LogicManager->m_Hero->RealFindPath(g_SceneManager->m_MainLayer->Tile2Pos(Vec2(taskTemp->pTargetPoint.x,taskTemp->pTargetPoint.y)));
//		}
//		break;
//	case TASK_STATE_SUBMITTASK:
//		{
//			//提交任务
//			g_LogicManager->m_Hero->m_iFindPathClickNpc = taskTemp->iReferNpcID;
//			const NPCInfo *npcTemp = g_CSVFileManager->GetNPCInfoByID(taskTemp->iReferNpcID);
//
//			//先判断是否需要寻路
//			if (g_LogicManager->m_CurrMapId == npcTemp->iMapid && startPos.x == npcTemp->pPos.x && startPos.y == npcTemp->pPos.y)
//			{
//				if (g_LogicManager->m_Hero->m_iFindPathClickNpc != 0)
//				{
//					NPC *tempNpc = g_LogicManager->GetNPCByID(g_LogicManager->m_Hero->m_iFindPathClickNpc);
//					if (tempNpc)
//					{
//						g_LogicManager->m_Hero->m_SelTarget = tempNpc;
//						((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
//					}
//					g_LogicManager->m_Hero->m_iFindPathClickNpc = 0;
//				}
//			}
//
//			g_LogicManager->m_Hero->RealFindPath(g_SceneManager->m_MainLayer->Tile2Pos(Vec2(npcTemp->pPos.x,npcTemp->pPos.y)));
//		}
//		break;
//	case TASK_STATE_FINISHTASK:
//		break;
//	case TASK_STATE_MAX:
//		break;
//	default:
//		break;
//	}
}

//更新任务显示
void UIMain::UpdateTask()
{
	//初始化TASK
    m_svPanel->setVisible(true);
	m_svPanel->removeAllChildren();
	m_mapTaskLayout.clear();
	Vector<TaskInfoIt>::iterator it;
	//可交接的任务
	for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
	{
		if (it->state == TASK_STATE_SUBMITTASK)
		{
			const TaskInfo *tempTask = g_CSVFileManager->GetTaskByID(it->id);
			if (tempTask)
			{
				AddTaskItem(m_svPanel,m_TaskTemplate->clone(),it->id);
			}
		}
	}
	//领取未完成的任务
	for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
	{
		if ( it->state == TASK_STATE_HAVETASK)
		{
			const TaskInfo *tempTask = g_CSVFileManager->GetTaskByID(it->id);
			if (tempTask)
			{
				AddTaskItem(m_svPanel,m_TaskTemplate->clone(),it->id);
			}
		}
	}
	//可接的任务
	map<int32,TaskInfo>::iterator itt;
	for(itt = g_CSVFileManager->m_TaskInfoMap.begin(); itt != g_CSVFileManager->m_TaskInfoMap.end() ; itt++)
	{
		TaskInfoIt *taskinfoitTemp = g_LogicManager->m_Hero->GetTaskInfoItByID(itt->first);
		//检测等级
		if (g_LogicManager->m_Hero->m_Level >= itt->second.iLevel)
		{
			//检测境界
			if (g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_REALMLEVEL] >= itt->second.iRealmID)
			{
				//检测前置任务是否有前置任务
				if (itt->second.iReqTaskID == 0)
				{
					if (!taskinfoitTemp)
					{
						AddTaskItem(m_svPanel,m_TaskTemplate->clone(),itt->first);
					}
					else
					{
						if (taskinfoitTemp->state == TASK_STATE_NOTASK)
						{
							AddTaskItem(m_svPanel,m_TaskTemplate->clone(),itt->first);
						}
					}
				}
				else
				{
					//如果有前置任务，看是否已经完成
					TaskInfoIt *protaskinfoIt = g_LogicManager->m_Hero->GetTaskInfoItByID(itt->second.iReqTaskID);
					if (protaskinfoIt)
					{
						if (protaskinfoIt->state == TASK_STATE_FINISHTASK)
						{
							if (!taskinfoitTemp)
							{
								AddTaskItem(m_svPanel,m_TaskTemplate->clone(),itt->first);
							}
							else
							{
								if (taskinfoitTemp->state == TASK_STATE_NOTASK)
								{
									AddTaskItem(m_svPanel,m_TaskTemplate->clone(),itt->first);
								}
							}
						}
					}
					
				}
			}
		}
	}

	m_svPanel->setLayoutType(Layout::Type::VERTICAL);
	m_svPanel->forceDoLayout();
	Size ContainerSize = m_svPanel->getInnerContainerSize();
	ContainerSize.height = m_svPanel->getChildrenCount() * m_TaskTemplate->getContentSize().height;
	m_svPanel->setInnerContainerSize(ContainerSize);
}

void UIMain::UpdateCD(float dt)
{
    m_CDInterval += dt;
    if (m_CDInterval > CD_interval)
    {
        m_CDInterval -= CD_interval;


        RedCD(dt);
        BlueCD(dt);
        //Skill1CD(dt);
        //Skill2CD(dt);
        //Skill3CD(dt);
        SkillxpCD(dt);
    }
}

//点击NPC
void UIMain::ClickNpc()
{
	////界面显示
    m_targetAtt->setVisible(true);
    TargetLevel->setVisible(false);
    TargetIcon->loadTexture(g_LogicManager->m_Hero->m_SelTarget->GetHeadIcon());
    TargetName->setString(BaseFunction::a2u(g_LogicManager->m_Hero->m_SelTarget->m_Name));
    TargetHP->setPercent(100);

	//Vector<TaskInfoIt>::iterator it;
	//map<int32,TaskInfo>::iterator itt;
	//int32 iTaskid = 0;
	////先查看是否有任务要交接
	//bool bIsTaskHand = false;
	//for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
	//{
	//	if (it->state == TASK_STATE_SUBMITTASK)
	//	{
	//		const TaskInfo *tempTask = g_CSVFileManager->GetTaskByID(it->id);
	//		if (tempTask)
	//		{
	//			if (tempTask->iReferNpcID == g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id)
	//			{
	//				bIsTaskHand = true;
	//				iTaskid = it->id;
	//			}
	//		}
	//	}
	//}

	//if (bIsTaskHand)
	//{
	//	const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id);
	//	Point pPos;
	//	Vec2 startPos = g_SceneManager->m_MainLayer->Pos2Tile(g_LogicManager->m_Hero->getPosition());
	//	pPos.x = startPos.x;
	//	pPos.y = startPos.y;

	//	int32 iError = 0;
	//	int32 iXerror = pPos.x - npcinfo->pPos.x;
	//	int32 iYerror = pPos.y - npcinfo->pPos.y;
	//	iError = abs(iXerror) + abs(iYerror);

	//	if (iError < 6)
	//	{
	//		g_UIManager->OpenUI(UI_TALK);
	//		UIBase *ui = g_UIManager->GetUI(UI_TALK);
	//		//前置对话，后置对话，NPC
	//		((UITalk*)ui)->InitProgress(iTaskid,g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id,false);
	//	}
	//}
	//else
	//{
	//	bool bIsGet = false;
	//	for(itt = g_CSVFileManager->m_TaskInfoMap.begin(); itt != g_CSVFileManager->m_TaskInfoMap.end() ; itt++)
	//	{
	//		//遍历所有任务
	//		bool bIsNull = true;
	//		for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
	//		{
	//			//查找是否列表中是否有可接任务
	//			if (it->id == itt->first)
	//			{
	//				bIsNull = false;
	//				if (it->state == TASK_STATE_NOTASK)
	//					bIsNull = true;
	//				break;
	//			}
	//		}
	//		if (bIsNull)
	//		{
	//			//检测等级
	//			if ( g_LogicManager->m_Hero->m_Level >= itt->second.iLevel)
	//			{
	//				//检测境界
	//				if (g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_REALMLEVEL] >= itt->second.iRealmID)
	//				{
	//					//检测前置任务是否有前置任务
	//					if (itt->second.iReqTaskID == 0)
	//					{
	//						const TaskInfo* taskTemp = g_CSVFileManager->GetTaskByID(itt->first);
	//						if (taskTemp->iTaskNpcID == g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id)
	//						{
	//							bIsGet = true;
	//							iTaskid = itt->first;
	//							break;
	//						}
	//					}
	//					else
	//					{
	//						for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
	//						{
	//							//查找前置任务是否完成
	//							if (it->id == itt->second.iReqTaskID)
	//							{
	//								if (it->state == TASK_STATE_FINISHTASK)
	//								{
	//									const TaskInfo* taskTemp = g_CSVFileManager->GetTaskByID(itt->first);
	//									if (taskTemp->iTaskNpcID == g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id)
	//									{
	//										bIsGet = true;
	//										iTaskid = itt->first;
	//									}
	//								}
	//								break;
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//	if (bIsGet)
	//	{
	//		const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id);
	//		Point pPos;
	//		Vec2 startPos = g_SceneManager->m_MainLayer->Pos2Tile(g_LogicManager->m_Hero->getPosition());
	//		pPos.x = startPos.x;
	//		pPos.y = startPos.y;

	//		int32 iError = 0;
	//		int32 iXerror = pPos.x - npcinfo->pPos.x;
	//		int32 iYerror = pPos.y - npcinfo->pPos.y;
	//		iError = abs(iXerror) + abs(iYerror);

	//		if (iError < 6)
	//		{
	//			g_UIManager->OpenUI(UI_TALK);
	//			UIBase *ui = g_UIManager->GetUI(UI_TALK);
	//			//前置对话，后置对话，NPC
	//			((UITalk*)ui)->InitProgress(iTaskid,g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id,true);
	//		}
	//	}
	//	else
	//	{
	//		const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id);
	//		Point pPos;
	//		Vec2 startPos = g_SceneManager->m_MainLayer->Pos2Tile(g_LogicManager->m_Hero->getPosition());
	//		pPos.x = startPos.x;
	//		pPos.y = startPos.y;

	//		int32 iError = 0;
	//		int32 iXerror = pPos.x - npcinfo->pPos.x;
	//		int32 iYerror = pPos.y - npcinfo->pPos.y;
	//		iError = abs(iXerror) + abs(iYerror);

	//		if (iError < 6)
	//		{
	//			g_UIManager->OpenUI(UI_TALK);
	//			UIBase *ui = g_UIManager->GetUI(UI_TALK);
	//			//前置对话，后置对话，NPC
	//			((UITalk*)ui)->InitProgress(0,g_LogicManager->m_Hero->m_SelTarget->m_NPCInfo->id,true);
	//		}
	//	}
	//}
}