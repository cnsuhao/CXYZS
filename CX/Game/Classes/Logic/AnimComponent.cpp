#include "AnimComponent.h"
#include "Object.h"
#include "CCVector.h"
#include "CCAnimation.h"
#include "ResManager.h"
#include "CSVFileManager.h"
#include "CCActionInstant.h"
#include "UILoadingBar.h"
#include "GameDef.h"
#include "UITextAtlas.h"
#include "UIImageView.h"
#include "LogicManager.h"
#include "deprecated/CCString.h"


USING_NS_CC;
using namespace ui;

const std::string ObjectLoadImage("UI2/Main/ObjectLoading.png");

const Vec2 ShaowPosition(0.0f,0.0f);
const Vec2 HpBarPosition(0.0f, 170.0f);

AnimComponent::AnimComponent(MapObj* Owner):m_Owner(Owner)
{
    m_ActionType = ACTION_MAX;
    m_Direction = DIRECTION_DOWN;

    m_ResouceLoading = false;
    m_FrameCount = 0;
    m_AsyncLoad = false;
    m_Shadow = NULL;
    m_HpBar = NULL;

    m_DropHPDelay = 0.0f;
}

AnimComponent::~AnimComponent()
{

}

void AnimComponent::LoadResouce(bool asyncLoad /*= true*/)
{
    m_AsyncLoad = asyncLoad;
}

void AnimComponent::ChangeDirection(DirectionType direction)
{
    if (m_Direction != direction)
    {
        m_Direction = direction;
        UpdataAction();
    }
}

void AnimComponent::ChangeAction(ActionType action)
{
    if (m_ActionType != action)
    {
        m_ActionType = action;
        UpdataAction();
    }
}

void AnimComponent::UpdataAction()
{
    StopAction();
}

void AnimComponent::StopAction()
{

}

void AnimComponent::SetOpacityWhenEnterMapTile(int inMask)
{

}

bool AnimComponent::IsSelection(const cc2d::Vec2 &point)
{
    return false;
}

cc2d::Node* AnimComponent::GetBody()
{
    return NULL;
}

void AnimComponent::EnableShadow(bool enable)
{
    cc2d::Node* pBody = GetBody();
    if (pBody == NULL)
        return;

    if (enable)
    {
        if (m_Shadow == NULL)
        {
            m_Shadow = Sprite::create("Public/Shadow.png");	
            if (m_Shadow)
            {
                m_Shadow->setPositionY(-10);
                m_Shadow->setVisible(false);
                pBody->addChild(m_Shadow,-1);
            }
        }
    }
    else
    {
        if (m_Shadow)
        {
            m_Shadow->removeFromParent();
            m_Shadow = NULL;
        }
    }
}

void AnimComponent::EnableHpBar(bool enable)
{
    cc2d::Node* pBody = GetBody();
    if (pBody == NULL)
        return;

    if (enable)
    {
        if (m_HpBar == NULL)
        {
            m_HpBar = LoadingBar::create("Public/HPBar.png");	
            if (m_HpBar)
            {
                m_HpBar->setPositionY(170.0f);
                m_HpBar->setVisible(false);
                pBody->addChild(m_HpBar,-1);
            }
        }
    }
    else
    {
        if (m_HpBar)
        {
            m_HpBar->removeFromParent();
            m_HpBar = NULL;
        }
    }
}


void AnimComponent::SetHpBarPercent(float percent)
{
    if (m_HpBar)
    {
        m_HpBar->setPercent(percent);
    }
}

