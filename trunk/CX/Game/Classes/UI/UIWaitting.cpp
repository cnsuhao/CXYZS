#include "UIWaitting.h"

USING_NS_CC;
using namespace cc2dui;

#include "BaseFunction.h"
#include "ResManager.h"
#include "CSVFileManager.h"

#include "CCSGUIReader.h"
#include "CocoStudio.h"

using namespace cocostudio;

UIWaitting::UIWaitting(void)
{
    m_Panel = NULL;
    m_Text = NULL;
}

UIWaitting::~UIWaitting(void)
{
}

bool UIWaitting::init()
{
    m_Panel = CSLoader::createNode("UI/Wait.csb");
    Node* tempNode = static_cast<Node*>(m_Panel->getChildByName("Node"));
    Sprite* pSpr = g_ResManager->CreateSprite(7);
    if (pSpr)
    {
        pSpr->setPosition(tempNode->getPosition());
        m_Panel->addChild(pSpr);
        tempNode->setVisible(false);
    }
    else
    {
        CCASSERT(0, "UIWaitting不能找到等待动画特效.");
    }

    m_Text = static_cast<Text*>(m_Panel->getChildByName("Text"));
    CCASSERT(m_Text, "");

    addChild(m_Panel);
    return true;
}

void UIWaitting::OnShow()
{
    UIBase::OnShow();
    setLocalZOrder(LAYER_UI + 1); //>调整显示级别比其他UI高.
}

void UIWaitting::SetTextID(uint32 id)
{
    const TextInfo* pInfo = g_CSVFileManager->GetTextByID(id);
    if (pInfo && m_Text)
    {
        m_Text->setString( BaseFunction::a2u(pInfo->strContent ) );
    }
}

void UIWaitting::SetText(string& content)
{
    if (!content.empty() && m_Text)
    {
        m_Text->setString(content);
    }
}
