#include "stdafx.h"
#include "NodeAttributeView.h"
#include "SpriteAttributeView.h"
#include "AnimAttributeView.h"
#include "TopologyView.h"
#include "NodeFactory.h"
#include "SelectHelper.h"
#include "AnimMng.h"
#include "2d/CCNode.h"

USING_NS_CC;

TCHAR gSprBuff[32];

NodeAttributeView::NodeAttributeView(UINT nResID):CDialog(nResID)
{

}

NodeAttributeView::~NodeAttributeView()
{

}

HWND NodeAttributeView::Create(CWnd* pParent)
{
    return DoModeless(pParent);
}

BOOL NodeAttributeView::OnInitDialog()
{
    AttachItem(IDC_NAME,	    editName);
    AttachItem(IDC_COORD_X,	    editCoord[0]);
    AttachItem(IDC_COORD_Y,	    editCoord[1]);
    AttachItem(IDC_ANCHOR_X,	editAnchro[0]);
    AttachItem(IDC_ANCHOR_Y,	editAnchro[1]);
    AttachItem(IDC_SCALE_X,	    editScale[0]);
    AttachItem(IDC_SCALE_Y,	    editScale[1]);

    SetCoordX(0);
    SetCoordY(0);
    SetAnchorX(0);
    SetAnchorY(0);
    SetScaleX(0);
    SetScaleY(0);

    return TRUE;
}

INT_PTR NodeAttributeView::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    return DialogProcDefault(uMsg, wParam, lParam);
}

void NodeAttributeView::OnOK()
{
    CWnd* focus = GetFocus();
    EditClass* eNode = gSelectHepler.GetSelectNode();
    Node* node = eNode->GetNode();
    if (focus && gSelectHepler.GetSelectNode() != NULL)
    {
        switch (focus->GetDlgCtrlID())
        {
        case IDC_NAME:
            {
                CString& text = GetDlgItemText(IDC_NAME);
                node->setName(text.c_str());
            }break;
        case IDC_COORD_X:
            {
                float t;
                CString& text = GetDlgItemText(IDC_COORD_X);
                if(sscanf(text.c_str(),"%f", &t) != 1)
                {
                    SetCoordX(node->getPositionX());
                }else
                {
                     node->setPositionX(t);
                     gSelectHepler.NodeMoveUpdate();
                }
            }break;
        case IDC_COORD_Y:
            {
                float t;
                CString& text = GetDlgItemText(IDC_COORD_Y);
                if(sscanf(text.c_str(),"%f", &t) != 1)
                {
                    SetCoordY(node->getPositionY());
                }else
                {
                    node->setPositionY(t);
                    gSelectHepler.NodeMoveUpdate();
                }
            }break;
        case IDC_ANCHOR_X:
            {
                float t;
                CString& text = GetDlgItemText(IDC_ANCHOR_X);
                if(sscanf(text.c_str(),"%f", &t) != 1)
                {
                    SetAnchorX(node->getAnchorPoint().x);
                }else
                {
                    Vec2 a = node->getAnchorPoint();
                    a.x = t;
                    node->setAnchorPoint(a);
                }
            }break;
        case IDC_ANCHOR_Y:
            {
                float t;
                CString& text = GetDlgItemText(IDC_ANCHOR_Y);
                if(sscanf(text.c_str(),"%f", &t) != 1)
                {
                    SetAnchorY(node->getAnchorPoint().y);
                }else
                {
                    Vec2 a = node->getAnchorPoint();
                    a.y = t;
                    node->setAnchorPoint(a);
                }
            }break;
        case IDC_SCALE_X:
            {
                float t;
                CString& text = GetDlgItemText(IDC_SCALE_X);
                if(sscanf(text.c_str(),"%f", &t) != 1)
                {
                    SetScaleX(node->getScaleX());
                }else
                {
                    node->setScaleX(t);
                }
            }break;
        case IDC_SCALE_Y:
            {
                float t;
                CString& text = GetDlgItemText(IDC_SCALE_Y);
                if(sscanf(text.c_str(),"%f", &t) != 1)
                {
                    SetScaleY(node->getScaleY());
                }else
                {
                    node->setScaleY(t);
                }
            }break;
        default:
            break;
        }
    }
}

void NodeAttributeView::OnNodeSelect(EditClass* eNode)
{
    Node* node = eNode->GetNode();
    if (!node)
    {
        return;
    }
    SetName(node->getName().c_str());
    SetCoordX(node->getPositionX());
    SetCoordY(node->getPositionY());
    SetAnchorX(node->getAnchorPoint().x);
    SetAnchorY(node->getAnchorPoint().y);
    SetScaleX(node->getScaleX());
    SetScaleY(node->getScaleY());
}

