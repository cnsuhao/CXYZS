#include "UIBag.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "Hero.h"
#include "CSVFileManager.h"
#include "UIItemInfo.h"
#include "ResManager.h"

using namespace cocostudio;

static const char* childSprNames[UIBag::GV_Column_Max] = { "Image_Grid1", "Image_Grid2", "Image_Grid3", "Image_Grid4", "Image_Grid5" }; 
static const char* childTextNames[UIBag::GV_Column_Max] = { "Text_Grid1", "Text_Grid2", "Text_Grid3", "Text_Grid4", "Text_Grid5" }; 
static const char* childQualityNames[UIBag::GV_Column_Max] = { "Image_Empty1", "Image_Empty2", "Image_Empty3", "Image_Empty4", "Image_Empty5" }; 

static const char* EquipPartQuality[UIBag::EP_Number] = { "Image_Quality1", "Image_Quality2", "Image_Quality3", "Image_Quality4", "Image_Quality5", "Image_Quality6", "Image_Quality7", "Image_Quality8", "Image_Quality9", "Image_Quality10" };
static const char* EquipPartText[UIBag::EP_Number] = { "Text_wq", "Text_tk", "Text_xl", "Text_yf", "Text_jz", "Text_yd", "Text_sp", "Text_xz", "Text_cb", "Text_sz" };
static const char* EquipPartIcon[UIBag::EP_Number] = { "Sprite_Equipable1", "Sprite_Equipable2", "Sprite_Equipable3", "Sprite_Equipable4", "Sprite_Equipable5", "Sprite_Equipable6", "Sprite_Equipable7", "Sprite_Equipable8", "Sprite_Equipable9", "Sprite_Equipable10" };

static const EquipType EquipPartToTypes[UIBag::EP_Number] = {EQUIPTYPE_WEAPON, EQUIPTYPE_HEAD, EQUIPTYPE_NECKLACE, EQUIPTYPE_BODY, EQUIPTYPE_RING, EQUIPTYPE_BELT, EQUIPTYPE_ORNAMENTS, EQUIPTYPE_SHOES, EQUIPTYPE_WING, EQUIPTYPE_FASHION };

UIBag::UIBag()
{
    _cboxAll = NULL;
    _cboxEquip = NULL;
    _cboxWing = NULL;
    _cboxConsume = NULL;
    _cboxStuff = NULL;
    _cboxOther = NULL;

    for (int i = 0; i < (int)SP_Number; ++i)
    {
        _texts[i] = NULL;
    }

    _textFaction = NULL;
    _textLevel = NULL;
    _textGold = NULL;
    _textBindGold = NULL;
    _textMoney = NULL;

    _imgGold = NULL;
    _imgBindGold = NULL;
    _imgMoney = NULL;

    for (int i = 0; i < EP_Number; ++i)
    {
        _equipPartNames[i] = NULL;
        _equipPartQuality[i] = NULL;
        _equipPartIcon[i] = NULL;
    }

    _scrollView = NULL;

    _pageIdx = SP_All;
    _rowNumber = 0;
    _unlockNextNumber = 0;
}

UIBag::~UIBag()
{

}

