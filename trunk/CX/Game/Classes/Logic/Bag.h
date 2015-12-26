#pragma once

/********************************************************************
created:	2015/7/6
author:		lishihai
desc:	    ������
*********************************************************************/

#include "GameDef.h"
#include "ClientDef.h"

extern const char* ItemQualityTextures[QUALITY_MAX];
extern const char* EmptyItemTexture;
extern const char* LockGridTexture;
extern const char* MoneyTypeIcon[5];

const char* GetMoneyIcon(AttributeType type);

#define EPSILON      1e-5f

inline bool IsFloatZero(float value)
{
    if (value > EPSILON || value < -EPSILON)
    {
        return false;
    }
    return true;
}

struct ItemsInfo;

struct CliItemInfo
{
    ItemInfoIt Data;
    const ItemsInfo* pCfg;

    bool IsEquipment();             //>�Ƿ���װ��

    bool CanUpStar();               //>�Ƿ������
    bool IsMaxUpStarLevel();        //>�Ƿ��Դﵽ������ǵȼ�
    bool CanCompose();              //>�Ƿ�ɺϳ�

    int GetBattlePower();           //>�����Ʒս����
    int GetComposeSpendMoney();     //>�����Ʒ�ϳ����Ľ��

    cocos2d::Color3B GetQualityColor();
};


class Bag
{
public:
    Bag(void);
    ~Bag(void);

    //>��ʼ��
    void Init(std::vector<CliItemInfo*>& pItems);
    //>�����Ʒ����
    uint32 GetSize() const { return _items.size(); }
    //>�򱳰���һ����Ʒ
    void AddItem(std::vector<CliItemInfo*>& items);
    //>�򱳰���һ����Ʒ
    void AddItem(CliItemInfo* item);
    //>�ӱ���ɾ��һ����Ʒ
    bool ReomveItem(uint64 guid);
    //>���������Ʒ
    void Clear() { _items.clear(); }
    //>�ӱ���ɾ��һ����Ʒ
    CliItemInfo* ReomveItemPtr(uint64 guid);
    //>ͨ��ΨһID�ҵ�һ����Ʒ
    CliItemInfo* GetItemByGuid(uint64 guid, bool includeEquip = true);
    //>ͨ������ID�ҵ�һ����Ʒ
    CliItemInfo* GetItemByTypeId(int ItemTypeId, int count = 1);
    //>���ĳ����Ʒ����
    int GetItemCount(int ItemTypeId) const;


    void GetPageItems(uint32 pageIdx, std::vector<CliItemInfo*>& items);

    void GetCanComposeItems(std::vector<CliItemInfo*>& items, bool includeEquip = true);

    void GetCanStarItems(std::vector<CliItemInfo*>& items, bool includeEquip = true);

    void GetCanInheritItems(std::vector<CliItemInfo*>& items, bool includeEquip = true);

    void NeedSort();


    // ��ҳ
    class Page
    {
    public:
        Page();
        //>�ؽ����ؽ���
        void Reload(int row, int col, std::vector<CliItemInfo*>& items);
        //>�ھ���λ�������Ʒ
        void AddItem(CliItemInfo* item, uint32 rowIdx, uint32 colIdx);
        //>ͨ��λ���ҵ���Ʒ
        CliItemInfo* GetItem(uint32 rowIdx, uint32 colIdx);
        //ͨ��ΨһId�ҵ���Ʒ
        bool FindItem(uint64 guid, uint32& rowIdx, uint32& colIdx);
        //>�Ƴ�ĳ��Ʒ
        bool Remove(uint32 rowIdx, uint32 colIdx);
        //>������Ʒ
        bool SwapItem(uint32 rowIdx1, uint32 colIdx1, uint32 rowIdx2, uint32 colIdx2);

        bool IsNeedReload() const { return _needReload; }
    private:

        int _row, _col;
        bool _needReload;       //>���·���
        //short * _array;
        std::vector<CliItemInfo*> _pageItems;
    };

    void LoadEquip(CliItemInfo* equipments[EQUIPTYPE_MAX]);
    void WearEquipment(EquipType type, CliItemInfo* equipment);
    void TakeoffEquipment(EquipType type);
    CliItemInfo* GetEquipment(EquipType type);
    CliItemInfo* FindWearEquipmentByGuid(uint64 guid);
    CliItemInfo* FindWearEquipment(uint32 typeId);
protected:

    std::vector<CliItemInfo*> _items;

    CliItemInfo*   _equips[EQUIPTYPE_MAX];
};

