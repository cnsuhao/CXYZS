#include "UIMiniMap.h"
#include "LogicManager.h"
#include "Hero.h"
#include "SceneManager.h"
#include "CSVFileManager.h"
USING_NS_CC;

#define MAP_ZORDER 400
#define HERO_ZORDER 401

UIMiniMap::UIMiniMap():_mapId(0),_clippingNode(nullptr),_mapSprite(nullptr)
{
    _mapScale = 1.0f;
}

UIMiniMap::~UIMiniMap()
{
}

bool UIMiniMap::init()
{
    bool bRet = false;
    do 
    {
        _clippingNode = ClippingNode::create();
        _clippingNode->setAlphaThreshold(0.0f);
        //_clippingNode->setInverted(true);

        auto stencil = Sprite::create("UI2/Main/MiniMapStencil.png");
        _clippingNode->setStencil(stencil);

        _heroFlag = GetFlag(0);
        _heroFlag->retain();
        //_clippingNode->addChild(_heroFlag, HERO_ZORDER);

        addChild(_clippingNode);

        bRet = true;
    } while (0);

    return bRet;
}

void UIMiniMap::SetMapId(int mapId)
{
    if (_mapId != mapId)
    {
        //schedule(CC_SCHEDULE_SELECTOR(UIMiniMap::Refresh), 0.25f);

        if (_mapSprite)
        {
            _clippingNode->removeChild(_mapSprite, true);
            _mapSprite = nullptr;
        }

        const MapInfo* map = g_CSVFileManager->GetMapByID(mapId);
        if (map)
        {
            std::string imagePath = map->strMapPath;
            size_t at = imagePath.find_last_of('.');
            if(std::string::npos != at)
            {
                imagePath.resize(at + 1);
                imagePath.append("png");
                _mapSprite = Sprite::create(imagePath);
               if (_mapSprite)
               {
				   _mapSprite->setAnchorPoint(Vec2::ZERO);
				   _mapSprite->setScale(_mapScale);
				   _mapSprite->addChild(_heroFlag, 100);
				   _cacheMapSize = g_SceneManager->m_MainLayer->GetMapSize();

				   _clippingNode->addChild(_mapSprite, MAP_ZORDER);
               }
            }
        }
    }
    _mapId = mapId;
}

void UIMiniMap::Refresh(float dt)
{
    if (_mapId && g_LogicManager->m_Hero)
    {
        const Vec2& heroPos = g_LogicManager->m_Hero->getPosition();
        Vec2 mappingToMiniMap = Mapping(heroPos);

        _heroFlag->setPosition(mappingToMiniMap);


        for (std::vector<SyncNode>::iterator itr = _syncNodes.begin(); itr != _syncNodes.end(); ++itr)
        {
            SyncNode& node = (*itr);
            const Vec2 position = Mapping(node.node->getPosition());
            if (node.flagSpr == NULL){
                if(_mapSprite){
                    node.flagSpr = GetFlag(node.flag);
                    _mapSprite->addChild(node.flagSpr);
                }
            }
            if(node.flagSpr)
            {
                node.flagSpr->setPosition(position);
            }
        }

        if (_mapSprite)
            _mapSprite->setPosition(-MappingWithClamp(heroPos)*_mapScale);
        
    }
}

Vec2 UIMiniMap::Mapping(const Vec2& position)
{
    if (_mapSprite)
    {
        Size miniMapSize = _mapSprite->getContentSize();
        Vec2 mappingToMiniMap((position.x/_cacheMapSize.width)*miniMapSize.width,
                              (position.y/_cacheMapSize.height)*miniMapSize.height);

        return mappingToMiniMap;
    }
    return Vec2::ZERO;
}

cc2d::Vec2 UIMiniMap::MappingWithClamp(const cc2d::Vec2& position)
{
    if (_mapSprite)
    {
        Vec2 mappingToMiniMap = Mapping(position);

        Size miniMapSize = _mapSprite->getContentSize();
        Vec2 halfClipSize = Vec2(_clippingNode->getStencil()->getContentSize() * 0.5f);
        mappingToMiniMap.x = clampf(mappingToMiniMap.x , min(halfClipSize.x, miniMapSize.width - halfClipSize.x),  max(halfClipSize.x, miniMapSize.width - halfClipSize.x));
        mappingToMiniMap.y = clampf(mappingToMiniMap.y , min(halfClipSize.y, miniMapSize.height - halfClipSize.y), max(halfClipSize.y, miniMapSize.height - halfClipSize.y));

        return mappingToMiniMap;
    }
    return Vec2::ZERO;
}

Sprite* UIMiniMap::GetFlag(int f)
{
    if (f == 0)
    {
        return Sprite::create("UI2/Main/HeroFlag.png");
    }
    else if (f == 1)
    {
        return Sprite::create("UI2/Main/MonsterFlag.png");
    }
    else if (f == 2)
    {
        return Sprite::create("UI2/Main/NpcFlag.png");
    }
    else if (f == 3)
    {
        return Sprite::create("UI2/Main/ConvertFlag.png");
    }
    CCASSERT(0, "");
    return NULL;
}

bool UIMiniMap::AddNode(cc2d::Node* node, int flag)
{
#ifdef _DEBUG
    
    for (std::vector<SyncNode>::iterator itr = _syncNodes.begin(); itr != _syncNodes.end(); ++itr)
    {
        if ((*itr).node == node)
        {
            //CCASSERT(0,"");
            return false;
        }
    }
#endif // _DEBUG

    _syncNodes.push_back(SyncNode(node, NULL, flag));
    return true;
}

bool UIMiniMap::RemoveNode(cc2d::Node* node)
{
    for (std::vector<SyncNode>::iterator itr = _syncNodes.begin(); itr != _syncNodes.end(); ++itr)
    {
        if ((*itr).node == node)
        {
            if ((*itr).flag && _mapSprite)
                _mapSprite->removeChild((*itr).flagSpr, true);

            _syncNodes.erase(itr);
            return true;
        }
    }
    return false;
}

void UIMiniMap::ClearAllNode()
{
    _syncNodes.clear();
}

