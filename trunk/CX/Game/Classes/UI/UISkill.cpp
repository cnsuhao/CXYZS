#include "UISkill.h"

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

const char* WuJiCheckBoxName[UISkill::WuJi_TypeNumer] = { "CheckBox_dao" , "CheckBox_jian" , "CheckBox_fochen" };
const char* WuJiSkillsName[UISkill::WuJi_SkillNumber] = { "Panel_skill1", "Panel_skill2", "Panel_skill3", "Panel_skill4" };
const char* PageIndexImagePath[2] = { "UI/Skill/skill_dot_0.png", "UI/Skill/skill_dot_1.png" };
const char* JueXuePageIndexName[UISkill::JueXue_PageNumber] = { "Image_up_sw1", "Image_up_sw2", "Image_up_sw3" };
const char* JueXuePortableIndexName[UISkill::JueXue_PortablePageNumber] = { "Image_down_sw1", "Image_down_sw2", "Image_down_sw3" };
const char* JueXuePageSkillName[UISkill::JueXue_NumberSkillPerPage] = { "juexue_1", "juexue_2", "juexue_3", "juexue_4", "juexue_5", "juexue_6", "juexue_7", "juexue_8", "juexue_9", "juexue_10", "juexue_11", "juexue_12", "juexue_13", "juexue_14", "juexue_15" };
const char* JuexuePortableName[UISkill::JueXue_PortableNumber] = { "juexue_s1", "juexue_s2", "juexue_s3", "juexue_s4", "juexue_s5", "juexue_sb", "juexue_s7" };

const int SkillIdOfDao[UISkill::WuJi_SkillNumber] = { 101, 102, 103, 104 };
const int SkillIdOfJian[UISkill::WuJi_SkillNumber] = { 105, 106, 107, 108 };
const int SkillIdOfFoChen[UISkill::WuJi_SkillNumber] = { 109, 110, 111, 112 };




UISkill::UISkill()
{
    m_RootPanel = NULL;
    m_WujiPanel = NULL;
    m_JueXuePanel = NULL;

    for (int i = 0; i < UISkill::Skill_Page; ++i)
    {
        m_SkillPage[i] = NULL;
    }
    for (int i = 0; i < UISkill::WuJi_TypeNumer; ++i)
    {
        m_WuJiType[i] = NULL;
    }
    for (int i = 0; i < UISkill::WuJi_SkillNumber; ++i)
    {
        m_WuJiSkill[i] = NULL;
        m_HeightLight[i] = NULL;
    }

    m_JueXueNextPage = NULL;
    m_JueXuePreviousPage = NULL;
    m_JueXuePortableSwitch = NULL;

    for (int i = 0; i < UISkill::JueXue_PageNumber; ++i)
    {
        m_JueXuePageIndex[i] = NULL;
    }

    for (int i = 0; i < UISkill::JueXue_PortablePageNumber; ++i)
    {
        m_JueXuePortableIndex[i] = NULL;
    }

    for (int i = 0; i < UISkill::JueXue_NumberSkillPerPage; ++i)
    {
        m_JueXueOfPage[i] = NULL;
    }

    for (int i = 0; i < UISkill::JueXue_PortableNumber; ++i)
    {
        m_JueXuePortable[i] = NULL;
    }

    m_SelectWuJiId = 0;
    m_CurJueXuePage = 0;
    m_CurPortableJueXuePage = 0;
}

UISkill::~UISkill()
{

}

