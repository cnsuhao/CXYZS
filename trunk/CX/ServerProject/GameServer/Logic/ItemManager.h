/********************************************************************
created:	2015/7/27
author:		lishihai
desc:		物品管理,处理物品及物品相关逻辑
*********************************************************************/

#ifndef __ITEMMANAGER_H__
#define __ITEMMANAGER_H__

#include "GameDef.h"
#include <vector>

class Player;
struct ItemsInfo;

struct SvrItemInfo
{
    ItemInfoIt Data;
    const ItemsInfo* pCfg;

    bool IsEquipment();             //>是否是装备

    bool CanUpStar();               //>是否可升星
    bool CanCompose();              //>是否可合成
    bool CanPrefix();               //>是否可以有前缀

    int GetBattlePower();           //>获得物品战斗力
    int GetComposeSpendMoney();     //>获得物品合成消耗金币
};

extern const uint64 INVALID_ITEM_GUID;

class ItemManager
{
public:
	ItemManager();
	~ItemManager();

	void Init();				//初始化
    void    SetItemGuid(uint64 itemGuid);

    void    InitBagItems(Player* player, std::vector<ItemInfoIt>& items);       //>初始背包物品
    void    InitEquipItems(Player* player, std::vector<ItemInfoIt>& items);     //>初始装备物品
    void    SendBagItemsToClient(Player* player, uint32 operateType);
    void    SendEquipItemsToClient(Player* player, uint32 operateType);

    //>玩家自身
    uint64  AddItem(Player* player, uint32 operateType, uint32 itemId, uint32 number, int32 starNumber = 0);     //>为玩家增加指定数量的物品,如果添加成功返回一个有效唯一物品Id
    bool    RemoveItem(Player* player, uint32 operateType, uint32 itemId, uint32 number);  //>从玩家扣除指定数量的物品,如果扣除成功返回值为真,反之为假
    bool    RemoveItem(Player* player, uint32 operateType, uint64 itemGuid, uint32 number);//>从玩家扣除指定数量的物品,如果扣除成功返回值为真,反之为假
    uint64  SplitItem(Player* player, uint32 operateType, uint64 itemGuid, uint32 splitNumber);  //>拆分一个物品,如果成功会产生一个新的物品唯一Id.
    bool    MergeItem(Player* player, uint32 operateType, uint64 itemGuid1, uint64 itemGuid2);        //>合并两个物品,如果成功返回值为真. (dwItemId2的数量累加到dwItemId1)
    int32   GetTotalNumber(Player* player, uint32 itemId);      //>获得物品总数量

    //>商店
    bool    BuyShopItem(Player* player, uint32 shopId, uint32 goodesId);
    bool    BuyItem(Player* player, uint32 itemId, uint32 number, uint8 costType, uint32 money);
    bool    SellItem(Player* player, uint64 itemGuid, uint32 number);


    //>合成
    bool    ComposeItem(Player* player, uint64 mainItemGuid, uint64 stuff1Guid, uint64 stuff2Guid, uint64 stuff3Guid, uint64 stuff4Guid);
    //>升星
    bool    UpStarEquipment(Player* player, uint64 equipmentGuid);
    //>传承
    bool    InheritEquipment(Player* player, uint64 equipmentGuid, uint64 stuffGuid);

    //>物品使用
    bool    ExpendItemUse(Player* player, uint64 itemGuid, int32 number);

	//>物品掉落
	void    DropItems(Player* player,int32 dropId,uint32 operateType);

    //>穿装备
    bool    WearEquipment(Player* player, uint32 operateType, uint64 equipmentGuid);
    bool    WearEquipment(Player* player, uint32 operateType, SvrItemInfo* equipment);
    SvrItemInfo* TakeoffEquipment(Player* player, uint32 operateType, EquipType type);
    SvrItemInfo* TakeoffEquipment(Player* player, uint32 operateType, uint64 equipmentGuid);
    bool         TakeoffEquipmentPutInBag(Player* player, uint32 operateType, uint64 equipmentGuid); 

    SvrItemInfo* FindWearEquipment(Player* player, EquipType type);
    SvrItemInfo* FindWearEquipment(Player* player, uint64 equipmentGuid);
    void    UpdateWearEquipment(Player* player, uint32 operateType, SvrItemInfo* equipment);
private:
    SvrItemInfo* FindBagItem(Player* player, uint32 itemId, bool lowLimitOverlap); //>根据模板Id查找一个物品,lowLimitOverlap优先选择未达到堆叠上限的.
    SvrItemInfo* FindBagItem(Player* player, uint64 itemGuid);                     //>根据唯一Id查找物品.
    SvrItemInfo* NewItem(Player* player, uint32 itemId, uint32 number, int32 starNumber);                         //>创建一个物品
    SvrItemInfo* NewBagItem(Player* player, uint32 operateType, uint32 itemId, uint32 number, int32 starNumber);  //>为玩家创建添加新物品到背包.
    bool         DelBagItem(Player* player, uint32 operateType, SvrItemInfo* pItem, bool removeFromPlayer = true);      //>删除一个背包物品
    void         UpdateBagItem(Player* player, uint32 operateType, SvrItemInfo* pItem);         //>更新背包物品
    bool         AddItem(Player* player, uint32 operateType, SvrItemInfo* pItem);               //>并不创建物品实例
    bool         RemoveItem(Player* player, uint32 operateType, SvrItemInfo* pItem);            //>并不删除物品实例

    inline uint64 GetNewItemGuid() { return ++m_ItemGuidCount; }
private:
    uint64 m_ItemGuidCount;
};


extern ItemManager* g_ItemManager;
#endif	//__ITEMMANAGER_H__
