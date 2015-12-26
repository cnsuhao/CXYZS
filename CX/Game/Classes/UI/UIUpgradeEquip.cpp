#include "UIUpgradeEquip.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "Bag.h"
#include "CSVFileManager.h"
#include "LogicManager.h"
#include "UIItemInfo.h"
#include "UIManager.h"
#include "UIWaitting.h"
#include "NetManager.h"
#include "ResManager.h"

using namespace cocostudio;

static const char* childSprNames[UIUpgradeEquip::GV_Column_Max] = { "Image_Grid1", "Image_Grid2", "Image_Grid3", "Image_Grid4", "Image_Grid5" }; 
static const char* qualitySprNames[UIUpgradeEquip::GV_Column_Max] = { "Image_Empty1", "Image_Empty2", "Image_Empty3", "Image_Empty4", "Image_Empty5"};
static const char* equipedSprNames[UIUpgradeEquip::GV_Column_Max] = { "Image_equiped1", "Image_equiped2", "Image_equiped3", "Image_equiped4", "Image_equiped5" };

static const char* equipSprNames[UIUpgradeEquip::Stuff_Number] = { "Image_item5", "Image_item1", "Image_item2", "Image_item3", "Image_item4"};
static const char* composeQualitySprNames[UIUpgradeEquip::Stuff_Number] = { "Sprite_empty5", "Sprite_empty1", "Sprite_empty2", "Sprite_empty3", "Sprite_empty4"};
static const char* starImageNames[UIItemInfo::s_ShowLevel] = { "Image_Star1", "Image_Star2", "Image_Star3", "Image_Star4", "Image_Star5",
                                                               "Image_Star6", "Image_Star7", "Image_Star8", "Image_Star9", "Image_Star10"  };

static const char* attriName[4] = { "Text_AttrName1", "Text_AttrName2", "Text_AttrName3", "Text_AttrName4" };
static const char* attriValue[4] = { "Text_AttrValue1", "Text_AttrValue2", "Text_AttrValue3", "Text_AttrValue4" };
static const char* attriValueAdd[4] = { "Text_AttrValueAdd1", "Text_AttrValueAdd2", "Text_AttrValueAdd3", "Text_AttrValueAdd4" };
static const char* kindOfStarPaths[UIItemInfo::s_StarKind] = { "UI/ItemInfo/item_info_star1.png", "UI/ItemInfo/item_info_star2.png", "UI/ItemInfo/item_info_star3.png" };


UIUpgradeEquip::UIUpgradeEquip()
{
    _buttons[0] = _buttons[1] = NULL;
    for (int i = 0; i < SP_Number; ++i)
    {
        _cboxArray[i] = NULL;
        m_Panel[i] = NULL;
    }
    
    _txtOwnGold = NULL;
    _txtSpendGold = NULL;
    _txtComposeMoney = NULL;

    _textYuanBao = NULL;
    _textGold = NULL;
    _textMoney = NULL;

    for (int i = 0; i < Stuff_Number; ++i)
    {
        _stuffSolt[i] = 0;
        _stuffImgs[i] = NULL;
    }

    m_ComposeItems.resize(Stuff_Number);

    _upStarEquName = NULL;
    _upStarEquStarNB = NULL;
    _upStarSpendMoney = NULL;
    _upStarHasStuff = NULL;
    _upstarEquipment.item = NULL;

    _InheritSrcIcon = NULL;
    _InheritSrcQuality = NULL;
    _InheritTargetIcon = NULL;
    _InheritTargetQuality = NULL;
    _InheritSpendGold = NULL;
    _InheritSrcItem.item = NULL;
    _InheritTagItem.item = NULL;

    _dragInGuid = INVALID_ID;
}

UIUpgradeEquip::~UIUpgradeEquip()
{
}

