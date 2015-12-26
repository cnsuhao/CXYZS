#include "UIItemInfo.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "CSVFileManager.h"
#include "Bag.h"
#include "BaseFunction.h"
#include "NetManager.h"
#include "UIWaitting.h"
#include "LogicManager.h"
#include "UIUpgradeEquip.h"
#include "ResManager.h"

static const char* starName[UIItemInfo::s_ShowLevel] = { "Image_Star1", "Image_Star2", "Image_Star3", "Image_Star4", "Image_Star5", "Image_Star6", "Image_Star7", "Image_Star8", "Image_Star9", "Image_Star10" }; 
static const char* kindOfStarPaths[UIItemInfo::s_StarKind] = { "UI/ItemInfo/item_info_star1.png", "UI/ItemInfo/item_info_star2.png", "UI/ItemInfo/item_info_star3.png" };

static const char*  attrNameArray[] = { "Text_sm", "Text_fy", "Text_mf", "Text_bj", "Text_mz", "Text_pj", "Text_cc", "Text_sb" };
static const char*  attrValueArray[] = { "Text_sm_nb", "Text_fy_nb", "Text_mf_nb", "Text_bj_nb", "Text_mz_nb", "Text_pj_nb", "Text_cc_nb", "Text_sb_nb" };
static const char*  attrAppendArray[] = { "Text_sm_x_nb", "Text_fy_x_nb", "Text_mf_x_nb", "Text_bj_x_nb", "Text_mz_x_nb", "Text_pj_x_nb", "Text_cc_x_nb", "Text_sb_x_nb" };
static const char*  equipSetName[] = { "Text_name1", "Text_name2","Text_name3","Text_name4","Text_name5","Text_name6","Text_name7","Text_name8" };
static const char*  equipAttrName[] = { "Text_set1", "Text_set2", "Text_set3", "Text_set4" };
static const char*  equipAttrDesc[] = { "Text_set1_info", "Text_set2_info", "Text_set3_info", "Text_set4_info" };

UIItemInfo::UIItemInfo(void)
{
    m_RootPanel = NULL;
    m_EquipmentPanel = NULL;
    m_ItemPanel = NULL;
    m_StarPanel = NULL;
    for (int i = 0; i < s_ShowLevel; ++i)
        m_StarImage[i] = NULL;
    
    m_AttachPart = NULL;
    m_BattlePowerLable = NULL;
    m_BindTxt = NULL;
    m_ItemBindTxt = NULL;
    m_itemGuid = 0;

    for (int i = 0; i < 8; ++i)
    {
        m_AttributeNames[i]     = NULL;
        m_AttributeValues1[i]   = NULL;
        m_AttributeValues2[i]   = NULL;
    }

    OnClickedOperation = nullptr;

}


UIItemInfo::~UIItemInfo(void)
{
}