bool UIBag::init()
{
    bool bRet = false;
    do 
    {
        //加载角色属性面板
        Node* panel = CSLoader::createNode("UI/Bag.csb");
        CCASSERT(panel,"");
        if (panel == NULL){
            return false;
        }
        addChild(panel);
        _cboxAll = static_cast<CheckBox*>(panel->getChildByName("CheckBox_All"));
        _cboxEquip = static_cast<CheckBox*>(panel->getChildByName("CheckBox_Equip"));
        _cboxWing = static_cast<CheckBox*>(panel->getChildByName("CheckBox_Wing"));
        _cboxConsume = static_cast<CheckBox*>(panel->getChildByName("CheckBox_Consume"));
        _cboxStuff = static_cast<CheckBox*>(panel->getChildByName("CheckBox_Stuff"));
        _cboxOther = static_cast<CheckBox*>(panel->getChildByName("CheckBox_Other"));
        CCASSERT(_cboxAll && _cboxEquip && _cboxWing && _cboxConsume &&  _cboxStuff && _cboxOther, "");

        CheckBox::ccCheckBoxCallback checkCallback = [this](Ref* _sender,CheckBox::EventType type)
        {
            static CheckBox* array[] = { _cboxAll , _cboxEquip , _cboxWing , _cboxConsume ,  _cboxStuff , _cboxOther };
            int size = sizeof(array)/sizeof(array[0]);
            CheckBox* cbox = static_cast<CheckBox*>(_sender);
            switch (type)
            {
            case cocos2d::ui::CheckBox::EventType::SELECTED:
            case cocos2d::ui::CheckBox::EventType::UNSELECTED:
                for (int i = 0; i < size; ++i)
                {
                    if (array[i] == cbox)
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
        _cboxAll->addEventListener(checkCallback);
        _cboxEquip->addEventListener(checkCallback);
        _cboxWing->addEventListener(checkCallback);
        _cboxConsume->addEventListener(checkCallback);
        _cboxStuff->addEventListener(checkCallback);
        _cboxOther->addEventListener(checkCallback);


        Widget::ccWidgetClickCallback clickcallback = ([this](Ref* _sender)
        {
            ImageView* img = (ImageView*)(_sender);
            OnClickEquipment(EquipPart(img->getTag()));
        });

        Layer* pHeroPanel = static_cast<Layer*>(panel->getChildByName("Panel_Hero"));
        CCASSERT(pHeroPanel, "");

        for (int i = 0; i < EP_Number; ++i)
        {
            _equipPartNames[i] = static_cast<Text*>(pHeroPanel->getChildByName(EquipPartText[i]));
            _equipPartQuality[i] = static_cast<ImageView*>(pHeroPanel->getChildByName(EquipPartQuality[i]));
            _equipPartIcon[i] = static_cast<Sprite*>(pHeroPanel->getChildByName(EquipPartIcon[i]));

            CCASSERT(_equipPartNames[i] && _equipPartQuality[i] && _equipPartIcon[i], "");

            _equipPartIcon[i]->setVisible(false);
            _equipPartQuality[i]->loadTexture(EmptyItemTexture);
            _equipPartQuality[i]->setTouchEnabled(true);
            _equipPartQuality[i]->setTag((EquipPart)i);
            _equipPartQuality[i]->addClickEventListener(clickcallback);
        }

        _textFaction = static_cast<Text*>(pHeroPanel->getChildByName("Text_Group"));
        _textLevel = static_cast<Text*>(pHeroPanel->getChildByName("Text_Level"));
        _playerFace = static_cast<Node*>(pHeroPanel->getChildByName("Charater_Attach_Node"));
        
        _texts[SP_All] = static_cast<Text*>(panel->getChildByName("Text_All"));
        _texts[SP_Equip] = static_cast<Text*>(panel->getChildByName("Text_Equip"));
        _texts[SP_Wing] = static_cast<Text*>(panel->getChildByName("Text_Wing"));
        _texts[SP_Consume] = static_cast<Text*>(panel->getChildByName("Text_Consume"));
        _texts[SP_Stuff] = static_cast<Text*>(panel->getChildByName("Text_Stuff"));
        _texts[SP_Other] = static_cast<Text*>(panel->getChildByName("Text_Other"));
        CCASSERT(_texts[SP_All] && _texts[SP_Equip] && _texts[SP_Wing] && _texts[SP_Consume] &&  _texts[SP_Stuff] && _texts[SP_Other], "");

        Button* btn = static_cast<Button*>(panel->getChildByName("Button_Close"));
        if (btn)
        {
            btn->addClickEventListener([this](Ref* _sender)
            {
                g_UIManager->HideUI(UI_BAG);
            });
        }

        btn = static_cast<Button*>(panel->getChildByName("Button_AutoArragne"));
        if (btn)
        {
            btn->addClickEventListener([this](Ref* _sender)
            {
                OnAutoArrange();
            });
        }

        btn = static_cast<Button*>(panel->getChildByName("Button_Purchase"));
        if (btn)
        {
            btn->setTitleText(BaseFunction::a2u(g_CSVFileManager->GetTextByID(600)->strContent));
            btn->addClickEventListener([this](Ref* _sender)
            {
                OnPurchase();
            });
        }

        _textMoney = static_cast<Text*>(panel->getChildByName("Text_Yuanbao"));
        _textBindGold = static_cast<Text*>(panel->getChildByName("Text_Gold"));
        _textGold = static_cast<Text*>(panel->getChildByName("Text_Money"));

        _imgMoney = static_cast<Sprite*>(panel->getChildByName("Sprite_17"));
        _imgBindGold = static_cast<Sprite*>(panel->getChildByName("Sprite_18"));
        _imgGold = static_cast<Sprite*>(panel->getChildByName("Sprite_19"));

        ScrollView* tempScrollView = static_cast<ScrollView*>(panel->getChildByName("ScrollView"));
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

        SetPage(0);
        bRet = true;
    } while (0);

    return bRet;
}

void UIBag::OnShow()
{
    UIBase::OnShow();
    SetPage(_pageIdx);

    ReflushEquipment();
    LoadHeroFace();
    ReloadHeroMoney();
}

void UIBag::SetPage(int page)
{
    static CheckBox* array[] = { _cboxAll , _cboxEquip , _cboxWing , _cboxConsume ,  _cboxStuff , _cboxOther };


    if (page >= (int)SP_All && page <= (int)SP_Other)
    {
        //if (_page == page)
        //{
        //    return;
        //}

        _pageIdx = (SelectPage)page;
        int size = sizeof(array)/sizeof(array[0]);
        for (int i = 0; i < size; ++i)
        {
            array[i]->setSelected(false);
        }
        array[_pageIdx]->setSelected(true);

        //if (_pages->IsNeedReload()) //占时总是重新加载
        {
            std::vector<CliItemInfo*> items;
            g_LogicManager->m_Hero->m_Bag.GetPageItems(_pageIdx, items);

            int row = max((int)4, int((items.size() / GV_Column_Max) + 1));
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
        }
    }
    else
    {
        log("UIBag::SetPage error page index.\n");
    }
}

void UIBag::SetLevel(unsigned int level)
{
    if (_textLevel)
    {
        _textLevel->setString(StringUtils::format("%d",level));
    }
}

void UIBag::SetFaction(int faction)
{
    if (_textFaction)
    {
        //_textFaction->setText(StringUtils::format("%d"))
    }
}

void UIBag::SetMoney1(unsigned int count)
{
    if (_textMoney)
    {
        _textMoney->setString(StringUtils::format("%d", count));
    }

    if (_imgMoney)
    {
        _imgMoney->setTexture(GetMoneyIcon(ATTRIBUTE_MONEY));
    }
}

void UIBag::SetMoney2(unsigned int count)
{
    if (_textBindGold)
    {
        _textBindGold->setString(StringUtils::format("%d", count));
    }
    if (_imgBindGold)
    {
        _imgBindGold->setTexture(GetMoneyIcon(ATTRIBUTE_BINDGOLD));
    }
}

void UIBag::SetMoney3(unsigned int count)
{
    if (_textGold)
    {
        _textGold->setString(StringUtils::format("%d", count));
    }
    if (_imgGold)
    {
        _imgGold->setTexture(GetMoneyIcon(ATTRIBUTE_GOLD));
    }
}

void UIBag::WearEquipment(CliItemInfo* pEquipment)
{
    CCASSERT(pEquipment->IsEquipment(), "");
    EquipType equipmentType = (EquipType)pEquipment->pCfg->iSamllClass;
    Text* pName = NULL;
    Sprite* pIcon = NULL;
    ImageView* pQuality = NULL;
    switch (equipmentType)
    {
    case EQUIPTYPE_HEAD:
        pName = _equipPartNames[EP_Head];
        pIcon = _equipPartIcon[EP_Head];
        pQuality = _equipPartQuality[EP_Head];
        break;
    case EQUIPTYPE_BODY:
        pName = _equipPartNames[EP_Body];
        pIcon = _equipPartIcon[EP_Body];
        pQuality = _equipPartQuality[EP_Body];
        break;
    case EQUIPTYPE_WEAPON:
        pName = _equipPartNames[EP_Weapon];
        pIcon = _equipPartIcon[EP_Weapon];
        pQuality = _equipPartQuality[EP_Weapon];
        break;
    case EQUIPTYPE_WING:
        pName = _equipPartNames[EP_Wing];
        pIcon = _equipPartIcon[EP_Wing];
        pQuality = _equipPartQuality[EP_Wing];
        break;
    case EQUIPTYPE_HORSE:
        break;
    case EQUIPTYPE_BELT:
        pName = _equipPartNames[EP_Belt];
        pIcon = _equipPartIcon[EP_Belt];
        pQuality = _equipPartQuality[EP_Belt];
        break;
    case EQUIPTYPE_SHOES:
        pName = _equipPartNames[EP_Shoes];
        pIcon = _equipPartIcon[EP_Shoes];
        pQuality = _equipPartQuality[EP_Shoes];
        break;
    case EQUIPTYPE_NECKLACE:
        pName = _equipPartNames[EP_Necklace];
        pIcon = _equipPartIcon[EP_Necklace];
        pQuality = _equipPartQuality[EP_Necklace];
        break;
    case EQUIPTYPE_RING:
        pName = _equipPartNames[EP_Ring];
        pIcon = _equipPartIcon[EP_Ring];
        pQuality = _equipPartQuality[EP_Ring];
        break;
    case EQUIPTYPE_ORNAMENTS:
        pName = _equipPartNames[EP_Ornaments];
        pIcon = _equipPartIcon[EP_Ornaments];
        pQuality = _equipPartQuality[EP_Ornaments];
        break;
    case EQUIPTYPE_FASHION:
        pName = _equipPartNames[EP_Fashion];
        pIcon = _equipPartIcon[EP_Fashion];
        pQuality = _equipPartQuality[EP_Fashion];
        break;
    case EQUIPTYPE_MAGICWEAPON:
        break;
    case EQUIPTYPE_MAX:
        break;
    default:
        break;
    }

    if (pName == NULL || pIcon == NULL || pQuality == NULL)
    {
        const TextInfo *pText = g_CSVFileManager->GetTextByID(649);
        g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //装备类型错误
        return;
    }

    pName->setVisible(false);
    pIcon->setTexture(EmptyItemTexture);
    pQuality->loadTexture(pEquipment->pCfg->strIcon);
}

void UIBag::OnClickEquipment(EquipPart part)
{
    Bag& bag = g_LogicManager->m_Hero->m_Bag;
    CliItemInfo* pItem = bag.GetEquipment(EquipPartToTypes[part]);
    if (pItem != NULL)
    {
        g_UIManager->OpenUI(UI_ITEM_INFO);
        UIItemInfo* pUIItemInfo = static_cast<UIItemInfo*>(g_UIManager->GetUI(UI_ITEM_INFO));
        if (pUIItemInfo)
        {
            vector<UIItemInfo::ButtonOperation> operations;
            MakeOperation(pItem, operations, true);
            pUIItemInfo->SetupItem(pItem, operations, NULL);
        }
    }
}

void UIBag::OnWearSuccess()
{
    ReflushEquipment();
}

void UIBag::OnTakeOffSuccess()
{
    ReflushEquipment();
}

void UIBag::OnAutoArrange()
{

}

void UIBag::OnPurchase()
{

}

void UIBag::OnSelectItem(unsigned int rowIndex, unsigned int columnIndex)
{
    CliItemInfo* pItem = _pages[_pageIdx].GetItem(rowIndex, columnIndex);
    if (pItem != NULL)
    {
        g_UIManager->OpenUI(UI_ITEM_INFO);
        UIItemInfo* pUIItemInfo = static_cast<UIItemInfo*>(g_UIManager->GetUI(UI_ITEM_INFO));
        if (pUIItemInfo)
        {
            vector<UIItemInfo::ButtonOperation> operations;
            MakeOperation(pItem, operations, false);
            pUIItemInfo->SetupItem(pItem, operations, NULL);
        }
    }
}

void UIBag::LoadHeroFace()
{
    if (_playerFace && g_LogicManager->m_Hero)
    {
        _playerFace->stopAllActions();
        _playerFace->removeAllChildrenWithCleanup(true);
        Node* node = g_LogicManager->m_Hero->CreateFaceCopyNode();
        if (node)
        {
            node->setScale(1.5f);
            _playerFace->addChild(node);
        }
    }
}

void UIBag::ReloadHeroMoney()
{
    //>设置元宝数
    SetMoney3(uint32(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_GOLD]));
    //>设置绑定元宝
    SetMoney2(uint32(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_BINDGOLD]));
    //>设置金币
    SetMoney1(uint32(g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_MONEY]));
}

