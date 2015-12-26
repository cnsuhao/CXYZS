#include "UIShop.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "CSVFileManager.h"
#include "NetManager.h"
#include "Bag.h"
#include "UIManager.h"
#include "UIWaitting.h"
#include "ResManager.h"
#include "LogicManager.h"

using namespace cocostudio;

UIShop::UIShop()
{
    m_DefaultLabelId = -1;
    m_SelectLabelId = -1;
    m_SelectGoodesId = -1;

    m_MoneyIcon = NULL;
}

UIShop::~UIShop()
{
}

bool UIShop::init()
{
    bool bRet = false;
    do 
    {
		//载入各种面板
		if (!LoadNode())	return false;

       
        bRet = true;
    } while (0);

    return bRet;
}

void UIShop::OnShow()
{
    UIBase::OnShow();

    ReloadHeroMoney();

    LoadTabelItems(m_SelectLabelId, true);
}

//加载各种面板
bool UIShop::LoadNode()
{
	//加载主面板
	m_shopPanel = CSLoader::createNode("UI/Shop.csb");
	if (m_shopPanel == NULL)
		return false;
	addChild(m_shopPanel);

	Button *btn = nullptr;
	//关闭按钮
	btn = static_cast<Button*>(m_shopPanel->getChildByName("bt_close"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref* _sender)
		{
			g_UIManager->HideUI(UI_SHOP);
		});
	}

	//模板
	m_TemplateItem = static_cast<Layout*>(m_shopPanel->getChildByName("panel_model"));
	m_TemplateItem->setVisible(false);

    m_TemplateLabel = static_cast<Layout*>(m_shopPanel->getChildByName("panel_label"));
    m_TemplateLabel->setVisible(false);

	//滚动条
	//m_shopSV = static_cast<cc2dui::ScrollView*>(m_shopPanel->getChildByName("sv_data"));
	//m_shopSV ->setSwallowTouches(true); 

	//m_shopSV->setLayoutType(Layout::Type::VERTICAL);
	//m_shopSV->forceDoLayout();

    m_MoneyIcon = static_cast<ImageView*>(m_shopPanel->getChildByName("img_money"));
    CCASSERT(m_MoneyIcon, "");

    m_MoneyNumber = static_cast<Text*>(m_shopPanel->getChildByName("tx_money"));
    CCASSERT(m_MoneyNumber, "");

    m_shopSV = static_cast<cc2dui::ScrollView*>(m_shopPanel->getChildByName("sv_data"));
    CCASSERT(m_shopSV, "");
    m_shopSV->setEnabled(false);
    Widget* panelGrids = static_cast<Widget*>(m_shopPanel->getChildByName("panel_model"));
    CCASSERT(panelGrids, "");
    panelGrids->setTouchEnabled(false);
    if(InitGrid(m_shopSV->getContentSize(), m_shopSV->getPosition(), panelGrids))
    {
        m_shopSV->getParent()->addChild((Node*)_scrollView);
    }
    else
    {
        CCASSERT(0,"");
        return false;
    }

    //标签滚动条
    m_labelSV = static_cast<cc2dui::ScrollView*>(m_shopPanel->getChildByName("sv_label"));
    m_labelSV ->setSwallowTouches(true); 

    m_labelSV->setLayoutType(Layout::Type::HORIZONTAL);
    m_labelSV->forceDoLayout();

	//>添加页签
    LoadTables();
    //>添加页签物品
    if (m_DefaultLabelId >= 0)
    {
        SelectLabel(m_DefaultLabelId);
    }

    btn = static_cast<Button*>(m_shopPanel->getChildByName("bt_buy"));
    if(btn){

        btn->addClickEventListener([this](Ref* _sender)
        {
            OnBuyGoodes(m_SelectGoodesId);
        });
    }
    
    return true;
}

void UIShop::LoadTables()
{
    ClearLabels();

    std::map<int32,Shop>::iterator it = g_CSVFileManager->m_ShopInfoMap.begin();
    for (std::map<int32,Shop>::iterator end = g_CSVFileManager->m_ShopInfoMap.end();
         it != end; 
         ++it)
    {
        if (m_DefaultLabelId < 0)
        {
            m_DefaultLabelId = it->first;
        }

        Shop& shopTpl = it->second;
        AddLabel(m_TemplateLabel->clone(), shopTpl.strName, it->first);    
    }

    CalcLabelsSVWidth();
}