void AnimComponent::PlayDropHpAnim(const DropHpMode& mode)
{
    cc2d::Node* pBody = GetBody();
    if (pBody == NULL)
        return;

    int effectId = mode.effectId;
    if (!(mode.attacker == g_LogicManager->m_Hero->m_GUID || m_Owner->m_GUID == g_LogicManager->m_Hero->m_GUID))
    {//>只播放特效动作.不飘伤害
        if (effectId > 0)
        {
            g_ResManager->ShowSprite(effectId, pBody, LAYER_EFFECT, pBody->getAnchorPointInPoints() - pBody->getPosition());
        }
        return;
    }

    bool isSelf = (m_Owner->m_GUID == g_LogicManager->m_Hero->m_GUID) ? true : false;
    const string textDamageFile(isSelf ? "Public/self_ss.png" : "Public/other_ss.png");

    float delay = m_DropHPDelay;
    Node* node = Node::create();
    int hp = mode.hp;

	TextAtlas* textAtlas = NULL;
    switch (mode.mode)
    {
    case DropHpMode::DropHpBuff:
    case DropHpMode::Normal:
        {
            textAtlas = TextAtlas::create(__String::createWithFormat(";%d",hp)->getCString(), textDamageFile,  34, 42, "0");
        }break;
    case DropHpMode::Critical:
        {
            ImageView *image = ImageView::create(string(isSelf ? "Public/self_ss_bj.png" : "Public/other_ss_bj.png"));
            image->setPositionX(-55.0f);
            node->addChild(image);
            textAtlas = TextAtlas::create(__String::createWithFormat(";%d",hp)->getCString(), textDamageFile,  34, 42, "0");
            textAtlas->setAnchorPoint(Vec2(0,0.5f));
        }break;
    case DropHpMode::JianShe:
        {
            ImageView *image = ImageView::create(string(isSelf ? "Public/self_ss_js.png" : "Public/self_ss_js.png"));
            image->setPositionX(-55.0f);
            node->addChild(image);
            textAtlas = TextAtlas::create(__String::createWithFormat(";%d",hp)->getCString(), textDamageFile,  34, 42, "0");
            textAtlas->setAnchorPoint(Vec2(0,0.5f));
        }break;
    case DropHpMode::Dodge:
        {
            ImageView *image = ImageView::create(string(isSelf ? "Public/self_ss_sb.png" : "Public/other_ss_sb.png"));
            node->addChild(image);
        }break;
    case DropHpMode::HuDunXiShouBuff:
        {
            ImageView *image = ImageView::create(string(isSelf ? "Public/self_ss_xs.png" : "Public/other_ss_xs.png"));
            image->setPositionX(-55.0f);
            node->addChild(image);
            textAtlas = TextAtlas::create(__String::createWithFormat(";%d",mode.effectId)->getCString(), textDamageFile,  34, 42, "0");
            effectId = 0;
            textAtlas->setAnchorPoint(Vec2(0,0.5f));
        }break;
    case DropHpMode::WuDiMianYiBuff:
        {
            ImageView *image = ImageView::create(string(isSelf ? "Public/self_ss_my.png" : "Public/other_ss_my.png"));
            node->addChild(image);
        }break;
    case DropHpMode::DamageInverseBuff:
        {
            ImageView *image = ImageView::create(string(isSelf ? "Public/self_ss_fs.png" : "Public/other_ss_fs.png"));
            image->setPositionX(-55.0f);
            node->addChild(image);
            textAtlas = TextAtlas::create(__String::createWithFormat(";%d",hp)->getCString(), textDamageFile,  34, 42, "0");
            textAtlas->setAnchorPoint(Vec2(0,0.5f));
        }break;
    default:
        break;
    }

    if (textAtlas)
        node->addChild(textAtlas);
        

    node->setVisible(false);
    //node->setPosition(Point(0,50));
    node->setPosition(pBody->getPosition());
    node->runAction(Sequence::create(DelayTime::create(delay),Show::create(),JumpBy::create(1.0f,Point(80,150),50,1), CallFunc::create(std::bind(&AnimComponent::ReleaseHpAnim,this,node)), nullptr));
    m_Owner->addChild(node, 15);

    if (effectId > 0)
    {
        g_ResManager->ShowSprite(effectId, pBody, LAYER_EFFECT, pBody->getAnchorPointInPoints() - pBody->getPosition());
    }

    m_DropHPDelay += 0.2f;
}

void AnimComponent::PlayAddHpAnmi(const AddHpMode& mode)
{
    cc2d::Node* pBody = GetBody();
    if (pBody == NULL)
        return;

    float delay = m_DropHPDelay;
    Node* node = Node::create();

    int hp = mode.hp;

    TextAtlas* textAtlas = TextAtlas::create(__String::createWithFormat(";%d",hp)->getCString(), "Public/self_zl.png",  34, 42, "0");
    textAtlas->setAnchorPoint(Vec2(0,0.5f));
    node->addChild(textAtlas);

    node->setVisible(false);
    node->setPosition(pBody->getAnchorPointInPoints() - pBody->getPosition());
    node->runAction(Sequence::create(DelayTime::create(delay),Show::create(),JumpBy::create(1.0f,Point(80,150),50,1),CallFunc::create(std::bind(&AnimComponent::ReleaseHpAnim,this,node)), nullptr));
    pBody->addChild(node);

    m_DropHPDelay += 0.2f;
}

void AnimComponent::PlayEffect(cc2d::Sprite* pEffect, int where /*= 0*/)
{
    cc2d::Node* pBody = GetBody();
    if (pBody == NULL)
        return;

    Vec2 position = pBody->getAnchorPointInPoints() - pBody->getPosition();
    pEffect->setPosition(pEffect->getPosition() + position);
    pBody->addChild(pEffect, LAYER_EFFECT);
}

