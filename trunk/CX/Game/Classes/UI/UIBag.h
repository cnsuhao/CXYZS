/********************************************************************
created:	2014/12/6
author:		LSH
desc:		背包面板
*********************************************************************/

#ifndef __UIBAG_H__
#define __UIBAG_H__

#include "UIBase.h"
#include "UIGrid.h"
#include "Bag.h"
#include "UIItemInfo.h"

class UIBag : public UIBase, public UIGrid
{
    friend class DataSource;
public:
	UIBag();
	~UIBag();	
	virtual bool init();
	CREATE_FUNC(UIBag);

    virtual bool DisabledJoystickWhenSelfShow() { return true; }

    enum SelectPage
    {
        SP_All,
        SP_Equip,
        SP_Wing,
        SP_Consume,
        SP_Stuff,
        SP_Other,
        SP_Number,
    };

    enum GridValue
    {
        GV_Column_Max = 5,
        GV_Column_Max_Index = GV_Column_Max -1,
    };

    enum EquipPart
    {
        EP_Weapon,
        EP_Head,
        EP_Necklace,
        EP_Body,
        EP_Ring,
        EP_Belt,
        EP_Ornaments,
        EP_Shoes,
        EP_Wing,
        EP_Fashion,
        EP_Number
    };

    virtual void OnShow();

    // 设置页
    unsigned int GetPage() const { return (unsigned int)_pageIdx; } 
    void SetPage(int page);

    // 设置等级数
    void SetLevel(unsigned int level);
    // 设置帮派
    void SetFaction(int faction);
    // 设置金币
    void SetMoney1(unsigned int count);
    // 设置绑定元宝
    void SetMoney2(unsigned int count);
    // 设置元宝数
    void SetMoney3(unsigned int count);
    // 穿装备
    void WearEquipment(CliItemInfo* pEquipment);
    // 点击装备
    void OnClickEquipment(EquipPart part);
    
    void OnWearSuccess();       //>穿戴成功
    void OnTakeOffSuccess();    //>脱下成功

    // 自动整理
    void OnAutoArrange();
    // 充值
    void OnPurchase();

    void OnSelectItem(unsigned int rowIndex, unsigned int columnIndex);
    //>加载角色外观
    void LoadHeroFace();
    //>加载角色财产
    void ReloadHeroMoney();
protected:
    virtual void Reload(unsigned int rowIndex,  cc2d::ui::Widget* cellTemplate);

    void ReflushEquipment();    //>刷新装备拦

    void MakeOperation(CliItemInfo* pItem, vector<UIItemInfo::ButtonOperation>& operations, bool wearing);

    cc2dui::CheckBox* _cboxAll;
    cc2dui::CheckBox* _cboxEquip;
    cc2dui::CheckBox* _cboxWing;
    cc2dui::CheckBox* _cboxConsume;
    cc2dui::CheckBox* _cboxStuff;
    cc2dui::CheckBox* _cboxOther;

    cc2dui::Text* _texts[SP_Number];
    cc2dui::Text* _textFaction;
    cc2dui::Text* _textLevel;
    cc2dui::Text* _textGold;
    cc2dui::Text* _textBindGold;
    cc2dui::Text* _textMoney;

    cc2d::Sprite* _imgGold;
    cc2d::Sprite* _imgBindGold;
    cc2d::Sprite* _imgMoney;

    cc2dui::Text* _equipPartNames[EP_Number];
    cc2d::Sprite* _equipPartIcon[EP_Number];
    cc2dui::ImageView* _equipPartQuality[EP_Number];

    SelectPage _pageIdx;
    unsigned int _rowNumber;
    unsigned int _unlockNextNumber;

    Bag::Page _pages[SP_Number];

    cc2d::Node*     _playerFace;
};

#endif	//__UIBAG_H__