void UIBag::Reload(unsigned int rowIndex, cc2d::ui::Widget* cellTemplate)
{
    ImageView* pSpr = NULL;
    Text* pText = NULL;
    ImageView* pSprQuality = NULL;
    for (int columnIndex = 0; columnIndex < GV_Column_Max; ++columnIndex)
    {
        pSpr = static_cast<ImageView*>(cellTemplate->getChildByName(childSprNames[columnIndex]));
        pText = static_cast<Text*>(cellTemplate->getChildByName(childTextNames[columnIndex]));
        pSprQuality = static_cast<ImageView*>(cellTemplate->getChildByName(childQualityNames[columnIndex]));
        if (pSpr && pText && pSprQuality)
        {
            pText->enableShadow();
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
                        OnSelectItem(theRowIndex, theColumnIndex);
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
                pText->setVisible(false);
            }
            else
            {
                CliItemInfo* pItem = _pages[_pageIdx].GetItem(rowIndex, columnIndex);
                if (pItem == NULL)
                {
                    //pSpr->loadTexture(_textrueUnlock.c_str());
                    pSpr->setVisible(false);
                    pText->setVisible(false);
                    pSprQuality->loadTexture(EmptyItemTexture);

                    //>套装特效代码
                    pSprQuality->removeAllChildren();
                }
                else
                {
                    pSpr->loadTexture(pItem->pCfg->strIcon);
                    pText->setVisible(true);
                    pText->setString(StringUtils::format("%d",pItem->Data.num));
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
            }
        }
    }
}

