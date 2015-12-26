/********************************************************************
created:	2015/3/9
author:		LSH
desc:		小地图
*********************************************************************/

#ifndef __UIMINIMAP_H__
#define __UIMINIMAP_H__

#include "ClientDef.h"
#include "CCLayer.h"
#include "CCClippingNode.h"

class UIMiniMap : public cc2d::Layer
{
public:
	UIMiniMap();
	~UIMiniMap();
	virtual bool init();
	CREATE_FUNC(UIMiniMap);
public:
    void SetMapId(int mapId);

    void Refresh(float dt);

    bool AddNode(cc2d::Node* node, int flag);

    bool RemoveNode(cc2d::Node* node);

    void ClearAllNode();
private:
    cc2d::Vec2 Mapping(const cc2d::Vec2& position);
    cc2d::Vec2 MappingWithClamp(const cc2d::Vec2& position);
    cc2d::Sprite* GetFlag(int f);

    int _mapId;
    cc2d::ClippingNode* _clippingNode;
    cc2d::Sprite*       _mapSprite;
    cc2d::Size          _cacheMapSize;
    
    cc2d::Sprite*       _heroFlag;
    float               _mapScale;

    typedef struct SyncNode_t
    {
        SyncNode_t(cc2d::Node* n,cc2d::Sprite* s, int f):node(n),flagSpr(s),flag(f)
        {
        }
        cc2d::Node* node;
        cc2d::Sprite* flagSpr;
        int flag;
    }SyncNode;

    std::vector<SyncNode>  _syncNodes;
};

#endif	//__UIMINIMAP_H__