bool UIItemInfo::init()
{

    bool bRet = false;
    do 
    {
        //>加载主面板
        m_RootPanel = CSLoader::createNode("UI/ItemInfo.csb");
        if (m_RootPanel == NULL)
            return false;
        addChild(m_RootPanel);

        //>回调事件
        ccWidgetClickCallback callback = ([this](Ref* _sender)
        {
            Button* pBtn = static_cast<Button*>(_sender);
            UIItemInfo::Operation operation = (UIItemInfo::Operation)pBtn->getTag();
            if (OnClickedOperation != NULL)
            {
                size_t idx = 0;
                vector<ButtonOperation>::iterator it = m_ShowOperation.begin();
                for (vector<ButtonOperation>::iterator end = m_ShowOperation.end();
                    it != end; ++it)
                {
                    if ((*it).operation == operation)
                        break;

                    ++idx;
                }
                CCASSERT(idx < m_ShowOperation.size(), "");
                OnClickedOperation(operation, m_ShowOperation[idx].status);
            }
            else
            {
                DefaulClickedOperation(operation, 0);
            }
        });

        //>空面板
        auto emptyPanel = static_cast<Layout*>(m_RootPanel->getChildByName("Panel_empty"));
        if (emptyPanel)
        {
            emptyPanel->setTouchEnabled(true);
            emptyPanel->addClickEventListener([this](Ref* _sender)
            {
                g_UIManager->HideUI(UI_ITEM_INFO);
            });
        }

        //>装备面板
        m_EquipmentPanel = static_cast<Layout*>(m_RootPanel->getChildByName("Panel_equipment"));
        if (m_EquipmentPanel)
        {
            //>星级面板
            m_StarPanel = static_cast<Layout*>(m_EquipmentPanel->getChildByName("Panel_Stars"));
            assert(m_StarPanel);
            for (int i = 0; i < s_ShowLevel; ++i)
            {
                m_StarImage[i] = static_cast<ImageView*>(m_StarPanel->getChildByName(starName[i]));
            }

            //>绑定按钮事件
            Button* pBtn = static_cast<Button*>(m_EquipmentPanel->getChildByName("Button_zb"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_EquipmentButtons.push_back(pBtn);
            }

            pBtn = static_cast<Button*>(m_EquipmentPanel->getChildByName("Button_sx"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_EquipmentButtons.push_back(pBtn);
            }

            pBtn = static_cast<Button*>(m_EquipmentPanel->getChildByName("Button_hc"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_EquipmentButtons.push_back(pBtn);
            }

            pBtn = static_cast<Button*>(m_EquipmentPanel->getChildByName("Button_js"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_EquipmentButtons.push_back(pBtn);
            }

            pBtn = static_cast<Button*>(m_EquipmentPanel->getChildByName("Button_cs"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_EquipmentButtons.push_back(pBtn);
            }

            pBtn = static_cast<Button*>(m_EquipmentPanel->getChildByName("Button_sd"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_EquipmentButtons.push_back(pBtn);
            }

            Text* pZdlTxt = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_zdl_nb"));
            if (pZdlTxt)
            {
                //>战斗数字
                m_BattlePowerLable = LabelAtlas::create("000000", "fonts/fontNb.png", 9, 16, '0');
                m_BattlePowerLable->setPosition(pZdlTxt->getPosition());
                m_BattlePowerLable->setLocalZOrder(pZdlTxt->getLocalZOrder());
                m_EquipmentPanel->addChild(m_BattlePowerLable);
                
                pZdlTxt->setVisible(false);
            }

            m_AttachPart = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_part"));

            m_BindTxt = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_binding"));

            for (int i = 0; i < 8; ++i)
            {
                m_AttributeNames[i] = static_cast<Text*>(m_EquipmentPanel->getChildByName(attrNameArray[i]));
                m_AttributeValues1[i] = static_cast<Text*>(m_EquipmentPanel->getChildByName(attrValueArray[i]));
                m_AttributeValues2[i] = static_cast<Text*>(m_EquipmentPanel->getChildByName(attrAppendArray[i]));
                CCASSERT(m_AttributeNames[i] && m_AttributeValues1[i] && m_AttributeValues2[i], "");
            }
        }

        //>套装面板
        m_EquipSetPanel = static_cast<Layout*>(m_RootPanel->getChildByName("Panel_EquipSet"));

        //>物品面板
        m_ItemPanel = static_cast<Layout*>(m_RootPanel->getChildByName("Panel_item"));
        if (m_ItemPanel)
        {
            //>绑定按钮事件
            Button* pBtn = static_cast<Button*>(m_ItemPanel->getChildByName("Button_sy"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_ItemButtons.push_back(pBtn);
            }

            pBtn = static_cast<Button*>(m_ItemPanel->getChildByName("Button_js"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_ItemButtons.push_back(pBtn);
            }

            pBtn = static_cast<Button*>(m_ItemPanel->getChildByName("Button_cc"));
            if (pBtn)
            {
                pBtn->addClickEventListener(callback);
                m_ItemButtons.push_back(pBtn);
            }

            m_ItemBindTxt  = static_cast<Text*>(m_ItemPanel->getChildByName("Text_binding"));
        }

        bRet = true;
    } while (0);

    return bRet;
}

void UIItemInfo::OnHide()
{
    UIBase::OnHide();

    m_itemGuid = 0;
}

void UIItemInfo::SetupItem(CliItemInfo* pItem, const vector<ButtonOperation>& showOperation, CliItemInfo equipments[EQUIPTYPE_MAX])
{
    assert(pItem);
    if (pItem == NULL)
        return;

    if (pItem->pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        //>装备类
        if(pItem->CanUpStar())
        {
            m_StarPanel->setVisible(true);
            SetStar(pItem->Data.starNum);
        }
        else
        {
            m_StarPanel->setVisible(false);
            Text* level = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_Level"));
            if (level)
                level->setVisible(false);
        }

        m_ItemPanel->setVisible(false);
        m_EquipmentPanel->setVisible(true);

        const EquipmentPrefix* pPrefix = g_CSVFileManager->GetEquipmentPrefixByID(pItem->Data.prefixID);
        Text* name = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_Name"));
        if (name)
        {
            std::string nameText;
            if(pPrefix)
                nameText.append(BaseFunction::a2u(pPrefix->strPrefixName));
            nameText.append(BaseFunction::a2u(pItem->pCfg->strName));
            name->setString(nameText);
            name->setColor(pItem->GetQualityColor());
        }

        ImageView* pIcon = static_cast<ImageView*>(m_EquipmentPanel->getChildByName("Image_ItemIcon"));
        if (pIcon && !pItem->pCfg->strIcon.empty())
        {
            pIcon->loadTexture(pItem->pCfg->strIcon);
        }
        ImageView* pSprQuality = static_cast<ImageView*>(m_EquipmentPanel->getChildByName("Image_Empty"));
        if (pSprQuality)
        {
            pSprQuality->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);

            //>套装特效代码
            if (pItem->pCfg->iSuitID != 0)
            {
                const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
                if (pInfo)
                {
                    Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, pSprQuality->getAnchorPointInPoints());
                    pSprQuality->removeAllChildren();
                    pSprQuality->addChild(sprite);
                }
            }
            else
            {
                pSprQuality->removeAllChildren();
            }
        }

        Text* limitLevel = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_level_nb"));
        if (limitLevel)
        {
            limitLevel->setString(StringUtils::format("%d", pItem->pCfg->iLevelLimit));
        }

        Text* hasNumber = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_count_nb"));
        if (hasNumber)
        {
            hasNumber->setString(StringUtils::format("%d", pItem->Data.num));
        }

        Text* describe = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_describe"));
        if (describe)
        {
            describe->setString(BaseFunction::a2u(pItem->pCfg->strDes));
        }

        if (pItem->pCfg->iSuitID != 0)
        {
            m_EquipSetPanel->setVisible(true);

            const SuitInfo* pSuitInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
            if (pSuitInfo)
            {
                Text* title = static_cast<Text*>(m_EquipSetPanel->getChildByName("Text_titile"));
                if (title)
                {
                    title->setString(BaseFunction::a2u(pSuitInfo->strName));
                }

                int hasCount = 0;
                int SetCount = 0;
                for (int i = 0; i < 8; ++i)
                {
                    int typeId = pSuitInfo->iSuitParts[i];

                    const ItemsInfo* pItemInfo = g_CSVFileManager->GetItemsByID(typeId);
                    if (pItemInfo)
                    {
                        Text* name = static_cast<Text*>(m_EquipSetPanel->getChildByName(equipSetName[i]));
                        if (name)
                        {
                            name->setVisible(true);
                            name->setString(BaseFunction::a2u(pItemInfo->strName));
                        }

                        CliItemInfo* pItem = NULL;
                        if (equipments != NULL)
                        {
                            for (int i = 0; i < EQUIPTYPE_MAX; ++i)
                            {
                                if(equipments[i].pCfg && equipments[i].pCfg->id == typeId)
                                {  
                                    pItem = &equipments[i];
                                    break;
                                }
                            }
                        }
                        else
                        {
                            pItem = g_LogicManager->m_Hero->m_Bag.FindWearEquipment(typeId);
                        }
                        if (pItem)
                        {
                            ++hasCount;
                            name->setColor(Color3B::YELLOW);
                        }
                        else
                        {
                            name->setColor(Color3B::GRAY);
                        }
                        ++SetCount;
                    }
                    else
                    {
                        Text* name = static_cast<Text*>(m_EquipSetPanel->getChildByName(equipSetName[i]));
                        if (name)
                        {
                            name->setVisible(false);
                        }
                    }
                }

                int ctrl = SetCount / 2;
                for (int i = 0; i < 4; ++i)
                {
                    Text* name = static_cast<Text*>(m_EquipSetPanel->getChildByName(equipAttrName[i]));
                    if (name)
                    {
                        //>如果装出现不足8的数量的情况下,需要隐藏
                        if (i > ctrl){
                            name->setVisible(false);
                        }
                        else
                        {
                            name->setVisible(true);
                            if ((i+1)*2 <= hasCount)
                                name->setColor(Color3B::GREEN);
                            else
                                name->setColor(Color3B::GRAY);
                        }
                    }
                    Text* desc = static_cast<Text*>(m_EquipSetPanel->getChildByName(equipAttrDesc[i]));
                    if (desc)
                    {
                        if (i > ctrl){
                            desc->setVisible(false);
                        }
                        else
                        {
                            desc->setVisible(true);
                            AttributeType aType = (AttributeType)pSuitInfo->iSuitAttribute[i];
                            float aValue = pSuitInfo->fSuitValue[i];
                            string aName = BaseFunction::a2u(g_CSVFileManager->GetAttributeName(aType));
                            aName.append("+");
                            if (g_CSVFileManager->IsPercentAttribute(aType))
                            {
                                float value = aValue / 100.0f;    
                                aName.append(StringUtils::format("%.2f%%", value));
                            }
                            else
                            {
                                aName.append(StringUtils::format("%d", int(aValue)));
                            }

                            desc->setString(aName);
                            if ((i+1)*2 <= hasCount)
                                desc->setColor(Color3B::GREEN);
                            else
                                desc->setColor(Color3B::GRAY);
                        }
                    }
                }
            }

            //>运行动画
            auto action = CSLoader::createTimeline("UI/ItemInfo.csb");
            m_RootPanel->runAction(action);
            action->gotoFrameAndPlay(0, 10, false);
        }
        else
        {
            //>运行动画
            m_EquipSetPanel->setVisible(false);
            auto action = CSLoader::createTimeline("UI/ItemInfo.csb");
            m_RootPanel->runAction(action);
            action->gotoFrameAndPause(0);
        }

        //>设置部位说明
        SetPart(pItem->pCfg->iSamllClass);

        //>设置绑定说明
        SetBind(pItem->Data.isBind);

        //>设置战斗力说明
        SetBattlePower(pItem->GetBattlePower());

        //>显示属性
        for (int i = 0; i < 8; ++i)
        {
            bool notData = false;
            AttributeType aType;
            float aValue;
            if (i == 0 && pPrefix != NULL)
            {
                aType = (AttributeType)pPrefix->iAddAttribute;
                aValue = pPrefix->iAddValue;
            }else
            {
                //> 如果有前缀的情况下.
                if (i - 1 < 4)
                {
                    aType = (AttributeType)pItem->pCfg->iBaseAtt[i - 1];
                    aValue = pItem->pCfg->iBaseAttParams[i - 1];
                    if (aType == ATTRIBUTE_MONEY && IsFloatZero(aValue))
                    {
                        notData = true;
                    }
                }else
                {
                    notData = true;
                }
            }

            if (!notData)
            {
                const string& aName = g_CSVFileManager->GetAttributeName(aType);
                m_AttributeNames[i]->setVisible(true);
                m_AttributeNames[i]->setString(BaseFunction::a2u(aName));

                string baseValueStr("+");
                if (g_CSVFileManager->IsPercentAttribute(aType))
                {
                    float value = aValue / 100.0f;    
                    baseValueStr.append("%.2f%%", value);
                }
                else
                {
                    baseValueStr.append(StringUtils::format("%d", int(aValue)));
                }

                if (pItem->Data.starNum > 0)
                {   
                    m_AttributeValues1[i]->setVisible(true);
                    m_AttributeValues1[i]->setString(baseValueStr);

                    if (i == 0 && pPrefix != NULL)
                    {
                        m_AttributeValues2[i]->setVisible(false);
                    }else
                    {
                        int upStarValue = g_CSVFileManager->CalcUpStarEffectBaseAttribute(aType, aValue, pItem->Data.starNum);
                        m_AttributeValues2[i]->setVisible(true);
                        m_AttributeValues2[i]->setColor(Color3B::GREEN);
                        m_AttributeValues2[i]->setString(StringUtils::format("(+%d)", upStarValue));
                    }
                }
                else
                {
                    m_AttributeValues1[i]->setVisible(false);

                    m_AttributeValues2[i]->setVisible(true);
                    m_AttributeValues2[i]->setColor(Color3B::WHITE);
                    m_AttributeValues2[i]->setString(baseValueStr);
                }
            }
            else
            {
                m_AttributeNames[i]->setVisible(false);
                m_AttributeValues1[i]->setVisible(false);
                m_AttributeValues2[i]->setVisible(false);
            }
        }

        //CCASSERT(showOperation.size() > 0, "");
        CCASSERT(showOperation.size() <= m_EquipmentButtons.size(), "");

        int idx = 0;
        int size = showOperation.size();
        vector<Button*>::iterator it = m_EquipmentButtons.begin();
        for (vector<Button*>::iterator end = m_EquipmentButtons.end();
             it != end; ++it, ++idx)
        {
            Button* pBtn = *it;
            if (idx < size)
            {
                pBtn->setVisible(true);
                const ButtonOperation& oper = showOperation[idx];
                pBtn->setTag(oper.operation);
                if (oper.textId > 0)
                {
                    const TextInfo* pText = g_CSVFileManager->GetTextByID(oper.textId);
                    if (pText)
                    {
                        pBtn->setTitleText(BaseFunction::a2u(pText->strContent));
                    }
                }

                if (oper.status == 1)
                {
                    pBtn->setEnabled(false);
                    pBtn->setColor(Color3B(64,64,64));
                }
                else
                {
                    pBtn->setEnabled(true);
                    pBtn->setColor(Color3B::WHITE);
                }
            }
            else
            {
                pBtn->setVisible(false);
            }
        }
    }
    else
    {
        //>物品类
        Text* name = static_cast<Text*>(m_ItemPanel->getChildByName("Text_Name"));
        if (name)
        {
            name->setString(BaseFunction::a2u(pItem->pCfg->strName));
            name->setColor(pItem->GetQualityColor());
        }

        ImageView* pIcon = static_cast<ImageView*>(m_ItemPanel->getChildByName("Image_ItemIcon"));
        if (pIcon && !pItem->pCfg->strIcon.empty())
        {
            pIcon->loadTexture(pItem->pCfg->strIcon);
        }
        ImageView* pSprQuality = static_cast<ImageView*>(m_ItemPanel->getChildByName("Image_Empty"));
        if (pSprQuality)
        {
            pSprQuality->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);
        }

        Text* limitLevel = static_cast<Text*>(m_ItemPanel->getChildByName("Text_level_nb"));
        if (limitLevel)
        {
            limitLevel->setString(StringUtils::format("%d", pItem->pCfg->iLevelLimit));
        }

        Text* hasNumber = static_cast<Text*>(m_ItemPanel->getChildByName("Text_count_nb"));
        if (hasNumber)
        {
            hasNumber->setString(StringUtils::format("%d", pItem->Data.num));
        }

        Text* describe = static_cast<Text*>(m_ItemPanel->getChildByName("Text_content"));
        if (describe)
        {
            describe->setString(BaseFunction::a2u(pItem->pCfg->strDes));
        }

        int idx = 0;
        int size = showOperation.size();
        vector<Button*>::iterator it = m_ItemButtons.begin();
        for (vector<Button*>::iterator end = m_ItemButtons.end();
            it != end; ++it, ++idx)
        {
            Button* pBtn = *it;
            if (idx < size)
            {
                pBtn->setVisible(true);
                const ButtonOperation& oper = showOperation[idx];
                pBtn->setTag(oper.operation);
                if (oper.textId > 0)
                {
                    const TextInfo* pText = g_CSVFileManager->GetTextByID(oper.textId);
                    if (pText)
                    {
                        pBtn->setTitleText(BaseFunction::a2u(pText->strContent));
                    }
                }

                if (oper.status == 1)
                {
                    pBtn->setEnabled(false);
                    pBtn->setColor(Color3B(64,64,64));
                }
                else
                {
                    pBtn->setEnabled(true);
                    pBtn->setColor(Color3B::WHITE);
                }
            }
            else
            {
                pBtn->setVisible(false);
            }
        }

        //>设置绑定说明
        SetBind(pItem->Data.isBind);

        m_ItemPanel->setVisible(true);
        m_EquipmentPanel->setVisible(false);
        m_EquipSetPanel->setVisible(false);
    }

    //>拷贝选项
    m_ShowOperation.resize(showOperation.size());
    std::copy(showOperation.begin(), showOperation.end(), m_ShowOperation.begin());

    //>设置金钱
    SetMoney(pItem->pCfg->iPicre * pItem->Data.num);

    //设置数量
    SetCount(pItem->Data.num);

    m_itemGuid = pItem->Data.guid;
}

void UIItemInfo::SetMoney(int number)
{
    if (m_ItemPanel)
    {
        Text* moneyNb = static_cast<Text*>(m_ItemPanel->getChildByName("Text_monry_nb"));
        if (moneyNb)
        {
            moneyNb->setString(StringUtils::format("%d", number));
        }
    }

    if (m_EquipmentPanel)
    {
        Text* moneyNb = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_monry_nb"));
        if (moneyNb)
        {
            moneyNb->setString(StringUtils::format("%d", number));
        }
    }
}

void UIItemInfo::SetCount(int number)
{
    if (m_ItemPanel)
    {
        Text* moneyNb = static_cast<Text*>(m_ItemPanel->getChildByName("Text_count_nb"));
        if (moneyNb)
        {
            moneyNb->setString(StringUtils::format("%d", number));
        }
    }

    if (m_EquipmentPanel)
    {
        Text* moneyNb = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_count_nb"));
        if (moneyNb)
        {
            moneyNb->setString(StringUtils::format("%d", number));
        }
    }
}

void UIItemInfo::OnEquip()
{
    MsgItem_Use wearMsg;
    Bag& bag = g_LogicManager->m_Hero->m_Bag;
    wearMsg.useItemGuid = m_itemGuid;
    if (NULL != bag.GetItemByGuid(m_itemGuid, false))
    {
        wearMsg.operateType = ItemOperate_WearEquipment;    //>在背包里,穿上装备
    }else
    {
        wearMsg.operateType = ItemOperate_EquipGetOff;  //>不在背包里,脱下装备
    }

    g_NetManager->SendMsg(wearMsg);
    g_UIManager->HideUI(UI_ITEM_INFO);
}

void UIItemInfo::OnUpStar()
{
    g_UIManager->OpenUI(UI_EQUIP_UPGRADE);
    UIUpgradeEquip* pUI = (UIUpgradeEquip*)g_UIManager->GetUI(UI_EQUIP_UPGRADE);
    if (pUI)
    {
        pUI->SetDragInGuid(m_itemGuid);
        pUI->SetPage(UIUpgradeEquip::SP_UpStar);
        g_UIManager->HideUI(UI_ITEM_INFO);
        g_UIManager->HideUI(UI_BAG);
        g_UIManager->HideUI(UI_PLAYERINFO);
    }
}

void UIItemInfo::OnCompose()
{
    g_UIManager->OpenUI(UI_EQUIP_UPGRADE);
    UIUpgradeEquip* pUI = (UIUpgradeEquip*)g_UIManager->GetUI(UI_EQUIP_UPGRADE);
    if (pUI)
    {
        pUI->SetDragInGuid(m_itemGuid);
        pUI->SetPage(UIUpgradeEquip::SP_Compose);
        g_UIManager->HideUI(UI_ITEM_INFO);
        g_UIManager->HideUI(UI_BAG);
        g_UIManager->HideUI(UI_PLAYERINFO);
    }
}

void UIItemInfo::OnSale()
{

}

void UIItemInfo::OnSell()
{
    if (m_itemGuid)
    {
        CliItemInfo* pItem = g_LogicManager->m_Hero->m_Bag.GetItemByGuid(m_itemGuid);
        if (pItem)
        {
            MsgItem_ShopSell sellMsg;
            sellMsg.itemGuid = m_itemGuid;
            sellMsg.number = pItem->Data.num;
            g_NetManager->SendMsg(sellMsg);

            g_UIManager->OpenUI(UI_WAITTING);
            UIWaitting* pUiWait= (UIWaitting*)g_UIManager->GetUI(UI_WAITTING);
            if (pUiWait)
                pUiWait->SetTextID(61);
        }
    }
}

void UIItemInfo::OnLock()
{

}

void UIItemInfo::OnUse()
{
    if (m_itemGuid)
    {
        CliItemInfo* pItem = g_LogicManager->m_Hero->m_Bag.GetItemByGuid(m_itemGuid);
        if (pItem)
        {
            if (pItem->pCfg->iBigClass == ITEMTYPE_MATERIAL && 
                pItem->pCfg->iSamllClass == MATERIALTYPE_STARM)    //>升星石使用,跳到升星界面
            {
                OnUpStar();
                return;
            }
           
            if (pItem->pCfg->bIsUse == false)
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(655);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //该物品不能使用.
                return;
            }

            if (pItem->pCfg->iBigClass == ITEMTYPE_EXPEND &&
                pItem->pCfg->iSamllClass == EXPENDTYPE_SKILLTREEEXP && 
                g_LogicManager->m_Hero->m_SkillList.empty())
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(656);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //技能树未开启,无法使用技能经验丹
                return;
            }

            if (pItem->pCfg->iBigClass == ITEMTYPE_EXPEND &&
                pItem->pCfg->iSamllClass == EXPENDTYPE_MOMENT)
            {
                if(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_HP] >= g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_HPMAX])
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(657);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //血量已满!
                    return;
                }
            }

            if (pItem->pCfg->iBigClass == ITEMTYPE_EXPEND &&
                pItem->pCfg->iSamllClass == EXPENDTYPE_MOMENTMP)
            {
                if(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MP] >= g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MPMAX])
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(660);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::MAGENTA); //魔法值已满!
                    return;
                }
            }

            MsgItem_Use useMsg;
            useMsg.operateType = ItemOperate_ItemUsed;
            useMsg.useItemGuid = m_itemGuid;
            g_NetManager->SendMsg(useMsg);

            //>等待UI
            g_UIManager->OpenUI(UI_WAITTING);
            UIWaitting* pUiWait= (UIWaitting*)g_UIManager->GetUI(UI_WAITTING);
            if (pUiWait)
                pUiWait->SetTextID(61);
        }
    }
}

