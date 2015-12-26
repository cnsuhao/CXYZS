#include "UIGongFa.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "Hero.h"
#include "CSVFileManager.h"
#include "SceneManager.h"
#include "NetManager.h"
#include "UIMain.h"

using namespace cocostudio;

static const char* skillItemNames[UIGongFa::ActivteSkillNum] = { "panel_skill1", "panel_skill2", "panel_skill3", "panel_skill4" };

static const char* skillUserNames[UIGongFa::CanUseSkillNum] = { "img_footskill1", "img_footskill2", "img_footskill3", "img_footskill4", "img_footskill5", "img_footskill6", "img_footskill7" };

UIGongFa::UIGongFa()
{
    m_gongfaPanel = NULL;

    for (int i = 0; i < ActivteSkillNum; ++i)
    {
        m_skillChecks[i] = NULL;
        m_HasSkill[i] = false;
    }

    for (int i = 0; i < CanUseSkillNum; ++i)
    {
        m_CanUseSkillSolt[i] = NULL;
    }

    m_Hero = NULL;
    
    m_startBtn = NULL;
    m_startImg = NULL;
}

UIGongFa::~UIGongFa()
{

}

bool UIGongFa::init()
{
    bool bRet = false;
    do 
    {
        m_Hero = g_LogicManager->m_Hero;
		//加载主面板
		m_gongfaPanel = CSLoader::createNode("UI/MagicArts.csb");
		if (m_gongfaPanel == NULL)	return false;
		addChild(m_gongfaPanel);

		Button *btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(m_gongfaPanel->getChildByName("bt_close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_GONGFA);
			});
		}

        const TextInfo *pText = g_CSVFileManager->GetTextByID(667);
		(static_cast<Button*>(m_gongfaPanel->getChildByName("bt_actmagic")))->setTitleText(BaseFunction::a2u(pText->strContent)); //激活功能


        m_startImg = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_strating"));
        if (m_startImg)
        {
            m_startImg->setVisible(false);
        }

        m_startBtn = static_cast<cc2dui::Button*>(m_gongfaPanel->getChildByName("bt_actmagic"));;
        if (m_startBtn)
        {
            m_startBtn->addClickEventListener([this](Ref* _sender)
            {
                OnActivateSkillType(m_CurTreeType);
            });
        }

        EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this](Touch* touch, Event* event)
        {
            ImageView* img = static_cast<ImageView*>(event->getCurrentTarget());
            if (img->hitTest(touch->getLocation()))
            {
                if(m_HasSkill[img->getTag()] == false) 
                    return false;

                int32 activateSkillId = m_skillChecks[img->getTag()]->getTag();
                const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(activateSkillId);
                if (pSkillTpl && pSkillTpl->kind != SKILL_CLASS_PASSIVESKILL)
                {
                    //Sprite* spr = Sprite::create(pSkillTpl->strSkillIcon);
                    //if (spr)
                    //{
                    //    spr->setPosition(touch->getStartLocation());
                    //    spr->setTag(activateSkillId);
                    //    g_UIManager->addChild(spr, LAYER_SCENE_EFFECT);
                    //    img->setUserData(spr);
                    //}
                    return true;
                }
            }
            return false;
        };

        listener->onTouchMoved = [this](Touch* touch, Event* event)
        {
            ImageView* img = static_cast<ImageView*>(event->getCurrentTarget());
            Sprite* spr = (Sprite*)img->getUserData();
            if (spr)
                spr->setPosition(touch->getLocation());
        };

        listener->onTouchEnded = [this](Touch* touch, Event* event)
        {
            ImageView* img = static_cast<ImageView*>(event->getCurrentTarget());
            Sprite* spr = (Sprite*)img->getUserData();
            if(spr)
            {
                if (m_CanUseSkillSolt[0]->hitTest(spr->getPosition()))
                {
                    OnSetCommonSkill(spr->getTag());
                }else if(m_CanUseSkillSolt[1]->hitTest(spr->getPosition()))
                {
                    OnSetSkill1(spr->getTag());
                }else if(m_CanUseSkillSolt[2]->hitTest(spr->getPosition()))
                {
                    OnSetSkill2(spr->getTag());
                }else if(m_CanUseSkillSolt[3]->hitTest(spr->getPosition()))
                {
                    OnSetSkill3(spr->getTag());
                }else if(m_CanUseSkillSolt[4]->hitTest(spr->getPosition()))
                {
                    OnSetSkill4(spr->getTag());
                }else if(m_CanUseSkillSolt[5]->hitTest(spr->getPosition()))
                {
                    OnSetSkill5(spr->getTag());
                }else if(m_CanUseSkillSolt[6]->hitTest(spr->getPosition()))
                {
                    OnSetSkill6(spr->getTag());
                }
                spr->removeFromParent();
                img->setUserData(NULL);
            }
            
        };

        for (int i = 0; i < ActivteSkillNum; ++i)
        {
            Node* panel = static_cast<cc2dui::ScrollView*>(m_gongfaPanel->getChildByName(skillItemNames[i]));
            if (panel)
            {
                CheckBox* skillCheckBox = static_cast<cc2dui::CheckBox*>(panel->getChildByName("cb_bg"));
                if (skillCheckBox)
                {
                    m_skillChecks[i] = skillCheckBox;
                    skillCheckBox->addEventListener([this](Ref* _sender,CheckBox::EventType type)
                    {
                        for (int j = 0; j < ActivteSkillNum; ++j)
                        {
                            m_skillChecks[j]->setSelected(false);
                        }

                        CheckBox* sender = static_cast<CheckBox*>(_sender);
                        if (sender)
                        {
                            int32 activateSkillId = sender->getTag();

                            sender->setSelected(true);

                            SetSkillPanelInfo(activateSkillId);
                        }
                    });
                }

                ImageView* skillBox = static_cast<cc2dui::ImageView*>(panel->getChildByName("img_skillbox"));
                if (skillBox)
                {
                    skillBox->setTag(i);
                    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener->clone(), skillBox);
                }
            }
        }

        for (int i = 0; i < CanUseSkillNum; ++i)
        {
            m_CanUseSkillSolt[i] = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName(skillUserNames[i]));
        }

        //模板
        m_Template = static_cast<Layout*>(m_gongfaPanel->getChildByName("Panel_model"));
        m_Template->setVisible(false);

        //最上面的滚动条
        m_magicSV = static_cast<cc2dui::ScrollView*>(m_gongfaPanel->getChildByName("sv_magicarts"));
        m_magicSV ->setSwallowTouches(true);

		LoadCheckBox();				//上面功法那一排的checkbox

        m_CurTreeType = 0;

        bRet = true;
    } while (0);

    return bRet;
}