bool UISkill::init()
{
    bool bRet = false;
    do 
    {
        //>加载主面板
        m_RootPanel = CSLoader::createNode("UI/Skill.csb");
        if (m_RootPanel)
        {
            //>关闭按钮
            Button* pButton = static_cast<Button*>(m_RootPanel->getChildByName("Button_Close"));
            if (pButton)
            {
                pButton->addClickEventListener([this](Ref* pRef)
                {
                    g_UIManager->HideUI(UI_GONGFA);
                });
            }

            m_SkillPage[0] = static_cast<CheckBox*>(m_RootPanel->getChildByName("CheckBox_wj"));
            m_SkillPage[1] = static_cast<CheckBox*>(m_RootPanel->getChildByName("CheckBox_jx"));

            CCASSERT(m_SkillPage[0] && m_SkillPage[1], "");
            m_SkillPage[0]->setTag(1);
            m_SkillPage[0]->setSelected(true);
            m_SkillPage[1]->setTag(2);
            m_SkillPage[1]->setSelected(false);

            for (int i = 0; i < UISkill::Skill_Page; ++i)
            {
                m_SkillPage[i]->addEventListener([this](Ref* _sender,CheckBox::EventType type)
                {
                    for (int i = 0; i < UISkill::Skill_Page; ++i)                   
                        m_SkillPage[i]->setSelected(false);
                    
                    CheckBox* sender = static_cast<CheckBox*>(_sender);
                    if (sender)
                    {
                        int32 selectPageId = sender->getTag();
                        sender->setSelected(true);
                        SwitchPage(selectPageId);
                    }
                });
            }

            //>武技面板
            m_WujiPanel = m_RootPanel->getChildByName("Panel_wuji");
            CCASSERT(m_WujiPanel, "");
            if (m_WujiPanel)
            {

                for (int i = 0; i < UISkill::WuJi_TypeNumer; ++i)
                {
                    m_WuJiType[i] = static_cast<CheckBox*>(m_WujiPanel->getChildByName(WuJiCheckBoxName[i]));
                    CCASSERT(m_WuJiType[i], "");
                    m_WuJiType[i]->setSelected(false);

                    m_WuJiType[i]->setTag(i);
                    m_WuJiType[i]->addEventListener([this](Ref* _sender,CheckBox::EventType type)
                    {//>设置种类

                        for (int i = 0; i < UISkill::WuJi_TypeNumer; ++i)                   
                            m_WuJiType[i]->setSelected(false);

                        CheckBox* sender = static_cast<CheckBox*>(_sender);
                        if (sender)
                        {
                            sender->setSelected(true);
                            SetWuJiType((UISkill::WuJiType)sender->getTag());
                        }
                    });
                }
         
                for (int i = 0; i < UISkill::WuJi_SkillNumber; ++i)
                {
                    m_WuJiSkill[i] = static_cast<Node*>(m_WujiPanel->getChildByName(WuJiSkillsName[i]));
                    CCASSERT(m_WuJiSkill[i], "");


                    ImageView* pImg = static_cast<ImageView*>(m_WuJiSkill[i]->getChildByName("Image_l1"));
                    if (pImg)
                    {
                        pImg->setTag(i);
                        pImg->setTouchEnabled(true);
                        pImg->addClickEventListener([this](Ref* pRef)
                        {
                            int idx = static_cast<Node*>(pRef)->getTag();
                            for (int i = 0; i < UISkill::WuJi_SkillNumber; ++i)
                            {
                                m_HeightLight[i]->setVisible(false);
                            }
                            m_HeightLight[idx]->setVisible(true);

                            int skillId = m_HeightLight[idx]->getTag();
                            SetWuJiSkillDesc(skillId);

                        });
                    }

                    m_HeightLight[i] = static_cast<ImageView*>(m_WuJiSkill[i]->getChildByName("Image_l2"));
                    CCASSERT(m_HeightLight[i], "");
                    m_HeightLight[i]->setVisible(false);
                }


                Button* pUpgradeButton = static_cast<Button*>(m_WujiPanel->getChildByName("Button_upgread"));
                if (pUpgradeButton)
                {
                    pUpgradeButton->addClickEventListener([this](Ref* pRef)
                    {
                        //if (m_SelectSkillId > 0)
                        //{

                        //}
                    });
                }

            }

            //>绝学面板
            m_JueXuePanel = m_RootPanel->getChildByName("Panel_juexue");
            CCASSERT(m_JueXuePanel, "");
            if (m_JueXuePanel)
            {
                m_JueXuePreviousPage =  static_cast<Button*>(m_JueXuePanel->getChildByName("Button_left"));
                m_JueXuePreviousPage->addClickEventListener([this](Ref* ndoe)
                {
                    SetJueXuePageIndex(--m_CurJueXuePage);
                });
                m_JueXueNextPage = static_cast<Button*>(m_JueXuePanel->getChildByName("Button_right"));
                m_JueXueNextPage->addClickEventListener([this](Ref* ndoe)
                {
                    SetJueXuePageIndex(++m_CurJueXuePage);
                });

                m_JueXuePortableSwitch = static_cast<Button*>(m_JueXuePanel->getChildByName("Button_swith"));
                m_JueXuePortableSwitch->addClickEventListener([this](Ref* ndoe)
                {
                    m_CurPortableJueXuePage = ++m_CurPortableJueXuePage % JueXue_PortablePageNumber;
                    SetJueXuePortableIndex(m_CurPortableJueXuePage);
                });
            
                for (int i = 0; i < UISkill::JueXue_PageNumber; ++i)
                {
                    m_JueXuePageIndex[i] = static_cast<ImageView*>(m_JueXuePanel->getChildByName(JueXuePageIndexName[i]));
                }

                for (int i = 0; i < UISkill::JueXue_PortablePageNumber; ++i)
                {
                    m_JueXuePortableIndex[i] = static_cast<ImageView*>(m_JueXuePanel->getChildByName(JueXuePortableIndexName[i]));
                }

                
                EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
                listener->setSwallowTouches(true);
                listener->onTouchBegan = [this](Touch* touch, Event* event)
                {
                    ImageView* img = static_cast<ImageView*>(event->getCurrentTarget());
                    if (img->hitTest(touch->getLocation()))
                    {

                        if(g_LogicManager->m_Hero->GetSkillByType(SKILL_ID_TO_TYPE(img->getTag()))) 
                            return false;

                        const SkillInfo* pSkillInfo = g_CSVFileManager->GetSkillByID(img->getTag());
                        if (pSkillInfo == NULL)
                            return false;
                        
                        Sprite* spr = Sprite::create(pSkillInfo->iconPath);
                        if (spr)
                        {
                            spr->setPosition(touch->getStartLocation());
                            spr->setTag(img->getTag());
                            g_UIManager->addChild(spr, LAYER_SCENE_EFFECT);
                            img->setUserData(spr);
                        }
                        return true;
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
                        //if (m_CanUseSkillSolt[0]->hitTest(spr->getPosition()))
                        //{
                        //    OnSetCommonSkill(spr->getTag());
                        //}else if(m_CanUseSkillSolt[1]->hitTest(spr->getPosition()))
                        //{
                        //    OnSetSkill1(spr->getTag());
                        //}else if(m_CanUseSkillSolt[2]->hitTest(spr->getPosition()))
                        //{
                        //    OnSetSkill2(spr->getTag());
                        //}else if(m_CanUseSkillSolt[3]->hitTest(spr->getPosition()))
                        //{
                        //    OnSetSkill3(spr->getTag());
                        //}else if(m_CanUseSkillSolt[4]->hitTest(spr->getPosition()))
                        //{
                        //    OnSetSkill4(spr->getTag());
                        //}else if(m_CanUseSkillSolt[5]->hitTest(spr->getPosition()))
                        //{
                        //    OnSetSkill5(spr->getTag());
                        //}else if(m_CanUseSkillSolt[6]->hitTest(spr->getPosition()))
                        //{
                        //    OnSetSkill6(spr->getTag());
                        //}
                        spr->removeFromParent();
                        img->setUserData(NULL);
                    }

                };

                for (int i = 0; i < UISkill::JueXue_NumberSkillPerPage; ++i)
                {
                    m_JueXueOfPage[i] = static_cast<Node*>(m_JueXuePanel->getChildByName(JueXuePageSkillName[i]));

                    m_JueXueOfPage[i]->setTag(i);
                    ImageView* pImg = static_cast<ImageView*>(m_JueXueOfPage[i]->getChildByName("Image_kuang"));
                    if (pImg)
                    {
                        //pImg->setTouchEnabled(true);
                        //pImg->addClickEventListener([this](Ref* node)
                        //{
                        //    ImageView* pImg = (ImageView*)node;
                        //    int skillID = pImg->getTag();
                        //    SetJueXueSkillDesc(skillID);
                        //});

                        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener->clone(), pImg);

                    }   
                }

                for (int i = 0; i < UISkill::JueXue_PortableNumber; ++i)
                {
                    m_JueXuePortable[i] = static_cast<Node*>(m_JueXuePanel->getChildByName(JuexuePortableName[i]));
                }
            }


            addChild(m_RootPanel);

            SwitchPage(1);
        }

        bRet = true;
    } while (0);

    return bRet;
}

