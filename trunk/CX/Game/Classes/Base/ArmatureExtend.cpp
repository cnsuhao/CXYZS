#include "ArmatureExtend.h"

USING_NS_CC;
using namespace cocostudio;

#include "ArmatureExPhantom.h"
#include "CCDirector.h"


ArmatureExtend::ArmatureExtend():_notify(NULL)
{

}

ArmatureExtend::~ArmatureExtend()
{
    if (_notify)
    {
        _notify->release();
    }
}

ArmatureExtend * ArmatureExtend::create()
{
    ArmatureExtend *armature = new (std::nothrow) ArmatureExtend();
    if (armature && armature->init())
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return nullptr;
}

ArmatureExtend * ArmatureExtend::create(const std::string& name)
{
    ArmatureExtend *armature = new (std::nothrow) ArmatureExtend();
    if (armature && armature->init(name))
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return nullptr;
}

ArmatureExtend * ArmatureExtend::create(const std::string& name, cocostudio::Bone *parentBone)
{
    ArmatureExtend *armature = new (std::nothrow) ArmatureExtend();
    if (armature && armature->init(name, parentBone))
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return nullptr;
}


void ArmatureExtend::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    // 先渲染幻影
    if (_notify)
    {
        _notify->Render(renderer);
    }

    for (auto& object : _children)
    {
        if (Bone *bone = dynamic_cast<Bone *>(object))
        {
            Node *node = bone->getDisplayRenderNode();

            if (nullptr == node)
                continue;

            switch (bone->getDisplayRenderNodeType())
            {
            case CS_DISPLAY_SPRITE:
                {
                    Skin *skin = static_cast<Skin *>(node);
                    skin->updateTransform();

                    BlendFunc func = bone->getBlendFunc();

                    if (func.src != BlendFunc::ALPHA_PREMULTIPLIED.src || func.dst != BlendFunc::ALPHA_PREMULTIPLIED.dst)
                    {
                        skin->setBlendFunc(bone->getBlendFunc());
                    }
                    else
                    {
                        if (_blendFunc == BlendFunc::ALPHA_PREMULTIPLIED && !skin->getTexture()->hasPremultipliedAlpha())
                        {
                            skin->setBlendFunc(_blendFunc.ALPHA_NON_PREMULTIPLIED);
                        }
                        else
                        {
                            skin->setBlendFunc(_blendFunc);
                        }
                    }
                    if (CheckSkinIsVisiable(renderer, transform, skin))
                    {
                        skin->draw(renderer, transform, flags);
                        if (_notify)
                        {
                            _notify->Sample(skin, renderer, getParent()->getNodeToParentTransform(), flags);
                        }
                    }
                }
                break;
            case CS_DISPLAY_ARMATURE:
                {
                    node->draw(renderer, transform, flags);
                }
                break;
            default:
                {
                    node->visit(renderer, transform, flags);
                    //                CC_NODE_DRAW_SETUP();
                }
                break;
            }
        }
        else if(Node *node = dynamic_cast<Node *>(object))
        {
            node->visit(renderer, transform, flags);
            //            CC_NODE_DRAW_SETUP();
        }
    }
}

void ArmatureExtend::Contect(class ArmatureExPhantom* notify)
{
    if (notify)
    {
        _notify = notify;
        _notify->retain();
    }
}

void ArmatureExtend::SetPhantomEnable(bool enable)
{
    if (GetPhantom())
    {
        GetPhantom()->SetEnable(enable);
    }
}

bool ArmatureExtend::CheckSkinIsVisiable(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, cocostudio::Skin* skin)
{
    bool visibility = false;

    Size screen = Director::getInstance()->getWinSize();

    V3F_C4B_T2F_Quad quad = skin->getQuad();
    Vec4 v4world, v4local;
    Vec3* v[4] = { &quad.bl.vertices, &quad.br.vertices, &quad.tl.vertices, &quad.tr.vertices };
    for (int i = 0; i < 4; ++i)
    {
        v4local.set(v[i]->x, v[i]->y, v[i]->z, 1.0f);
        transform.transformVector(v4local, &v4world);
        v4world.x = v4world.x / v4world.w;
        v4world.y = v4world.y / v4world.w;
        //这里没有考虑正交透视,如果使用多相机或者非全屏投影计算会错误.
        if (v4world.x >= 0 && v4world.x < screen.width - 1 && v4world.y >= 0 && v4world.y < screen.height -1)
        {
            
            visibility = true;
            break;
        }
        else
        {
            //log("%f,%f,%f,%f", v4world.x,v4world.y,v4world.z,v4world.w);
        }
    }

    return visibility;
}