void UIGongFa::OnShow()
{
    UIBase::OnShow();
    SetCurSkillTreeType(m_Hero->m_ActivateSkillType);
    ReloadChoseSkillIcon();
}

void UIGongFa::SetCurSkillTreeType(uint32 treeType)
{
    /*
    SkillTreeInfoIt* pSkillTree = NULL;

    if (treeType == 0)
    {
        if (m_Hero->m_SkillTreeList.empty())
            return;

        treeType = SKILL_ID_TO_TYPE(m_Hero->m_SkillTreeList[0].id);
        pSkillTree = &(m_Hero->m_SkillTreeList[0]);
    }
    else
    {
        bool findIt = false;
        vector<SkillTreeInfoIt>::iterator it = m_Hero->m_SkillTreeList.begin();
        for (vector<SkillTreeInfoIt>::iterator end = m_Hero->m_SkillTreeList.end();
             it != end; ++it)
        {
            if (SKILL_ID_TO_TYPE((*it).id) == treeType)
            {
                pSkillTree = &(*it);
                findIt = true;
                break;
            }
        }

        if (findIt == false)
            return;
    }
    map<int,CheckBox*>::iterator it =  m_mainChecks.begin();
    for (; it != m_mainChecks.end(); it++)
    {
        if (it->first == treeType)
        {
            it->second->setSelected(true);
        }
        else
        {
            it->second->setSelected(false);
        }
    }

    //switch...
    SetupSkillsByTreeId(pSkillTree->id);
    SetSkillTreeInfo(treeType);
    */
}

void UIGongFa::RelaodSkillTree()
{
    if (m_gongfaPanel)
    {
        LoadCheckBox();
    }
}