bool UIUpgradeEquip::init()
{
bool bRet = false;
    do 
    {
        //加载角色属性面板
        Node* panel = CSLoader::createNode("UI/Strengthen.csb");
        CCASSERT(panel,"");
        if (panel == NULL){
            return false;
        }
        addChild(panel);


        _cboxArray[SP_Compose] = static_cast<CheckBox*>(panel->getChildByName("CheckBox_compose"));
        _cboxArray[SP_UpStar] = static_cast<CheckBox*>(panel->getChildByName("CheckBox_shengxing"));
        _cboxArray[SP_Inherit] = static_cast<CheckBox*>(panel->getChildByName("CheckBox_inherit"));

        CCASSERT(_cboxArray[SP_Compose] && _cboxArray[SP_UpStar] && _cboxArray[SP_Inherit], "");

        CheckBox::ccCheckBoxCallback checkCallback = [this](Ref* _sender,CheckBox::EventType type)
        {
            int size = sizeof(_cboxArray)/sizeof(_cboxArray[0]);
            CheckBox* cbox = static_cast<CheckBox*>(_sender);
            switch (type)
            {
            case cocos2d::ui::CheckBox::EventType::SELECTED:
            case cocos2d::ui::CheckBox::EventType::UNSELECTED:
                for (int i = 0; i < size; ++i)
                {
                    if (_cboxArray[i] == cbox)
                    {
                        SetPage(i);
                        break;
                    }
                }
                break;
            default:
                break;
            }
        };
        for (int i = 0; i < SP_Number; ++i)
        {
            _cboxArray[i]->addEventListener(checkCallback);
        }
        _textYuanBao = static_cast<Text*>(panel->getChildByName("Text_Yuanbao"));
        _textGold = static_cast<Text*>(panel->getChildByName("Text_Gold"));
        _textMoney = static_cast<Text*>(panel->getChildByName("Text_Money"));

        Sprite* pIcon = static_cast<Sprite*>(panel->getChildByName("Sprite_17"));
        if (pIcon)
            pIcon->setTexture(GetMoneyIcon(ATTRIBUTE_MONEY));

        pIcon = static_cast<Sprite*>(panel->getChildByName("Sprite_18"));
        if (pIcon)
            pIcon->setTexture(GetMoneyIcon(ATTRIBUTE_BINDGOLD));

        pIcon = static_cast<Sprite*>(panel->getChildByName("Sprite_19"));
        if (pIcon)
            pIcon->setTexture(GetMoneyIcon(ATTRIBUTE_GOLD));
        

        m_Panel[SP_Compose] = static_cast<Layout*>(panel->getChildByName("Panel_Compose"));
        if (m_Panel[SP_Compose])
        {
            _txtSpendGold = static_cast<Text*>(m_Panel[SP_Compose]->getChildByName("Text_xiaohao"));
            _txtSpendGold->setString("0");

            for (int i = 0; i < Stuff_Number; ++i)
            {
                _stuffImgs[i] = static_cast<ImageView*>(m_Panel[SP_Compose]->getChildByName(equipSprNames[i]));
                if (_stuffImgs[i])
                {
                    _stuffImgs[i]->setTag(i);   //> set index.
                    _stuffImgs[i]->setVisible(false);
                    _stuffImgs[i]->setTouchEnabled(true);
                    _stuffImgs[i]->addClickEventListener([this](Ref* _sender)
                    {
                        int idx = static_cast<ImageView*>(_sender)->getTag(); //> get index.
                        OnCancelStuffSolt(idx);
                    });
                }
                _stuffQuality[i] = static_cast<Sprite*>(m_Panel[SP_Compose]->getChildByName(composeQualitySprNames[i]));

            }

            assert(_stuffImgs[MainStuff] && _stuffImgs[Stuff1] && _stuffImgs[Stuff2] && _stuffImgs[Stuff3] && _stuffImgs[Stuff4]);
            assert(_stuffQuality[MainStuff] && _stuffQuality[Stuff1] && _stuffQuality[Stuff2] && _stuffQuality[Stuff3] && _stuffQuality[Stuff4]);
        }
        
        m_Panel[SP_UpStar] = static_cast<Layout*>(panel->getChildByName("Panel_UpStar"));
        if (m_Panel[SP_UpStar])
        {
            _upStarIcon = static_cast<ImageView*>(m_Panel[SP_UpStar]->getChildByName("Image_ItemIcon"));
            _upStarIcon->setTouchEnabled(true);
            _upStarIcon->addClickEventListener([this](Ref* _sender)
            {
                OnCancelUpStar();
            });
            _upStarQuality = static_cast<ImageView*>(m_Panel[SP_UpStar]->getChildByName("Image_Empty"));

            for (int i = 0; i < UIItemInfo::s_ShowLevel; i++)
            {
                _starImage[i] = static_cast<ImageView*>(m_Panel[SP_UpStar]->getChildByName(starImageNames[i]));
            }

            for (int i = 0; i < 4; ++i)
            {
                _attributeName[i] = static_cast<Text*>(m_Panel[SP_UpStar]->getChildByName(attriName[i]));
                _attributeValue[i] = static_cast<Text*>(m_Panel[SP_UpStar]->getChildByName(attriValue[i]));
                _attributeAddValue[i] = static_cast<Text*>(m_Panel[SP_UpStar]->getChildByName(attriValueAdd[i]));
            }

            _upStarEquName = static_cast<Text*>(m_Panel[SP_UpStar]->getChildByName("Text_Name"));
            _upStarEquStarNB = static_cast<Text*>(m_Panel[SP_UpStar]->getChildByName("Text_StarNum"));
            _upStarSpendMoney = static_cast<Text*>(m_Panel[SP_UpStar]->getChildByName("Text_spend_nb"));
            _upStarHasStuff = static_cast<Text*>(m_Panel[SP_UpStar]->getChildByName("Text_stuff_nb"));

            CCASSERT(_upStarIcon && _upStarQuality && _starImage[0] && _attributeName[0] && _attributeValue[0] && _attributeAddValue &&
                     _upStarEquName && _upStarEquStarNB && _upStarSpendMoney && _upStarHasStuff, "");
        }

        m_Panel[SP_Inherit] = static_cast<Layout*>(panel->getChildByName("Panel_Inherit"));
        if (m_Panel[SP_Inherit])
        {
            _InheritSrcIcon = static_cast<ImageView*>(m_Panel[SP_Inherit]->getChildByName("Image_Icon1"));
            _InheritSrcQuality = static_cast<ImageView*>(m_Panel[SP_Inherit]->getChildByName("Image_Empty1"));
            _InheritTargetIcon = static_cast<ImageView*>(m_Panel[SP_Inherit]->getChildByName("Image_Icon2"));
            _InheritTargetQuality = static_cast<ImageView*>(m_Panel[SP_Inherit]->getChildByName("Image_Empty2"));
            _InheritSpendGold = static_cast<Text*>(m_Panel[SP_Inherit]->getChildByName("Text_SpendMoney"));
            _InheritStarTet[0] = static_cast<Text*>(m_Panel[SP_Inherit]->getChildByName("Text_StarNum1"));
            _InheritStarTet[1] = static_cast<Text*>(m_Panel[SP_Inherit]->getChildByName("Text_StarNum2"));

            CCASSERT(_InheritSrcIcon && _InheritSrcQuality && _InheritTargetIcon && _InheritTargetIcon && _InheritTargetQuality && _InheritSpendGold && _InheritStarTet[0] && _InheritStarTet[1], "");

            _InheritSrcQuality->setTouchEnabled(true);
            _InheritSrcQuality->addClickEventListener([this](Ref* _sender)
            {
                ClearInheritSource();
            });

            _InheritTargetQuality->setTouchEnabled(true);
            _InheritTargetQuality->addClickEventListener([this](Ref* _sender)
            {
                ClearInheritTarget();
            });

            _InheritStarTet[0]->setString("0");
            _InheritStarTet[1]->setString("0");
        }

        Button* btn = static_cast<Button*>(panel->getChildByName("Button_Close"));
        if (btn)
        {
            btn->addClickEventListener([this](Ref* _sender)
            {
                g_UIManager->HideUI(UI_EQUIP_UPGRADE);
            });
        }
        _buttons[0] = static_cast<Button*>(panel->getChildByName("Button_config"));
        if (_buttons)
        {
            _buttons[0]->addClickEventListener([this](Ref* _sender)
            {
                if (_pageIdx == SP_Compose)
                {
                    OnOneKey();
                }
                else if (_pageIdx == SP_UpStar)
                {
                    OnBuyStuff();
                }
            });
        }
        _buttons[1] = static_cast<Button*>(panel->getChildByName("Button_compose"));
        if (_buttons[1])
        {
            _buttons[1]->addClickEventListener([this](Ref* _sender)
            {
                if (_pageIdx == SP_Compose)
                {
                    OnCompose();
                }
                else if (_pageIdx == SP_UpStar)
                {
                    OnUpStar();
                }
            });
        }
        _buttons[2] = static_cast<Button*>(panel->getChildByName("Button_inherit"));
        if (_buttons[2])
        {
            _buttons[2]->addClickEventListener([this](Ref* _sender)
            {
                OnInherit();
            });
        }


        cc2dui::ScrollView* tempScrollView = static_cast<cc2dui::ScrollView*>(panel->getChildByName("ScrollView"));
        CCASSERT(tempScrollView, "");
        tempScrollView->setEnabled(false);
        Widget* panelGrids = static_cast<Widget*>(panel->getChildByName("Panel_Grids"));
        CCASSERT(panelGrids, "");
        if(InitGrid(tempScrollView->getContentSize(), tempScrollView->getPosition(), panelGrids))
        {
            addChild((Node*)_scrollView);
        }
        else
        {
            CCASSERT(0,"");
            return false;
        }

        bRet = true;
    } while (0);

    return bRet;
}