void AnimComponent::Update(float dt)
{
    if (m_ResouceLoading && !m_ResourceKey.empty())
    {
        Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(m_ResourceKey);
        if (!SpriteFrame.empty())
        {
            m_ResourceKey.clear();
            UpdataAction();
            m_ResouceLoading = false;
        }
        else
        {
            ++m_FrameCount;
            if (m_FrameCount > 100)
            {
                log("===============can't load resouce key[%s]===============", m_ResourceKey.c_str());
                m_FrameCount = 0;
            }
        }
    }

    UpdateShadowAndHpBar();
}

void AnimComponent::UpdateShadowAndHpBar()
{
    if (m_ResouceLoading){
        m_Shadow->setVisible(false);
        return;
    }

    cc2d::Node* pBody = GetBody();
    if (pBody == NULL)
        return;

    if (m_Shadow && m_Shadow->isVisible() == false && pBody->getContentSize().width > 0)
    {
        m_Shadow->setPosition(pBody->getAnchorPointInPoints() - pBody->getPosition() + ShaowPosition);
        m_Shadow->setVisible(true);
    }

    if (m_HpBar && m_HpBar->isVisible() == false && pBody->getContentSize().width > 0)
    {
        m_HpBar->setPosition(pBody->getAnchorPointInPoints() - pBody->getPosition() + HpBarPosition);
        m_HpBar->setVisible(true);
    }
}


void AnimComponent::ReleaseHpAnim(cc2d::Node *sender)
{
    sender->removeFromParentAndCleanup(true);
    m_DropHPDelay -= 0.2f;
    if (m_DropHPDelay < 0.0f)
    {
        m_DropHPDelay = 0.0f;
    }
}

void AnimComponent::CheckAsyncLoading(const std::string& resourceKey)
{
    m_ResourceKey.assign(resourceKey);
    m_ResouceLoading = true;
}

NpcAnimComp::NpcAnimComp(MapObj* pObject, Interface* pInterface):AnimComponent(pObject),m_Interfece(pInterface)
{
    m_Body = NULL;
}

void NpcAnimComp::LoadResouce(bool asyncLoad /*= true*/)
{
    AnimComponent::LoadResouce(asyncLoad);
    assert(m_Interfece);
    if (asyncLoad)
        g_ResManager->CreatePlayerPartAsync(m_Interfece->GetModelName());
    else
        g_ResManager->CreatePlayerPart(m_Interfece->GetModelName());

    if (m_Body == NULL)
    {
        m_Body = Sprite::create();    
        m_Owner->addChild(m_Body,0);
    }
}

void NpcAnimComp::SetBodyAnchorPoint(const Vec2& anchor)
{
    if (m_Body)
    {
        m_Body->setAnchorPoint(anchor);
    }
}

void NpcAnimComp::SetBodySacle(float sacle)
{
    if (m_Body)
    {
        m_Body->setScale(sacle);
    }
}

void NpcAnimComp::UpdataAction()
{
    AnimComponent::UpdataAction();

    if (m_Body && m_Interfece)
    {
        static char path[64];
        sprintf(path, "%s_%d_%d", m_Interfece->GetModelName(), m_ActionType, GetShowDirection(m_Direction));
        static string name;
        name = path;
        Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
        if (SpriteFrame.empty())
        {
            if (m_AsyncLoad)
            {
                CheckAsyncLoading(name);
                m_Body->setTexture(ObjectLoadImage);
            }
        }
        else
        {
            Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
            if (m_ActionType > ACTION_DRIVE_MOVE)
            {
                m_Body->runAction(Sequence::create(Animate::create(animation),CallFunc::create(std::bind(&NpcAnimComp::AttcakAnimEnd,this)),NULL));               				
            }
            else
            {
                Action* action = RepeatForever::create( Animate::create(animation));
                m_Body->runAction(action);
                action->step(0.0f);
            }

            //>镜像图片
            if (GetShowDirection(m_Direction) != m_Direction)
                m_Body->setFlippedX(true);
            else
                m_Body->setFlippedX(false);
        }
    }
}


void NpcAnimComp::StopAction()
{
    if (m_Body)
    {
        m_Body->stopAllActions();
    }
}

void NpcAnimComp::SetOpacityWhenEnterMapTile(int inMask)
{
    if (m_Body)
    {
        if (inMask)
        {
            m_Body->setOpacity(160);
        }
        else
        {
            m_Body->setOpacity(255);
        }
    }
}

