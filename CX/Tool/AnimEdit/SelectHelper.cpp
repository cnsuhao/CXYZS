#include "stdafx.h"
#include "SelectHelper.h"
#include "2d/CCNode.h"
#include "2d/CCDrawNode.h"
USING_NS_CC;

SelectHelper gSelectHepler;

SelectHelper::SelectHelper(void)
{
    drawNode = NULL;
    selectNode = NULL;
    touchNode = NULL;
}


SelectHelper::~SelectHelper(void)
{
}

bool SelectHelper::Init(cc2d::Node* scene)
{
    drawNode = DrawNode::create();
    scene->addChild(drawNode, 99999);

    return true;
}

void SelectHelper::SetSelectNode(EditClass* node, int type)
{
    drawNode->clear();
    selectNode = node;
    if (GetMyApp()->m_Frame.GetView().sceneRoot != node)
    {
        Rect drawRect;
        drawRect.origin = selectNode->GetNode()->convertToWorldSpace(Size::ZERO);
        drawRect.size = selectNode->GetNode()->convertToWorldSpace(selectNode->GetNode()->getContentSize());
        drawNode->drawRect(drawRect.origin, drawRect.size, Color4F::GREEN);
    }
    // ÏÔÊ¾¿Ø¼þÊôÐÔ
    GetMyApp()->m_Frame.OnNodeSelect(node, type);
}

void SelectHelper::NodeMoveUpdate()
{
    if (selectNode)
    {
        drawNode->clear();
        Rect drawRect;
        drawRect.origin = selectNode->GetNode()->convertToWorldSpace(Size::ZERO);
        drawRect.size = selectNode->GetNode()->convertToWorldSpace(selectNode->GetNode()->getContentSize());
        drawNode->drawRect(drawRect.origin, drawRect.size, Color4F::GREEN);
    }
}
