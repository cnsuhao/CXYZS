#include "stdafx.h"
#include "SelectHelper.h"
#include "EditApp.h"
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

void SelectHelper::SetSelectNode(EditClass* node)
{
    drawNode->clear();
    selectNode = node;
    if (selectNode)
    {
        //if (GetMyApp()->m_Frame.GetView().sceneRoot != node)
        {
            Rect drawRect;
            drawRect.origin = selectNode->GetNode()->convertToWorldSpace(Size::ZERO);
            drawRect.size = selectNode->GetNode()->convertToWorldSpace(selectNode->GetNode()->getContentSize());
            //if (selectNode->GetType() == NodeType::SPRITE)
            //{
            //    drawRect.size = static_cast<Sprite*>(selectNode->GetNode())->getTextureRect().size;
            //}
            drawNode->drawRect(drawRect.origin, drawRect.size, Color4F::GREEN);
        }
        // ÏÔÊ¾¿Ø¼þÊôÐÔ
        GetMyApp()->m_Frame.OnNodeSelect(node);
    }
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

void SelectHelper::HideSelect()
{
    drawNode->clear();
}
