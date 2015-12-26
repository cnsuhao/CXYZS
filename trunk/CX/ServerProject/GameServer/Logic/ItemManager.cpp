#include "ItemManager.h"
#include "CSVFileManager.h"
#include "Player.h"
#include "LogManage.h"
#include "StackStream.h"
#include "MsgInfo.h"
#include "GameServer.h"
#include "SkillTreeManager.h"
#include "TaskManager.h"
#include "Map.h"
#include "CopyManager.h"
#include "BattleManager.h"

bool SvrItemInfo::IsEquipment()
{
    if (pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        return true;
    }
    return false;
}

bool SvrItemInfo::CanUpStar()
{
    if (pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        if (pCfg->iSamllClass == EQUIPTYPE_HEAD || pCfg->iSamllClass == EQUIPTYPE_BODY || pCfg->iSamllClass == EQUIPTYPE_WEAPON ||
            pCfg->iSamllClass == EQUIPTYPE_BELT || pCfg->iSamllClass == EQUIPTYPE_SHOES || pCfg->iSamllClass == EQUIPTYPE_NECKLACE ||
            pCfg->iSamllClass == EQUIPTYPE_RING || pCfg->iSamllClass == EQUIPTYPE_ORNAMENTS )
        {
            return true;
        }
    }
    return false;
}

bool SvrItemInfo::CanCompose()
{
    if (pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        if (pCfg->iSamllClass == EQUIPTYPE_HEAD || pCfg->iSamllClass == EQUIPTYPE_BODY || pCfg->iSamllClass == EQUIPTYPE_WEAPON ||
            pCfg->iSamllClass == EQUIPTYPE_BELT || pCfg->iSamllClass == EQUIPTYPE_SHOES || pCfg->iSamllClass == EQUIPTYPE_NECKLACE ||
            pCfg->iSamllClass == EQUIPTYPE_RING || pCfg->iSamllClass == EQUIPTYPE_ORNAMENTS )
        {
            return true;
        }
    }
    return false;
}

bool SvrItemInfo::CanPrefix()
{
    if (pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        if (pCfg->iSuitID == 0)
        {
            if (pCfg->iSamllClass == EQUIPTYPE_HEAD || pCfg->iSamllClass == EQUIPTYPE_BODY || pCfg->iSamllClass == EQUIPTYPE_WEAPON ||
                pCfg->iSamllClass == EQUIPTYPE_BELT || pCfg->iSamllClass == EQUIPTYPE_SHOES || pCfg->iSamllClass == EQUIPTYPE_NECKLACE ||
                pCfg->iSamllClass == EQUIPTYPE_RING || pCfg->iSamllClass == EQUIPTYPE_ORNAMENTS)
            {
                return true;
            }
        }
    }
    return false;
}

int SvrItemInfo::GetBattlePower()
{
    int battlePower = 0;
    //>计算基础属性
    for (int i = 0; i < 4; ++i)
    {
        int attributeId = pCfg->iBaseAtt[i];
        if (attributeId > 0 && pCfg->iBaseAttParams[i] > 0)
        {
            const ForceForAttribute* attribute = g_CSVFileManager->GetAttributeByID(attributeId);
            if (attribute)
            {
                battlePower += int(attribute->iEffect * pCfg->iBaseAttParams[i]);
            }
        }
    }

    //>计算前缀属性
    if (Data.prefixID != INVALID_ID)
    {
        const EquipmentPrefix* prefix = g_CSVFileManager->GetEquipmentPrefixByID(Data.prefixID);
        if (prefix && prefix->iAddAttribute != INVALID_ID)
        {
            const ForceForAttribute* attribute = g_CSVFileManager->GetAttributeByID(prefix->iAddAttribute);
            if (attribute)
            {
                battlePower += int(attribute->iEffect * prefix->iAddValue);
            }
        }
    }

    return battlePower;
}

int SvrItemInfo::GetComposeSpendMoney()
{
	return int(pCfg->iPicre * g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_COMPOSEEXPEND]);  
}


extern const uint64 INVALID_ITEM_GUID = 0;

ItemManager *g_ItemManager = NULL;

ItemManager::ItemManager()
{
    m_ItemGuidCount = 0;
}

ItemManager::~ItemManager()
{
}

void ItemManager::Init()
{
}

void ItemManager::SetItemGuid(uint64 itemGuid) 
{
    assert(itemGuid >= m_ItemGuidCount);
    m_ItemGuidCount = itemGuid; 
}

void ItemManager::InitBagItems(Player* player, std::vector<ItemInfoIt>& items)
{
    if (!items.empty())
    {
        ItemInfoIt* pInfo = NULL;
        player->m_BagList.resize(items.size());
        for (size_t i = 0; i < items.size(); ++i)
        {
            pInfo = &(items[i]);
            const ItemsInfo* pTpl = g_CSVFileManager->GetItemsByID(pInfo->id);
            assert(pTpl);
            player->m_BagList[i] = new SvrItemInfo();
            player->m_BagList[i]->pCfg = pTpl;
            memcpy(&player->m_BagList[i]->Data, &(items[i]), sizeof(ItemInfoIt));
        }
    }
}

void ItemManager::InitEquipItems(Player* player, std::vector<ItemInfoIt>& items)
{
    if (!items.empty())
    {
        ItemInfoIt* pInfo = NULL; 
        assert(items.size() == EQUIPTYPE_MAX);
        for (int i = 0; i < EQUIPTYPE_MAX; ++i)
        {
            pInfo = &(items[i]);
            if (pInfo->guid != INVALID_ITEM_GUID)
            {   
                player->m_Equip[i] = new SvrItemInfo();
                //设置模板指针方便以后使用
                const ItemsInfo* pTpl = g_CSVFileManager->GetItemsByID(pInfo->id);
                assert(pTpl);
                player->m_Equip[i]->pCfg = pTpl;
                memcpy(&(player->m_Equip[i]->Data), pInfo, sizeof(ItemInfoIt));
            } 
        }
    }
}

void ItemManager::SendBagItemsToClient(Player* player, uint32 operateType)
{
    //>设置头
    MsgItem_Add_Set addMsg;
    addMsg.OBJID = player->m_GUID;
    addMsg.SysID = ID_G2D;
    addMsg.operateType = operateType;
    addMsg.addNumItem = player->m_BagList.size();
    //>写入头到消息流
    StackStream<BAG_ITEMS_STREAM_SZIE> addMsgStream;
    addMsgStream.Wirte(&addMsg, sizeof(addMsg));
    SvrItemInfo* pItem = NULL;
    std::vector<SvrItemInfo*>::iterator it = player->m_BagList.begin();
    for (std::vector<SvrItemInfo*>::iterator end = player->m_BagList.end(); 
         it != end; ++it)
    {
        pItem = *it;

        addMsgStream.Wirte(&pItem->Data.guid, sizeof(uint64));
        addMsgStream.Wirte(&pItem->Data.id, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.prefixID, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.starNum, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.isBind, sizeof(uint8));
        addMsgStream.Wirte(&pItem->Data.num, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.isLock, sizeof(uint8));
        addMsgStream.Wirte(&pItem->Data.time, sizeof(int));
    }

    MsgItem_Add_Set* pAddMsg = (MsgItem_Add_Set*)addMsgStream.Get();
    pAddMsg->Len = addMsgStream.GetSize();
    //>发送到客户端
    g_GameServer->SendToClient(pAddMsg);
}