void UIUpgradeEquip::OnShow()
{
    UIBase::OnShow();

    SetPage(SP_Compose);

    ReloadHeroMoney();
}

void UIUpgradeEquip::SetPage(int page)
{
    if (page >= (int)SP_Compose && page < (int)SP_Number)
    {
        //if (_page == page)
        //{
        //    return;
        //}

        _pageIdx = (SelectPage)page;
        for (int i = 0; i < SP_Number; ++i)
        {
            _cboxArray[i]->setSelected(false);
            m_Panel[i]->setVisible(false);
        }
        _cboxArray[_pageIdx]->setSelected(true);
        m_Panel[_pageIdx]->setVisible(true);

        //if (_pages->IsNeedReload()) //占时总是重新加载
        {

            std::vector<CliItemInfo*> items;
            switch (_pageIdx)
            {
            case UIUpgradeEquip::SP_Compose:{
                g_LogicManager->m_Hero->m_Bag.GetCanComposeItems(items);
                _buttons[0]->setVisible(true);
                _buttons[0]->setTitleText(BaseFunction::a2u(g_CSVFileManager->GetTextByID(602)->strContent));
                _buttons[1]->setVisible(true);
                _buttons[1]->setTitleText(BaseFunction::a2u(g_CSVFileManager->GetTextByID(603)->strContent));
                _buttons[2]->setVisible(false);
                }break;
            case UIUpgradeEquip::SP_UpStar:{
                g_LogicManager->m_Hero->m_Bag.GetCanStarItems(items);
                _buttons[0]->setVisible(true);
                _buttons[0]->setTitleText(BaseFunction::a2u(g_CSVFileManager->GetTextByID(605)->strContent));
                _buttons[1]->setVisible(true);
                _buttons[1]->setTitleText(BaseFunction::a2u(g_CSVFileManager->GetTextByID(604)->strContent));
                _buttons[2]->setVisible(false);
                }break;
            case UIUpgradeEquip::SP_Inherit:{
                g_LogicManager->m_Hero->m_Bag.GetCanInheritItems(items);
                _buttons[0]->setVisible(false);
                _buttons[1]->setVisible(false);
                _buttons[2]->setVisible(true);
                }break;
            case UIUpgradeEquip::SP_Number:
            default:
                break;
            }

            int row = max((int)5, int((items.size() / GV_Column_Max) + 1));
            _pages[_pageIdx].Reload(row, GV_Column_Max, items);

            ClearRow();
            for (int i = 0; i < row - 1; ++i)
            {
                AddLockedRow();
                UnlockNextRow();
            }
            AddLockedRow();
            UnlockNextRow(true);

            //>更新格子显示
            UpdataView();

            if (_pageIdx == SP_Compose)
            {
                ClearStuffSolt();
            }

            if (_pageIdx == SP_UpStar)
            {
                ClearUpStarEquipment();
            }

            if (_pageIdx == SP_Inherit)
            {
                ClearInheritEquipment();
            }

            if (_dragInGuid != INVALID_ID)
            {
                ProcessDragInGuid(_dragInGuid);
                _dragInGuid = INVALID_ID;
            }
        }
    }
    else
    {
        log("UIUpgradeEquip::SetPage error page index.\n");
    }
}

void UIUpgradeEquip::ReloadHeroMoney()
{
    //>设置元宝数
    SetMoney1(uint32(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY]));
    //>设置金币
    SetMoney2(uint32(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_BINDGOLD]));
    //>设置
    SetMoney3(uint32(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_GOLD]));
}

void UIUpgradeEquip::SetMoney1(unsigned int count)
{
    if (_textYuanBao)
    {
        _textYuanBao->setString(StringUtils::format("%d", count));
    }
}

void UIUpgradeEquip::SetMoney2(unsigned int count)
{
    if (_textGold)
    {
        _textGold->setString(StringUtils::format("%d", count));
    }
}

void UIUpgradeEquip::SetMoney3(unsigned int count)
{
    if (_textMoney)
    {
        _textMoney->setString(StringUtils::format("%d", count));
    }
}

