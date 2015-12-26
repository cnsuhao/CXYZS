#include "Bag.h"

#include "UIBag.h"
#include "CSVFileManager.h"

const char* ItemQualityTextures[QUALITY_MAX] = { "UI/Common/com_white.png", "UI/Common/com_green.png", "UI/Common/com_blue.png", "UI/Common/com_purple.png", "UI/Common/com_yellow.png" };
const char* EmptyItemTexture = "UI/Common/com_emptybox.png";
const char* LockGridTexture = "UI/Common/com_lockbox.png";
const char* MoneyTypeIcon[5] = { "UI/Common/com_money.png", "UI/Common/com_gold.png", "UI/Common/com_bind_gold.png", "UI/Common/com_arena_money.png", "UI/Common/com_battlefield_money.png" };

const char* GetMoneyIcon(AttributeType type)
{
    static char* emptyChar = "";

    if (type == AttributeType::ATTRIBUTE_MONEY)
    {
        return MoneyTypeIcon[0];
    }
    else if (type == AttributeType::ATTRIBUTE_GOLD)
    {
        return MoneyTypeIcon[1];
    }
    else if(type == AttributeType::ATTRIBUTE_BINDGOLD)
    {
        return MoneyTypeIcon[2];
    }
    else if (type == AttributeType::ATTRIBUTE_ARENAMONEY)
    {
        return MoneyTypeIcon[3];
    }
    else if(type == AttributeType::ATTRIBUTE_BATTLEFIELDMONEY)
    {
        return MoneyTypeIcon[4];
    }

    return emptyChar;
}

bool CliItemInfo::IsEquipment()
{
    if (pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        return true;
    }
    return false;
}

bool CliItemInfo::CanUpStar()
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


bool CliItemInfo::IsMaxUpStarLevel()
{
    const EquipmentUpgrade* pUpgrade = g_CSVFileManager->GetEquipmentUpgradeByID(Data.starNum + 1);
    if (pUpgrade == NULL)
    {
        return true;
    }
    return false;
}

bool CliItemInfo::CanCompose()
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

int CliItemInfo::GetBattlePower()
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

            if (Data.starNum > 0)
            {
                //>升星影响基础属性
                int upStarValue = g_CSVFileManager->CalcUpStarEffectBaseAttribute(AttributeType(attributeId), int(pCfg->iBaseAttParams[i]), Data.starNum);
                battlePower += int(attribute->iEffect * upStarValue);
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

int CliItemInfo::GetComposeSpendMoney()
{
   return int(g_CSVFileManager->m_RatioInfo[ATTRIBUTE_RATIO_COMPOSEEXPEND]); 
   
}

cocos2d::Color3B CliItemInfo::GetQualityColor()
{
    if (pCfg->iQuality < QUALITY_MAX)
    {
        return g_CSVFileManager->m_QualityColor[pCfg->iQuality];
    }
    return g_CSVFileManager->m_QualityColor[0];
}

Bag::Bag(void)
{
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        _equips[i] = NULL;
    }
}


Bag::~Bag(void)
{
}

void Bag::Init(std::vector<CliItemInfo*>& pItems)
{
    _items.resize(pItems.size());
    for (size_t i = 0; i < pItems.size(); ++i)
    {
        _items[i] = pItems[i];
    }
}

void Bag::AddItem(std::vector<CliItemInfo*>& items)
{
    _items.insert(_items.end(), items.begin(), _items.end());
    NeedSort();
}

void Bag::AddItem(CliItemInfo* item)
{
    _items.push_back(item);
    NeedSort();
}

bool Bag::ReomveItem(uint64 guid)
{
    for (size_t i = 0; i < _items.size(); ++i)
    {
        if(_items[i]->Data.guid == guid)
        {
            CliItemInfo* item = _items[i];
            _items.erase(_items.begin() + i);
            delete item;
            NeedSort();
            return true;
        }
    }
    return  false;
}

CliItemInfo* Bag::ReomveItemPtr(uint64 guid)
{
    CliItemInfo* pItem = NULL;
    for (size_t i = 0; i < _items.size(); ++i)
    {
        if(_items[i]->Data.guid == guid)
        {
            pItem = _items[i];
            _items.erase(_items.begin() + i);
            NeedSort();
            break;
        }
    }
    return pItem;
}

CliItemInfo* Bag::GetItemByGuid(uint64 guid, bool includeEquip /* = true*/ )
{
    if (includeEquip)
    {
        for (int i = 0; i < EQUIPTYPE_MAX; ++i)
        {
            if (_equips[i] && _equips[i]->Data.guid == guid)
            {
                return _equips[i];
            }
        }
    }

    for (size_t i = 0; i < _items.size(); ++i)
    {
        if(_items[i]->Data.guid == guid)
        {
            return _items[i];
        }
    }
    return NULL;
}