void ItemManager::SendEquipItemsToClient(Player* player, uint32 operateType)
{
    //>设置头
    MsgEquip_List listMsg;
    listMsg.OBJID = player->m_GUID;
    listMsg.SysID = ID_G2C;
    listMsg.operateType = operateType;
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if (player->m_Equip[i] != NULL)
        {
            memcpy(&listMsg.equipments[i], player->m_Equip[i], sizeof(ItemInfoIt));
        }
        else
        {
            memset(&listMsg.equipments[i], 0, sizeof(ItemInfoIt));
        }
    }
    //>发送到客户端
    g_GameServer->SendToClient(&listMsg);

    //>通知改变外观
    player->OnChangeEquipment();
}

uint64 ItemManager::AddItem(Player* player, uint32 operateType, uint32 itemId, uint32 number, int32 starNumber)
{
    if (player->m_BagList.size() >= 500)
    {
        //>发通知
        return INVALID_ITEM_GUID;
    }

	//!通知任务系统
	g_TaskManager->OnGetItem(itemId, player);           
	const MapInfo* mapInfo = g_CSVFileManager->GetMapByID(player->m_Map->GetMapID());
	if(mapInfo->iMapType == 3)
	{
		g_CopyManager->updateCopyProgress(player,0,itemId);
	}


    SvrItemInfo* pItem = FindBagItem(player, itemId, true);
    if (pItem == NULL)
    {
        pItem = NewBagItem(player, operateType, itemId, number, starNumber);
        return pItem ? pItem->Data.guid : 0;
    }
    else
    {
        if (pItem->Data.num + number > (uint32)pItem->pCfg->iNumLimit)
        {
            if (number == pItem->pCfg->iNumLimit)
            {
                //>如果直接为一组上限物品.那么直接添加新物品
                SvrItemInfo* pNewItem = NewBagItem(player, operateType, itemId, number, starNumber);
                return pNewItem ? pNewItem->Data.guid : 0;
            }
            
            int oldNum = pItem->Data.num;
            int lateNumber = (pItem->Data.num + number) - pItem->pCfg->iNumLimit;
            pItem->Data.num = pItem->pCfg->iNumLimit;
            if (lateNumber > pItem->pCfg->iNumLimit)
            {
                lateNumber = min(lateNumber, pItem->pCfg->iNumLimit);
                gLog.AddLog(LOG_LEVEL_ERROR, "Add number of itemId=%d, overflow the item iNumLimit=%d", itemId, pItem->pCfg->iNumLimit); //!>这里直接预判拆分后直接可以容纳.可能以后要改.
            }

            if(oldNum != pItem->Data.num)
                UpdateBagItem(player, operateType, pItem);

            SvrItemInfo* pNewItem = NewBagItem(player, operateType, itemId, lateNumber, starNumber);
            return pNewItem ? pNewItem->Data.guid : 0;
        }
        else
        {
            pItem->Data.num += number;
            UpdateBagItem(player, operateType, pItem);
            return pItem->Data.guid;
        }
    }
}

bool ItemManager::RemoveItem(Player* player, uint32 operateType, uint32 itemId, uint32 number)
{
    SvrItemInfo* pItem = FindBagItem(player, itemId, true);
    if (pItem)
    {
        if ((uint32)pItem->Data.num > number)
        {
            pItem->Data.num -= number;
            UpdateBagItem(player, operateType, pItem);
            return true;
        }
        else if ((uint32)pItem->Data.num == number)
        {
            DelBagItem(player, operateType, pItem);
            return true;
        }
        else
        {
            unsigned int hasNumber = GetTotalNumber(player, itemId);
            if (hasNumber > number)
            {
                //>优先消耗,未达到堆叠上限的
                number -= pItem->Data.num;
                DelBagItem(player, operateType, pItem);
                //>然后再循环扣除
                for (std::vector<SvrItemInfo*>::iterator it = player->m_BagList.begin(); 
                    it != player->m_BagList.end(); )
                {
                    SvrItemInfo* pItem = *it;
                    if (pItem->pCfg->id == itemId)
                    {
                        if((uint32)pItem->Data.num < number)
                        {
                            number -= pItem->Data.num;
                            DelBagItem(player, operateType, pItem, false); //>这里删除工作在外面来做了
                            delete pItem;
                            it = player->m_BagList.erase(it);
                        }
                        else
                        {
                            pItem->Data.num -= number;
                            UpdateBagItem(player, operateType, pItem);
                            break;
                        }
                    }
                    else
                    {
                        ++it;
                    }
                }
                return true;
            }
            else
            {
                gLog.AddLog(LOG_LEVEL_WARNING, "number not enough by RemoveItem.");
                return false;
            }
        }
    }
    
    return false;
}

bool ItemManager::RemoveItem(Player* player, uint32 operateType, uint64 itemGuid, uint32 number)
{
    SvrItemInfo* pItem = FindBagItem(player, itemGuid);
    if (pItem)
    {
        if (pItem->Data.num >= (int32)number)
        {
            pItem->Data.num -= number;
            if (pItem->Data.num <= 0)
            {
                DelBagItem(player, operateType, pItem);
            }
            else
            {
                UpdateBagItem(player, operateType, pItem);
            }
            return true;
        }
    }
    return false;
}

uint64 ItemManager::SplitItem(Player* player, uint32 operateType, uint64 itemGuid, uint32 splitNumber)
{
    SvrItemInfo* pItem = FindBagItem(player, itemGuid);
    if (pItem && ((uint32)pItem->Data.num > splitNumber))
    {
        pItem->Data.num -= splitNumber;
        UpdateBagItem(player, operateType, pItem);
        SvrItemInfo* pNewItem = NewBagItem(player, operateType, pItem->pCfg->id, splitNumber, pItem->Data.starNum);
        return pNewItem ? pNewItem->Data.guid : 0;
    }

    return 0;
}

bool ItemManager::MergeItem(Player* player, uint32 operateType, uint64 itemGuid1, uint64 itemGuid2)
{

    SvrItemInfo* pItem1 = FindBagItem(player, itemGuid1);
    SvrItemInfo* pItem2 = FindBagItem(player, itemGuid2);

    if (pItem1->Data.num == pItem1->pCfg->iNumLimit)
    {
        return true;
    }

    if (pItem1 && pItem2 && pItem1->pCfg->id == pItem2->pCfg->id)
    {
        if (pItem1->Data.num + pItem2->Data.num <= pItem1->pCfg->iNumLimit)
        {
            pItem1->Data.num += pItem2->Data.num;
            UpdateBagItem(player, operateType, pItem1);
            DelBagItem(player, operateType, pItem2);
            return true;
        }
        else
        {
            pItem2->Data.num -= pItem1->pCfg->iNumLimit - pItem1->Data.num;
            UpdateBagItem(player, operateType, pItem2);
            pItem1->Data.num = pItem1->pCfg->iNumLimit;
            UpdateBagItem(player, operateType, pItem1);
            return true;
        }
    }
    
    return false;
}

int32 ItemManager::GetTotalNumber(Player* player, uint32 itemId)
{
    int32 totalNumber = 0;
    std::vector<SvrItemInfo*>::iterator end = player->m_BagList.end();
    for (std::vector<SvrItemInfo*>::iterator it = player->m_BagList.begin(); 
        it != end; ++it)
    {
        SvrItemInfo* pItem = *it;
        if(pItem->pCfg->id == itemId)
        {
            totalNumber += pItem->Data.num;
        }
    }

    return totalNumber;
}

