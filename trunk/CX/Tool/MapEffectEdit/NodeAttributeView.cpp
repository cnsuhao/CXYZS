#include "stdafx.h"
#include "cocos2d.h"
#include "NodeAttributeView.h"
#include "SelectHelper.h"

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
    SetOpacity(255);
    SetRenderLevel(0);

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
        case IDC_ROT:
            {
                float t;
                CString& text = GetDlgItemText(IDC_ROT);
                if(sscanf(text.c_str(),"%f", &t) != 1)
                {
                    SetRotation(node->getRotation());
                }else
                {
                    node->setRotation(t);
                }
            }break;
        case IDC_OPACITY:
            {
                int t;
                CString& text = GetDlgItemText(IDC_OPACITY);
                if(sscanf(text.c_str(),"%d", &t) != 1)
                {
                    SetOpacity(node->getOpacity());
                }else
                {
                    node->setOpacity((GLubyte)t);
                }
            }break;
        case IDC_RENDER_LEVEL:
            {
                int t;
                CString& text = GetDlgItemText(IDC_RENDER_LEVEL);
                if (sscanf(text.c_str(),"%d",&t) != 1)
                {
                    SetRenderLevel(node->getLocalZOrder());
                }
                else
                {
                    node->setLocalZOrder(t);
                }
            }
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
    SetRotation(node->getRotation());
    SetOpacity(node->getOpacity());
    SetRenderLevel(node->getLocalZOrder());
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

void NodeAttributeView::SetRotation(float v)
{
    _stprintf(gSprBuff, _T("%f"), v);
    SetDlgItemText(IDC_ROT, gSprBuff);
}

void NodeAttributeView::SetOpacity(GLubyte v)
{
    _stprintf(gSprBuff, _T("%d"), v);
    SetDlgItemText(IDC_OPACITY, gSprBuff);
}

void NodeAttributeView::SetRenderLevel(int level)
{
    _stprintf(gSprBuff, _T("%d"), level);
    SetDlgItemText(IDC_RENDER_LEVEL, gSprBuff);
}

NodeAttributeDocker::NodeAttributeDocker(void)
{
    nodeView = NULL;
    spriteView = NULL;
    animView = NULL;

    SetView(m_view);
    SetBarWidth(4);
    SetCaption(_T("ÊôÐÔ"));
}

NodeAttributeDocker::~NodeAttributeDocker(void)
{

}

void NodeAttributeDocker::OnInitialUpdate()
{

} 

void NodeAttributeDocker::OnNodeSelect(EditClass* node)
{
    if (node->GetType() == NodeType::SPRITE)
    {
        if (nodeView == NULL)
        {
            nodeView = new NodeAttributeView(IDD_NODE);
            m_view.AddTabPage(nodeView, _T("ÌØÐ§"), IDD_NODE);
        }
        nodeView->OnNodeSelect(node);
    }
}