void UIShop::LoadTabelItems(int32 labelId, bool force)
{
    if (!force && m_SelectLabelId == labelId)
    {
        return;
    }
    m_SelectLabelId = labelId;

    const std::map<int32,ShopItem>* pGoodsItems = g_CSVFileManager->GetShopItemByID(labelId);
    if (pGoodsItems)
    {
        ReloadHeroMoney();

        int row = pGoodsItems->size() / 2 + pGoodsItems->size() % 2;
        if (row > 0)
        {
            ClearItems();
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
}

void UIShop::SelectLabel(int32 labelId)
{
    if (m_labelSV)
    {
        Vector<Node*>& childs = m_labelSV->getChildren();
        size_t count = childs.size();
        Vector<Node*>::iterator it = childs.begin();
        for (Vector<Node*>::iterator end = childs.end();
             it != end;
             ++it)
        {
            if ((*it)->getTag() == labelId)
            {
                cc2dui::CheckBox* checkbox = static_cast<cc2dui::CheckBox*>((*it)->getChildByName("cb_vip"));
                if (checkbox)
                {
                    //>设置标签为选中
                    checkbox->setSelected(true);
                    
                    //>加载标签物品
                    LoadTabelItems(labelId);

                    m_SelectLabelId = labelId;

                    m_SelectGoodesId = -1;
                }
                break;
            }
        }
    }
}

void UIShop::OnBuyGoodes(int32 goodesId)
{
    if (goodesId > 0 && m_SelectLabelId > 0)
    {
        const std::map<int32,ShopItem>* pGoodsItems = g_CSVFileManager->GetShopItemByID(m_SelectLabelId);
        if (pGoodsItems)
        {
            const Shop* pShopTpl = g_CSVFileManager->GetShopByID(m_SelectLabelId);
            if (!(pShopTpl->iCurrencyType == ATTRIBUTE_MONEY || 
                pShopTpl->iCurrencyType == ATTRIBUTE_GOLD ||
                pShopTpl->iCurrencyType == ATTRIBUTE_BINDGOLD ||
                pShopTpl->iCurrencyType == ATTRIBUTE_ARENAMONEY ||
                pShopTpl->iCurrencyType == ATTRIBUTE_BATTLEFIELDMONEY
                ))
            {
                CCASSERT(0, "invalid type of money.");
                return;
            }

            auto it = pGoodsItems->find(goodesId);
            if (it != pGoodsItems->end())
            {
                const ShopItem& pGoodsInfo = it->second;
                int hasMoney = int(g_LogicManager->m_Hero->m_Attribute[(AttributeType)pShopTpl->iCurrencyType]);
                int spendMoney = pGoodsInfo.iPrice * pGoodsInfo.iNumber;
                if (spendMoney > hasMoney)
                {
                    const TextInfo *pText = g_CSVFileManager->GetTextByID(625);
                    g_UIManager->Notify(BaseFunction::a2u(pText->strContent), Color3B::RED); //货币不足.
                    return;
                }

                MsgItem_ShopBuy msg;
                msg.shopId = m_SelectLabelId;
                msg.goodesId = goodesId;

                g_NetManager->SendMsg(msg);

                g_UIManager->OpenUI(UI_WAITTING);
                UIWaitting* pUiWait= (UIWaitting*)g_UIManager->GetUI(UI_WAITTING);
                if (pUiWait)
                    pUiWait->SetTextID(61);
            }
        }
        
    }
}

void UIShop::AddLabel(cc2dui::Widget* label, const std::string& labelName, int32 labelId, bool calcHeight)
{
    if (m_labelSV)
    {
        cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(label);
        newLayout->setVisible(true);
        newLayout->setTouchEnabled(true);
        newLayout->setTag(labelId);

        Text* text = static_cast<Text*>(label->getChildByName("tx_vip"));
        if (text)
        {
            text->setString(BaseFunction::a2u(labelName));
        }

        cc2dui::CheckBox* checkbox = static_cast<cc2dui::CheckBox*>(label->getChildByName("cb_vip"));
        if (checkbox)
        {
            m_mutexLabels.push_back(checkbox);

            checkbox->setSelected(false);
            checkbox->addEventListener([this](Ref* _sender,CheckBox::EventType type)
            {
                std::vector<CheckBox*>::iterator it = m_mutexLabels.begin();
                for (std::vector<CheckBox*>::iterator end = m_mutexLabels.end(); 
                     it != end;
                     ++it)
                {
                    CheckBox* cb = *it;
                    cb->setSelected(false);
                }

                CheckBox* sender = static_cast<CheckBox*>(_sender);
                if (sender)
                {
                    int32 selectLabelId = sender->getParent()->getTag();

                    sender->setSelected(true);

                    LoadTabelItems(selectLabelId);

                    m_SelectLabelId = selectLabelId;

                    m_SelectGoodesId = -1;
                }
            });
        }

        m_labelSV->addChild(newLayout);

        if (calcHeight)
        {
            CalcLabelsSVWidth();
        }
    }
}

void UIShop::CalcItemsSVHeight()
{
    if (m_shopSV)
    {
        Size TempSize = m_shopSV->getInnerContainerSize();
        TempSize.height = m_shopSV->getChildrenCount() * m_TemplateItem->getContentSize().height;
        m_shopSV->setInnerContainerSize(TempSize);
        m_shopSV->forceDoLayout();
    }
}

void UIShop::CalcLabelsSVWidth()
{
    if (m_labelSV)
    {
        Size TempSize = m_labelSV->getInnerContainerSize();
        TempSize.width = m_labelSV->getChildrenCount() * m_TemplateLabel->getContentSize().width;
        m_labelSV->setInnerContainerSize(TempSize);
        m_labelSV->forceDoLayout();
    }
}

void UIShop::ClearItems()
{
    if (m_shopSV)
    {
        m_shopSV->removeAllChildren();
        m_mutexGoodses.clear();
    }
}

void UIShop::ClearLabels()
{
    if (m_labelSV)
    {
        m_labelSV->removeAllChildren();
        m_mutexLabels.clear();
    }
}

void UIShop::Reload(unsigned int rowIndex, cc2d::ui::Widget* cellTemplate)
{
    const std::map<int32,ShopItem>* pGoodsItems = g_CSVFileManager->GetShopItemByID(m_SelectLabelId);
    if (pGoodsItems)
    {
        const ShopItem* pShopTpl = NULL;
        const ShopItem* pShopTpl2 = NULL;

        int maxRowNumber = GetRowNumber();

        int startIdx = rowIndex * 2;
        int Idx = 0;
        std::map<int32,ShopItem>::const_iterator it = pGoodsItems->begin();
        for (std::map<int32,ShopItem>::const_iterator end = pGoodsItems->end(); 
            it != end; 
            ++it)
        {
            if (startIdx == Idx)
            {
                pShopTpl = &(it->second);

                if (++it != end)
                    pShopTpl2 = &(it->second);
                
                break;
            }
            ++Idx;
        }

        cc2dui::Layout* pGoods1 = static_cast<cc2dui::Layout*>(cellTemplate->getChildByName("panel_goods"));
        if (pGoods1 && pShopTpl)
        {
            SetGoods(pGoods1 , pShopTpl);
        }
        cc2dui::Layout* pGoods2 = static_cast<cc2dui::Layout*>(cellTemplate->getChildByName("panel_goods_2"));
        if (pGoods2)
        {
            if (pShopTpl2)
            {
                pGoods2->setVisible(true);
                SetGoods(pGoods2 , pShopTpl2);
            }
            else
            {
                pGoods2->setVisible(false);
            }
        }
    }
}

void UIShop::SetMoneyType(int32 moneyType, int32 number)
{
    if (m_MoneyIcon)
    {
        AttributeType type = (AttributeType)moneyType;
        m_MoneyIcon->loadTexture(GetMoneyIcon(type));
        m_MoneyIconPath.assign(GetMoneyIcon(type));
    }

    if (m_MoneyNumber)
    {
        m_MoneyNumber->setString(StringUtils::format("%d", number));
    }
}

void UIShop::SetGoods(cc2dui::Widget* pGoodsPanel, const ShopItem* pShopTpl)
{
    cc2dui::Layout* newGoods = static_cast<Layout*>(pGoodsPanel);

    newGoods->setTag(pShopTpl->iGoodsid);
    Text* pGoodsName = static_cast<Text*>(newGoods->getChildByName("tx_leftname"));
    const ItemsInfo* pItemTpl = g_CSVFileManager->GetItemsByID(pShopTpl->iItemId);
    if (pGoodsName && pItemTpl) //>设置商品名
    {
        pGoodsName->setString(BaseFunction::a2u(pItemTpl->strName));
    }

    ImageView* pIcon = static_cast<ImageView*>(newGoods->getChildByName("img_leftequbg"));
    if (pIcon && pItemTpl) //>商品图标
    {
        pIcon->loadTexture(pItemTpl->strIcon);
    }

    ImageView* pQualityImg = static_cast<ImageView*>(newGoods->getChildByName("img_leftQuality"));
    if (pQualityImg && pItemTpl)
    {
        pQualityImg->loadTexture(ItemQualityTextures[pItemTpl->iQuality]);

        //>套装特效代码
        if (pItemTpl->iSuitID != 0)
        {
            const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItemTpl->iSuitID);
            if (pInfo)
            {
                Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, pQualityImg->getAnchorPointInPoints());
                pQualityImg->removeAllChildren();
                pQualityImg->addChild(sprite);
            }
        }
        else
        {
            pQualityImg->removeAllChildren();
        }
    }

    ImageView* pMoney = static_cast<ImageView*>(newGoods->getChildByName("img_leftmoney"));
    if (pMoney) //>货币类型
    {
        pMoney->loadTexture(m_MoneyIconPath);
    }

    Text* pGoodsCost = static_cast<Text*>(newGoods->getChildByName("tx_leftnum"));
    if (pGoodsCost)   //>设置价格
    {
        pGoodsCost->setString(StringUtils::format("%d", pShopTpl->iPrice*pShopTpl->iNumber));
    }

    Text* pBuyCount = static_cast<Text*>(newGoods->getChildByName("tx_count"));
    if (pBuyCount)   //>设置数量
    {
        pBuyCount->setString(StringUtils::format("%d", pShopTpl->iNumber));
    }

    CheckBox* pCheckBox = static_cast<CheckBox*>(newGoods->getChildByName("cb_left"));
    if (pCheckBox)
    {
        m_mutexGoodses.push_back(pCheckBox);

        pCheckBox->setSelected(false);
        pCheckBox->setTouchEnabled(false);
        pCheckBox->setTag(pShopTpl->iGoodsid);
        EventListenerTouchOneByOne* oldListener = (EventListenerTouchOneByOne*)pCheckBox->getUserObject();
        if (oldListener){
            Director::getInstance()->getEventDispatcher()->removeEventListener(oldListener);
        }

        EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(false);
        listener->onTouchBegan = [this](Touch* touch, Event* event)
        {
            CheckBox* pCheckBox = static_cast<CheckBox*>(event->getCurrentTarget());
            if(!IsClippingContainsTouch(touch, pCheckBox))
                return false;
            Vec2 nsp = pCheckBox->convertTouchToNodeSpace(touch);
            Size size = pCheckBox->getContentSize();
            Rect rect(0,0,size.width, size.height);
            if (rect.containsPoint(nsp))
            {
                pCheckBox->setUserData(new Vec2(touch->getLocation()));
                return true;
            }
            return false;
        };

        listener->onTouchEnded = [this](Touch* touch, Event* event)
        {
            CheckBox* pCheckBox = static_cast<CheckBox*>(event->getCurrentTarget());
            Vec2& beginPos = *(Vec2*)pCheckBox->getUserData();
            Vec2 nsp = pCheckBox->convertTouchToNodeSpace(touch);
            Size size = pCheckBox->getContentSize();
            Rect rect(0,0,size.width, size.height);
            if (rect.containsPoint(nsp))
            {
                const Vec2& endPos = touch->getLocation();
                if( (beginPos.x - endPos.x) * (beginPos.x - endPos.x) < 64.0f && (beginPos.y - endPos.y) * (beginPos.y - endPos.y) < 64.0f )
                {
                    std::vector<CheckBox*>::iterator it = m_mutexGoodses.begin();
                    for (std::vector<CheckBox*>::iterator end = m_mutexGoodses.end(); 
                        it != end;
                        ++it)
                    {
                        CheckBox* cb = *it;
                        cb->setSelected(false);
                    }
                    pCheckBox->setSelected(true);
                    int32 goodesId = pCheckBox->getTag();
                    m_SelectGoodesId = goodesId;
                }
            }
            delete (Vec2*)pCheckBox->getUserData();
            pCheckBox->setUserData(NULL);
        };

        pCheckBox->setUserObject(listener);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, pCheckBox);
    }
}

void UIShop::ReloadHeroMoney()
{
    if (m_SelectLabelId > 0)
    {
        const Shop* pShopTpl = g_CSVFileManager->GetShopByID(m_SelectLabelId);
        if (!(pShopTpl->iCurrencyType == ATTRIBUTE_MONEY || 
            pShopTpl->iCurrencyType == ATTRIBUTE_GOLD ||
            pShopTpl->iCurrencyType == ATTRIBUTE_BINDGOLD ||
            pShopTpl->iCurrencyType == ATTRIBUTE_ARENAMONEY ||
            pShopTpl->iCurrencyType == ATTRIBUTE_BATTLEFIELDMONEY
            ))
        {
            CCASSERT(0, "invalid type of money.");
            return;
        }

        SetMoneyType(pShopTpl->iCurrencyType, g_LogicManager->m_Hero->m_Attribute[(AttributeType)pShopTpl->iCurrencyType]);
    }
}