void UIGongFa::OnSetCommonSkill(uint32 skillId)
{
    /*
    const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillTpl)
    {
        if (pSkillTpl->kind != SKILL_CLASS_NORMALATTACK)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(652);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //只能放入普通攻击!
            return;
        }

        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_skill1"));
        if (img)
        {
            //img->loadTexture(pSkillTpl->strSkillIcon);
        }

        if (m_Hero->m_SkillList[0] != skillId)
        {
            m_Hero->SetChoseSkill(0, skillId);

            ReloadChoseSkillIcon();
            UIMain* pMainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
            if (pMainUI)
            {
                pMainUI->ReloadSkill();
            }
        }
    }
    */
}

void UIGongFa::OnSetSkill1(uint32 skillId)
{
    /*
    const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillTpl)
    {
        if (pSkillTpl->kind == SKILL_CLASS_NORMALATTACK || pSkillTpl->kind == SKILL_CLASS_PASSIVESKILL)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(653);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //不能放入普通攻击或被动技能!
            return;
        }

        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_skill2"));
        if (img)
        {
            //img->loadTexture(pSkillTpl->strSkillIcon);
        }

        if (m_Hero->m_SkillList[1] != skillId)
        {
            m_Hero->SetChoseSkill(1, skillId);

            ReloadChoseSkillIcon();
            UIMain* pMainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
            if (pMainUI)
            {
                pMainUI->ReloadSkill();
            }
        }
    }
    */
}

void UIGongFa::OnSetSkill2(uint32 skillId)
{
    /*
    const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillTpl)
    {
        if (pSkillTpl->kind == SKILL_CLASS_NORMALATTACK || pSkillTpl->kind == SKILL_CLASS_PASSIVESKILL)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(653);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //不能放入普通攻击或被动技能!
            return;
        }

        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_skill3"));
        if (img)
        {
            //img->loadTexture(pSkillTpl->strSkillIcon);
        }

        if (m_Hero->m_SkillList[2] != skillId)
        {
            m_Hero->SetChoseSkill(2, skillId);

            ReloadChoseSkillIcon();
            UIMain* pMainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
            if (pMainUI)
            {
                pMainUI->ReloadSkill();
            }
        }
    }
    */
}

void UIGongFa::OnSetSkill3(uint32 skillId)
{
    /*
    const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillTpl)
    {
        if (pSkillTpl->kind == SKILL_CLASS_NORMALATTACK || pSkillTpl->kind == SKILL_CLASS_PASSIVESKILL)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(653);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //不能放入普通攻击或被动技能!
            return;
        }

        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_skill4"));
        if (img)
        {
            //img->loadTexture(pSkillTpl->strSkillIcon);
        }

        if (m_Hero->m_SkillList[3] != skillId)
        {
            m_Hero->SetChoseSkill(3, skillId);

            ReloadChoseSkillIcon();
            UIMain* pMainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
            if (pMainUI)
            {
                pMainUI->ReloadSkill();
            }
        }
    }
    */
}

void UIGongFa::OnSetSkill4(uint32 skillId)
{
    /*
    const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillTpl)
    {
        if (pSkillTpl->kind == SKILL_CLASS_NORMALATTACK || pSkillTpl->kind == SKILL_CLASS_PASSIVESKILL)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(653);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //不能放入普通攻击或被动技能!
            return;
        }

        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_skill5"));
        if (img)
        {
            //img->loadTexture(pSkillTpl->strSkillIcon);
        }

        if (m_Hero->m_SkillList[4] != skillId)
        {
            m_Hero->SetChoseSkill(4, skillId);

            ReloadChoseSkillIcon();
            UIMain* pMainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
            if (pMainUI)
            {
                pMainUI->ReloadSkill();
            }
        }
    }
    */
}

void UIGongFa::OnSetSkill5(uint32 skillId)
{
    /*
    const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillTpl)
    {
        if (pSkillTpl->kind == SKILL_CLASS_NORMALATTACK || pSkillTpl->kind == SKILL_CLASS_PASSIVESKILL)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(653);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //不能放入普通攻击或被动技能!
            return;
        }

        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_skill6"));
        if (img)
        {
            //img->loadTexture(pSkillTpl->strSkillIcon);
        }

        if (m_Hero->m_SkillList[5] != skillId)
        {
            m_Hero->SetChoseSkill(5, skillId);

            ReloadChoseSkillIcon();
            UIMain* pMainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
            if (pMainUI)
            {
                pMainUI->ReloadSkill();
            }
        }
    }
    */
}