void UIBag::ReflushEquipment()
{
    Bag& bag = g_LogicManager->m_Hero->m_Bag;

    _textFaction->setString(BaseFunction::a2u(g_LogicManager->m_Hero->m_Name));
    const TextInfo *pText = g_CSVFileManager->GetTextByID(666);
    _textLevel->setString(BaseFunction::a2u(StringUtils::format(pText->strContent.c_str(), g_LogicManager->m_Hero->m_Level))); //%d级

    for (int i = 0; i < EP_Number; ++i)
    {
        CliItemInfo* pItem = bag.GetEquipment(EquipPartToTypes[i]);
        if (pItem == NULL)
        {   
            _equipPartNames[i]->setVisible(true);
            _equipPartIcon[i]->setVisible(false);
            _equipPartQuality[i]->loadTexture(EmptyItemTexture);

            _equipPartQuality[i]->removeAllChildren();
        }
        else
        {
            _equipPartNames[i]->setVisible(false);
            _equipPartIcon[i]->setTexture(pItem->pCfg->strIcon);
            _equipPartIcon[i]->setVisible(true);
            _equipPartQuality[i]->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);

            //>套装特效代码
            if (pItem->pCfg->iSuitID != 0)
            {
                const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
                if (pInfo)
                {
                    Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, _equipPartQuality[i]->getAnchorPointInPoints());
                    _equipPartQuality[i]->removeAllChildren();
                    _equipPartQuality[i]->addChild(sprite);
                }
            }
            else
            {
                _equipPartQuality[i]->removeAllChildren();
            }
        }
    }
}