bool ItemManager::BuyShopItem(Player* player, uint32 shopId, uint32 goodesId)
{
    const std::map<int32,ShopItem>* pGoodses = g_CSVFileManager->GetShopItemByID(shopId);
    const Shop* pShopTpl = g_CSVFileManager->GetShopByID(shopId);
    if (pGoodses && pShopTpl)
    {

        //int vipLevel = player->m_BaseInfo.vipLevel;
        //pShopTpl->bIsVip;

        std::map<int32,ShopItem>::const_iterator it = pGoodses->find(goodesId);
        if (it != pGoodses->end())
        {
            const ShopItem& goodsesTpl = it->second;
            assert(goodsesTpl.iNumber > 0);
            return BuyItem(player, goodsesTpl.iItemId, goodsesTpl.iNumber, pShopTpl->iCurrencyType, goodsesTpl.iPrice);
        }else
        {
            gLog.Info("unknown shopId[%d] or goodesId[%d] when call BuyItem", shopId, goodesId);
            return false;
        }
    }
    else
    {
        gLog.Info("unknown shopId[%d] or goodesId[%d] when call BuyItem", shopId, goodesId);
    }
    return false;
}

bool ItemManager::BuyItem(Player* player, uint32 itemId, uint32 number, uint8 costType, uint32 price)
{
    int money = price * number;
    //!>特殊处理,以后删掉
    if (itemId == 73000001)
    {
        money = 0;
    }

    bool canBuy = false;
    switch (costType)
    {
    case ATTRIBUTE_MONEY: //>金币
        {
            int hasMoney = player->GetMoney();
            if (hasMoney >= money)
            {
                canBuy = player->SpendMoney(money);
            }
            else
            {
                gLog.Info("money not enough[has:%d, spend:%d] when call BuyItem.", hasMoney, money);
            }
        }break;
    case ATTRIBUTE_GOLD: //>元宝
        {
            int hasGold = player->GetGold();
            if (hasGold >= money)
            {
                canBuy = player->SpendGold(money);
            }else
            {
                gLog.Info("gold not enough[has:%d, spend:%d] when call BuyItem.", hasGold, money);
            }
        }break;
    case ATTRIBUTE_BINDGOLD: //>绑定元宝
        {
            int hasBindGold = player->GetBindGold();
            if (hasBindGold >= money)
            {
                canBuy = player->SpendBindGold(money);
            }else
            {
                gLog.Info("bind gold not enough[has:%d, spend:%d] when call BuyItem.", hasBindGold, money);
            }
        }break;
    case ATTRIBUTE_ARENAMONEY: //>竞技场货币
        {
            int hasArenaMoney = player->GetArenaMoney();
            if (hasArenaMoney >= money)
            {
                canBuy = player->SpendArenaMoney(money);
            }else
            {
                gLog.Info("arena money not enough[has:%d, spend:%d] when call BuyItem.", hasArenaMoney, money);
            }
        }break;
    case ATTRIBUTE_BATTLEFIELDMONEY : //>战场货币数
        {
            int hasBattlefieldMoney = player->GetBattlefieldMoney();
            if (hasBattlefieldMoney >= money)
            {
                canBuy = player->SpendBattlefieldMoney(money);
            }else
            {
                gLog.Info("battlefield money not enough[has:%d, spend:%d] when call BuyItem.", hasBattlefieldMoney, money);
            }
        }break;
    default:
        gLog.AddLog(LOG_LEVEL_WARNING, "unknown cost type[%d] when call BuyItem", costType);
        break;
    }

    if (canBuy)
    {
        return INVALID_ITEM_GUID != AddItem(player, ItemOperate_BuyMallItem, itemId, number);
    }

    return false;
}

bool ItemManager::SellItem(Player* player, uint64 itemGuid, uint32 number)
{
    SvrItemInfo* pItem = FindBagItem(player, itemGuid);
    if (pItem)
    {
        const ItemsInfo* pInfo = g_CSVFileManager->GetItemsByID(pItem->pCfg->id);
        if (pInfo)
        {
            bool br = RemoveItem(player, ItemOperate_SellBagItem, itemGuid, number);
            if(br)
            {
                player->AddMoney(pInfo->iPicre * number);
            }
            return br;
        }
    }
    else
    {
        SvrItemInfo* pWearEquipment = FindWearEquipment(player, itemGuid);
        if (pWearEquipment)
        {
            SvrItemInfo* pItem = TakeoffEquipment(player, ItemOperate_SellBagItem, itemGuid);
            if (pItem)
            {
                const ItemsInfo* pInfo = g_CSVFileManager->GetItemsByID(pItem->pCfg->id);
                if (pInfo)
                {
                    delete pItem;   //卖到商店.直接删除了.
                    player->AddMoney(pInfo->iPicre);
                    return true;
                }
            }
        }
    }

    return false;
}

bool ItemManager::ComposeItem(Player* player, uint64 mainItemGuid, uint64 stuff1Guid, uint64 stuff2Guid, uint64 stuff3Guid, uint64 stuff4Guid)
{
    assert(player);
    //>查找主材料
    bool isWearing = false;
    SvrItemInfo* pMainItem = FindBagItem(player, mainItemGuid);
    if (pMainItem == NULL)
    {
        pMainItem = FindWearEquipment(player, mainItemGuid);
        if (pMainItem == NULL)
        {
            gLog.AddLog(LOG_LEVEL_WARNING, "can't find item[%llu] when call ComposeItem.", mainItemGuid);
            return false;
        }
        isWearing = true;
    }

    if (pMainItem->pCfg->iCompositeID == INVALID_ITEM_GUID) //>主材料必须有合成目标
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "not has compose target for item[%llu] when call ComposeItem.", mainItemGuid);
        return false;
    }

    if (pMainItem->pCfg->iSuitID != 0)  //>主材料不能是套装
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "main stuff[%llu] can't be equip-set when call ComposeItem.", mainItemGuid);
        return false;
    }

    //>查找材料
    SvrItemInfo* pStuff1 = FindBagItem(player, stuff1Guid);
    SvrItemInfo* pStuff2 = FindBagItem(player, stuff2Guid);
    SvrItemInfo* pStuff3 = FindBagItem(player, stuff3Guid);
    SvrItemInfo* pStuff4 = FindBagItem(player, stuff4Guid);

    if (pStuff1 == NULL || pStuff2 == NULL || pStuff3 == NULL || pStuff4 == NULL)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't find stuff in bag when call ComposeItem.", mainItemGuid);
        return false;
    }

    //bool hasBindStuff = false;
    //SvrItemInfo* pStuffs[4] = { pStuff1, pStuff2, pStuff3, pStuff4};
    //for (int i = 0; i < 4; ++i)
    //{
    //    if(pStuffs[i]->Data.isBind){
    //        hasBindStuff = true;
    //        break;
    //    }
    //}

    //>检测是否同品质
    bool isSameQuality = false;
    if(pMainItem->pCfg->iQuality == pStuff1->pCfg->iQuality)
    {
        if (pMainItem->pCfg->iQuality == pStuff2->pCfg->iQuality)
        {
            if (pMainItem->pCfg->iQuality == pStuff3->pCfg->iQuality)
            {
                if (pMainItem->pCfg->iQuality == pStuff4->pCfg->iQuality)
                {
                    isSameQuality = true;
                }
            }
        }
    }
    if (false == isSameQuality)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't compose because stuff is not same quality when call ComposeItem.", mainItemGuid);
        return false;
    }

    int hasMoney = player->GetMoney();
    int spendMoney = pMainItem->GetComposeSpendMoney();
    if (spendMoney > hasMoney)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't compose because money not enough[has:%d, spend:%d] when call ComposeItem.", hasMoney, spendMoney);
        return false;
    }

    //>如果主物品是绑定物品.合成物品也为绑定物品.


    //>删除合成材料
    bool rsm;
    uint32 compositeID = pMainItem->pCfg->iCompositeID;
    int32 starNumber = pMainItem->Data.starNum;
    pMainItem = NULL; //>会被删除.不在用pMainItem指针.
    if (false == isWearing)
    {
        rsm = RemoveItem(player, ItemOperate_ItemMerge, mainItemGuid, 1);
    }
    else
    {
        SvrItemInfo* pTakeoffEquipment = TakeoffEquipment(player, ItemOperate_ItemMerge, mainItemGuid);
        rsm = pTakeoffEquipment != NULL;
        if (pTakeoffEquipment)
        {
            delete pTakeoffEquipment;   //>从身上脱掉直接删除.
        }
    }
    bool rs1 = RemoveItem(player, ItemOperate_ItemMerge, stuff1Guid, 1);
    bool rs2 = RemoveItem(player, ItemOperate_ItemMerge, stuff2Guid, 1);
    bool rs3 = RemoveItem(player, ItemOperate_ItemMerge, stuff3Guid, 1);
    bool rs4 = RemoveItem(player, ItemOperate_ItemMerge, stuff4Guid, 1);

    //>消耗金钱
    player->SpendMoney(spendMoney);

    if (rsm && rs1 && rs2 && rs3 && rs4)
    {
        if (false == isWearing)
        {
            //>添加新合成物品
            uint64 guid = AddItem(player, ItemOperate_ItemMerge, compositeID, 1, starNumber); // if hasBindStuff add bind flag
            if (guid == INVALID_ITEM_GUID)
            {
                gLog.AddLog(LOG_LEVEL_WARNING, "create new item failure and spend stuffs[%llu,%llu,%llu,%llu,%llu] success when call ComposeItem.", mainItemGuid, stuff1Guid, stuff2Guid, stuff3Guid, stuff4Guid);
                return false;
            }
        }
        else
        {
            SvrItemInfo* pNewItem = NewItem(player, compositeID, 1, starNumber);
            if (pNewItem)
            {
                bool rs = WearEquipment(player, ItemOperate_ItemMerge, pNewItem);
                if (rs)
                    player->OnEquipmentCompose(pNewItem);

                return rs;
            }
        }

        return true;
    }

    return false;
}