void UISkill::SetWuJiType(WuJiType type)
{

    const int* pIdArray = NULL;
    switch (type)
    {
    case UISkill::WuJiType_Dao:
        pIdArray = SkillIdOfDao;
        break;
    case UISkill::WuJiType_Jian:
        pIdArray = SkillIdOfJian;
        break;
    case UISkill::WuJiType_FoChen:
        pIdArray = SkillIdOfFoChen;
        break;
    default:
        break;
    }

    if (pIdArray)
    {
        for (int i = 0; i < UISkill::WuJi_SkillNumber; ++i)
        {
            const SkillInfoIt* pInfo = g_LogicManager->m_Hero->GetSkillByType(pIdArray[i]);
            if (pInfo)
            {
                SetWuJiSkillInfo(i, SKILL_TYPE_AND_LEVEL_TO_ID(pInfo->skillType, pInfo->level), i == 0);
            }
            else
            {
                SetWuJiSkillInfo(i, SKILL_TYPE_AND_LEVEL_TO_ID(pIdArray[i], 1), i == 0);
            }
        }
    }
}

void UISkill::SetJueXuePageIndex(int index)
{
    if (index < UISkill::JueXue_PageNumber)
    {
        for (int i = 0; i < UISkill::JueXue_PageNumber; ++i)
        {
            if (i == index)
            {
                m_JueXuePageIndex[i]->loadTexture(PageIndexImagePath[1]);
                LoadJueXuePageIndex(index);
            }
            else
            {
                m_JueXuePageIndex[i]->loadTexture(PageIndexImagePath[0]);
            }
        }

        m_JueXuePreviousPage->setEnabled(true);
        m_JueXueNextPage->setEnabled(true);

        if (index == 0)
        {
            m_JueXuePreviousPage->setEnabled(false);
        }
        else if (index == (JueXue_PageNumber - 1))
        {
            m_JueXueNextPage->setEnabled(false);
        }
    }
}