CliItemInfo* Bag::GetItemByTypeId(int ItemTypeId, int count /*= 1*/)
{
    for (size_t i = 0; i < _items.size(); ++i)
    {
        if (_items[i]->pCfg->id == ItemTypeId && _items[i]->Data.num >= count)
        {
            return _items[i];
        }
    }
    return NULL;
}

int Bag::GetItemCount(int ItemTypeId) const
{
    int count = 0;
    for (size_t i = 0; i < _items.size(); ++i)
    {
        if (_items[i]->pCfg->id == ItemTypeId)
        {
            count += _items[i]->Data.num;
        }
    }
    return count;
}

//CliItemInfo* Bag::GetPageItem(uint32 pageIdx, uint32 rowIdx, uint32 colIdx)
//{
//    if (pageIdx < EPageCount)
//    {
//        return _pages[pageIdx].GetItem(rowIdx, colIdx);
//    }
//    return NULL;
//}

void Bag::GetPageItems(uint32 pageIdx, std::vector<CliItemInfo*>& outItems)
{
    if (pageIdx == 0)  //>'全部'就是所有的物品直接放而不筛选.
    {
        outItems.clear();
        for (size_t i = 0; i < _items.size(); ++i)
        {
            outItems.push_back(_items[i]);
        }
    }
    else if (pageIdx == 1) //>'装备' 根据配置筛选装备.
    {
        outItems.clear();
        for (size_t i = 0; i < _items.size(); ++i)
        {
            if (_items[i]->pCfg && _items[i]->pCfg->iBigClass == ITEMTYPE_EQUIP)
            {
                outItems.push_back(_items[i]);
            }
        }
    }
    else if (pageIdx == 2) //>'翅膀碎片' 根据配置筛选翅膀碎片.
    {
        outItems.clear();
        for (size_t i = 0; i < _items.size(); ++i)
        {
            if (_items[i]->pCfg && _items[i]->pCfg->iBigClass == ITEMTYPE_WINGPIECE)
            {
                outItems.push_back(_items[i]);
            }
        }
    }
    else if (pageIdx == 3) //>'消耗品' 根据配置筛选消耗品.
    {
        outItems.clear();
        for (size_t i = 0; i < _items.size(); ++i)
        {
            if (_items[i]->pCfg && _items[i]->pCfg->iBigClass == ITEMTYPE_EXPEND)
            {
                outItems.push_back(_items[i]);
            }
        }
    }
    else if (pageIdx == 4) //>'材料' 根据配置筛选材料.
    {
        outItems.clear();
        for (size_t i = 0; i < _items.size(); ++i)
        {
            if (_items[i]->pCfg && _items[i]->pCfg->iBigClass == ITEMTYPE_MATERIAL)
            {
                outItems.push_back(_items[i]);
            }
        }
    }
    else if (pageIdx == 5) //>'任务道具' 根据配置筛选任务道具.
    {
        outItems.clear();
        for (size_t i = 0; i < _items.size(); ++i)
        {
            if (_items[i]->pCfg && _items[i]->pCfg->iBigClass == ITEMTYPE_TASK)
            {
                outItems.push_back(_items[i]);
            }
        }
    }
}

void Bag::GetCanComposeItems(std::vector<CliItemInfo*>& items, bool includeEquip /* = true*/)
{
    items.clear();

    if (includeEquip)
    {
        for (int i = 0; i < EQUIPTYPE_MAX; ++i)
        {
            if (_equips[i] && _equips[i]->CanCompose())
            {
                items.push_back(_equips[i]);
            }
        }
    }

    for (size_t i = 0; i < _items.size(); ++i)
    {
        if(_items[i]->CanCompose())
        {
            items.push_back(_items[i]);
        }
    }
}

void Bag::GetCanStarItems(std::vector<CliItemInfo*>& items, bool includeEquip /* = true*/)
{
    items.clear();

    if (includeEquip)
    {
        for (int i = 0; i < EQUIPTYPE_MAX; ++i)
        {
            if (_equips[i] && _equips[i]->CanUpStar() && !_equips[i]->IsMaxUpStarLevel())
            {
                items.push_back(_equips[i]);
            }
        }
    }

    for (size_t i = 0; i < _items.size(); ++i)
    {
        if(_items[i]->CanUpStar() && !_items[i]->IsMaxUpStarLevel())
        {
            items.push_back(_items[i]);
        }
    }
}