bool ItemManager::UpStarEquipment(Player* player, uint64 equipmentGuid)
{
    assert(player);
    bool isWearing = false;
    SvrItemInfo* pMainItem = FindBagItem(player, equipmentGuid);
    if (pMainItem == NULL)
    {
        pMainItem = FindWearEquipment(player, equipmentGuid);
        if (pMainItem == NULL)
        {
            gLog.AddLog(LOG_LEVEL_WARNING, "can't find item[%llu] when call UpStarEquipment.", equipmentGuid);
            return false;
        }
        isWearing = true;
    }

    if (!pMainItem->CanUpStar())
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't upStar equipmentGuid[%llu] when call UpStarEquipment.", equipmentGuid);
        return false;
    }
    
    const EquipmentUpgrade* pUpgradeTpl = g_CSVFileManager->GetEquipmentUpgradeByID(pMainItem->Data.starNum + 1);
    if (pUpgradeTpl == NULL)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't find EquipmentUpgrade table data when call UpStarEquipment. star level[%d] ", pMainItem->Data.starNum + 1);
        return false;
    }

    if (pUpgradeTpl->iMaterialNum > GetTotalNumber(player, pUpgradeTpl->iMaterialid))
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "stuff number not enough by upStar has[%d] need[%d].", GetTotalNumber(player, pUpgradeTpl->iMaterialid), pUpgradeTpl->iMaterialNum);
        return false;
    }

    int hasMoney = player->GetMoney();
    int spendMoney = pUpgradeTpl->iCurrency;
    if (spendMoney > hasMoney)
    {
        gLog.Info("money not enough by upStar has[%d] need[%d].", hasMoney, spendMoney);
        return false;
    }
    else
    {
        bool rs = player->SpendMoney(spendMoney);
        assert(rs);
    }

    bool rs = RemoveItem(player, ItemOperate_EquipUpLevel, (uint32)pUpgradeTpl->iMaterialid, (uint32)pUpgradeTpl->iMaterialNum);
    if (rs)
    {
        int chance = (int)pUpgradeTpl->fChance;
        srand(GetTickCount());
        int rand = BaseFunction::Random(0, 10000);
        if (rand < chance)
        {
            pMainItem->Data.starNum += 1;
            if (false == isWearing)
            {
                UpdateBagItem(player, ItemOperate_EquipUpLevel, pMainItem); 
            }
            else
            {
                UpdateWearEquipment(player, ItemOperate_EquipUpLevel, pMainItem);
                player->OnEquipmentUpstart(pMainItem);
            }
        }
        else
        {
            MsgNotify notify(ID_G2C, Notify_S2C_EquipmentUpStar_Failture);
            notify.OBJID = player->m_GUID;
            g_GameServer->SendToClient(&notify);
        }

        return true;
    }
    return false;
}

bool ItemManager::InheritEquipment(Player* player, uint64 equipmentGuid, uint64 stuffGuid)
{
    assert(player);
    bool isMainWearing = false;
    SvrItemInfo* pMainItem = FindBagItem(player, equipmentGuid);
    if (pMainItem == NULL)
    {
        pMainItem = FindWearEquipment(player, equipmentGuid);
        if (pMainItem == NULL)
        {
            gLog.AddLog(LOG_LEVEL_WARNING, "can't find item[%llu] when call InheritEquipment.", equipmentGuid);
            return false;
        }
        isMainWearing = true;
    }

    if (!pMainItem->CanUpStar())
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't upStar equipmentGuid[%llu] when call InheritEquipment.", equipmentGuid);
        return false;
    }

    bool isStuffWearing = false;
    SvrItemInfo* pStuffItem = FindBagItem(player, stuffGuid);
    if (pStuffItem == NULL)
    {
        pStuffItem = FindWearEquipment(player, stuffGuid);
        if (pStuffItem == NULL)
        {
            gLog.AddLog(LOG_LEVEL_WARNING, "can't find item[%llu] when call InheritEquipment.", stuffGuid);
            return false;
        }
        isStuffWearing = true;
    }

    if (!pStuffItem->CanUpStar())
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't upStar stuffGuid[%llu] when call InheritEquipment.", stuffGuid);
        return false;
    }

    if (pStuffItem->Data.starNum <= pMainItem->Data.starNum)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "stuff equipment' star number must big than inherit target equipment.");
        return false;
    }

    //> 优先消耗绑定元宝

    //> 如果是绑定材料,将导致被传承的物品变为绑定

    pMainItem->Data.starNum = pStuffItem->Data.starNum;
    pStuffItem->Data.starNum = 0;

    if (pStuffItem->Data.isBind && !pMainItem->Data.isBind){
        pMainItem->Data.isBind = true;
    }

    if (isMainWearing)
    {
        UpdateWearEquipment(player, ItemOperate_EquipInherite, pMainItem);
        player->OnEquipmentInherit();
    }
    else
        UpdateBagItem(player, ItemOperate_EquipInherite, pMainItem);

    if (isStuffWearing)
    {
        UpdateWearEquipment(player, ItemOperate_EquipInherite, pStuffItem);
        player->OnEquipmentInherit();    
    }
    else
        UpdateBagItem(player, ItemOperate_EquipInherite, pStuffItem);


    return true;
}