void UIUpgradeEquip::SetOwnGold(unsigned int number)
{
    if (_txtOwnGold)
    {
        _txtOwnGold->setString(StringUtils::format("%d", number));
    }
}

void UIUpgradeEquip::SetSpendGold(unsigned int number)
{
    if (_txtSpendGold)
    {
        _txtSpendGold->setString(StringUtils::format("%d", number));
    }
}

void UIUpgradeEquip::SetComposeMoney(unsigned int a, unsigned int b)
{
    if (_txtComposeMoney)
    {
        if (a > b)
        {
            _txtComposeMoney->setColor(Color3B::GREEN);
        }
        else
        {
            _txtComposeMoney->setColor(Color3B::RED);
        }
        _txtComposeMoney->setString(StringUtils::format("%d/%d", a, b));
    }
}

void UIUpgradeEquip::OnOneKey()
{

}

void UIUpgradeEquip::OnCompose()
{
    for (int i = MainStuff; i < Stuff_Number; ++i)
    {
        if (_stuffSolt[i] == 0)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(635);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //尚未满足合成条件.
            return;
        }
    }

    int hasMoney = int(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY]);
    int spendMoney = m_ComposeItems[MainStuff].item->GetComposeSpendMoney();
    if (spendMoney > hasMoney)
    {
        const TextInfo *pText = g_CSVFileManager->GetTextByID(625);
        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //货币不足.
        return;
    }

    MsgItem_Compose composeMsg;
    composeMsg.mainStuffGuid = m_ComposeItems[MainStuff].item->Data.guid;
    composeMsg.Stuff1Guid = m_ComposeItems[Stuff1].item->Data.guid;
    composeMsg.Stuff2Guid = m_ComposeItems[Stuff2].item->Data.guid;
    composeMsg.Stuff3Guid = m_ComposeItems[Stuff3].item->Data.guid;
    composeMsg.Stuff4Guid = m_ComposeItems[Stuff4].item->Data.guid;

    g_NetManager->SendMsg(composeMsg);
    //>等待UI
    g_UIManager->OpenUI(UI_WAITTING);
    UIWaitting* pUiWait= (UIWaitting*)g_UIManager->GetUI(UI_WAITTING);
    if (pUiWait)
        pUiWait->SetTextID(61);
}

void UIUpgradeEquip::OnBuyStuff()
{
    g_UIManager->OpenUI(UI_SHOP);
    g_UIManager->HideUI(UI_EQUIP_UPGRADE);
}

void UIUpgradeEquip::OnInherit()
{
    if (_InheritSrcItem.item == NULL || _InheritTagItem.item == NULL)
    {
        return;
    }

    if (_InheritSrcItem.item->Data.starNum <= _InheritTagItem.item->Data.starNum)
    {
        const TextInfo *pText = g_CSVFileManager->GetTextByID(636);
        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //传承装备星级必需高于继承装备.
        return;
    }

    //int spendGold = int(max(_InheritSrcItem.item->Data.starNum - int(g_CSVFileManager->GetRatioByID(54)->fValue), 0) * g_CSVFileManager->GetRatioByID(55)->fValue);
    //if (spendGold > hasGold)
    //{
    //    g_UIManager->Notify(BaseFunction::a2u("合成元宝不足."), Color3B::RED);
    //    return;
    //}

    //if (_InheritSrcItem.item->Data.isBind && !_InheritTagItem.item->Data.isBind)
    //{
    //    g_UIManager->Notify(BaseFunction::a2u("传承装备为绑定的继承装备也将转换为绑定的."), Color3B::RED);
    //    return;
    //}

    MsgItem_Inherit inheritMsg;
    inheritMsg.sourceEquipmentGuid = _InheritSrcItem.item->Data.guid;
    inheritMsg.targetEquipmentGuid = _InheritTagItem.item->Data.guid;
    g_NetManager->SendMsg(inheritMsg);

    //>等待UI
    g_UIManager->OpenUI(UI_WAITTING);
    UIWaitting* pUiWait= (UIWaitting*)g_UIManager->GetUI(UI_WAITTING);
    if (pUiWait)
        pUiWait->SetTextID(61);
}

