#include "stdafx.h"
#include "NodeFactory.h"
#include "SelectHelper.h"
#include <sstream>
using namespace std;
#include "2d/CCNode.h"
#include "2d/CCDrawNode.h"
#include "2d/CCSprite.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"

USING_NS_CC;

static int nodeId = 0;
static int spriteId = 0;

static Vec2 p1 = Vec2(100, 100);
static Vec2 p2 = Vec2(200, 200);

static bool _shift = false;

EditRoot* NodeFactory::CreateRoot()
{
    EditRoot* eRoot = new EditRoot();
    DrawNode* node = DrawNode::create();
    eRoot->node = node;
    node->setName("Root");

    node->setContentSize(Size(1024,615));
    node->ignoreAnchorPointForPosition(false);
    node->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    node->drawSolidRect(Vec2::ZERO, Vec2(1024,615),Color4F::GRAY);
    /*
    node->drawCubicBezier(Vec2::ZERO, p1, p2, Vec2(900.0, 600.0f), 100, Color4F::YELLOW);
    // 设置点击回调
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event)
    {
        DrawNode* node = static_cast<DrawNode*>(event->getCurrentTarget());
        Vec2 nsp = node->convertTouchToNodeSpace(touch);
        Size size = node->getContentSize();
        Rect rect(0,0,size.width, size.height);
        if (rect.containsPoint(nsp))
        {
            return true;
        }
        return false;
    };

    listener->onTouchMoved = [](Touch* touch, Event* event)
    {
        DrawNode* node = static_cast<DrawNode*>(event->getCurrentTarget());
        if (_shift)
        {
            p1 = node->convertTouchToNodeSpace(touch);
        }
        else
        {
            p2 = node->convertTouchToNodeSpace(touch);
        }
        node->clear();
        node->drawCubicBezier(Vec2::ZERO, p1, p2, node->getContentSize(), 100, Color4F::YELLOW);
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, node);

    //键盘监听
    auto listener2 = EventListenerKeyboard::create();
    listener2->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event)
    {
        if (keyCode == EventKeyboard::KeyCode::KEY_Z)
        {
            _shift = 1;
        }
    };

    listener2->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event)
    {
        if (keyCode == EventKeyboard::KeyCode::KEY_Z)
        {
            _shift = 0;
        }
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener2, node);
    */
    eRoot->MoveToScreenCenter();
    return eRoot;
}

EditNode* NodeFactory::CreateNode()
{
    EditNode* eNode = new EditNode();
    DrawNode* node = DrawNode::create();
    eNode->node = node;
    node->setUserData(eNode);
    node->setContentSize(Size(100,100));
    node->drawSolidRect(Vec2::ZERO, node->getContentSize(), Color4F(1.0f,0,0,0.25f));
    // 设置点击回调
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event)
    {
        Node* node = static_cast<Node*>(event->getCurrentTarget());
        Vec2 nsp = node->convertTouchToNodeSpace(touch);
        Size size = node->getContentSize();
        Rect rect(0,0,size.width, size.height);
        if (rect.containsPoint(nsp))
        {
            EditNode* eNode = reinterpret_cast<EditNode*>(node->getUserData());
            // 显示高亮框
            gSelectHepler.SetSelectNode(eNode, NODE);
            // 修改鼠标
            //TRACE("----------->onTouchBegin\n");
            gSelectHepler.SetTouchNode(eNode);
            return true;
        }
        return false;
    };

    listener->onTouchMoved = [](Touch* touch, Event* event)
    {
        Node* node = static_cast<Node*>(event->getCurrentTarget());
        EditNode* eNode = reinterpret_cast<EditNode*>(node->getUserData());
        if (gSelectHepler.GetTouchNode() == eNode)
        {
            Vec2 dt = touch->getDelta();
            node->setPosition(node->getPosition() + dt);
            gSelectHepler.NodeMoveUpdate();
        }
    };

    listener->onTouchEnded = [](Touch* touch, Event* event)
    {
        Node* node = static_cast<Node*>(event->getCurrentTarget());
        EditNode* eNode = reinterpret_cast<EditNode*>(node->getUserData());
        if (gSelectHepler.GetTouchNode() == eNode)
        {
            GetMyApp()->m_Frame.OnNodeMoveEnd(eNode);
            gSelectHepler.SetTouchNode(NULL);
        }
        //TRACE("----------->onTouchEnd\n");
    };

    stringstream name;
    name << "node_";
    name << ++nodeId;
    node->setName(name.str().c_str());

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, node);
    return eNode;
}

EditSprite* NodeFactory::CreateSprite()
{
    EditSprite* eSprite = new EditSprite();
    Sprite* sprite = Sprite::create("DefaultSprite.png");
    eSprite->sprite = sprite;
    sprite->setUserData(eSprite);

    // 设置点击回调
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event)
    {
        Sprite* sprite = static_cast<Sprite*>(event->getCurrentTarget());
        EditSprite* eSprite = reinterpret_cast<EditSprite*>(sprite->getUserData());
        Vec2 nsp = sprite->convertTouchToNodeSpace(touch);
        Size size = sprite->getContentSize();
        Rect rect(0,0,size.width, size.height);
        if (rect.containsPoint(nsp))
        {
            // 显示高亮框
            gSelectHepler.SetSelectNode(eSprite, SPRITE);
            // 修改鼠标
            //TRACE("----------->onTouchBegin\n");
            gSelectHepler.SetTouchNode(eSprite);
            return true;
        }
        return false;
    };

    listener->onTouchMoved = [](Touch* touch, Event* event)
    {
        Sprite* sprite = static_cast<Sprite*>(event->getCurrentTarget());
        EditSprite* eSprite = reinterpret_cast<EditSprite*>(sprite->getUserData());
        if (gSelectHepler.GetTouchNode() == eSprite)
        {
            Vec2 dt = touch->getDelta();
            sprite->setPosition(sprite->getPosition() + dt);
            gSelectHepler.NodeMoveUpdate();
        }
    };

    listener->onTouchEnded = [](Touch* touch, Event* event)
    {
        Sprite* sprite = static_cast<Sprite*>(event->getCurrentTarget());
        EditSprite* eSprite = reinterpret_cast<EditSprite*>(sprite->getUserData());
        if (gSelectHepler.GetTouchNode() == eSprite)
        {
            GetMyApp()->m_Frame.OnNodeMoveEnd(eSprite);
            gSelectHepler.SetTouchNode(NULL);
        }
        //TRACE("----------->onTouchEnd\n");
    };
    
    stringstream name;
    name << "sprite_";
    name << ++spriteId;
    sprite->setName(name.str().c_str());



    //ScaleTo* a1 = ScaleTo::create(1.0,2.0);
    //MoveTo* a2 = MoveTo::create(1.0, Vec2(300.f,300.f));
    //sprite->runAction(Sequence::create(a1, a2, NULL));
    //sprite->runAction(EaseElasticOut::create(a1)); sprite->runAction(a2);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, sprite);
    return eSprite;
}

void NodeFactory::DeleateEditClass(EditClass* editClass)
{
    if (editClass)
    {
        editClass->Destroy();
        delete editClass;
    }
}