bool NpcAnimComp::IsSelection(const cc2d::Vec2 &point)
{
    Vec2 pos = m_Owner->convertToNodeSpace(point);
    Rect rect = RectApplyAffineTransform(m_Body->getTextureRect(), m_Body->getNodeToParentAffineTransform());
    rect.origin.x = m_Body->getOffsetPosition().x - m_Body->getContentSize().width * m_Body->getAnchorPoint().x + m_Body->getPositionX();
    rect.origin.y = m_Body->getOffsetPosition().y - m_Body->getContentSize().height * m_Body->getAnchorPoint().y + m_Body->getPositionY();

    if (rect.containsPoint(pos))
    {
        return true;
    }
    return false;
}

cc2d::Node* NpcAnimComp::GetBody()
{
    return m_Body;
}

void NpcAnimComp::AttcakAnimEnd()
{
    if (m_Interfece)
    {
        m_Interfece->OnAttcakAnimEnd();
    }
}

static const int DefaultBodyFace = 1;
static const int DefaultHorse = 1;
static const int DefaultWing = 1;

PlayerAnimComp::PlayerAnimComp(MapObj* pObject, Interface* pInterface):AnimComponent(pObject),m_Interfece(pInterface)
{
    m_Body = NULL;
    m_Head = NULL;
    m_Weapon = NULL;
    m_Wing = NULL;
    m_Horse = NULL;

    m_DieActionToLastFrame = false;
}


Node* PlayerAnimComp::CreateFaceCopyNode(int equipId[EQUIPTYPE_MAX], int sex)
{
    Node* copyNode = Node::create();
    static char path[64];
    static string name;

    //>同步创建,可能会卡.
    if(1)
    {
        Sprite* pBody = Sprite::create();
        copyNode->addChild(pBody);
        int faceId = equipId[EQUIPTYPE_BODY] != 0 ? equipId[EQUIPTYPE_BODY] : DefaultBodyFace;
        faceId = equipId[EQUIPTYPE_FASHION] != 0 ? equipId[EQUIPTYPE_FASHION] : faceId;
        sprintf(path, "player_%d_%d_%d_%d_%d", sex, EQUIPTYPE_BODY, faceId, ACTION_IDLE, DIRECTION_DOWN);
        name = path;
        Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
        if (SpriteFrame.empty()){
            g_ResManager->CreatePlayerPart(StringUtils::format("player_%d_%d_%d", sex, EQUIPTYPE_BODY, faceId)); 
            SpriteFrame = g_ResManager->GetSpriteFrame(name);
        }
        Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
        pBody->runAction(RepeatForever::create( Animate::create(animation)));
        int zorder = g_CSVFileManager->m_PartLayer[DIRECTION_DOWN][EQUIPTYPE_BODY];
        pBody->setLocalZOrder(zorder);
    }

    if (equipId[EQUIPTYPE_WEAPON])
    {
        Sprite* pWeapon = Sprite::create();
        copyNode->addChild(pWeapon);
        //男女共用武器~统一为男性资源
        sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WEAPON, equipId[EQUIPTYPE_WEAPON], ACTION_IDLE, DIRECTION_DOWN);
        name = path;
        Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
        if (SpriteFrame.empty()){
            g_ResManager->CreatePlayerPart(StringUtils::format("player_%d_%d_%d", 1, EQUIPTYPE_WEAPON, equipId[EQUIPTYPE_WEAPON]));
            SpriteFrame = g_ResManager->GetSpriteFrame(name);
        }
        Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
        pWeapon->runAction(RepeatForever::create( Animate::create(animation)));
        int zorder = g_CSVFileManager->m_PartLayer[DIRECTION_DOWN][EQUIPTYPE_WEAPON];
        pWeapon->setLocalZOrder(zorder);
    }

    if (1)
    {
        Sprite* pWing = Sprite::create();
        copyNode->addChild(pWing);
        //男女共用翅膀~统一为男性资源
        sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WING, equipId[EQUIPTYPE_WING] != 0 ? equipId[EQUIPTYPE_WING] : DefaultWing, ACTION_IDLE, DIRECTION_DOWN);
        name = path;
        Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
        if (SpriteFrame.empty()){
            g_ResManager->CreatePlayerPart(StringUtils::format("player_%d_%d_%d", 1, EQUIPTYPE_WING, equipId[EQUIPTYPE_WING]));
            SpriteFrame = g_ResManager->GetSpriteFrame(name);
        }
        Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
        pWing->runAction(RepeatForever::create( Animate::create(animation)));
        int zorder = g_CSVFileManager->m_PartLayer[DIRECTION_DOWN][EQUIPTYPE_WING];
        pWing->setLocalZOrder(zorder);
    }

    CCASSERT(copyNode->getChildrenCount() > 0, "");
    return copyNode;
}

