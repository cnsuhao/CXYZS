#include "ArmatureExPhantom.h"
#include "CCDirector.h"
#include "CCRenderer.h"

USING_NS_CC;
using namespace cocostudio;

ArmatureExPhantom::ArmatureExPhantom(void):_sampleDalay(0.5f),_phantomLife(1.5f),_deltaTime(0.0f),_enable(false)
{
    _color = Color3B::WHITE;
}


ArmatureExPhantom::~ArmatureExPhantom(void)
{
    if (!renderDatas.empty())
    {
        for (std::vector<RenderData*>::iterator it = renderDatas.begin(); it != renderDatas.end(); )
        {
            (*it)->clonSkin->release();
            delete (*it);
        }
        renderDatas.clear();
    }
}

ArmatureExPhantom* ArmatureExPhantom::create()
{
    ArmatureExPhantom *phantom = new (std::nothrow) ArmatureExPhantom();
    if (phantom)
    {
        phantom->autorelease();
        return phantom;
    }
    CC_SAFE_DELETE(phantom);
    return nullptr;
}

inline void SetColor4B(Color4B& c4b, Color3B& c3b)
{
    c4b.r = c3b.r;
    c4b.g = c3b.g;
    c4b.b = c3b.b;
}

void ArmatureExPhantom::Sample(cocostudio::Skin* skin, cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if (!_enable)
        return;

    _deltaTime += (1.0f / 30.0f);
    if (_deltaTime > _sampleDalay)
    {
        RenderData* pData = new RenderData();
        pData->time = _phantomLife;
        pData->clonSkin = Skin::createWithSpriteFrameName(skin->getDisplayName());
        pData->clonSkin->retain();
        pData->transform = transform;
        pData->quad = skin->getQuad();
        SetColor4B(pData->quad.bl.colors, _color); 
        SetColor4B(pData->quad.br.colors, _color);
        SetColor4B(pData->quad.tl.colors, _color);
        SetColor4B(pData->quad.tr.colors, _color);
        pData->flags = flags;

        renderDatas.push_back(pData);

        _deltaTime -= _sampleDalay;
    }

    return;
}

void ArmatureExPhantom::Render(cocos2d::Renderer *renderer)
{
    if (!renderDatas.empty())
    {
        for (std::vector<RenderData*>::iterator it = renderDatas.begin(); it != renderDatas.end(); )
        {
            (*it)->time -= (1.0f / 30.0f);
            if((*it)->time < 0.0f)
            {
                (*it)->clonSkin->release();
                delete (*it);
                it = renderDatas.erase(it);
            }
            else
                ++it;
        }

        Mat4 mv2 = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        Mat4 mv1 = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        Mat4 mv = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        for (std::vector<RenderData*>::reverse_iterator it = renderDatas.rbegin(); it != renderDatas.rend(); ++it)
        {
            if((*it)->time > 0.0f)
            {
                (*it)->clonSkin->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
                GLubyte alpha = clampf((*it)->time / _phantomLife, 0.0f, 0.65f) * 255;
                (*it)->quad.tl.colors.a = alpha;
                (*it)->quad.tr.colors.a = alpha;
                (*it)->quad.bl.colors.a = alpha;
                (*it)->quad.br.colors.a = alpha;
                
                (*it)->quadCommand.init((*it)->clonSkin->getLocalZOrder(),(*it)->clonSkin->getTexture()->getName(),(*it)->clonSkin->getGLProgramState(),(*it)->clonSkin->getBlendFunc(), &(*it)->quad, 1, mv * (*it)->transform);
                renderer->addCommand(&(*it)->quadCommand);
            }
        }

        Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, mv1);
        Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, mv2);
    }
}

