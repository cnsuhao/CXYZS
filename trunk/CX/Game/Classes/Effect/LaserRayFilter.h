#pragma once

#include "FilterBase.h"

// ¿ÿ…‰π‚’’¬Àæµ–ßπ˚.
class LaserRayFilter : public FilterBase
{
protected:
    LaserRayFilter(void);
public:
    
    ~LaserRayFilter(void);

    static LaserRayFilter* create();

    virtual const std::string& GetProgramKey() const;

    virtual cc2d::GLProgram* CreateProgram();

    virtual void Setup(class SpriteWithFilter* spr);

    void SetTime(float time);

    void SetUseParamPos(bool use);

    void SetParamPos(cc2d::Vec2& pos); // between 0,1 of Vec2 x and y; 

    void SetNumSample(int num);

    void SetWeight(float weight);
protected:
    float time;
    bool use;
    cc2d::Vec2 exitPos;
    int numSample;
    float weight;
};

