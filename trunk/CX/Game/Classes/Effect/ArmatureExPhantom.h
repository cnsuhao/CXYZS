#pragma once

/********************************************************************
created:	2015/5/4
author:		lishihai
desc:	    残影类
*********************************************************************/

#include "CCNode.h"
#include "ClientDef.h"
#include "CCSkin.h"

class ArmatureExPhantom : public cc2d::Ref
{
protected:
    ArmatureExPhantom(void);
public:
    virtual ~ArmatureExPhantom(void);

    static ArmatureExPhantom* create();

    // 设置采样间隔时间. 并不是每帧都采样.
    inline void SetSampleDalay(float sampleDalay = 0.25f) { _sampleDalay = sampleDalay; }
    // 设置幻影生命时间
    inline void SetPhantomLife(float lifeTime = 0.5f) { _phantomLife = lifeTime; }
    // 开启/关闭幻影
    inline void SetEnable(bool enable) { _enable = enable; }
    // 设置颜色
    inline void SetColor(cc2d::Color3B c) { _color = c; }

    void Sample(cocostudio::Skin* skin, cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);

    void Render(cocos2d::Renderer *renderer);

    struct RenderData
    {
        float time;
        cc2d::Mat4 transform;
        cc2d::Sprite* clonSkin;
        uint32_t flags;
        cc2d::QuadCommand quadCommand;
        cc2d::V3F_C4B_T2F_Quad quad;
    };

    std::vector<RenderData*> renderDatas;

    cc2d::Vec2 lastPosition;
protected:
    float _sampleDalay; // 延迟采样时间
    float _phantomLife; // 残影生存时间
    float _deltaTime;
    bool _enable;
    cc2d::Color3B _color;
};

