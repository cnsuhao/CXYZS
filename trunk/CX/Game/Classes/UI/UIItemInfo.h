/********************************************************************
created:	2015/8/25
author:		lishihai
desc:		��Ʒ��ϸ��Ϣ���
*********************************************************************/
#pragma once

#include "UIBase.h"
#include "CCLabelAtlas.h"

struct CliItemInfo;

class UIItemInfo : public UIBase
{
public:
    static const uint8 s_ShowLevel = 10;        //>����ʾ���Ǽ�����
    static const uint8 s_StarKind = 3;          //>�Ǽ�����.

    enum Operation
    {
        Equip,      //>װ��
        UpStar,     //>����
        Compose,    //>�ϳ�
        Sales,      //>����
        Sell,       //>����
        Lock,       //>����
        Use,        //>ʹ��
        Input,      //>����
        


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

    virtual void OnHide();      //>���ػص�,�������ؼǵõ��ø��෽��
    virtual bool DisabledJoystickWhenSelfShow() { return true; }

    void SetupItem(CliItemInfo* pItem, const vector<ButtonOperation>& showOperation, CliItemInfo equipments[EQUIPTYPE_MAX]);
    void SetMoney(int number);
    void SetCount(int number);

    void OnEquip();     //>װ��
    void OnUpStar();    //>����
    void OnCompose();   //>�ϳ�
    void OnSale();      //>����
    void OnSell();      //>����
    void OnLock();      //>����
    void OnUse();       //>ʹ��

    void DefaulClickedOperation(Operation operation, int32 status);

    std::function<void(Operation operation, int32 status)> OnClickedOperation;
    

protected:
    void SetStar(uint8 starNumber);     //>�����Ǽ�
    void SetPart(int partType);         //>���ò�λ
    void SetBind(bool bound);           //>���ð�
    void SetBattlePower(int number);    //>����ս����
    void SetAttribute();                //>��������

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