void UIGongFa::OnSetSkill6(uint32 skillId)
{
    /*
    const SkillInfo* pSkillTpl = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillTpl)
    {
        if (pSkillTpl->kind == SKILL_CLASS_NORMALATTACK || pSkillTpl->kind == SKILL_CLASS_PASSIVESKILL)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(653);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //不能放入普通攻击或被动技能!
            return;
        }

        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_skill7"));
        if (img)
        {
            //img->loadTexture(pSkillTpl->strSkillIcon);
        }

        if (m_Hero->m_SkillList[6] != skillId)
        {
            m_Hero->SetChoseSkill(6, skillId);

            ReloadChoseSkillIcon();
            UIMain* pMainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
            if (pMainUI)
            {
                pMainUI->ReloadSkill();
            }
        }
    }
    */
}

void UIGongFa::OnActivateSkillType(int32 skillTreeType)
{
    if (skillTreeType != 0)
    {
        MsgSkill_Activate activateMsg;
        activateMsg.ActivateSkillType = skillTreeType;
        g_NetManager->SendMsg(activateMsg);

        m_Hero->m_ActivateSkillType = skillTreeType;

        m_startImg->setVisible(true);
        m_startBtn->setVisible(false);
    }
}

void UIGongFa::SetupSkillsByTreeId(int32 skilltreeId)
{

    const SkillActivate* pActivateTpl = g_CSVFileManager->GetSkillActivateByType(SKILL_ID_TO_TYPE(skilltreeId));
    assert(pActivateTpl);

    const std::vector<const SkillInfo*>* pSkillArray = g_CSVFileManager->GetSkillTreeSkills(skilltreeId);
    assert(pSkillArray);

    for (int i = 0; i < ActivteSkillNum; ++i)
    {
        m_HasSkill[i] = false;
        int32 activateSkillId = pActivateTpl->activateSkillId[i] ;
        if (activateSkillId == 0)
        {
            //隐藏
            Node* panel = static_cast<cc2dui::ScrollView*>(m_gongfaPanel->getChildByName(skillItemNames[i]));
            if (panel)
                panel->setVisible(false);
        }
        else
        {
            //显示
            Node* panel = static_cast<cc2dui::ScrollView*>(m_gongfaPanel->getChildByName(skillItemNames[i]));
            if (panel)
            {
                panel->setVisible(true);


                if ((int)pSkillArray->size() > i)
                {
                    activateSkillId = (*pSkillArray)[i]->id;
                    m_HasSkill[i] = true;
                }

                const SkillInfo* pSkillInfo = g_CSVFileManager->GetSkillByID(activateSkillId);
                assert(pSkillInfo);
                if (pSkillInfo)
                {
                    //>设置技能id
                    m_skillChecks[i]->setTag(activateSkillId);
                    //>设置技能名字
                    Text* skillNameTxt = static_cast<cc2dui::Text*>(panel->getChildByName("tx_skillname"));
                    if (skillNameTxt)
                    {
                        skillNameTxt->setString(BaseFunction::a2u(pSkillInfo->strName));
                    }

                    ImageView* skillDiableImg = static_cast<cc2dui::ImageView*>(panel->getChildByName("img_disable"));
                    if (skillDiableImg)
                    {
                        if (m_HasSkill[i])
                        {
                            skillDiableImg->setVisible(false);
                        }else
                        {
                            skillDiableImg->setVisible(true);
                        }
                    }

                    Text* skillTypeTxt = static_cast<cc2dui::Text*>(panel->getChildByName("tx_type"));
                    if (skillTypeTxt)
                    {
                        skillTypeTxt->setColor(Color3B::YELLOW);
                        SkillClass kind = (SkillClass)pSkillInfo->kind;
                        switch (kind)   //技能种类
                        {
                        case SKILL_CLASS_SKILL: //1.主动技能
                            skillTypeTxt->setString(BaseFunction::a2u(g_CSVFileManager->GetTextByID(608)->strContent));
                            break;
                        case SKILL_CLASS_NORMALATTACK:  //2.普通攻击
                            skillTypeTxt->setString(BaseFunction::a2u(g_CSVFileManager->GetTextByID(609)->strContent));
                            break;
                        case SKILL_CLASS_XPSKILL: //3.XP技能
                            skillTypeTxt->setString(BaseFunction::a2u(g_CSVFileManager->GetTextByID(610)->strContent));
                            break;
                        case SKILL_CLASS_PASSIVESKILL: //4.被动技能
                            skillTypeTxt->setString(BaseFunction::a2u(g_CSVFileManager->GetTextByID(611)->strContent));
                            skillTypeTxt->setColor(Color3B(0,128,255));
                            break;
                        case SKILL_CLASS_MAX:
                            CCASSERT(0, "error skill class type");
                            break;
                        default:
                            break;
                        }
                    }

                    Text* skillLevelTxt = static_cast<cc2dui::Text*>(panel->getChildByName("tx_level"));
                    if (skillLevelTxt)
                    {
                        if (m_HasSkill[i])
                        {
                            skillLevelTxt->setString(StringUtils::format("Lv.%d", activateSkillId % 100));
                        }
                        else
                        {
                            skillLevelTxt->setString("Lv.0");
                        }
                    }

                    ImageView* skillIconImg = static_cast<cc2dui::ImageView*>(panel->getChildByName("img_skill"));
                    if (skillIconImg)
                    {
                        //skillIconImg->loadTexture(pSkillInfo->strSkillIcon);
                    }
                }
            }
        }
    }
}