void NodeAttributeView::OnMoveEnd(EditClass* eNode)
{
    Node* node = eNode->GetNode();
    if (node)
    {
        SetCoordX(node->getPositionX());
        SetCoordY(node->getPositionY());
    }
}

void NodeAttributeView::SetName(const char* name)
{
    SetDlgItemText(IDC_NAME, name);
}

void NodeAttributeView::SetCoordX(float v)
{
    _stprintf(gSprBuff, _T("%f"), v);
    SetDlgItemText(IDC_COORD_X, gSprBuff);

}

void NodeAttributeView::SetCoordY(float v)
{
    _stprintf(gSprBuff, _T("%f"), v);
    SetDlgItemText(IDC_COORD_Y, gSprBuff);

}

void NodeAttributeView::SetAnchorX(float v)
{
    _stprintf(gSprBuff, _T("%f"), v);
    SetDlgItemText(IDC_ANCHOR_X, gSprBuff);

}

void NodeAttributeView::SetAnchorY(float v)
{
    _stprintf(gSprBuff, _T("%f"), v);
    SetDlgItemText(IDC_ANCHOR_Y, gSprBuff);

}

void NodeAttributeView::SetScaleX(float v)
{
    _stprintf(gSprBuff, _T("%f"), v);
    SetDlgItemText(IDC_SCALE_X,gSprBuff);
}

void NodeAttributeView::SetScaleY(float v)
{
    _stprintf(gSprBuff, _T("%f"), v);
    SetDlgItemText(IDC_SCALE_Y, gSprBuff);
}




NodeAttributeDocker::NodeAttributeDocker(void)
{
    nodeView = NULL;
    spriteView = NULL;
    animView = NULL;

    SetView(m_view);
    SetBarWidth(4);
    SetCaption(_T("属性"));
}

NodeAttributeDocker::~NodeAttributeDocker(void)
{

}

void NodeAttributeDocker::OnInitialUpdate()
{

} 

void NodeAttributeDocker::OnNodeSelect(EditClass* eNode, int type)
{
    Node* node = eNode->GetNode();
    if (gAnimMng.IsEditingNode())
    {
        return;
    }

    if (animView)
    {
        m_view.RemoveTabPage(m_view.GetTabIndex(animView));
        animView = NULL;
    }

    if (nodeView == NULL)
    {
        nodeView = new NodeAttributeView(IDD_NODE);
        m_view.AddTabPage(nodeView, _T("节点"), IDD_NODE);
    }
    nodeView->OnNodeSelect(eNode);

    if (type == SPRITE)
    {
        if (spriteView == NULL)
        {
            spriteView = new SpriteAttributeView(IDD_SPRITE);
            m_view.AddTabPage(spriteView, _T("精灵"), IDD_SPRITE);
        }
        spriteView->OnNodeSelect(node);
    }
}

void NodeAttributeDocker::OnEditAnimNode(EditClass* eNode, int type)
{
    if (!gAnimMng.IsEditingNode())
    {
        return;
    }
    if (nodeView)
    {
        m_view.RemoveTabPage(m_view.GetTabIndex(nodeView));
        nodeView = NULL;
    }
    if (spriteView)
    {
        m_view.RemoveTabPage(m_view.GetTabIndex(spriteView));
        spriteView = NULL;
    }

    if (animView == NULL)
    {
        animView = new AnimAttributeView(IDD_ANIM);
        m_view.AddTabPage(animView, _T("动画数据"), IDD_ANIM);
    }
    animView->OnNodeSelect(eNode, type);
}

void NodeAttributeDocker::OnMoveEnd(EditClass* eNode)
{
    if (gAnimMng.IsEditingNode() && gAnimMng.GetEditingNode() == eNode)
    {
        if (animView)
        {
            animView->OnMoveEnd(eNode);
        }
    }
    else
    {
        if (nodeView)
        {
            nodeView->OnMoveEnd(eNode);
        }
    }
}

void NodeAttributeDocker::ResetToStart()
{
    if (nodeView)
    {
        m_view.RemoveTabPage(m_view.GetTabIndex(nodeView));
        nodeView = NULL;
    }
    if (spriteView)
    {
        m_view.RemoveTabPage(m_view.GetTabIndex(spriteView));
        spriteView = NULL;
    }
    if (animView)
    {
        m_view.RemoveTabPage(m_view.GetTabIndex(animView));
        animView = NULL;
    }
}

unsigned char NodeAttributeDocker::GetInterpolation()
{
    return animView->GetInterpolationIndex();
}