void Bag::GetCanInheritItems(std::vector<CliItemInfo*>& items, bool includeEquip /*= true*/)
{
    items.clear();

    if (includeEquip)
    {
        for (int i = 0; i < EQUIPTYPE_MAX; ++i)
        {
            if (_equips[i] && _equips[i]->CanUpStar())
            {
                items.push_back(_equips[i]);
            }
        }
    }

    for (size_t i = 0; i < _items.size(); ++i)
    {
        if(_items[i]->CanUpStar())
        {
            items.push_back(_items[i]);
        }
    }
}

void Bag::NeedSort()
{
    //for (int i = 0; i < EPageCount; ++i)
    //{
    //    _pages[i]._needResort = true;
    //}
}

//-----------------------------------------------------------------------------------------
Bag::Page::Page()
{
    _needReload = true;

    _row = _col = 0;
}

void Bag::Page::Reload(int row, int col, std::vector<CliItemInfo*>& items)
{
    _row = row;
    _col = col;
    size_t count = size_t(row * col);

    _pageItems.resize(count);
    for (size_t i = 0; i < count; ++i)
    {
        if (i < items.size())
        {
            _pageItems[i] = items[i];
        }
        else
        {
            _pageItems[i] = NULL;
        }
    }

    _needReload = false;
}

void Bag::Page::AddItem(CliItemInfo* item, uint32 rowIdx, uint32 colIdx)
{
    uint32 arrayIdx = rowIdx * _col + colIdx;
    if (arrayIdx < uint32(_col * _row))
    {
        if (_pageItems[arrayIdx] == NULL)
        {
            _pageItems[arrayIdx] = item;
        }
    }
}

CliItemInfo* Bag::Page::GetItem(uint32 rowIdx, uint32 colIdx)
{
    uint32 arrayIdx = rowIdx * _col + colIdx;
    if (arrayIdx < uint32(_col * _row))
    {
        return _pageItems[arrayIdx];
    }
    return NULL;
}

bool Bag::Page::FindItem(uint64 guid, uint32& rowIdx, uint32& colIdx)
{
    size_t count = size_t(_row * _col);
    for (size_t i = 0; i < count; ++i)
    {
        if (_pageItems[i] && _pageItems[i]->Data.guid == guid)
        {
            rowIdx = i / _col;
            colIdx = i % _col;
            return true;
        }
    }
    return false;
}

bool Bag::Page::Remove(uint32 rowIdx, uint32 colIdx)
{
    uint32 arrayIdx = rowIdx * _col + colIdx;
    if (arrayIdx < uint32(_col * _row))
    {
        if (_pageItems[arrayIdx] != NULL)
        {
            _pageItems[arrayIdx] = NULL;
            return true;
        }
    }
    return false;
}

bool Bag::Page::SwapItem(uint32 rowIdx1, uint32 colIdx1, uint32 rowIdx2, uint32 colIdx2)
{
    uint32 arrayIdx1 = rowIdx1 * _col + colIdx1;
    uint32 arrayIdx2 = rowIdx2 * _col + colIdx2;

    if (arrayIdx1 < uint32(_col * _row) && arrayIdx2 < uint32(_col * _row))
    {
        //>如果索引都有效
        if (_pageItems[arrayIdx1] != NULL)
        {
            CliItemInfo* temp = _pageItems[arrayIdx2];
            _pageItems[arrayIdx2] = _pageItems[arrayIdx1];
            _pageItems[arrayIdx1] = temp;
            return true;
        }
    }
    return false;
}


void Bag::LoadEquip(CliItemInfo* equipments[EQUIPTYPE_MAX])
{
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if(equipments[i] && equipments[i]->Data.guid != INVALID_ID)
        {
            _equips[i] = new CliItemInfo();
            memcpy(_equips[i], equipments[i], sizeof(CliItemInfo));
        }
        else
        {
            _equips[i] = NULL;
        }
    }
}

void Bag::WearEquipment(EquipType type, CliItemInfo* equipment)
{
    if (type < EQUIPTYPE_MAX)
    {
        _equips[type] = equipment;
    }
}

void Bag::TakeoffEquipment(EquipType type)
{
    if (type < EQUIPTYPE_MAX)
    {
        _equips[type] = NULL;
    }
}

CliItemInfo* Bag::GetEquipment(EquipType type)
{
    if (type < EQUIPTYPE_MAX)
    {
        return _equips[type];
    }
    return NULL;
}

CliItemInfo* Bag::FindWearEquipmentByGuid(uint64 guid)
{
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if(_equips[i] && _equips[i]->Data.guid == guid)
        {  
            return _equips[i];
        }
    }
    return NULL;
}

CliItemInfo* Bag::FindWearEquipment(uint32 typeId)
{
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        if(_equips[i] && _equips[i]->pCfg->id == typeId)
        {  
            return _equips[i];
        }
    }
    return NULL;
}

