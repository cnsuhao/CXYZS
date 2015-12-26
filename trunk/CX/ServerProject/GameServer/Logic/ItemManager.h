/********************************************************************
created:	2015/7/27
author:		lishihai
desc:		��Ʒ����,������Ʒ����Ʒ����߼�
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

    bool IsEquipment();             //>�Ƿ���װ��

    bool CanUpStar();               //>�Ƿ������
    bool CanCompose();              //>�Ƿ�ɺϳ�
    bool CanPrefix();               //>�Ƿ������ǰ׺

    int GetBattlePower();           //>�����Ʒս����
    int GetComposeSpendMoney();     //>�����Ʒ�ϳ����Ľ��
};

extern const uint64 INVALID_ITEM_GUID;

class ItemManager
{
public:
	ItemManager();
	~ItemManager();

	void Init();				//��ʼ��
    void    SetItemGuid(uint64 itemGuid);

    void    InitBagItems(Player* player, std::vector<ItemInfoIt>& items);       //>��ʼ������Ʒ
    void    InitEquipItems(Player* player, std::vector<ItemInfoIt>& items);     //>��ʼװ����Ʒ
    void    SendBagItemsToClient(Player* player, uint32 operateType);
    void    SendEquipItemsToClient(Player* player, uint32 operateType);

    //>�������
    uint64  AddItem(Player* player, uint32 operateType, uint32 itemId, uint32 number, int32 starNumber = 0);     //>Ϊ�������ָ����������Ʒ,�����ӳɹ�����һ����ЧΨһ��ƷId
    bool    RemoveItem(Player* player, uint32 operateType, uint32 itemId, uint32 number);  //>����ҿ۳�ָ����������Ʒ,����۳��ɹ�����ֵΪ��,��֮Ϊ��
    bool    RemoveItem(Player* player, uint32 operateType, uint64 itemGuid, uint32 number);//>����ҿ۳�ָ����������Ʒ,����۳��ɹ�����ֵΪ��,��֮Ϊ��
    uint64  SplitItem(Player* player, uint32 operateType, uint64 itemGuid, uint32 splitNumber);  //>���һ����Ʒ,����ɹ������һ���µ���ƷΨһId.
    bool    MergeItem(Player* player, uint32 operateType, uint64 itemGuid1, uint64 itemGuid2);        //>�ϲ�������Ʒ,����ɹ�����ֵΪ��. (dwItemId2�������ۼӵ�dwItemId1)
    int32   GetTotalNumber(Player* player, uint32 itemId);      //>�����Ʒ������

    //>�̵�
    bool    BuyShopItem(Player* player, uint32 shopId, uint32 goodesId);
    bool    BuyItem(Player* player, uint32 itemId, uint32 number, uint8 costType, uint32 money);
    bool    SellItem(Player* player, uint64 itemGuid, uint32 number);


    //>�ϳ�
    bool    ComposeItem(Player* player, uint64 mainItemGuid, uint64 stuff1Guid, uint64 stuff2Guid, uint64 stuff3Guid, uint64 stuff4Guid);
    //>����
    bool    UpStarEquipment(Player* player, uint64 equipmentGuid);
    //>����
    bool    InheritEquipment(Player* player, uint64 equipmentGuid, uint64 stuffGuid);

    //>��Ʒʹ��
    bool    ExpendItemUse(Player* player, uint64 itemGuid, int32 number);

	//>��Ʒ����
	void    DropItems(Player* player,int32 dropId,uint32 operateType);

    //>��װ��
    bool    WearEquipment(Player* player, uint32 operateType, uint64 equipmentGuid);
    bool    WearEquipment(Player* player, uint32 operateType, SvrItemInfo* equipment);
    SvrItemInfo* TakeoffEquipment(Player* player, uint32 operateType, EquipType type);
    SvrItemInfo* TakeoffEquipment(Player* player, uint32 operateType, uint64 equipmentGuid);
    bool         TakeoffEquipmentPutInBag(Player* player, uint32 operateType, uint64 equipmentGuid); 

    SvrItemInfo* FindWearEquipment(Player* player, EquipType type);
    SvrItemInfo* FindWearEquipment(Player* player, uint64 equipmentGuid);
    void    UpdateWearEquipment(Player* player, uint32 operateType, SvrItemInfo* equipment);
private:
    SvrItemInfo* FindBagItem(Player* player, uint32 itemId, bool lowLimitOverlap); //>����ģ��Id����һ����Ʒ,lowLimitOverlap����ѡ��δ�ﵽ�ѵ����޵�.
    SvrItemInfo* FindBagItem(Player* player, uint64 itemGuid);                     //>����ΨһId������Ʒ.
    SvrItemInfo* NewItem(Player* player, uint32 itemId, uint32 number, int32 starNumber);                         //>����һ����Ʒ
    SvrItemInfo* NewBagItem(Player* player, uint32 operateType, uint32 itemId, uint32 number, int32 starNumber);  //>Ϊ��Ҵ����������Ʒ������.
    bool         DelBagItem(Player* player, uint32 operateType, SvrItemInfo* pItem, bool removeFromPlayer = true);      //>ɾ��һ��������Ʒ
    void         UpdateBagItem(Player* player, uint32 operateType, SvrItemInfo* pItem);         //>���±�����Ʒ
    bool         AddItem(Player* player, uint32 operateType, SvrItemInfo* pItem);               //>����������Ʒʵ��
    bool         RemoveItem(Player* player, uint32 operateType, SvrItemInfo* pItem);            //>����ɾ����Ʒʵ��

    inline uint64 GetNewItemGuid() { return ++m_ItemGuidCount; }
private:
    uint64 m_ItemGuidCount;
};


extern ItemManager* g_ItemManager;
#endif	//__ITEMMANAGER_H__
