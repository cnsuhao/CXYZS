#pragma once

#include "CCArmature.h"

class ArmatureExtend : public cocostudio::Armature
{
    friend class ArmatureExPhantom;
protected:
    ArmatureExtend();
public:
    virtual ~ArmatureExtend();

    static ArmatureExtend *create();

    static ArmatureExtend *create(const std::string& name);

    static ArmatureExtend *create(const std::string& name, cocostudio::Bone *parentBone);

    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    void Contect(class ArmatureExPhantom* notify);

    // ¿ªÆô/¹Ø±Õ»ÃÓ°
    void SetPhantomEnable(bool enable);

    inline ArmatureExPhantom* GetPhantom() { return _notify; }
protected:
    class ArmatureExPhantom* _notify;

private:
    bool CheckSkinIsVisiable(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, cocostudio::Skin* skin);
};