void UIGongFa::SetSkillTreeInfo(int32 treeType)
{
    /*
    const SkillInfoIt* pSkillTree = m_Hero->GetSkillByType(treeType);
    if (pSkillTree)
    {
        m_CurTreeType = treeType;
        const SkillTreeInfo* pSkillTreeTpl = g_CSVFileManager->GetSkillTreeByID(pSkillTree->id);
        if (pSkillTreeTpl)
        {

            ImageView* skillTreeIcon = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName("Image_icon"));
            if (skillTreeIcon)
            {
                const SkillActivate* pActivateTpl = g_CSVFileManager->GetSkillActivateByType(treeType);
                if (pActivateTpl)
                {
                    skillTreeIcon->loadTexture(pActivateTpl->skllTreeIcon);
                }
            }

            Text* skillTreeName = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("value_spellname"));
            if (skillTreeName)
            {
                skillTreeName->setString(BaseFunction::a2u(pSkillTreeTpl->strName));
            }

            Text* skillTreeLevel = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("value_spelllevel"));
            if (skillTreeLevel)
            {
                skillTreeLevel->setString(StringUtils::format("%d", pSkillTree->level));
            }

            Text* skillTreeExp = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("value_exp"));
            if (skillTreeExp)
            {
                skillTreeExp->setString(StringUtils::format("%d/%d", pSkillTree->exp, pSkillTreeTpl->iLevelExp));
            }

            LoadingBar* skillTreeExpBar = static_cast<cc2dui::LoadingBar*>(m_gongfaPanel->getChildByName("LoadingBar"));
            if (skillTreeExpBar)
            {
                skillTreeExpBar->setPercent(pSkillTree->exp * 100.0f / pSkillTreeTpl->iLevelExp);
            }

            if (treeType == m_Hero->m_ActivateSkillType)
            {
                m_startImg->setVisible(true);
                m_startBtn->setVisible(false);
            }
            else
            {
                m_startImg->setVisible(false);
                m_startBtn->setVisible(true);
            }
        }
    }
    */
}