void UIBag::MakeOperation(CliItemInfo* pItem, vector<UIItemInfo::ButtonOperation>& operations, bool wearing)
{
    if (pItem->pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        //>添加装备按钮
        UIItemInfo::ButtonOperation operation;
        operation.operation = UIItemInfo::Equip;
        if (wearing)
            operation.textId = 607;
        else
            operation.textId = 606;
        operations.push_back(operation);

        //>添加合成按钮
        if (pItem->CanCompose())
        {
            operation.operation = UIItemInfo::Compose;
            operation.textId = 603;
            operations.push_back(operation);
        }

        //>添加升星按钮
        if (pItem->CanUpStar() && !pItem->IsMaxUpStarLevel())
        {
            operation.operation = UIItemInfo::UpStar;
            operation.textId = 604;
            operations.push_back(operation);
        }
        
        //>最后添加出售按钮
        operation.operation = UIItemInfo::Sell;
        operation.textId = 613;
        operations.push_back(operation);
    }
    else
    {
        //>添加使用按钮
        UIItemInfo::ButtonOperation operation;
        operation.operation = UIItemInfo::Use;
        operation.textId = 616;
        operations.push_back(operation);


        //>最后添加出售按钮
        operation.operation = UIItemInfo::Sell;
        operation.textId = 613;
        operations.push_back(operation);
    }
}
