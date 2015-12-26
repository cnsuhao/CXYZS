#include "stdafx.h"
#include "EditorFrame.h"
#include "NodeFactory.h"

#include "AnimationView.h"
#include "NodeAttributeView.h"
#include "TopologyView.h"
#include "SelectHelper.h"
#include "AnimMng.h"
#include "2d/CCNode.h"
#include "2d/CCTweenFunction.h"
#include "2d/CCSprite.h"

USING_NS_CC;

EidtorFrame::EidtorFrame(void)
{
    //Set m_View as the view window of the frame
    SetView(m_view);

    // Set the registry key name, and load the initial window position
    // Use a registry key name like "CompanyName\\Application"
    LoadRegistrySettings(_T("Win32++\\AnimEdit"));
}


EidtorFrame::~EidtorFrame(void)
{
}

BOOL EidtorFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch(LOWORD(wParam))
    {
    case IDM_FILE_OPEN:		 OnFileOpen();		return TRUE;
    case IDM_FILE_SAVE:		 OnFileSave();		return TRUE;
    case IDM_FILE_SAVEAS:	 OnFileSave();		return TRUE;
    case IDM_FILE_PRINT:	 OnFilePrint();		return TRUE;
    case IDM_FILE_EXIT:		 OnFileExit();		return TRUE;
    case IDW_VIEW_STATUSBAR: OnViewStatusBar();	return TRUE;
    case IDW_VIEW_TOOLBAR:	 OnViewToolBar();	return TRUE;
    case IDM_HELP_ABOUT:	 OnHelp();			return TRUE;
    case IDM_ADD_NODE:        OnNewNode();       return TRUE;
    case IDM_ADD_SPRITE:      OnNewSprite();     return TRUE;
    }

    return FALSE;
}

int EidtorFrame::OnCreate(LPCREATESTRUCT pcs)
{
    // OnCreate controls the way the frame is created.
    // Overriding CFrame::Oncreate is optional.
    // The default for the following variables is TRUE

    // m_UseIndicatorStatus = FALSE;	// Don't show keyboard indicators in the StatusBar
    // m_UseMenuStatus = FALSE;			// Don't show menu descriptions in the StatusBar
    // m_UseReBar = FALSE;				// Don't use a ReBar
    // m_UseThemes = FALSE;				// Don't use themes
    // m_UseToolBar = FALSE;			// Don't use a ToolBar

    // call the base class function
    return CFrame::OnCreate(pcs);
}

void EidtorFrame::OnFileExit()
{
    // Issue a close request to the frame
    PostMessage(WM_CLOSE);
}

void EidtorFrame::OnInitialUpdate()
{
    // The frame is now created.
    // Place any additional startup code here.

    TRACE("Frame created\n");

    if (!LoadDockRegistrySettings(GetRegistryKeyName()))
    {
        CDocker* docker = AddDockedChild(NewDockerFromID(ID_DOCK_TREE_LIST1), DS_DOCKED_LEFT, 200, ID_DOCK_TREE_LIST1);
        docker->SetDockStyle(docker->GetDockStyle() | DS_CLIENTEDGE | DS_NO_CLOSE);
        docker = docker->AddDockedChild(NewDockerFromID(ID_DOCK_TREE_LIST3), DS_DOCKED_BOTTOM, 400, ID_DOCK_TREE_LIST3);
        docker->SetDockStyle(docker->GetDockStyle() | DS_CLIENTEDGE | DS_NO_CLOSE);
        docker = AddDockedChild(NewDockerFromID(ID_DOCK_TREE_LIST2), DS_DOCKED_BOTTOM, 200, ID_DOCK_TREE_LIST2);
        docker->SetDockStyle(docker->GetDockStyle() | DS_CLIENTEDGE | DS_NO_CLOSE);

    }

    m_view.CreateCocosScene();
}

void EidtorFrame::OnFileOpen()
{
    // Bring up the dialog, and open the file
    CFile File;
    CString str = File.OpenFileDialog(0, 0, 0, 0);

    // TODO:
    // Add your own code here. Refer to the tutorial for additional information 
}

void EidtorFrame::OnFileSave()
{
    CFile File;
    CString str = File.SaveFileDialog(0, 0, 0, 0, 0);

    // TODO:
    // Add your own code here. Refer to the tutorial for additional information 
}

void EidtorFrame::OnFilePrint()
{
    // Bring up a dialog to choose the printer
    //PRINTDLG pd;
    //ZeroMemory(&pd, sizeof(PRINTDLG));
    //pd.lStructSize = sizeof( pd );
    //pd.Flags = PD_RETURNDC;
    //pd.hwndOwner = m_hWnd;

    // Retrieve the printer DC
    //PrintDlg( &pd );

    // TODO:
    // Add your own code here. Refer to the tutorial for additional information 
}

LRESULT EidtorFrame::OnNotify(WPARAM wParam, LPARAM lParam)
{
    // Process notification messages sent by child windows
    //	switch(((LPNMHDR)lParam)->code)
    //	{
    //		Add case statements for each notification message here
    //	}

    // Some notifications should return a value when handled
    return CFrame::OnNotify(wParam, lParam);
}

void EidtorFrame::PreCreate(CREATESTRUCT &cs)
{
    // This function is called before the frame is created.
    // It provides an opportunity to modify the various CREATESTRUCT
    // parameters used in the frame window's creation.

    // The WS_EX_LAYOUTRTL style requires Windows 2000 or above in targetver.h
    // cs.dwExStyle = WS_EX_LAYOUTRTL;		// Set Right-To-Left Window Layout

    // Call base clase to set defaults
    CFrame::PreCreate(cs);

    // cs.style &= ~WS_VISIBLE;	// Remove the WS_VISIBLE style. The frame will be initially hidden.
}