void PlayerAnimComp::LoadResouce(bool asyncLoad)
{
    AnimComponent::LoadResouce(asyncLoad);
    char path1[64];
    string path2;

    int face[EQUIPTYPE_MAX];
    m_Interfece->GetFaces(face);
    int sex = m_Interfece->GetSex();

    int horseFaceId = face[EQUIPTYPE_HORSE] != 0 ? face[EQUIPTYPE_HORSE] : DefaultHorse;
    //男女共用坐骑~统一为男性资源
    sprintf(path1, "player_%d_%d_%d", 1, EQUIPTYPE_HORSE, horseFaceId);
    path2 = path1;
    if (asyncLoad)
        g_ResManager->CreatePlayerPartAsync(path2);
    else
        g_ResManager->CreatePlayerPart(path2);

    if (m_Horse == NULL)
    {
        m_Horse = Sprite::create();	
        m_Owner->addChild(m_Horse,0);
    }

    if (face[EQUIPTYPE_HEAD] != 0)
    {
        sprintf(path1, "player_%d_%d_%d", sex, EQUIPTYPE_HEAD, face[EQUIPTYPE_HEAD]);
        path2 = path1;
        if (asyncLoad)
            g_ResManager->CreatePlayerPartAsync(path2);
        else
            g_ResManager->CreatePlayerPart(path2);

        if (m_Head == NULL)
        {
            m_Head = Sprite::create();
            m_Owner->addChild(m_Head);
        }
        m_Head->setVisible(true);
    }else
    {
        if(m_Head)
            m_Head->setVisible(false);
    }

    int bodyFaceId = face[EQUIPTYPE_BODY] != 0 ? face[EQUIPTYPE_BODY] : DefaultBodyFace;
    bodyFaceId = face[EQUIPTYPE_FASHION] != 0 ? face[EQUIPTYPE_FASHION] : bodyFaceId;
    sprintf(path1, "player_%d_%d_%d", sex, EQUIPTYPE_BODY, bodyFaceId);
    path2 = path1;
    if (asyncLoad)
        g_ResManager->CreatePlayerPartAsync(path2);
    else
        g_ResManager->CreatePlayerPart(path2);

    if (m_Body == NULL)
    {
        m_Body = Sprite::create();
        m_Owner->addChild(m_Body);
    }

    if (face[EQUIPTYPE_WEAPON] != 0)
    {
        //男女共用武器~统一为男性资源
        sprintf(path1, "player_%d_%d_%d", 1, EQUIPTYPE_WEAPON, face[EQUIPTYPE_WEAPON]);
        path2 = path1;
        if (asyncLoad)
            g_ResManager->CreatePlayerPartAsync(path2);
        else
            g_ResManager->CreatePlayerPart(path2);
        if (m_Weapon == NULL)
        {
            m_Weapon = Sprite::create();
            m_Owner->addChild(m_Weapon);
        }
        m_Weapon->setVisible(true);
    }else
    {
        if(m_Weapon)
            m_Weapon->setVisible(false);
    }


    if (face[EQUIPTYPE_WING] != 0)
    {
        //男女共用翅膀~统一为男性资源
        sprintf(path1, "player_%d_%d_%d", 1, EQUIPTYPE_WING, face[EQUIPTYPE_WING]);
        path2 = path1;
        if (asyncLoad)
            g_ResManager->CreatePlayerPartAsync(path2);
        else
            g_ResManager->CreatePlayerPart(path2);

        if (m_Wing == NULL)
        {
            m_Wing = Sprite::create();
            m_Owner->addChild(m_Wing);
        }
        m_Wing->setVisible(true);
    }else
    {
        sprintf(path1, "player_%d_%d_%d", 1, EQUIPTYPE_WING, DefaultWing);    //>默认外观ID
        path2 = path1;
        if (asyncLoad)
            g_ResManager->CreatePlayerPartAsync(path2);
        else
            g_ResManager->CreatePlayerPart(path2);

        if (m_Wing == NULL)
        {
            m_Wing = Sprite::create();
            m_Owner->addChild(m_Wing);
        }
    }


    if (m_Horse)
        m_Horse->setPositionY(80.0f);

    if (m_Head)
        m_Head->setPositionY(80.0f);

    if (m_Body)
        m_Body->setPositionY(80.0f);

    if (m_Weapon)
        m_Weapon->setPositionY(80.0f);

    if (m_Wing)
        m_Wing->setPositionY(80.0f);

    //>不等待上次未完成的异步加载了
    m_ResouceLoading = false;
    m_ResourceKey.clear();

    UpdataAction();
}

void PlayerAnimComp::ChangeAction(ActionType action)
{
    if (action != ActionType::ACTION_DEAD)
    {
        m_DieActionToLastFrame = false;
    }
    AnimComponent::ChangeAction(action);
}

