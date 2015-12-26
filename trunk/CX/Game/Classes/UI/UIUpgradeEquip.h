/********************************************************************
created:	2015/5/26
author:		LSH
desc:		装备进阶面板
*********************************************************************/

#ifndef __UIUPGRADEEQUIP_H__
#define __UIUPGRADEEQUIP_H__

#include "UIBase.h"
#include "UIGrid.h"
#include "Bag.h"
#include "UIItemInfo.h"

class UIUpgradeEquip : public UIBase, public UIGrid
{
protected:
    UIUpgradeEquip();
public:
    ~UIUpgradeEquip();
    virtual bool init();
    CREATE_FUNC(UIUpgradeEquip);

    virtual void OnShow();      //>显示回调,子类重载记得调用父类方法
    virtual bool DisabledJoystickWhenSelfShow() { return true; }

    enum SelectPage
    {
        SP_Compose,
        SP_UpStar,
        SP_Inherit,
        SP_Number,
    };

    enum GridValue
    {
        GV_Column_Max = 5,
        GV_Column_Max_Index = GV_Column_Max -1,
    };

    enum StuffPositon
    {
        MainStuff,
        Stuff1,
        Stuff2,
        Stuff3,
        Stuff4,

        Stuff_Number
    };

    // 设置页
    int GetPage() const { return _pageIdx; } 
    void SetPage(int page);

    //>加载角色财产
    void ReloadHeroMoney();

    // 设置货币1
    void SetMoney1(unsigned int count);
    // 设置货币2
    void SetMoney2(unsigned int count);
    // 设置货币3
    void SetMoney3(unsigned int count);

    // 设置持有数量
    void SetOwnGold(unsigned int number);
    // 设置消耗数量
    void SetSpendGold(unsigned int number);
    // 设置
    void SetComposeMoney(unsigned int a, unsigned int b);
    
    void OnOneKey(); //>一键配置
    void OnCompose(); //>合成
    void OnBuyStuff();//>购买材料
    void OnUpStar();  //>升星
    void OnInherit(); //>传承

    void OnSelectItem(unsigned int rowIndex, unsigned int columnIndex); //>选择物品

    void ClearStuffSolt();  //>清空材料槽位
    void OnCancelStuffSolt(int idx); //>取消某材料

    void ClearUpStarEquipment();        //>清空升星装备
    void SetStar(uint8 starNumber);     //>设置升星等级
    void OnCancelUpStar();              //>取消升星装备
    void OnUpStarSuccess();             //>装备升星成功
    void OnUpStarFailture();            //>装备升星失败
    void PlayStarAnim(uint8 starNumber);//>播放升星动画           
    
    void ClearInheritEquipment();       //>清空传承装备
    void ClearInheritSource();          //>清除传承装备
    void ClearInheritTarget();          //>清除目标装备
    void OnInheritSuccess();            //>传承成功

    void SetDragInGuid(uint64 guid);    //>传入物品guid
protected:
    void ProcessDragInGuid(uint64 guid);
    void OnConfirmSelectItem(unsigned int rowIndex, unsigned int columnIndex); //>确认选择物品
protected:
    virtual void Reload(unsigned int rowIndex,  cc2d::ui::Widget* cellTemplate);
protected:
    cc2dui::Button*     _buttons[3];
    cc2dui::CheckBox* _cboxArray[SP_Number];

    cc2dui::Text* _txtOwnGold;
    cc2dui::Text* _txtSpendGold;
    cc2dui::Text* _txtComposeMoney;

    cc2dui::Text* _textYuanBao;
    cc2dui::Text* _textGold;
    cc2dui::Text* _textMoney;

    SelectPage _pageIdx;

    Bag::Page _pages[SP_Number];
    cc2dui::Layout *m_Panel[SP_Number];

    struct BagItem
    {
        int rowIdx;
        int colIdx;
        CliItemInfo* item;
    };

    std::vector<BagItem> m_ComposeItems;
    cc2dui::ImageView* _stuffImgs[Stuff_Number];
    cc2d::Sprite*   _stuffQuality[Stuff_Number];
    int      _stuffSolt[Stuff_Number];

    cc2dui::ImageView* _upStarIcon;
    cc2dui::ImageView* _upStarQuality;
    cc2dui::ImageView* _starImage[UIItemInfo::s_ShowLevel];
    cc2dui::Text*   _attributeName[4];
    cc2dui::Text*   _attributeValue[4];
    cc2dui::Text*   _attributeAddValue[4];
    cc2dui::Text*   _upStarEquName;
    cc2dui::Text*   _upStarEquStarNB;
    cc2dui::Text*   _upStarSpendMoney;
    cc2dui::Text*   _upStarHasStuff;
    BagItem _upstarEquipment;

    cc2dui::ImageView* _InheritSrcIcon;
    cc2dui::ImageView* _InheritSrcQuality;
    cc2dui::ImageView* _InheritTargetIcon;
    cc2dui::ImageView* _InheritTargetQuality;
    cc2dui::Text*      _InheritSpendGold;
    BagItem    _InheritSrcItem;
    BagItem    _InheritTagItem;
    cc2dui::Text*      _InheritStarTet[2];


    uint64      _dragInGuid;
    int32       _TempRowIndex;
    int32       _TempColumnIndexIndex;
};

#endif	//__UIUPGRADEEQUIP_H__