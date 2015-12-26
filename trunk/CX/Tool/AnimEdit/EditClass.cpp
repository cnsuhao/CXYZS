#include "stdafx.h"
#include "EditClass.h"
#include "2d/CCNode.h"
#include "base/CCDirector.h"

USING_NS_CC;

EditClass::EditClass(void)
{
}


EditClass::~EditClass(void)
{

}

void EditClass::Destroy()
{
    Node* node = GetNode();
    if (node)
    {
        node->removeFromParentAndCleanup(true);
    }
}

void EditNode::OnSelect()
{

}

void EditSprite::OnSelect()
{

}

EditRoot::EditRoot()
{

}

void EditRoot::OnSelect()
{

}

void EditRoot::MoveToScreenCenter()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    GetNode()->setPosition(Vec2(visibleSize / 2.0f) + origin);
}