void PlayerAnimComp::UpdataAction()
{
    static char path[64];
    static string name;

    StopAction();

    float player_animation_frame_time = PLAYER_ANIMATION_FRAME_TIME;
    if (m_ActionType == ACTION_ATTACK3)
    {
        player_animation_frame_time /= 2.0f;
    }
    float moveAnimSpeed = 1.5f;

    //>接口获得参数
    float moveSpeed = m_Interfece->AC_GetMoveSpeed();
    int face[EQUIPTYPE_MAX];
    memset(face, 0, sizeof(face));
    m_Interfece->GetFaces(face);
    int sex = m_Interfece->GetSex();

    switch (m_ActionType)
    {
    case ACTION_HORSE_IDLE:
    case ACTION_HORSE_MOVE:
    case ACTION_DRIVE_IDLE:
    case ACTION_DRIVE_MOVE:
        {
            if (m_Horse)
            {
                //男女共用坐骑~统一为男性资源
                m_Horse->setVisible(true);
                sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_HORSE, face[EQUIPTYPE_HORSE] != 0 ? face[EQUIPTYPE_HORSE] : DefaultHorse, m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                }

                Animation* animation = NULL;
                if (m_ActionType == ACTION_HORSE_MOVE || m_ActionType == ACTION_DRIVE_MOVE)
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time * moveAnimSpeed / moveSpeed);
                else
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time);

                m_Horse->runAction(RepeatForever::create( Animate::create(animation)));
                int zorder = g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_HORSE];
                //log("set house zorder=%d", zorder);
                m_Horse->setLocalZOrder(zorder);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Horse->setFlippedX(true);
                else
                    m_Horse->setFlippedX(false);
            }
        }
    case ACTION_IDLE:
    case ACTION_MOVE:
        {
            if (m_Body)
            {
                int faceId = face[EQUIPTYPE_BODY] != 0 ? face[EQUIPTYPE_BODY] : DefaultBodyFace;
                faceId = face[EQUIPTYPE_FASHION] != 0 ? face[EQUIPTYPE_FASHION] : faceId;
                sprintf(path, "player_%d_%d_%d_%d_%d", sex, EQUIPTYPE_BODY, faceId, m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty())
                {
                    CheckAsyncLoading(name);
                    m_Body->setTexture(ObjectLoadImage);
                    return;
                }
                else
                {
                    Animation* animation = NULL;
                    if (m_ActionType == ACTION_MOVE)
                        animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time * moveAnimSpeed / moveSpeed);
                    else
                        animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time);

                    Action* action = RepeatForever::create( Animate::create(animation));
                    m_Body->runAction(action);
                    action->step(0.0f);
                    int zorder = g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_BODY];
                    //log("set body zorder=%d", zorder);
                    m_Body->setLocalZOrder(zorder);

                    //>镜像图片
                    if (GetShowDirection(m_Direction) != m_Direction)
                        m_Body->setFlippedX(true);
                    else
                        m_Body->setFlippedX(false);
                }
            }
            if (m_Head && face[EQUIPTYPE_HEAD])
            {
                sprintf(path, "player_%d_%d_%d_%d_%d", sex, EQUIPTYPE_HEAD, face[EQUIPTYPE_HEAD], m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }

                Animation* animation = NULL;
                if (m_ActionType == ACTION_MOVE)
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time * moveAnimSpeed / moveSpeed);
                else
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time);

                m_Head->runAction(RepeatForever::create( Animate::create(animation)));
                int zorder = g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_HEAD];
                log("set head zorder=%d", zorder);
                m_Head->setLocalZOrder(zorder);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Head->setFlippedX(true);
                else
                    m_Head->setFlippedX(false);
            }
            if (m_Weapon && face[EQUIPTYPE_WEAPON])
            {
                //男女共用武器~统一为男性资源
                sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WEAPON, face[EQUIPTYPE_WEAPON], m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }

                Animation* animation = NULL;
                if (m_ActionType == ACTION_MOVE)
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time * moveAnimSpeed / moveSpeed);
                else
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time);

                m_Weapon->runAction(RepeatForever::create( Animate::create(animation)));
                int zorder = g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_WEAPON];
                //log("set weapon zorder=%d", zorder);
                m_Weapon->setLocalZOrder(zorder);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Weapon->setFlippedX(true);
                else
                    m_Weapon->setFlippedX(false);
            }

            if (m_Wing)
            {
                //男女共用翅膀~统一为男性资源
                sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WING, face[EQUIPTYPE_WING] != 0 ? face[EQUIPTYPE_WING] : DefaultWing, m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }

                Animation* animation = NULL;
                if (m_ActionType == ACTION_MOVE)
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time * moveAnimSpeed / moveSpeed);
                else
                    animation = Animation::createWithSpriteFrames(SpriteFrame, player_animation_frame_time);

                m_Wing->runAction(RepeatForever::create( Animate::create(animation)));
                int zorder = g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_WING];
                //log("set wing zorder=%d", zorder);
                m_Wing->setLocalZOrder(zorder);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Wing->setFlippedX(true);
                else
                    m_Wing->setFlippedX(false);
            }
        }
        break;
    case ACTION_BIRTH:
    case ACTION_DEAD:
        {
            if (m_Body)
            {
                int faceId = face[EQUIPTYPE_BODY] != 0 ? face[EQUIPTYPE_BODY] : DefaultBodyFace;
                faceId = face[EQUIPTYPE_FASHION] != 0 ? face[EQUIPTYPE_FASHION] : faceId;
                sprintf(path, "player_%d_%d_%d_%d_%d", sex, EQUIPTYPE_BODY, faceId, m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty())
                {
                    CheckAsyncLoading(name);
                    m_Body->setTexture(ObjectLoadImage);
                    return;
                }
                else
                {
                    Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
                    Animate* action = Animate::create(animation);
                    m_Body->runAction(action);
                    if (m_DieActionToLastFrame){
                        action->step(0.0f);
                        action->step(action->getDuration());
                    }
                    m_Body->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_BODY]);

                    //>镜像图片
                    if (GetShowDirection(m_Direction) != m_Direction)
                        m_Body->setFlippedX(true);
                    else
                        m_Body->setFlippedX(false);
                }
            }
            if (m_Head && face[EQUIPTYPE_HEAD])
            {
                sprintf(path, "player_%d_%d_%d_%d_%d", sex, EQUIPTYPE_HEAD, face[EQUIPTYPE_HEAD], m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }
                Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);				
                Animate* action = Animate::create(animation);
                m_Head->runAction(action);
                if (m_DieActionToLastFrame){
                    action->step(0.0f);
                    action->step(action->getDuration());
                }
                m_Head->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_HEAD]);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Head->setFlippedX(true);
                else
                    m_Head->setFlippedX(false);
            }		
            if (m_Weapon && face[EQUIPTYPE_WEAPON])
            {
                //男女共用武器~统一为男性资源
                sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WEAPON, face[EQUIPTYPE_WEAPON], m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }
                Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
                Animate* action = Animate::create(animation);
                m_Weapon->runAction(action);
                if (m_DieActionToLastFrame){
                    action->step(0.0f);
                    action->step(action->getDuration());
                }
                m_Weapon->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_WEAPON]);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Weapon->setFlippedX(true);
                else
                    m_Weapon->setFlippedX(false);
            }		
            if (m_Wing)
            {
                //男女共用翅膀~统一为男性资源
                sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WING, face[EQUIPTYPE_WING] != 0 ? face[EQUIPTYPE_WING] : DefaultWing, m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }
                Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
                Animate* action = Animate::create(animation);
                m_Wing->runAction(action);
                if (m_DieActionToLastFrame){
                    action->step(0.0f);
                    action->step(action->getDuration());
                }
                m_Wing->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_WING]);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Wing->setFlippedX(true);
                else
                    m_Wing->setFlippedX(false);
            }
        }
        break;
    case ACTION_ATTACK:
    case ACTION_ATTACK1:
    case ACTION_ATTACK2:
    case ACTION_ATTACK3:
        {
            if (m_Body)
            {
                int faceId = face[EQUIPTYPE_BODY] != 0 ? face[EQUIPTYPE_BODY] : DefaultBodyFace;
                faceId = face[EQUIPTYPE_FASHION] != 0 ? face[EQUIPTYPE_FASHION] : faceId;
                sprintf(path, "player_%d_%d_%d_%d_%d", sex, EQUIPTYPE_BODY, faceId, m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty())
                {
                    CheckAsyncLoading(name);
                    m_Body->setTexture(ObjectLoadImage);
                    return;
                }
                else
                {
                    Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
                    m_Body->runAction(Sequence::create(Animate::create(animation),CallFunc::create(std::bind(&PlayerAnimComp::AttcakAnimEnd,this)),NULL));
                    m_Body->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_BODY]);

                    //>镜像图片
                    if (GetShowDirection(m_Direction) != m_Direction)
                        m_Body->setFlippedX(true);
                    else
                        m_Body->setFlippedX(false);
                }
            }
            if (m_Head && face[EQUIPTYPE_HEAD])
            {
                sprintf(path, "player_%d_%d_%d_%d_%d", sex, EQUIPTYPE_HEAD, face[EQUIPTYPE_HEAD], m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }
                Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);				
                m_Head->runAction(Animate::create(animation));	
                m_Head->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_HEAD]);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Head->setFlippedX(true);
                else
                    m_Head->setFlippedX(false);
            }		
            if (m_Weapon && face[EQUIPTYPE_WEAPON])
            {
                //男女共用武器~统一为男性资源
                sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WEAPON, face[EQUIPTYPE_WEAPON], m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }
                Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
                m_Weapon->runAction(Animate::create(animation));	
                m_Weapon->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_WEAPON]);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Weapon->setFlippedX(true);
                else
                    m_Weapon->setFlippedX(false);
            }		
            if (m_Wing)
            {
                //男女共用翅膀~统一为男性资源
                sprintf(path, "player_%d_%d_%d_%d_%d", 1, EQUIPTYPE_WING, face[EQUIPTYPE_WING] != 0 ? face[EQUIPTYPE_WING] : DefaultWing, m_ActionType, GetShowDirection(m_Direction));
                name = path;
                Vector<SpriteFrame*> SpriteFrame = g_ResManager->GetSpriteFrame(name);
                if (SpriteFrame.empty()){
                    CheckAsyncLoading(name);
                    return;
                }
                Animation* animation = Animation::createWithSpriteFrames(SpriteFrame, PLAYER_ANIMATION_FRAME_TIME);
                m_Wing->runAction(Animate::create(animation));	
                m_Wing->setLocalZOrder(g_CSVFileManager->m_PartLayer[m_Direction][EQUIPTYPE_WING]);

                //>镜像图片
                if (GetShowDirection(m_Direction) != m_Direction)
                    m_Wing->setFlippedX(true);
                else
                    m_Wing->setFlippedX(false);
            }
        }
        break;
    default:
        break;
    }
}

