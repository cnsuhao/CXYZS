/********************************************************************
created:	2015/8/25
author:		lishihai
desc:		物品详细信息面板
*********************************************************************/
#pragma once

#include "UIBase.h"
#include "CCLabelAtlas.h"

struct CliItemInfo;

class UIItemInfo : public UIBase
{
public:
    static const uint8 s_ShowLevel = 10;        //>可显示的星级数量
    static const uint8 s_StarKind = 3;          //>星级种类.

    enum Operation
    {
        Equip,      //>装备
        UpStar,     //>升星
        Compose,    //>合成
        Sales,      //>寄售
        Sell,       //>出售
        Lock,       //>锁定
        Use,        //>使用
        Input,      //>放入
        


        OperationNumber
    };

    struct ButtonOperation
    {
        Operation   operation;
        int32       textId;
        int32       status;
        ButtonOperation():operation(OperationNumber),textId(0),status(0){}
    };


public:
    UIItemInfo(void);
    ~UIItemInfo(void);

    virtual bool init();
    CREATE_FUNC(UIItemInfo);

    virtual void OnHide();      //>隐藏回调,子类重载记得调用父类方法
    virtual bool DisabledJoystickWhenSelfShow() { return true; }

    void SetupItem(CliItemInfo* pItem, const vector<ButtonOperation>& showOperation, CliItemInfo equipments[EQUIPTYPE_MAX]);
    void SetMoney(int number);
    void SetCount(int number);

    void OnEquip();     //>装备
    void OnUpStar();    //>升星
    void OnCompose();   //>合成
    void OnSale();      //>寄售
    void OnSell();      //>出售
    void OnLock();      //>锁定
    void OnUse();       //>使用

    void DefaulClickedOperation(Operation operation, int32 status);

    std::function<void(Operation operation, int32 status)> OnClickedOperation;
    

protected:
    void SetStar(uint8 starNumber);     //>设置星级
    void SetPart(int partType);         //>设置部位
    void SetBind(bool bound);           //>设置绑定
    void SetBattlePower(int number);    //>设置战斗力
    void SetAttribute();                //>设置属性

private:
    cc2d::Node *m_RootPanel;
    cc2dui::Layout *m_EquipmentPanel;
    cc2dui::Layout *m_ItemPanel;
    cc2dui::Layout *m_StarPanel;
    cc2dui::Layout* m_EquipSetPanel;
    cc2dui::ImageView* m_StarImage[s_ShowLevel];
    cc2d::LabelAtlas* m_BattlePowerLable;
    cc2dui::Text*       m_AttachPart;
    cc2dui::Text*       m_BindTxt;
    cc2dui::Text*       m_ItemBindTxt;

    cc2dui::Text*       m_AttributeNames[8];
    cc2dui::Text*       m_AttributeValues1[8];
    cc2dui::Text*       m_AttributeValues2[8];

    vector<cc2dui::Button*> m_EquipmentButtons;
    vector<cc2dui::Button*> m_ItemButtons;

    uint64      m_itemGuid;

    vector<ButtonOperation> m_ShowOperation;
};