void UISkill::SetJueXuePortableIndex(int index)
{

    if (index < UISkill::JueXue_PortablePageNumber)
    {
        for (int i = 0; i < UISkill::JueXue_PortablePageNumber; ++i)
        {
            if (i == index)
            {
                m_JueXuePortableIndex[i]->loadTexture(PageIndexImagePath[1]);
                LoadJueXuePortableIndex(index);
            }
            else
            {
                m_JueXuePortableIndex[i]->loadTexture(PageIndexImagePath[0]);
            }
        }
    }
}

void UISkill::SetWuJiSkillInfo(int index, int skillId, bool isSeleced)
{
    if (index < UISkill::WuJi_SkillNumber)
    {
        const SkillInfo*  pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
        if (pSkillInfo == NULL)
            return;

        m_HeightLight[index]->setTag(skillId);
        if (isSeleced)
        {
            m_HeightLight[index]->setVisible(true);
            SetWuJiSkillDesc(skillId);
        }
        else
        {
            m_HeightLight[index]->setVisible(false);
        }
        
        ImageView* pSkillIcon = static_cast<ImageView*>(m_WuJiSkill[index]->getChildByName("Image_sk"));
        if (pSkillIcon)
        {
            pSkillIcon->loadTexture(pSkillInfo->iconPath);
        }


        Text* pSkillName = static_cast<Text*>(m_WuJiSkill[index]->getChildByName("Text_skill_name"));
        if (pSkillName)
        {
            pSkillName->setString(BaseFunction::a2u(pSkillInfo->strName));
        }

        Text* pSkillLevel = static_cast<Text*>(m_WuJiSkill[index]->getChildByName("Text_skill_level"));
        if (pSkillLevel)
        {
            pSkillLevel->setString(StringUtils::format("lv.%d", skillId%1000));
        }
    }
}

void UISkill::SetWuJiSkillDesc(int skillId)
{
    const SkillInfo*  pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillInfo == NULL)
        return;

    ImageView* pSkillIcon = static_cast<ImageView*>(m_WujiPanel->getChildByName("Image_skill_icon"));
    if (pSkillIcon)
    {
        pSkillIcon->loadTexture(pSkillInfo->iconPath);
    }

    Text* pSkillName = static_cast<Text*>(m_WujiPanel->getChildByName("Text_skill_name"));
    if (pSkillName)
    {
        pSkillName->setString(BaseFunction::a2u(pSkillInfo->strName));
    }

    Text* pSkillMpCost = static_cast<Text*>(m_WujiPanel->getChildByName("Text_nl_value"));
    if (pSkillMpCost)
    {
        pSkillMpCost->setString(StringUtils::format("%d", pSkillInfo->mpCost));
    }

    Text* pSkillCD = static_cast<Text*>(m_WujiPanel->getChildByName("Text_lq_value"));
    if (pSkillCD)
    {
        pSkillCD->setString(StringUtils::format("%.2fs", pSkillInfo->CD));
    }

    Text* pSkillDest = static_cast<Text*>(m_WujiPanel->getChildByName("Text_level"));
    if (pSkillDest)
    {
        pSkillDest->setString(BaseFunction::a2u(pSkillInfo->desc));
    }

    Text* pSkillNextDest = static_cast<Text*>(m_WujiPanel->getChildByName("Text_next_level"));
    if (pSkillNextDest)
    {
        const SkillInfo*  pNextLevelSkillInfo = g_CSVFileManager->GetSkillByID(skillId + 1);
        if (pNextLevelSkillInfo)
        {
            pSkillNextDest->setVisible(true);
            pSkillNextDest->setString(BaseFunction::a2u(pNextLevelSkillInfo->desc));
        }
        else
        {
            pSkillNextDest->setVisible(false);
        }
    }
    
    Text* pSkillMoneyCost = static_cast<Text*>(m_WujiPanel->getChildByName("Text_xiaohao"));
    if (pSkillMoneyCost)
    {
        pSkillMoneyCost->setString(StringUtils::format("%d/%d", 0, pSkillInfo->upgradeMoneyCost));
    }
}