void ItemManager::DropItems(Player* player,int32 dropId,uint32 operateType)
{
	assert(player);
	srand(GetTickCount());

	int32 dropWeight = 0;
	std::map<int32,std::vector<Drop>>::iterator it = g_CSVFileManager->m_DropMap.find(dropId);

	if(it != g_CSVFileManager->m_DropMap.end())
	{
		std::vector<Drop>::iterator itDrop;
		for(itDrop = it->second.begin();itDrop != it->second.end();itDrop++)
		{
			if(itDrop->iDroptype == 2)
			{
				int32 randNum = BaseFunction::Random(0,32766);
				if(randNum <= itDrop->iDropProbability)
				{
					AddItem(player,operateType,itDrop->iItemid,itDrop->iItemNum);
					gLog.Info("玩家[%s]获得物品[%d],数量[%d]", player->m_Name,itDrop->iItemid,itDrop->iItemNum);
				}
			}
			else
			{
				dropWeight  += itDrop->iDropProbability;
			}
		}
	}
	else
	{
		return;
	}
	if(dropWeight == 0)
	{
		return;
	}
	int32 startNum = 0;
	int randNum = BaseFunction::Random(0,dropWeight);

	std::vector<Drop>::iterator itDrop;
	for(itDrop = it->second.begin();itDrop != it->second.end();itDrop++)
	{
		if( randNum >= startNum && randNum <= (startNum + itDrop->iDropProbability))
		{
			AddItem(player,operateType,itDrop->iItemid,itDrop->iItemNum);
			gLog.Info("玩家[%s]获得物品[%d],数量[%d]", player->m_Name,itDrop->iItemid,itDrop->iItemNum);
			return;
		}
		startNum += itDrop->iDropProbability;
	}
}

bool ItemManager::ExpendItemUse(Player* player, uint64 itemGuid, int32 number)
{
    assert(player);

    SvrItemInfo* pBagItem = FindBagItem(player, itemGuid);
    if (pBagItem == NULL)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't find item[%llu] when call ItemUse.", itemGuid);
        return false;
    }

    if (!pBagItem->pCfg->bIsUse)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "item[%llu] can't be use when call ItemUse.", itemGuid);
        return false;
    }

    if (pBagItem->pCfg->iBigClass != ITEMTYPE_EXPEND)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "item[%llu] isn't expend type when call ItemUse.", itemGuid);
        return false;
    }

    if (!(pBagItem->Data.num >= number))
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "item[%llu] number not enough when call ItemUse.", itemGuid);
        return false;
    }

    ExpendType expendType = (ExpendType)pBagItem->pCfg->iSamllClass;
    bool usedSuccess = false;
    float param1 = pBagItem->pCfg->fParams[0];
    float param2 = pBagItem->pCfg->fParams[1];
    float param3 = pBagItem->pCfg->fParams[2];
    switch (expendType)
    {
    case EXPENDTYPE_CONTINUED:  //>持续回血药
        {
            if (player->IsHpCDCooled())
            {
                player->StartHpCD();
                //player->AddBuff((int)param1);
				//player->OnObjBuff(player,(int)param1,0);
				//player->LoadPlayerOtherAttribute();
                usedSuccess = true;
            }else
            {
                MsgNotify notify(ID_G2C, Notify_S2C_ItemUse_CD_Cooling);
                notify.OBJID = player->m_GUID;
                g_GameServer->SendToClient(&notify);
            }
        }break;
    case EXPENDTYPE_MOMENT: //>瞬间回血药
        {
            if (player->IsHpCDCooled())
            {
                player->StartHpCD();
                player->ChangePlayerAttribute(ATTRIBUTE_HP, param1);
                gLog.Info("玩家[%s]瞬间回血%d", player->m_Name, param1);
                usedSuccess = true;
            }
            else
            {
                MsgNotify notify(ID_G2C, Notify_S2C_ItemUse_CD_Cooling);
                notify.OBJID = player->m_GUID;
                g_GameServer->SendToClient(&notify);
            }
        }break;
    case EXPENDTYPE_CONTINUEDMP: //>持续回蓝药
        {
            if (player->IsMpCDCooled())
            {
                player->StartMpCD();
                //player->AddBuff((int)param1);
				//player->OnObjBuff(player,(int)param1,0);
                usedSuccess = true;
            }else
            {
                MsgNotify notify(ID_G2C, Notify_S2C_ItemUse_CD_Cooling);
                notify.OBJID = player->m_GUID;
                g_GameServer->SendToClient(&notify);
            }
        }break;
    case EXPENDTYPE_MOMENTMP: //>瞬间回蓝药
        {
            if (player->IsMpCDCooled())
            {
                player->StartMpCD();
                player->ChangePlayerAttribute(ATTRIBUTE_MP, param1);
                gLog.Info("玩家[%s]瞬间回蓝%d", player->m_Name, param1);
                usedSuccess = true;
            }else
            {
                MsgNotify notify(ID_G2C, Notify_S2C_ItemUse_CD_Cooling);
                notify.OBJID = player->m_GUID;
                g_GameServer->SendToClient(&notify);
            }
        }break;
    case EXPENDTYPE_BUFF:  //>获得BUFF
        {

            // usedSuccess = g_BattleManager->AddBuff(player, iParam1);
			/*
            player->AddBuff((int)param1);
			const BuffInfo* buffInfo = g_CSVFileManager->GetBuffByID((int)param1);
			if(buffInfo->iEffectType1 == ATTRIBUTE_EXPMULTIPLE)
			{
				player->m_Attribute[ATTRIBUTE_EXPMULTIPLE].SetBuffValue(buffInfo->fEffectValue1);
			}
			if(buffInfo->iEffectType2 == ATTRIBUTE_EXPMULTIPLE)
			{
				player->m_Attribute[ATTRIBUTE_EXPMULTIPLE] .SetBuffValue(buffInfo->fEffectValue2);
			}
			if(buffInfo->iEffectType1 == ATTRIBUTE_SKILLEXPMULTIPLE)
			{
				player->m_Attribute[ATTRIBUTE_SKILLEXPMULTIPLE].SetBuffValue(buffInfo->fEffectValue1);
			}
			if(buffInfo->iEffectType2 == ATTRIBUTE_SKILLEXPMULTIPLE)
			{
				player->m_Attribute[ATTRIBUTE_SKILLEXPMULTIPLE].SetBuffValue(buffInfo->fEffectValue2);
			}
            */
			usedSuccess = true;
        }break;
    case EXPENDTYPE_EXP: //>人物经验丹
        {
            if (1 /*player is not max level*/)
            {
                player->ChangePlayerAttribute(ATTRIBUTE_EXP, param1);
                usedSuccess = true;
            }
        }break;
    case EXPENDTYPE_SKILLTREEEXP: //>功法经验丹
        {
            //usedSuccess = g_SkillManager->AddSkillTreeExp(player, player->m_activeSkillType, (uint32)param1);
        }break;
    case EXPENDTYPE_MAGICWEAPONEXP: //>法宝经验丹
        {
            //usedSuccess = g_SkillTreeManager->AddMagicWeaponExp(player, iParam1, iParam2);
        }break;
    case EXPENDTYPE_RealmExp: //>境界经验丹
        {
             //usedSuccess = player->RealmExp(iParam1);
        }break;
    case EXPENDTYPE_SKILLBOOK: //>功法书
        {
            //usedSuccess = g_SkillManager->AddSkillTree(player, (int32)param1);
        }break;
    case EXPENDTYPE_MAGICWEAPONBOOK: //>法宝书
        {
            //usedSuccess = g_SkillTreeManager->AddMagicWeapon(player, iParam1);
        }break;
    case EXPENDTYPE_SPIRITSBOOK: //>器灵书
        {
            //usedSuccess = xxxxxxxx->AddSpriteBook(player, iParam1);
        }break;
    case EXPENDTYPE_PACKAGE: //>礼包
        {
            //usedSuccess == OpenPackage(palyer, iParam1);
        }break;
    case EXPENDTYPE_MONEY: //>游戏币兑换
        {
            player->AddMoney(int(param1));
            usedSuccess = true;
        }break;
    case EXPENDTYPE_GOLD: //>充值元宝兑换
        {
            player->AddGold(int(param1));
            usedSuccess = true;
        }break;
    case EXPENDTYPE_BINDGOLD: //>绑定元宝兑换
        {
            player->AddBindGold(int(param1));
            usedSuccess = true;
        }break;
    case EXPENDTYPE_ARENAMONEY: //>竞技场货币兑换
        {
            player->AddArenaMoney(int(param1));
            usedSuccess = true;
        }break;
    case EXPENDTYPE_BATTLEFIELDMONEY: //>战场货币兑换
        {
            player->AddBattlefieldMoney(int(param1));
            usedSuccess = true;
        }break;
    case EXPENDTYPE_MAX:
        gLog.AddLog(LOG_LEVEL_WARNING, "unknown type of expend [%d] when call ItemUse.", expendType);
        break;
    }

    if (usedSuccess)
    {
        return RemoveItem(player, ItemOperate_ItemUsed, itemGuid, number);
    }

    return usedSuccess;
}