void UIItemInfo::DefaulClickedOperation(Operation operation, int32 status)
{
    switch (operation)
    {
    case UIItemInfo::Equip:
        OnEquip();
        break;
    case UIItemInfo::UpStar:
        OnUpStar();
        break;
    case UIItemInfo::Compose:
        OnCompose();
        break;
    case UIItemInfo::Sales:
        OnSale();
        break;
    case UIItemInfo::Sell:
        OnSell();
        break;
    case UIItemInfo::Lock:
        OnLock();
        break;
    case UIItemInfo::Use:
        OnUse();
        break;
    default:
        break;
    }
}

void UIItemInfo::SetStar(uint8 starNumber)
{
    m_StarPanel->setVisible(false);
    //if (starNumber > 0)
    {
        m_StarPanel->setVisible(true);
        //>哪个星级
        uint8 kind = starNumber / s_ShowLevel;
        uint8 index = starNumber % s_ShowLevel;

        if (kind + 1 > s_StarKind)
        {
            log("kind[%d] must be lower than UIItemInfo::s_StarKind number[%d]", kind, s_StarKind);
            return;
        }

        for (int i = index; i < s_ShowLevel; ++i)
        {
            m_StarImage[i]->loadTexture(kindOfStarPaths[kind]);
        }

        for(int j = 0; j < index; ++j)
        {
            m_StarImage[j]->loadTexture(kindOfStarPaths[kind + 1]);
        }
    }

    Text* level = static_cast<Text*>(m_EquipmentPanel->getChildByName("Text_Level"));
    if (level)
    {
        level->setVisible(false);
        if (starNumber > 0)
        {
            level->setVisible(true);
            level->setString(StringUtils::format("+%d", starNumber));
        }
    }
}

void UIItemInfo::SetPart(int partType)
{
    EquipType equipType = (EquipType)partType;

    if (m_AttachPart)
    {
        m_AttachPart->setString(BaseFunction::a2u(g_CSVFileManager->GetStrByEquipType(equipType)));
    }
}

void UIItemInfo::SetBind(bool bound)
{
    const TextInfo* textInfo = NULL;
    if (!bound)
    {
        textInfo = g_CSVFileManager->GetTextByID(59);
    }
    else
    {
        textInfo = g_CSVFileManager->GetTextByID(60);
    }

    if (textInfo)
    {
        std::string content = BaseFunction::a2u(textInfo->strContent);
        m_BindTxt->setString(content);
        m_ItemBindTxt->setString(content);
    }
}

void UIItemInfo::SetBattlePower(int number)
{
    if (m_BattlePowerLable)
    {
        m_BattlePowerLable->setString(StringUtils::format("%d", number));
    }
}