void UISkill::SetJueXueSkillDesc(int skillId)
{
    const SkillInfo*  pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillInfo == NULL)
        return;

    Node* pSkillIconNode = static_cast<Node*>(m_JueXuePanel->getChildByName("juexue_x1"));
    if (pSkillIconNode)
    {

        ImageView* pSkillIcon = static_cast<ImageView*>(pSkillIconNode->getChildByName("Image_icon"));
        if (pSkillIcon)
        {
            pSkillIcon->loadTexture(pSkillInfo->iconPath);
        }

        ImageView* pSkillLevelBg = static_cast<ImageView*>(pSkillIconNode->getChildByName("Image_lvbg"));
        if (pSkillLevelBg)
        {
            pSkillLevelBg->setVisible(false);
        }

        Text* pSkillLevel = static_cast<Text*>(pSkillIconNode->getChildByName("Text_level"));
        if (pSkillLevel)
        {
            pSkillLevel->setVisible(false);
        }
    }

    Text* pSkillName = static_cast<Text*>(m_JueXuePanel->getChildByName("Text_skill_name"));
    if (pSkillName)
    {
        pSkillName->setString(BaseFunction::a2u(pSkillInfo->strName));
    }

    Text* pSkillLevel = static_cast<Text*>(m_JueXuePanel->getChildByName("Text_skill_level"));
    if (pSkillLevel)
    {
        pSkillLevel->setString(StringUtils::format("lv.%d", skillId % 1000));
    }

    Text* pSkillTriggerDesc = static_cast<Text*>(m_JueXuePanel->getChildByName("Text_skill_level"));
    if (pSkillTriggerDesc)
    {
        pSkillTriggerDesc->setString(BaseFunction::a2u(pSkillInfo->triggerDesc));
    }

    Text* pSkillDest = static_cast<Text*>(m_JueXuePanel->getChildByName("Text_level"));
    if (pSkillDest)
    {
        pSkillDest->setString(BaseFunction::a2u(pSkillInfo->desc));
    }

    Text* pSkillNextDest = static_cast<Text*>(m_JueXuePanel->getChildByName("Text_next_level"));
    if (pSkillNextDest)
    {
        const SkillInfo*  pNextLevelSkillInfo = g_CSVFileManager->GetSkillByID(skillId + 1);
        if (pNextLevelSkillInfo)
        {
            pSkillNextDest->setVisible(true);
            pSkillNextDest->setString(BaseFunction::a2u(pNextLevelSkillInfo->desc));
        }
        else
        {
            pSkillNextDest->setVisible(false);
        }
    }

    Node* pSkillMatrialNode = static_cast<Node*>(m_JueXuePanel->getChildByName("juexue_cailiao"));
    if (pSkillMatrialNode)
    {
        ImageView* pSkillIcon = static_cast<ImageView*>(pSkillMatrialNode->getChildByName("Image_icon"));
        if (pSkillIcon)
        {
            const ItemsInfo* pItemInfo = g_CSVFileManager->GetItemsByID(pSkillInfo->upgradeItemId);
            if (pItemInfo)
            {
                pSkillIcon->loadTexture(pItemInfo->strIcon);
            }            
        }
    }

    Text* pSkillMatrialCost = static_cast<Text*>(m_JueXuePanel->getChildByName("Text_xiaohao_cl"));
    if (pSkillMatrialCost)
    {
        pSkillMatrialCost->setString(StringUtils::format("%d/%d", 0, pSkillInfo->upgradeMoneyCost));
    }

    Text* pSkillMoneyCost = static_cast<Text*>(m_JueXuePanel->getChildByName("Text_xiaohao_jb"));
    if (pSkillMoneyCost)
    {
        pSkillMoneyCost->setString(StringUtils::format("%d/%d", 0, pSkillInfo->upgradeMoneyCost));
    }
}