void UIUpgradeEquip::OnSelectItem(unsigned int rowIndex, unsigned int columnIndex)
{
    CliItemInfo* pItem = _pages[_pageIdx].GetItem(rowIndex, columnIndex);
    if (pItem != NULL)
    {
        if (_pageIdx == SP_Compose)
        {
            bool hasMainStuff = false;
            int32 mainStuffQuality = 0;
            if (_stuffSolt[MainStuff] != 0)
            {
                if (g_LogicManager->m_Hero->m_Bag.FindWearEquipmentByGuid(pItem->Data.guid))
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(638);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //已装备物品只能做主材料.
                    return;
                }
                hasMainStuff = true;
                mainStuffQuality = m_ComposeItems[MainStuff].item->pCfg->iQuality;
            }

            if (!hasMainStuff)
            {
                if (pItem->pCfg->iCompositeID == 0)
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(639);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //此装备不能合成.
                    return;
                }

                if (pItem->pCfg->iSuitID != 0)
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(640);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //套装不能作为主材料合成.
                    return;
                }
            }

            for (int i = MainStuff; i < Stuff_Number; ++i)
            {
                if (_stuffSolt[i] == 0)
                {
                    if (hasMainStuff && mainStuffQuality != pItem->pCfg->iQuality)
                    {
                        const TextInfo *pText = g_CSVFileManager->GetTextByID(670);
                        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //合成材料必须要和主材料同一品质.
                        return;
                    }

                    if (i == MainStuff)
                    {
                        _txtSpendGold->setString(StringUtils::format("%d", pItem->GetComposeSpendMoney()));
                    }

                    _stuffSolt[i] = 1;
                    _stuffImgs[i]->setVisible(true);
                    _stuffImgs[i]->loadTexture(pItem->pCfg->strIcon);
                    _stuffQuality[i]->setTexture(ItemQualityTextures[pItem->pCfg->iQuality]);

                    //>套装特效代码 (注解掉,套装不能合成)
                    if (pItem->pCfg->iSuitID != 0)
                    {
                        const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
                        if (pInfo)
                        {
                            Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, _stuffQuality[i]->getAnchorPointInPoints());
                            _stuffQuality[i]->removeAllChildren();
                            _stuffQuality[i]->addChild(sprite);
                        }
                    }
                    else
                    {
                        _stuffQuality[i]->removeAllChildren();
                    }

                    m_ComposeItems[i].rowIdx = rowIndex;
                    m_ComposeItems[i].colIdx = columnIndex;
                    m_ComposeItems[i].item = pItem;

                    _pages[SP_Compose].Remove(rowIndex, columnIndex);
                    UpdataView();
                    return;
                }
            }

            const TextInfo *pText = g_CSVFileManager->GetTextByID(671);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //合成材料已满.
        }
        else if(_pageIdx == SP_UpStar)
        {
            if (pItem->IsMaxUpStarLevel())
            {
                const TextInfo *pText = g_CSVFileManager->GetTextByID(671);
                g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //装备已是最高星级.
                return;
            }

            ClearUpStarEquipment();

            _upstarEquipment.rowIdx = rowIndex;
            _upstarEquipment.colIdx = columnIndex;
            _upstarEquipment.item = pItem;

            SetStar(pItem->Data.starNum);
            //>设置名字
            std::string nameText;
            const EquipmentPrefix* pPrefix = g_CSVFileManager->GetEquipmentPrefixByID(pItem->Data.prefixID);
            if (pPrefix)
                nameText.append(BaseFunction::a2u(pPrefix->strPrefixName));

            nameText.append(BaseFunction::a2u(pItem->pCfg->strName));
            _upStarEquName->setVisible(true);
            _upStarEquName->setString(nameText);
            _upStarEquName->setColor(pItem->GetQualityColor());
            //>设置星级
            if (pItem->Data.starNum > 0)
            {
                _upStarEquStarNB->setVisible(true);
                _upStarEquStarNB->setString(StringUtils::format("+%d", pItem->Data.starNum));
            }
            else
            {
                _upStarEquStarNB->setVisible(false);
            }

            //>设置图片
            _upStarIcon->setVisible(true);
            _upStarIcon->loadTexture(pItem->pCfg->strIcon);
            _upStarQuality->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);


            //>套装特效代码
            if (pItem->pCfg->iSuitID != 0)
            {
                const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
                if (pInfo)
                {
                    Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, _upStarQuality->getAnchorPointInPoints());
                    _upStarQuality->removeAllChildren();
                    _upStarQuality->addChild(sprite);
                }
            }
            else
            {
                _upStarQuality->removeAllChildren();
            }

            //>设置属性
            for (int i = 0; i < 4; ++i)
            {
                AttributeType aType = (AttributeType)pItem->pCfg->iBaseAtt[i];
                int aValue = int(pItem->pCfg->iBaseAttParams[i]);
                if (aType == ATTRIBUTE_MONEY && IsFloatZero(aValue))
                {
                    _attributeName[i]->setVisible(false);
                    _attributeValue[i]->setVisible(false);
                    _attributeAddValue[i]->setVisible(false);
                }
                else
                {
                    const string& aName = g_CSVFileManager->GetAttributeName(aType);
                    //>名称
                    _attributeName[i]->setVisible(true);
                    _attributeName[i]->setString(BaseFunction::a2u(aName));
                    //>本身数值
                    _attributeValue[i]->setVisible(true);
                    _attributeValue[i]->setString(StringUtils::format("%d", aValue));
                    //>升星数值
                    _attributeAddValue[i]->setVisible(true);
                    _attributeAddValue[i]->setString(StringUtils::format("(+%d)", g_CSVFileManager->CalcUpStarEffectBaseAttribute(aType, aValue, pItem->Data.starNum + 1)));
                }
            }

            const EquipmentUpgrade* pUpgrade = g_CSVFileManager->GetEquipmentUpgradeByID(pItem->Data.starNum + 1);
            int hasStuffCount = g_LogicManager->m_Hero->m_Bag.GetItemCount(pUpgrade->iMaterialid);
            _upStarSpendMoney->setString(StringUtils::format("%d", pUpgrade->iCurrency));
            _upStarHasStuff->setString(StringUtils::format("%d/%d", hasStuffCount,pUpgrade->iMaterialNum));
            if (hasStuffCount < pUpgrade->iMaterialNum)
                _upStarHasStuff->setColor(Color3B::RED);
            else
                _upStarHasStuff->setColor(Color3B::WHITE);

            _pages[SP_UpStar].Remove(rowIndex, columnIndex);
            UpdataView();
        }
        else if(_pageIdx == SP_Inherit)
        {
            if (_InheritSrcItem.item == NULL)
            {
                //>作为传承物
                if (pItem->Data.starNum <= 0)
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(637);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //传承装备必需为升星过的装备!
                    return;
                }

                _InheritSrcItem.rowIdx = rowIndex;
                _InheritSrcItem.colIdx = columnIndex;
                _InheritSrcItem.item = pItem;

                _InheritSrcIcon->setVisible(true);
                _InheritSrcIcon->loadTexture(pItem->pCfg->strIcon);
                _InheritSrcQuality->setVisible(true);
                _InheritSrcQuality->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);

                //>套装特效代码
                if (pItem->pCfg->iSuitID != 0)
                {
                    const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
                    if (pInfo)
                    {
                        Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, _InheritSrcQuality->getAnchorPointInPoints());
                        _InheritSrcQuality->removeAllChildren();
                        _InheritSrcQuality->addChild(sprite);
                    }
                }
                else
                {
                    _InheritSrcQuality->removeAllChildren();
                }

                _InheritStarTet[0]->setString(StringUtils::format("%d", pItem->Data.starNum));

                int spendGold = int(max(pItem->Data.starNum - int(g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_INHERITUPDATE]), 0) * g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_INHERITEXPEND]);
                _InheritSpendGold->setString(StringUtils::format("%d", spendGold));
                int hasGold = g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY];
                if (spendGold <= hasGold)
                {
                    _InheritSpendGold->setColor(Color3B::WHITE);
                }
                else
                {
                    _InheritSpendGold->setColor(Color3B::RED);
                }

                _pages[SP_Inherit].Remove(rowIndex, columnIndex);
                UpdataView();
            }
            else
            {
                //>作为被传承物
                if (_InheritSrcItem.item->Data.starNum <= pItem->Data.starNum)
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(636);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //传承装备星级必需高于继承装备.
                    return;
                }

                if (_InheritTagItem.item)
                {
                    ClearInheritTarget();
                }

                _InheritTagItem.rowIdx = rowIndex;
                _InheritTagItem.colIdx = columnIndex;
                _InheritTagItem.item = pItem;

                _InheritTargetIcon->setVisible(true);
                _InheritTargetIcon->loadTexture(pItem->pCfg->strIcon);
                _InheritTargetQuality->setVisible(true);
                _InheritTargetQuality->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);

                //>套装特效代码
                if (pItem->pCfg->iSuitID != 0)
                {
                    const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
                    if (pInfo)
                    {
                        Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, _InheritTargetQuality->getAnchorPointInPoints());
                        _InheritTargetQuality->removeAllChildren();
                        _InheritTargetQuality->addChild(sprite);
                    }
                }
                else
                {
                    _InheritTargetQuality->removeAllChildren();
                }

                _InheritStarTet[1]->setString(StringUtils::format("%d", pItem->Data.starNum));

                _pages[SP_Inherit].Remove(rowIndex, columnIndex);
                UpdataView();
            }
        }
    }
}

