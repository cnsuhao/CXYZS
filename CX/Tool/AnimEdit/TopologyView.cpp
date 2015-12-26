#include "stdafx.h"
#include "TopologyView.h"
#include "EditApp.h"
#include "AnimMng.h"
#include "SelectHelper.h"
#include "NodeFactory.h"
#include "2d/CCNode.h"
using namespace std;
USING_NS_CC;

TopologyView::TopologyView(UINT nResID):CDialog(nResID)
{

}

TopologyView::~TopologyView(void)
{
}

HWND TopologyView::Create(CWnd* pParent)
{
    return DoModeless(pParent);
}

BOOL TopologyView::OnInitDialog()
{
    AttachItem(IDW_TOPOLOGY_TREE, tree);

    return TRUE;
}

BOOL TopologyView::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

LRESULT TopologyView::OnNotify(WPARAM wParam, LPARAM lParam)
{
    LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;
    switch (pnmtv->hdr.code)
    {
    case TVN_SELCHANGED:
        {
            TRACE("TVN_SELCHANGED\n");
            //pnmtv->itemNew.hItem;
        }break;
    case TVN_SELCHANGING:
        {
            TRACE("TVN_SELCHANGING\n");
        }break;
    case TVN_ITEMEXPANDED:
        {
            TRACE("TVN_ITEMEXPANDED\n");
        }break;
    case TVN_ITEMEXPANDING:
        {
            TRACE("TVN_ITEMEXPANDING\n");
        }break;
    default:
        break;
    }
    return 0L;
}

INT_PTR TopologyView::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DialogProcDefault(uMsg, wParam, lParam);
}

TopologyView::TopologyTree::TopologyTree()
{

}

TopologyView::TopologyTree::~TopologyTree()
{
    if (IsWindow()) DeleteAllItems();
}

HTREEITEM TopologyView::TopologyTree::AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage)
{
    TVITEM tvi;
    ZeroMemory(&tvi, sizeof(TVITEM));
    tvi.mask = TVIF_TEXT;
    tvi.iImage = 0;
    tvi.iSelectedImage = 0;
    tvi.pszText = (LPTSTR)szText;

    TVINSERTSTRUCT tvis;
    ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));
    tvis.hParent = hParent;
    tvis.item = tvi;

	return InsertItem(tvis);
}

void TopologyView::TopologyTree::UpdateByNode(EditClass* root)
{
    DeleteAllItems();
    AddNode(NULL, root);
    Expand(GetRootItem(), TVE_EXPAND);
}

LRESULT TopologyView::TopologyTree::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    //char buff[32];
    //sprintf(buff,"%s(%x)\n", TranslateWMsg(uMsg), uMsg);
    //TRACE(buff);
    
    switch(uMsg)
    {
    case WM_LBUTTONDOWN:
        {
            return TRUE;
        }break;
    case WM_LBUTTONUP :
        {
            SetForegroundWindow();
            TVHITTESTINFO thti;
            CPoint pt = GetCursorPos();
            ScreenToClient(pt);
            thti.pt = pt;
            thti.flags = TVHT_TORIGHT;
            HTREEITEM htItem = HitTest(thti);
            if (htItem)
            {
                SetFocus();
                SelectItem(htItem);
                EditClass* eNode = reinterpret_cast<EditClass*>(GetItemData(htItem));
                gSelectHepler.SetSelectNode(eNode, eNode->GetType());
                return TRUE;
            }
        }break;
    case WM_RBUTTONDOWN:
        {
            return TRUE;
        }break;
    case WM_RBUTTONUP :
        {
            CMenu TopMenu(IDW_MENU_NODE);
            CMenu* pSubMenu = TopMenu.GetSubMenu(0);
            SetForegroundWindow();
            TVHITTESTINFO thti;
            CPoint pt = GetCursorPos();
            ScreenToClient(pt);
            thti.pt = pt;
            thti.flags = TVHT_TORIGHT;
            HTREEITEM htItem = HitTest(thti);
            if (htItem)
            {
                SetFocus();
                if (htItem != GetRootItem())
                {
                    if (gAnimMng.IsEditingNode())
                    {
                        pSubMenu->EnableMenuItem(IDM_DELETE_NODE, TRUE);
                    }
                    else
                    {
                        pSubMenu->EnableMenuItem(IDM_DELETE_NODE, FALSE);
                    }
                    CString str = GetItemText(htItem, 255);
                    EditClass* eNode = reinterpret_cast<EditClass*>(GetItemData(htItem));
                    SelectItem(htItem);
                    UINT uSelected = pSubMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, GetCursorPos().x, GetCursorPos().y, this, NULL);
                    switch (uSelected)
                    {
                    case IDM_POS_ANIM : 
                        {
                            gAnimMng.ExitEidtNode();
                            gAnimMng.EnterEditNode(eNode, (int)FrameType::Position);
                        }break;
                    case IDM_SCL_ANIM : 
                        {
                            gAnimMng.ExitEidtNode();
                            gAnimMng.EnterEditNode(eNode, (int)FrameType::Scale);
                        } break;
                    case IDM_FADE_ANIM : 
                        {
                            gAnimMng.ExitEidtNode();
                            gAnimMng.EnterEditNode(eNode, (int)FrameType::Opacity);
                        }break;
                    case IDM_ROT_ANIM : 
                        {
                            gAnimMng.ExitEidtNode();
                            gAnimMng.EnterEditNode(eNode, (int)FrameType::Rotation);
                        }break;
                    case IDM_DELETE_NODE: 
                        {
                            if (gSelectHepler.GetSelectNode() == eNode)
                            {
                                gSelectHepler.SetSelectNode(GetMyApp()->m_Frame.GetView().sceneRoot, int(NodeType::ROOT));
                            }
                            NodeFactory::DeleateEditClass(eNode);

                            GetMyApp()->m_Frame.UpdateTopology();
                        }break; 
                    default:
                        break;
                    }
                    return TRUE;
                }
            }
        }
    default:
        break;
    }
 
    return WndProcDefault(uMsg, wParam, lParam);
}

void TopologyView::TopologyTree::AddNode(HTREEITEM hParent, EditClass* eNode)
{
    HTREEITEM  h = AddItem(hParent, eNode->GetNode()->getName().c_str(),0);
    SetItemData(h, (DWORD_PTR)eNode);

    Vector<Node*>& childrens =  eNode->GetNode()->getChildren();
    Node* childNode = NULL;
    for (int i = 0; i < childrens.size(); ++i)
    {
        childNode = childrens.at(i);
        EditClass* childeNode = reinterpret_cast<EditClass*>(childNode->getUserData());
        AddNode(h, childeNode);
    }
}

TopologyDocker::TopologyDocker(void):topologyView(IDD_TOPOLOGY)
{
    SetView(topologyView);

    SetBarWidth(4);
    SetCaption(_T("∏Ÿ¡Ï"));
}

TopologyDocker::~TopologyDocker(void)
{

}

void TopologyDocker::OnInitialUpdate()
{

}

void TopologyDocker::UpdateTopology(EditClass* root)
{
    topologyView.UpdateByNode(root);
}