void PlayerAnimComp::StopAction()
{
    if (m_Head)
    {
        m_Head->stopAllActions();
    }
    if (m_Body)
    {
        m_Body->stopAllActions();
    }
    if (m_Weapon)
    {
        m_Weapon->stopAllActions();
    }
    if (m_Wing)
    {
        m_Wing->stopAllActions();
    }
    if (m_Horse)
    {
        m_Horse->stopAllActions();
    }
}

void PlayerAnimComp::SetOpacityWhenEnterMapTile(int inMask)
{
    int opacity = 255;

    if (inMask){
        opacity = 160;
    }

    if (m_Head)
    {
        m_Head->setOpacity(opacity);
    }    
    if (m_Body)
    {
        m_Body->setOpacity(opacity);
    }    
    if (m_Weapon)
    {
        m_Weapon->setOpacity(opacity);
    }
    if (m_Wing)
    {
        m_Wing->setOpacity(opacity);
    }
    if (m_Horse)
    {
        m_Horse->setOpacity(opacity);
    }
}

bool PlayerAnimComp::IsSelection(const cc2d::Vec2 &point)
{
    Vec2 pos = m_Owner->convertToNodeSpace(point);
    Rect rect;
    if (m_Body)
    {
        rect = RectApplyAffineTransform(m_Body->getTextureRect(), m_Body->getNodeToParentAffineTransform());
        rect.origin.x = m_Body->getOffsetPosition().x - m_Body->getContentSize().width * m_Body->getAnchorPoint().x + m_Body->getPositionX();
        rect.origin.y = m_Body->getOffsetPosition().y - m_Body->getContentSize().height * m_Body->getAnchorPoint().y + m_Body->getPositionY();
        if (rect.containsPoint(pos))
        {
            return true;
        }	
    }
    if (m_Head)
    {
        rect = RectApplyAffineTransform(m_Head->getTextureRect(), m_Head->getNodeToParentAffineTransform());
        rect.origin.x = m_Head->getOffsetPosition().x - m_Head->getContentSize().width * m_Head->getAnchorPoint().x;
        rect.origin.y = m_Head->getOffsetPosition().y - m_Head->getContentSize().height * m_Head->getAnchorPoint().y;
        if (rect.containsPoint(pos))
        {
            return true;
        }
    }
    if (m_Horse && m_Horse->isVisible())
    {
        rect = RectApplyAffineTransform(m_Horse->getTextureRect(), m_Horse->getNodeToParentAffineTransform());
        rect.origin.x = m_Horse->getOffsetPosition().x - m_Horse->getContentSize().width * m_Horse->getAnchorPoint().x;
        rect.origin.y = m_Horse->getOffsetPosition().y - m_Horse->getContentSize().height * m_Horse->getAnchorPoint().y;
        if (rect.containsPoint(pos))
        {
            return true;
        }
    }

    return false;
}

cc2d::Node* PlayerAnimComp::GetBody()
{
    return m_Body;
}

void PlayerAnimComp::AttcakAnimEnd()
{
    if (m_Interfece)
    {
        m_Interfece->OnAttcakAnimEnd();
    }
}