void UIUpgradeEquip::ClearStuffSolt()
{
    for (int i = 0; i < Stuff_Number; ++i)
    {
        _stuffSolt[i] = 0;
        if (_stuffImgs[i])
        {
            _stuffImgs[i]->setVisible(false);
            _stuffQuality[i]->setTexture(EmptyItemTexture);
            _stuffQuality[i]->removeAllChildren();
        }
    }
}

void UIUpgradeEquip::OnCancelStuffSolt(int idx)
{
    if (idx < Stuff_Number &&  _stuffSolt[idx])
    {
        if (idx == MainStuff)
        {
            for (int i = 0; i < Stuff_Number; ++i)
            {
                if (_stuffSolt[i] != 0)
                {
                    _pages[SP_Compose].AddItem(m_ComposeItems[i].item, m_ComposeItems[i].rowIdx, m_ComposeItems[i].colIdx);
                }
            }
            //>设置消耗为0
            _txtSpendGold->setString("0");

            ClearStuffSolt();
            UpdataView();
        }else
        {
            _stuffSolt[idx] = 0;
            _stuffImgs[idx]->setVisible(false);
            _stuffQuality[idx]->setTexture(EmptyItemTexture);
            _stuffQuality[idx]->removeAllChildren();

            _pages[SP_Compose].AddItem(m_ComposeItems[idx].item, m_ComposeItems[idx].rowIdx, m_ComposeItems[idx].colIdx);
            UpdataView();
        }
    }
}

void UIUpgradeEquip::ClearUpStarEquipment()
{
    if (_upstarEquipment.item != NULL)
    {
        _pages[SP_UpStar].AddItem(_upstarEquipment.item, _upstarEquipment.rowIdx, _upstarEquipment.colIdx);
        _upstarEquipment.item = NULL;
        UpdataView();
    }

    SetStar(0);

    _upStarIcon->setVisible(false);
    _upStarQuality->loadTexture(EmptyItemTexture);
    _upStarQuality->removeAllChildren();

    _upStarEquName->setVisible(false);
    _upStarEquStarNB->setVisible(false);
    _upStarSpendMoney->setString("0");
    _upStarHasStuff->setString("0/0");
    
    for (int i = 0; i < 4; ++i)
    {
        _attributeName[i]->setVisible(false);
        _attributeValue[i]->setVisible(false);
        _attributeAddValue[i]->setVisible(false);
    }
}

void UIUpgradeEquip::SetStar(uint8 starNumber)
{
    //>哪个星级
    uint8 kind = starNumber / UIItemInfo::s_ShowLevel;
    uint8 index = starNumber % UIItemInfo::s_ShowLevel;

    if (kind + 1 > UIItemInfo::s_StarKind)
    {
        log("kind[%d] must be lower than UIItemInfo::s_StarKind number[%d]", kind, UIItemInfo::s_StarKind);
        return;
    }

    for (int i = index; i < UIItemInfo::s_ShowLevel; ++i)
    {
        _starImage[i]->loadTexture(kindOfStarPaths[kind]);
    }

    for(int j = 0; j < index; ++j)
    {
        _starImage[j]->loadTexture(kindOfStarPaths[kind + 1]);
    }
}

void UIUpgradeEquip::OnCancelUpStar()
{
    ClearUpStarEquipment();
}

void UIUpgradeEquip::OnUpStarSuccess()
{
    if (_upstarEquipment.item)
    {
        int starNum = _upstarEquipment.item->Data.starNum;
        ClearUpStarEquipment();
        OnSelectItem(_upstarEquipment.rowIdx , _upstarEquipment.colIdx);
        PlayStarAnim(starNum);
    }
}

void UIUpgradeEquip::OnUpStarFailture()
{
    if (_upstarEquipment.item)
    {
        ClearUpStarEquipment();
        OnSelectItem(_upstarEquipment.rowIdx , _upstarEquipment.colIdx);
    }
}

