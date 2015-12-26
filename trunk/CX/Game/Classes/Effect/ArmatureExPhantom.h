#pragma once

/********************************************************************
created:	2015/5/4
author:		lishihai
desc:	    ��Ӱ��
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

    // ���ò������ʱ��. ������ÿ֡������.
    inline void SetSampleDalay(float sampleDalay = 0.25f) { _sampleDalay = sampleDalay; }
    // ���û�Ӱ����ʱ��
    inline void SetPhantomLife(float lifeTime = 0.5f) { _phantomLife = lifeTime; }
    // ����/�رջ�Ӱ
    inline void SetEnable(bool enable) { _enable = enable; }
    // ������ɫ
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
    float _sampleDalay; // �ӳٲ���ʱ��
    float _phantomLife; // ��Ӱ����ʱ��
    float _deltaTime;
    bool _enable;
    cc2d::Color3B _color;
};