bool ItemManager::WearEquipment(Player* player, uint32 operateType, uint64 equipmentGuid)
{
    assert(player);

    SvrItemInfo* pEquipment = FindBagItem(player, equipmentGuid);
    if (pEquipment == NULL)
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "can't find equipment[%llu] when call WearEquipment.", equipmentGuid);
        return false;
    }
    
    if (!pEquipment->IsEquipment())
    {
        gLog.AddLog(LOG_LEVEL_WARNING, "not equipment for itemGuid[%llu] when call WearEquipment.", equipmentGuid);
        return false;
    }

    //>从背包移除.
    bool rs = RemoveItem(player, operateType, pEquipment);
    if (rs)
    {
        //>放入背包
        return WearEquipment(player, operateType, pEquipment);
    }
    return false;
}

bool ItemManager::WearEquipment(Player* player, uint32 operateType, SvrItemInfo* pEquipment)
{

    EquipType type = (EquipType)pEquipment->pCfg->iSamllClass;
    SvrItemInfo* pWearEquipment = FindWearEquipment(player, type);
    if (pWearEquipment)
    {
        bool bs = TakeoffEquipmentPutInBag(player, ItemOperate_EquipGetOff, pWearEquipment->Data.guid);
        assert(bs);
    }

    player->m_Equip[type] = pEquipment;

    //>通知改变外观
    player->OnChangeEquipment();
	//>计算穿上新装备后的玩家属性
	const ItemsInfo* itemInfo = g_CSVFileManager->GetItemsByID(pEquipment->Data.id);
	if(itemInfo)
	{
		for(int i = 0;i < 4; i++)
		{
			switch (itemInfo->iBaseAtt[i])
			{
			case Attribute_Equipment_HpMaxValue:
				player->m_Attribute[ATTRIBUTE_HPMAX].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_HpMaxPercent:
				player->m_Attribute[ATTRIBUTE_HPMAX].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_MpMaxValue:
				player->m_Attribute[ATTRIBUTE_MPMAX].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_MpMaxPercent:
				player->m_Attribute[ATTRIBUTE_MPMAX].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_AttackValue:
				player->m_Attribute[ATTRIBUTE_ATTACK].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_AttackPercent:
				player->m_Attribute[ATTRIBUTE_ATTACK].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_DefenseValue:
				player->m_Attribute[ATTRIBUTE_DEFENSE].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_DefensePercent:
				player->m_Attribute[ATTRIBUTE_DEFENSE].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_HitValue:
				player->m_Attribute[ATTRIBUTE_HIT].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_HitPercent:
				player->m_Attribute[ATTRIBUTE_HIT].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_DodgeValue:
				player->m_Attribute[ATTRIBUTE_DODGE].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_DodgePercent:
				player->m_Attribute[ATTRIBUTE_DODGE].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_CritValue:
				player->m_Attribute[ATTRIBUTE_CRIT].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_CritPercent:
				player->m_Attribute[ATTRIBUTE_CRIT].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_TenaCityValue:
				player->m_Attribute[ATTRIBUTE_TENACITY].AddValue(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_TenaCityPercent:
				player->m_Attribute[ATTRIBUTE_TENACITY].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_HitRatio:
				player->m_Attribute[ATTRIBUTE_HIT_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_DodgeRatio:
				player->m_Attribute[ATTRIBUTE_DODGE_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_CritRatio:
				player->m_Attribute[ATTRIBUTE_CRIT_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_TenaCityRatio:
				player->m_Attribute[ATTRIBUTE_TENACITY_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_MoveSpeedOdds:
				player->m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_HurtStateOdds:
				player->m_Attribute[ATTRIBUTE_HUR_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_AttackStateOdds:
				player->m_Attribute[ATTRIBUTE_ATK_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_TreatStateOdds:
				player->m_Attribute[ATTRIBUTE_TENACITY_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			case Attribute_Equipment_DoubleCritOdds:
				player->m_Attribute[ATTRIBUTE_CRIT_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		gLog.Warning("Player:[%llu] can't find Item[%d] from Table!",player->m_GUID,pEquipment->Data.id);
		return false;
	}

    MsgEquip_Update updateMsg;
    updateMsg.operateType = operateType;
    updateMsg.equipmentType = type;
    updateMsg.OBJID = player->m_GUID;
    memcpy(&updateMsg.equipment, &(pEquipment->Data), sizeof(ItemInfoIt));

    //>发送到数据库
    g_GameServer->SendToDB(&updateMsg);
    //>发送到客户端
    g_GameServer->SendToClient(&updateMsg);
    return true;
}

SvrItemInfo* ItemManager::TakeoffEquipment(Player* player, uint32 operateType, EquipType type)
{
    assert(type < EQUIPTYPE_MAX);
    if (player->m_Equip[type] != NULL)
    {
        SvrItemInfo* pItem = player->m_Equip[type];

        //>置空
        player->m_Equip[type] = NULL;

        //>通知改变外观
        player->OnChangeEquipment();

		const ItemsInfo* itemInfo = g_CSVFileManager->GetItemsByID(pItem->Data.id);
		if(itemInfo)
		{
			for(int i = 0;i < 4; i++)
			{
				switch (itemInfo->iBaseAtt[i])
				{
				case Attribute_Equipment_HpMaxValue:
					player->m_Attribute[ATTRIBUTE_HPMAX].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_HpMaxPercent:
					player->m_Attribute[ATTRIBUTE_HPMAX].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_MpMaxValue:
					player->m_Attribute[ATTRIBUTE_MPMAX].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_MpMaxPercent:
					player->m_Attribute[ATTRIBUTE_MPMAX].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_AttackValue:
					player->m_Attribute[ATTRIBUTE_ATTACK].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_AttackPercent:
					player->m_Attribute[ATTRIBUTE_ATTACK].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_DefenseValue:
					player->m_Attribute[ATTRIBUTE_DEFENSE].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_DefensePercent:
					player->m_Attribute[ATTRIBUTE_DEFENSE].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_HitValue:
					player->m_Attribute[ATTRIBUTE_HIT].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_HitPercent:
					player->m_Attribute[ATTRIBUTE_HIT].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_DodgeValue:
					player->m_Attribute[ATTRIBUTE_DODGE].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_DodgePercent:
					player->m_Attribute[ATTRIBUTE_DODGE].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_CritValue:
					player->m_Attribute[ATTRIBUTE_CRIT].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_CritPercent:
					player->m_Attribute[ATTRIBUTE_CRIT].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_TenaCityValue:
					player->m_Attribute[ATTRIBUTE_TENACITY].AddValue(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_TenaCityPercent:
					player->m_Attribute[ATTRIBUTE_TENACITY].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_HitRatio:
					player->m_Attribute[ATTRIBUTE_HIT_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_DodgeRatio:
					player->m_Attribute[ATTRIBUTE_DODGE_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_CritRatio:
					player->m_Attribute[ATTRIBUTE_CRIT_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_TenaCityRatio:
					player->m_Attribute[ATTRIBUTE_TENACITY_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_MoveSpeedOdds:
					player->m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_HurtStateOdds:
					player->m_Attribute[ATTRIBUTE_HUR_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_AttackStateOdds:
					player->m_Attribute[ATTRIBUTE_ATK_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_TreatStateOdds:
					player->m_Attribute[ATTRIBUTE_TENACITY_PROB].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				case Attribute_Equipment_DoubleCritOdds:
					player->m_Attribute[ATTRIBUTE_CRIT_FACTOR].AddPercent(itemInfo->iBaseAttParams[i]);
					break;
				default:
					break;
				}
			}
		}
		else
		{
			gLog.Warning("Player:[%llu] can't find Item[%d] from Table!",player->m_GUID,pItem->Data.id);
			return false;
		}
        MsgEquip_Update updateMsg;
        updateMsg.operateType = operateType;
        updateMsg.equipmentType = type;
        updateMsg.OBJID = player->m_GUID;
        memset(&updateMsg.equipment, 0, sizeof(ItemInfoIt));
        //>发送到数据库
        g_GameServer->SendToDB(&updateMsg);
        //>发送到客户端
        g_GameServer->SendToClient(&updateMsg);

        return pItem;
    }
    return NULL;
}

SvrItemInfo* ItemManager::TakeoffEquipment(Player* player, uint32 operateType, uint64 equipmentGuid)
{
    assert(player);
    SvrItemInfo* pItem = FindWearEquipment(player, equipmentGuid);
    if (pItem != NULL)
    {
        return TakeoffEquipment(player, operateType, (EquipType)pItem->pCfg->iSamllClass);
    }
    return NULL;
}

bool ItemManager::TakeoffEquipmentPutInBag(Player* player, uint32 operateType, uint64 equipmentGuid)
{
    SvrItemInfo* pEquipment = TakeoffEquipment(player, operateType, equipmentGuid);
    if (pEquipment)
        return AddItem(player, operateType, pEquipment);
     
    return false;
}

SvrItemInfo* ItemManager::FindWearEquipment(Player* player, EquipType type)
{
    if (type < EQUIPTYPE_MAX)
    {
        return player->m_Equip[type];
    }
    return NULL;
}

SvrItemInfo* ItemManager::FindWearEquipment(Player* player, uint64 equipmentGuid)
{
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if (player->m_Equip[i] && player->m_Equip[i]->Data.guid == equipmentGuid)
        {
            return player->m_Equip[i];
        }
    }
    return NULL;
}

void ItemManager::UpdateWearEquipment(Player* player, uint32 operateType, SvrItemInfo* pEquipment)
{
    assert(pEquipment);
    EquipType type = (EquipType)pEquipment->pCfg->iSamllClass;
    MsgEquip_Update updateMsg;
    updateMsg.operateType = operateType;
    updateMsg.equipmentType = type;
    updateMsg.OBJID = player->m_GUID;
    memcpy(&updateMsg.equipment, &pEquipment->Data, sizeof(pEquipment->Data));
    //>发送到数据库
    g_GameServer->SendToDB(&updateMsg);
    //>发送到客户端
    g_GameServer->SendToClient(&updateMsg);
}

SvrItemInfo* ItemManager::FindBagItem(Player* player, uint32 itemId, bool lowLimitOverlap)
{
    assert(player);
    SvrItemInfo* pItem = NULL;
    SvrItemInfo* pItemNumLimit = NULL;
    std::vector<SvrItemInfo*>::iterator end = player->m_BagList.end();
    for (std::vector<SvrItemInfo*>::iterator it = player->m_BagList.begin(); 
        it != end; ++it)
    {
        if((*it)->pCfg->id == itemId)
        {
            pItem = *it;
            if (lowLimitOverlap)
            {
                if (pItem->Data.num < pItem->pCfg->iNumLimit)
                {
                    return pItem;
                }
                else
                {
                    pItemNumLimit = pItem;
                }
            }
            else
            {
                return pItem;
            }
        }
    }

    return pItem ? pItem : pItemNumLimit;
}

SvrItemInfo* ItemManager::FindBagItem(Player* player, uint64 itemGuid)
{
    std::vector<SvrItemInfo*>::iterator end = player->m_BagList.end();
    for (std::vector<SvrItemInfo*>::iterator it = player->m_BagList.begin(); 
        it != end; ++it)
    {
        SvrItemInfo* pItem = *it;
        if(pItem->Data.guid == itemGuid)
        {
            return pItem;
        }
    }
    return NULL;
}

SvrItemInfo* ItemManager::NewItem(Player* player, uint32 itemId, uint32 number, int32 starNumber)
{
    assert(player);
    const ItemsInfo* pItemInfo =  g_CSVFileManager->GetItemsByID(itemId);
    if (pItemInfo == NULL)
    {
        gLog.Error("not find item template id=(%d)", itemId);
        return NULL;
    }

    SvrItemInfo* pItem = new SvrItemInfo(); 
    if (pItem != NULL)
    {
        pItem->Data.guid = GetNewItemGuid();
        pItem->Data.id = itemId;
        pItem->Data.isBind = false;
        pItem->Data.prefixID = 0;
        pItem->Data.starNum = starNumber;
        pItem->Data.time = GetTickCount(); 
        pItem->Data.num = number;
        pItem->Data.isLock = false;
        //>设置配置
        pItem->pCfg = pItemInfo;
    }

    //>产生装备前缀
    const EquipmentPrefix* pPrefix = NULL;
    if (pItem->CanPrefix())
    {
        pPrefix = g_CSVFileManager->GenerateEquipmentPrefix(pItemInfo->iLevelLimit, pItemInfo->iQuality);
        if (pPrefix == NULL)
        {
            gLog.Error("not find item prefix data by new Item.(levelLimit=%d, Quality=%d)", pItemInfo->iLevelLimit, pItemInfo->iQuality);
            return NULL;
        }
        pItem->Data.prefixID = pPrefix == NULL ? 0 : pPrefix->id;
    }

    return pItem;
}

SvrItemInfo* ItemManager::NewBagItem(Player* player, uint32 operateType, uint32 itemId, uint32 number, int32 starNumber)
{

    SvrItemInfo* pItem = NewItem(player, itemId, number, starNumber);
    if (pItem)
    {
        player->m_BagList.push_back(pItem);

        MsgItem_Add_Set addMsg;
        addMsg.OBJID = player->m_GUID;
        addMsg.SysID = ID_G2D;
        addMsg.operateType = operateType;
        addMsg.addNumItem = 1;

        StackStream<> addMsgStream;
        addMsgStream.Wirte(&addMsg, sizeof(addMsg));

        addMsgStream.Wirte(&pItem->Data.guid, sizeof(uint64));
        addMsgStream.Wirte(&pItem->Data.id, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.prefixID, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.starNum, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.isBind, sizeof(uint8));
        addMsgStream.Wirte(&pItem->Data.num, sizeof(int));
        addMsgStream.Wirte(&pItem->Data.isLock, sizeof(uint8));
        addMsgStream.Wirte(&pItem->Data.time, sizeof(int));

        //>发送到数据库
        MsgItem_Add_Set* pAddMsg = (MsgItem_Add_Set*)addMsgStream.Get();
        pAddMsg->Len = addMsgStream.GetSize();
        g_GameServer->SendToDB(pAddMsg);

        //>发送到客户端
        g_GameServer->SendToClient(pAddMsg);

        return pItem;
    }

    return NULL;
}

bool ItemManager::DelBagItem(Player* player, uint32 operateType, SvrItemInfo* pItem, bool removeFromPlayer/* = true*/)
{
    assert(player);

    StackStream<> delMsgStream;
    MsgItem_Del_Set delMsgHead;
    delMsgHead.OBJID = player->m_GUID;
    delMsgHead.SysID = ID_G2D;
    delMsgHead.operateType = operateType;
    delMsgHead.delNumItem = 1;
    delMsgStream.Wirte(&delMsgHead, sizeof(delMsgHead));
    delMsgStream.Wirte(&pItem->Data.guid, sizeof(uint64));

    //>发送到数据库
    MsgItem_Del_Set* pDelMsg = (MsgItem_Del_Set*)delMsgStream.Get();
    pDelMsg->Len = delMsgStream.GetSize();
    g_GameServer->SendToDB(pDelMsg);

    //>发送到客户端
    pDelMsg->SysID = ID_G2C;
    g_GameServer->SendToClient(pDelMsg);

    if (removeFromPlayer)
    {
        std::vector<SvrItemInfo*>::iterator end = player->m_BagList.end();
        for (std::vector<SvrItemInfo*>::iterator it = player->m_BagList.begin(); 
            it != end; ++it)
        {
            if(*it == pItem)
            {
                delete pItem;
                player->m_BagList.erase(it); 
                break;
            }
        }
    }

    return true;
}

void ItemManager::UpdateBagItem(Player* player, uint32 operateType, SvrItemInfo* pItem)
{
    assert(player);

    StackStream<> updateMsgStream;
    MsgItem_Update_Set updateMsgHead;
    updateMsgHead.OBJID = player->m_GUID;
    updateMsgHead.SysID = ID_G2D;
    updateMsgHead.operateType = operateType;
    updateMsgHead.updateNumItem = 1;
    updateMsgStream.Wirte(&updateMsgHead, sizeof(updateMsgHead));

    updateMsgStream.Wirte(&pItem->Data.guid, sizeof(uint64));
    updateMsgStream.Wirte(&pItem->Data.id, sizeof(int));
    updateMsgStream.Wirte(&pItem->Data.prefixID, sizeof(int));
    updateMsgStream.Wirte(&pItem->Data.starNum, sizeof(int));
    updateMsgStream.Wirte(&pItem->Data.isBind, sizeof(uint8));
    updateMsgStream.Wirte(&pItem->Data.num, sizeof(int));
    updateMsgStream.Wirte(&pItem->Data.isLock, sizeof(uint8));
    updateMsgStream.Wirte(&pItem->Data.time, sizeof(int));

    //>发送到数据库
    MsgItem_Update_Set* pUpdateMsg = (MsgItem_Update_Set*)updateMsgStream.Get();
    pUpdateMsg->Len = updateMsgStream.GetSize();
    g_GameServer->SendToDB(pUpdateMsg);

    //>发送到客户端
    g_GameServer->SendToClient(pUpdateMsg);
}

bool ItemManager::AddItem(Player* player, uint32 operateType, SvrItemInfo* pItem)
{
    assert(player);
    player->m_BagList.push_back(pItem);

    MsgItem_Add_Set addMsg;
    addMsg.OBJID = player->m_GUID;
    addMsg.SysID = ID_G2C;
    addMsg.operateType = operateType;
    addMsg.addNumItem = 1;

    StackStream<> addMsgStream;
    addMsgStream.Wirte(&addMsg, sizeof(addMsg));

    addMsgStream.Wirte(&pItem->Data.guid, sizeof(uint64));
    addMsgStream.Wirte(&pItem->Data.id, sizeof(int));
    addMsgStream.Wirte(&pItem->Data.prefixID, sizeof(int));
    addMsgStream.Wirte(&pItem->Data.starNum, sizeof(int));
    addMsgStream.Wirte(&pItem->Data.isBind, sizeof(uint8));
    addMsgStream.Wirte(&pItem->Data.num, sizeof(int));
    addMsgStream.Wirte(&pItem->Data.isLock, sizeof(uint8));
    addMsgStream.Wirte(&pItem->Data.time, sizeof(int));

    //>发送到数据库
    MsgItem_Add_Set* pAddMsg = (MsgItem_Add_Set*)addMsgStream.Get();
    pAddMsg->Len = addMsgStream.GetSize();
    g_GameServer->SendToDB(pAddMsg);

    //>发送到客户端
    g_GameServer->SendToClient(pAddMsg);
    return true;
}

bool ItemManager::RemoveItem(Player* player, uint32 operateType, SvrItemInfo* pRemoveItem)
{
    assert(player);

    StackStream<> delMsgStream;
    MsgItem_Del_Set delMsgHead;
    delMsgHead.OBJID = player->m_GUID;
    delMsgHead.SysID = ID_G2D;
    delMsgHead.operateType = operateType;
    delMsgHead.delNumItem = 1;
    delMsgStream.Wirte(&delMsgHead, sizeof(delMsgHead));
    delMsgStream.Wirte(&pRemoveItem->Data.guid, sizeof(uint64));

    SvrItemInfo* pItem = NULL;
    std::vector<SvrItemInfo*>::iterator end = player->m_BagList.end();
    for (std::vector<SvrItemInfo*>::iterator it = player->m_BagList.begin(); 
        it != end; ++it)
    {
        pItem = *it;
        if (pItem == pRemoveItem)
        {
            player->m_BagList.erase(it);
            break;
        }
        pItem = NULL;
    }

    if (pItem != NULL)
    {
        //>发送到数据库
        MsgItem_Del_Set* pDelMsg = (MsgItem_Del_Set*)delMsgStream.Get();
        pDelMsg->Len = delMsgStream.GetSize();
        g_GameServer->SendToDB(pDelMsg);

        //>发送到客户端
        pDelMsg->SysID = ID_G2C;
        g_GameServer->SendToClient(pDelMsg);

        return true;
    }

    return false;
}