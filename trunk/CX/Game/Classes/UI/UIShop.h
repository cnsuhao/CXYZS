/********************************************************************
created:	2014/12/6
author:		LSH
desc:		商店面板
*********************************************************************/

#ifndef __UISHOP_H__
#define __UISHOP_H__

#include "UIBase.h"
#include "UIGrid.h"

struct ShopItem;

class UIShop : public UIBase, public UIGrid
{
public:
	UIShop();
	~UIShop();	
	virtual bool init();
	CREATE_FUNC(UIShop);

    virtual void OnShow();

    virtual bool DisabledJoystickWhenSelfShow() { return true; }
	//主面板
	cc2d::Node *m_shopPanel;

	//加载面板
	bool LoadNode();

    void LoadTables();
    void LoadTabelItems(int32 labelId, bool force = false);

    void SelectLabel(int32 labelId);
    void OnBuyGoodes(int32 goodesId);

    void AddLabel(cc2dui::Widget* label, const std::string& labelName, int32 labelId, bool calcWidth = false);
    void CalcItemsSVHeight();
    void CalcLabelsSVWidth();
    void ClearItems();
    void ClearLabels();

    void SetGoods(cc2dui::Widget* pGoodsPanel, const ShopItem* pShopTpl);

    void ReloadHeroMoney();
    void SetMoneyType(int32 moneyType, int32 number);
protected:
    virtual void Reload(unsigned int rowIndex,  cc2d::ui::Widget* cellTemplate);
private:

    //>模板
    cc2dui::Layout *m_TemplateItem;
    cc2dui::Layout *m_TemplateLabel;
    //>滚动容器
    cc2dui::ScrollView *m_shopSV;
    cc2dui::ScrollView *m_labelSV;
    //>互斥容器
    std::vector<cc2dui::CheckBox*> m_mutexLabels;
    std::vector<cc2dui::CheckBox*> m_mutexGoodses;

    cc2dui::ImageView*  m_MoneyIcon;
    cc2dui::Text*       m_MoneyNumber;
    std::string         m_MoneyIconPath;

    int32               m_DefaultLabelId;
    int32               m_SelectLabelId;
    int32               m_SelectGoodesId;
};

#endif	//__UISHOP_H__