void UIUpgradeEquip::PlayStarAnim(uint8 starNumber)
{
    //>哪个星级
    if (starNumber > 0)
    {
        uint8 index = (starNumber - 1) % UIItemInfo::s_ShowLevel;
        g_ResManager->ShowSprite(30000, _starImage[index]->getParent(), 4, _starImage[index]->getPosition());
    }
}

void UIUpgradeEquip::ClearInheritEquipment()
{
    ClearInheritSource();
    ClearInheritTarget();
}

void UIUpgradeEquip::ClearInheritSource()
{
    if (_InheritSrcItem.item != NULL)
    {
        _pages[SP_Inherit].AddItem(_InheritSrcItem.item, _InheritSrcItem.rowIdx, _InheritSrcItem.colIdx);
        _InheritSrcItem.item = NULL;
        _InheritStarTet[0]->setString("0");
        UpdataView();
    }

    _InheritSrcIcon->setVisible(false);
    _InheritSrcQuality->loadTexture(EmptyItemTexture);
    _InheritSrcQuality->removeAllChildren();

    _InheritSpendGold->setString("0");
}

void UIUpgradeEquip::ClearInheritTarget()
{
    if (_InheritTagItem.item != NULL)
    {
        _pages[SP_Inherit].AddItem(_InheritTagItem.item, _InheritTagItem.rowIdx, _InheritTagItem.colIdx);
        _InheritTagItem.item = NULL;
        _InheritStarTet[1]->setString("0");
        UpdataView();
    }

    _InheritTargetIcon->setVisible(false);
    _InheritTargetQuality->loadTexture(EmptyItemTexture);
    _InheritTargetQuality->removeAllChildren();
}

void UIUpgradeEquip::OnInheritSuccess()
{
    //>播放动画
    Vec2 postion((_InheritTargetQuality->getPositionX() + _InheritSrcQuality->getPositionX()) / 2, _InheritTargetQuality->getPositionY());
    g_ResManager->ShowSprite(30001, _InheritTargetQuality->getParent(), 4, postion);

    std::string str = _InheritStarTet[0]->getString();
    //_InheritStarTet[0]->setString(_InheritStarTet[1]->getString());
    _InheritStarTet[0]->setString("0");
    _InheritStarTet[1]->setString(str);

}

void UIUpgradeEquip::SetDragInGuid(uint64 guid)
{
    _dragInGuid = guid;
}

void UIUpgradeEquip::OnUpStar()
{
    if (_upstarEquipment.item != NULL)
    {
        CliItemInfo* pItem = _upstarEquipment.item;
        const EquipmentUpgrade* pUpgradeTpl = g_CSVFileManager->GetEquipmentUpgradeByID(pItem->Data.starNum + 1);
        if (pUpgradeTpl == NULL)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(641);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //装备已是最高星级.
            return;
        }

        //>检查金币
        int hasMoney = int(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY]);
        int spendMoney = pUpgradeTpl->iCurrency;
        if (spendMoney > hasMoney)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(625);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //货币不足.
            return;
        }

        //>检查材料
        int hasStuffCount = g_LogicManager->m_Hero->m_Bag.GetItemCount(pUpgradeTpl->iMaterialid);
        if (hasStuffCount < pUpgradeTpl->iMaterialNum)
        {
            const TextInfo *pText = g_CSVFileManager->GetTextByID(626);
            g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //材料不足.
            return;
        }

        MsgItem_UpStar upstarMsg;
        upstarMsg.equipmentGuid = pItem->Data.guid;

        g_NetManager->SendMsg(upstarMsg);
        //>等待UI
        g_UIManager->OpenUI(UI_WAITTING);
        UIWaitting* pUiWait= (UIWaitting*)g_UIManager->GetUI(UI_WAITTING);
        if (pUiWait)
            pUiWait->SetTextID(61);
    }

}

void UIUpgradeEquip::ProcessDragInGuid(uint64 guid)
{
    uint32 rowIdx;
    uint32 colIdx;
    if(_pages[_pageIdx].FindItem(guid, rowIdx, colIdx))
    {
        OnSelectItem(rowIdx, colIdx);
    }
}

void UIUpgradeEquip::OnConfirmSelectItem(unsigned int rowIndex, unsigned int columnIndex)
{
    CliItemInfo* pItem = _pages[_pageIdx].GetItem(rowIndex, columnIndex);
    if (pItem != NULL)
    {
        _TempRowIndex = rowIndex;
        _TempColumnIndexIndex = columnIndex;
        g_UIManager->OpenUI(UI_ITEM_INFO);
        UIItemInfo* pUIItemInfo = static_cast<UIItemInfo*>(g_UIManager->GetUI(UI_ITEM_INFO));
        if (pUIItemInfo)
        {
            pUIItemInfo->OnClickedOperation = ([this](UIItemInfo::Operation operation, int32 status)
            {
                if (operation == UIItemInfo::Input)
                {
                    OnSelectItem(_TempRowIndex, _TempColumnIndexIndex);
                    g_UIManager->HideUI(UI_ITEM_INFO);
                }
                else
                {
                    static_cast<UIItemInfo*>(g_UIManager->GetUI(UI_ITEM_INFO))->DefaulClickedOperation(operation, status);
                }
            });

            vector<UIItemInfo::ButtonOperation> operations;
            if (_pageIdx == SP_Compose)
            {
                if (pItem->pCfg->iBigClass == ITEMTYPE_EQUIP)
                {
                    //>添加放入按钮
                    UIItemInfo::ButtonOperation operation;
                    operation.operation = UIItemInfo::Input;
                    operation.textId = 617;
                    operations.push_back(operation);

                    //>添加升星按钮
                    if (!pItem->IsMaxUpStarLevel())
                    {
                        operation.operation = UIItemInfo::UpStar;
                        operation.textId = 604;
                        operations.push_back(operation);
                    }

                    //>最后添加出售按钮
                    //operation.operation = UIItemInfo::Sell;
                    //operation.textId = 613;
                    //operations.push_back(operation);
                }
            }
            else if(_pageIdx == SP_UpStar)
            {
                if (pItem->pCfg->iBigClass == ITEMTYPE_EQUIP)
                {
                    //>添加放入按钮
                    UIItemInfo::ButtonOperation operation;
                    operation.operation = UIItemInfo::Input;
                    operation.textId = 617;
                    operations.push_back(operation);

                    //>添加合成按钮
                    operation.operation = UIItemInfo::Compose;
                    operation.textId = 603;
                    operations.push_back(operation);

                    //>最后添加出售按钮
                    //operation.operation = UIItemInfo::Sell;
                    //operation.textId = 613;
                    //operations.push_back(operation);
                }
            }
            else if(_pageIdx == SP_Inherit)
            {
                if (pItem->pCfg->iBigClass == ITEMTYPE_EQUIP)
                {
                    //>添加放入按钮
                    UIItemInfo::ButtonOperation operation;
                    operation.operation = UIItemInfo::Input;
                    operation.textId = 617;
                    operations.push_back(operation);

                    //>添加升星按钮
                    if (!pItem->IsMaxUpStarLevel())
                    {
                        operation.operation = UIItemInfo::UpStar;
                        operation.textId = 604;
                        operations.push_back(operation);
                    }

                    //>添加合成按钮
                    operation.operation = UIItemInfo::Compose;
                    operation.textId = 603;
                    operations.push_back(operation);

                    //>最后添加出售按钮
                    //operation.operation = UIItemInfo::Sell;
                    //operation.textId = 613;
                    //operations.push_back(operation);
                }
            }
            pUIItemInfo->SetupItem(pItem, operations, NULL);
        }
    }
}