void UISkill::SwitchPage(int id)
{
    if (id == 1)
    {
        m_WujiPanel->setVisible(true);
        m_JueXuePanel->setVisible(false);
        m_WuJiType[WuJiType_Dao]->setSelected(true);
        SetWuJiType(WuJiType_Dao);
    }
    else if(id == 2)
    {
        m_JueXuePanel->setVisible(true);
        m_WujiPanel->setVisible(false);
        SetJueXuePageIndex(0);
    }
}

void UISkill::LoadJueXuePageIndex(int index)
{
    m_CurJueXuePage = index;
    int startId = index * JueXue_NumberSkillPerPage;
    int end = startId + JueXue_NumberSkillPerPage;
    
    const std::vector<int32>& jueXueOrder = g_CSVFileManager->GetSkillJueXueOrder();
    int count = jueXueOrder.size();
    for (int c = startId, i = 0; c < end; ++c, ++i)
    {
        if (c < count)
        {//>有绝学
            m_JueXueOfPage[i]->setVisible(true);


            int skillType = jueXueOrder[c];
            int skillId;

            const SkillInfoIt* pSkillData = g_LogicManager->m_Hero->GetSkillByType(skillType);
            if (pSkillData)
            {
                skillId = SKILL_TYPE_AND_LEVEL_TO_ID(pSkillData->skillType, pSkillData->level);
            }
            else
            {
                skillId = SKILL_TYPE_AND_LEVEL_TO_ID(skillType, 1);
            }

            const SkillInfo* pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
            if (pSkillInfo)
            {
                Text* pSkillName = static_cast<Text*>(m_JueXueOfPage[i]->getChildByName("Text_name"));
                if (pSkillName)
                {
                    pSkillName->setString(BaseFunction::a2u(pSkillInfo->strName));
                }

                Text* pSkillLevel = static_cast<Text*>(m_JueXueOfPage[i]->getChildByName("Text_level"));
                if (pSkillLevel)
                {
                    pSkillLevel->setString(BaseFunction::a2u(StringUtils::format("lv.%d", skillId%1000)));
                }

                ImageView* pSkillIcon = static_cast<ImageView*>(m_JueXueOfPage[i]->getChildByName("Image_icon"));
                if (pSkillIcon)
                {
                    pSkillIcon->loadTexture(pSkillInfo->iconPath);
                }

                ImageView* pSkillKuang = static_cast<ImageView*>(m_JueXueOfPage[i]->getChildByName("Image_kuang"));
                if (pSkillKuang)
                {
                    pSkillKuang->setTag(skillId);
                }
            }
        }
        else
        {//>无绝学
            m_JueXueOfPage[i]->setVisible(false);
        }
    }
}

void UISkill::LoadJueXuePortableIndex(int index)
{
    for (int i = 0; i < UISkill::JueXue_PortableNumber - 2; ++i)
    {
        
        bool portable = false;


        ImageView* pSkillIcon = static_cast<ImageView*>(m_JueXuePortable[i]->getChildByName("Image_icon"));
        if (pSkillIcon)
        {
            if (portable)
            {
                //pSkillIcon->loadTexture("");
            }else
            {
                pSkillIcon->loadTexture("UI/Skill/skill_lock3.png");
            }
        }

        ImageView* pSkillIconBg = static_cast<ImageView*>(m_JueXuePortable[i]->getChildByName("Image_lvbg"));
        if (pSkillIconBg)
        {
            if (portable)
            {
                pSkillIconBg->setVisible(true);
            }
            else
            {
                pSkillIconBg->setVisible(false);
            }
        }

        Text* pSkillLevel = static_cast<Text*>(m_JueXuePortable[i]->getChildByName("Text_level"));
        if (pSkillLevel)
        {
            if (portable)
            {
                pSkillLevel->setVisible(true);
                //pSkillLevel->setString();
            }
            else
            {
                pSkillLevel->setVisible(false);
            }
        }

    }
}