void EidtorFrame::SetupToolBar()
{
    // Set the Resource IDs for the toolbar buttons
    AddToolBarButton( IDM_FILE_NEW   );
    AddToolBarButton( IDM_FILE_OPEN  );
    AddToolBarButton( IDM_FILE_SAVE  );

    AddToolBarButton( 0 );						// Separator
    AddToolBarButton( IDM_EDIT_CUT,   FALSE );	// disabled button
    AddToolBarButton( IDM_EDIT_COPY,  FALSE );	// disabled button
    AddToolBarButton( IDM_EDIT_PASTE, FALSE );	// disabled button

    AddToolBarButton( 0 );						// Separator
    AddToolBarButton( IDM_FILE_PRINT );

    AddToolBarButton( 0 );						// Separator
    AddToolBarButton( IDM_HELP_ABOUT );
}

CDocker* EidtorFrame::NewDockerFromID(int nID)
{
    if (ID_DOCK_TREE_LIST1 == nID)
    {
        return new NodeAttributeDocker();
    }
    else if(ID_DOCK_TREE_LIST2 == nID)
    {
       return new AnimDocker();
    }
    else if (ID_DOCK_TREE_LIST3 == nID)
    {
        return new TopologyDocker();
    }
    return NULL;
}

BOOL EidtorFrame::SaveRegistrySettings()
{
    if (CFrame::SaveRegistrySettings())
        return SaveDockRegistrySettings(GetRegistryKeyName());
    else
        return FALSE;
}

LRESULT EidtorFrame::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //	switch (uMsg)
    //	{
    //		Add case statements for each messages to be handled here
    //	}

    // pass unhandled messages on for default processing
    return WndProcDefault(uMsg, wParam, lParam);
}

void EidtorFrame::OnNewNode()
{
   EditNode* eNode = NodeFactory::CreateNode();
    if (gSelectHepler.GetSelectNode())
    {
        gSelectHepler.GetSelectNode()->GetNode()->addChild(eNode->node);
        UpdateTopology();
        gSelectHepler.SetSelectNode(eNode, eNode->GetType());
    }
}

void EidtorFrame::OnNewSprite()
{
    EditSprite* eSpr = NodeFactory::CreateSprite();
    if (gSelectHepler.GetSelectNode())
    {
        gSelectHepler.GetSelectNode()->GetNode()->addChild(eSpr->sprite);
        UpdateTopology();
        gSelectHepler.SetSelectNode(eSpr, eSpr->GetType());
    }
}

void EidtorFrame::OnNodeSelect(EditClass* node, int type)
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST1);
    if (pDocker)
    {
        if (node != m_view.sceneRoot)
        {
            ((NodeAttributeDocker*)pDocker)->OnNodeSelect(node, type);
        }
        else
        {
            ((NodeAttributeDocker*)pDocker)->ResetToStart();
        }
    }
}

const char* GetEditDesc(int type)
{
    switch (type)
    {
    case FrameType::Position:
        return"¶¯»­--±à¼­Î»ÖÃ";
    case FrameType::Opacity:
        return"¶¯»­--±à¼­ÏÔÓ°";
    case FrameType::Scale:
        return"¶¯»­--±à¼­ÏÔÓ°";
    case FrameType::Rotation:
        return"¶¯»­--±à¼­Ðý×ª";
    default:
        break;
    }
    return "";
}

void EidtorFrame::OnEditAnimNode(EditClass* eNode, int type)
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST1);
    if (pDocker)
    {
        ((NodeAttributeDocker*)pDocker)->OnEditAnimNode(eNode, type);
    }
    pDocker = GetDockFromID(ID_DOCK_TREE_LIST2);
    if (pDocker)
    {
        ((AnimDocker*)pDocker)->OnEditAnimNode(eNode, type);
        pDocker->SetCaption(GetEditDesc(type));
    }
}

void EidtorFrame::OnExitAnim()
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST2);
    if (pDocker)
    {
        ((AnimDocker*)pDocker)->OnExitAnim();
    }
    ResetAttributeToStart();
}

void EidtorFrame::OnNodeMoveEnd(EditClass* eNode)
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST1);
    if (pDocker)
    {
        ((NodeAttributeDocker*)pDocker)->OnMoveEnd(eNode);
    }
}

void EidtorFrame::UpdateTopology(EditClass* root)
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST3);
    if (pDocker)
    {
        ((TopologyDocker*)pDocker)->UpdateTopology(root);
    }
}

void EidtorFrame::UpdateTopology()
{
    UpdateTopology(m_view.sceneRoot);
}

void EidtorFrame::RedrawAnim()
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST2);
    if (pDocker)
    {
        ((AnimDocker*)pDocker)->ViewRedraw();
    }
}

void EidtorFrame::ResetAttributeToStart()
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST1);
    if (pDocker)
    {
        ((NodeAttributeDocker*)pDocker)->ResetToStart();
    }
}

void EidtorFrame::PlayAnim()
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST2);
    if (pDocker)
    {
        ((AnimDocker*)pDocker)->PlayAnim();
    }
}

unsigned char EidtorFrame::GetInterpolation()
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST1);
    if (pDocker)
    {
        return ((NodeAttributeDocker*)pDocker)->GetInterpolation();
    }
    return (unsigned char)(tweenfunc::Linear);
}