void UIUpgradeEquip::Reload(unsigned int rowIndex, cc2d::ui::Widget* cellTemplate)
{
    ImageView* pSpr = NULL;
    ImageView* pSprQuality = NULL;
    ImageView* pSprEquiped = NULL;
    for (int columnIndex = 0; columnIndex < GV_Column_Max; ++columnIndex)
    {
        pSpr = static_cast<ImageView*>(cellTemplate->getChildByName(childSprNames[columnIndex]));
        pSprQuality = static_cast<ImageView*>(cellTemplate->getChildByName(qualitySprNames[columnIndex]));
        pSprEquiped = static_cast<ImageView*>(cellTemplate->getChildByName(equipedSprNames[columnIndex]));
        if (pSpr && pSprQuality && pSprEquiped)
        {
            pSpr->setVisible(true);
            pSpr->setTag(rowIndex * GV_Column_Max + columnIndex);
            //pSpr->setTouchEnabled(true);
            //pSpr->addClickEventListener([this](Ref* _sender)
            //{
            //    ImageView* img = static_cast<ImageView*>(_sender);
            //    uint32 theRowIndex = (uint32)img->getTag() / GV_Column_Max;
            //    uint32 theColumnIndex = (uint32)img->getTag() % GV_Column_Max;
            //    OnSelectItem(theRowIndex, theColumnIndex);
            //});

#pragma region 添加回调事件
            EventListenerTouchOneByOne* oldListener = (EventListenerTouchOneByOne*)pSpr->getUserObject();
            if (oldListener){
                Director::getInstance()->getEventDispatcher()->removeEventListener(oldListener);
            }

            EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
            listener->setSwallowTouches(false);
            listener->onTouchBegan = [this](Touch* touch, Event* event)
            {
                ImageView* img = static_cast<ImageView*>(event->getCurrentTarget());
                if(!IsClippingContainsTouch(touch, img))
                    return false;
                Vec2 nsp = img->convertTouchToNodeSpace(touch);
                Size size = img->getContentSize();
                Rect rect(0,0,size.width, size.height);
                if (rect.containsPoint(nsp))
                {
                    img->setUserData(new Vec2(touch->getLocation()));
                    return true;
                }
                return false;
            };

            listener->onTouchEnded = [this](Touch* touch, Event* event)
            {
                ImageView* img = static_cast<ImageView*>(event->getCurrentTarget());
                Vec2& beginPos = *(Vec2*)img->getUserData();
                Vec2 nsp = img->convertTouchToNodeSpace(touch);
                Size size = img->getContentSize();
                Rect rect(0,0,size.width, size.height);
                if (rect.containsPoint(nsp))
                {
                    const Vec2& endPos = touch->getLocation();
                    if( (beginPos.x - endPos.x) * (beginPos.x - endPos.x) < 64.0f && (beginPos.y - endPos.y) * (beginPos.y - endPos.y) < 64.0f )
                    {
                        uint32 theRowIndex = (uint32)img->getTag() / GV_Column_Max;
                        uint32 theColumnIndex = (uint32)img->getTag() % GV_Column_Max;
                        OnConfirmSelectItem(theRowIndex, theColumnIndex);
                    }
                }
                delete (Vec2*)img->getUserData();
                img->setUserData(NULL);
            };
            pSpr->setUserObject(listener);
            Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, pSpr);
#pragma endregion

            if (rowIndex >= GetUnlockNextNumer())
            {
                pSpr->loadTexture(LockGridTexture);
                pSprEquiped->setVisible(false);
            }
            else
            {
                CliItemInfo* pItem = _pages[_pageIdx].GetItem(rowIndex, columnIndex);
                if (pItem == NULL)
                {
                    pSpr->setVisible(false);
                    pSprQuality->loadTexture(EmptyItemTexture);
                    pSprEquiped->setVisible(false);
                    //>套装特效代码
                    pSprQuality->removeAllChildren();
                }
                else
                {
                    pSpr->loadTexture(pItem->pCfg->strIcon);
                    pSprQuality->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);
                    pSprEquiped->setVisible(false);
                    if (g_LogicManager->m_Hero->m_Bag.FindWearEquipmentByGuid(pItem->Data.guid))
                    {
                        pSprEquiped->setVisible(true);
                    }

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
            }
        }
    }
}
