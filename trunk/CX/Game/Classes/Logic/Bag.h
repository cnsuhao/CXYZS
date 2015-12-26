#pragma once

/********************************************************************
created:	2015/7/6
author:		lishihai
desc:	    背包类
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

    bool IsEquipment();             //>是否是装备

    bool CanUpStar();               //>是否可升星
    bool IsMaxUpStarLevel();        //>是否以达到最大升星等级
    bool CanCompose();              //>是否可合成

    int GetBattlePower();           //>获得物品战斗力
    int GetComposeSpendMoney();     //>获得物品合成消耗金币

    cocos2d::Color3B GetQualityColor();
};


class Bag
{
public:
    Bag(void);
    ~Bag(void);

    //>初始化
    void Init(std::vector<CliItemInfo*>& pItems);
    //>获得物品数量
    uint32 GetSize() const { return _items.size(); }
    //>向背包放一堆物品
    void AddItem(std::vector<CliItemInfo*>& items);
    //>向背包放一个物品
    void AddItem(CliItemInfo* item);
    //>从背包删除一个物品
    bool ReomveItem(uint64 guid);
    //>清空所有物品
    void Clear() { _items.clear(); }
    //>从背包删除一个物品
    CliItemInfo* ReomveItemPtr(uint64 guid);
    //>通过唯一ID找到一个物品
    CliItemInfo* GetItemByGuid(uint64 guid, bool includeEquip = true);
    //>通过类型ID找到一个物品
    CliItemInfo* GetItemByTypeId(int ItemTypeId, int count = 1);
    //>获得某类物品数量
    int GetItemCount(int ItemTypeId) const;


    void GetPageItems(uint32 pageIdx, std::vector<CliItemInfo*>& items);

    void GetCanComposeItems(std::vector<CliItemInfo*>& items, bool includeEquip = true);

    void GetCanStarItems(std::vector<CliItemInfo*>& items, bool includeEquip = true);

    void GetCanInheritItems(std::vector<CliItemInfo*>& items, bool includeEquip = true);

    void NeedSort();


    // 分页
    class Page
    {
    public:
        Page();
        //>重建加载界面
        void Reload(int row, int col, std::vector<CliItemInfo*>& items);
        //>在具体位置添加物品
        void AddItem(CliItemInfo* item, uint32 rowIdx, uint32 colIdx);
        //>通过位置找到物品
        CliItemInfo* GetItem(uint32 rowIdx, uint32 colIdx);
        //通过唯一Id找到物品
        bool FindItem(uint64 guid, uint32& rowIdx, uint32& colIdx);
        //>移除某物品
        bool Remove(uint32 rowIdx, uint32 colIdx);
        //>交换物品
        bool SwapItem(uint32 rowIdx1, uint32 colIdx1, uint32 rowIdx2, uint32 colIdx2);

        bool IsNeedReload() const { return _needReload; }
    private:

        int _row, _col;
        bool _needReload;       //>重新分类
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