void UIGongFa::SetSkillPanelInfo(uint32 skillId)
{
    const SkillInfo* pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillInfo)
    {
        Text* skillNameText = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("tx_skillname"));
        if (skillNameText)
        {
            skillNameText->setString(BaseFunction::a2u(pSkillInfo->strName));
        }

        Text* skillLevelText = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("value_skillname"));
        if (skillLevelText)
        {
            skillLevelText->setString(StringUtils::format("Lv.%d", skillId % 100));
        }

        Text* skillCoolText = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("value_cooling"));
        if (skillCoolText)
        {
            skillCoolText->setString(StringUtils::format("%.1fS", pSkillInfo->CD));
        }

        Text* skillCostText = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("value_consumption"));
        if (skillCostText)
        {
            skillCostText->setString(StringUtils::format("%dMP", pSkillInfo->mpCost));
        }

        Text* skillDesc1Text = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("tx_skillintroduce1"));
        if (skillDesc1Text)
        {
            //skillDesc1Text->setString(BaseFunction::a2u(pSkillInfo->strDesc[0]));
        }

        Text* skillDesc2Text = static_cast<cc2dui::Text*>(m_gongfaPanel->getChildByName("tx_skillintroduce2"));
        if (skillDesc2Text)
        {
            //skillDesc2Text->setString(BaseFunction::a2u(pSkillInfo->strDesc[1]));
        }
    }
}

void UIGongFa::ReloadChoseSkillIcon()
{
    /*
    static const char* skillIconName[MAX_CHOSESKILL - 1] = { "Image_skill1", "Image_skill2", "Image_skill3", "Image_skill4", "Image_skill5", "Image_skill6", "Image_skill7" };

    for (int i = 0; i < MAX_CHOSESKILL - 1; ++i)
    {
        ImageView* img = static_cast<cc2dui::ImageView*>(m_gongfaPanel->getChildByName(skillIconName[i]));
        if (img)
        {
            if (m_Hero->m_SkillList[i] != 0)
            {
                const SkillInfo* pSkill = g_CSVFileManager->GetSkillByID(m_Hero->m_SkillList[i]);
                //if (pSkill)
                //    img->loadTexture(pSkill->strSkillIcon);
            }
            else
            {
                img->loadTexture("UI/MagicArts/skilltemp.png");
            }
        }
    }
    */
}

void UIGongFa::LoadCheckBox()
{
    /*
    m_magicSV->removeAllChildren();
    m_mainChecks.clear();

	for (vector<SkillTreeInfoIt>::iterator iter = m_Hero->m_SkillTreeList.begin();
         iter != m_Hero->m_SkillTreeList.end(); 
         ++iter)
	{
		 const SkillTreeInfo *tagSkillTree = g_CSVFileManager->GetSkillTreeByID((*iter).id);
		 AddMagicItem(m_magicSV,m_Template->clone(),tagSkillTree);
	}

    UpdateMagicItems();
    */
}

void UIGongFa::AddMagicItem(cc2dui::ScrollView* sv,cc2dui::Widget* layout,const SkillTreeInfo *tagSkillTree)
{
    /*
	if (tagSkillTree == NULL)
		return;
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	int32 skillTreeType = SKILL_ID_TO_TYPE(tagSkillTree->id);
	int32 skillTreeLevel = tagSkillTree->id - skillTreeType * 1000;
	char szGongfaLevel[10];
	sprintf(szGongfaLevel, "Lv.%d", skillTreeLevel);

	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_level"))->setString(szGongfaLevel);
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_model"))->setString(BaseFunction::a2u(tagSkillTree->strName.c_str()));
	CheckBox *cb;
	cb =  static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("cb_model"));
	if (cb)
	{
		m_mainChecks.insert(pair<int,CheckBox*>(skillTreeType, cb));
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				map<int,CheckBox*>::iterator it =  m_mainChecks.begin();
				for (; it != m_mainChecks.end(); it++)
				{
                    it->second->setSelected(false);
                    if (tcb == it->second)
                    {
                        int treeType = it->first;
                        const SkillTreeInfoIt* pTree = m_Hero->GetSkillTreeByType(treeType);
                        if (pTree){
                            SetupSkillsByTreeId(pTree->id);
                            SetSkillTreeInfo(treeType);
                        }
                    }
				}
				tcb->setSelected(true);
			}
		});
	}
	sv->addChild(newLayout);
    */
}

void UIGongFa::UpdateMagicItems()
{
    if (m_magicSV)
    {
        m_magicSV->setLayoutType(Layout::Type::HORIZONTAL);
        m_magicSV->forceDoLayout();
        Size TemplateSize = m_magicSV->getInnerContainerSize();
        TemplateSize.width = m_magicSV->getChildrenCount() * m_Template->getContentSize().width;
        m_magicSV->setInnerContainerSize(TemplateSize);
    }
}
