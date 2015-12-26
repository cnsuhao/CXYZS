#include "stdafx.h"
#include "EditorFrame.h"
#include "EditClass.h"
#include "SelectHelper.h"
#include "NodeAttributeView.h"
#include "RedoAndUndoMng.h"
//#include "SelectHelper.h"
//#include "AnimMng.h"

#include <iostream>
#include <fstream>
#include <sstream>

USING_NS_CC;

using namespace std;

EidtorFrame::EidtorFrame(void)
{
    //Set m_View as the view window of the frame
    SetView(m_view);

    // Set the registry key name, and load the initial window position
    // Use a registry key name like "CompanyName\\Application"
    LoadRegistrySettings(_T("Win32++\\EidtorEdit"));

    _mapLoaded = false;
}


EidtorFrame::~EidtorFrame(void)
{
}

BOOL EidtorFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch(LOWORD(wParam))
    {
    case IDM_FILE_OPEN:		 OnFileMap();		return TRUE;
    case IDM_FILE_SAVE:		 OnFileSave();		return TRUE;
    case IDM_FILE_SAVEAS:	 OnFileSave();		return TRUE;
    case IDM_FILE_PRINT:	 OnFilePrint();		return TRUE;
    case IDM_FILE_EXIT:		 OnFileExit();		return TRUE;
    case IDW_VIEW_STATUSBAR: OnViewStatusBar();	return TRUE;
    case IDW_VIEW_TOOLBAR:	 OnViewToolBar();	return TRUE;
    case IDM_HELP_ABOUT:	 OnHelp();			return TRUE;
    case IDM_ADD_EFFECT:     OnNewEffect();       return TRUE;
    case IDM_ADD_SPRITE:     OnNewSprite();     return TRUE;
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
    m_UseToolBar = FALSE;			// Don't use a ToolBar

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
    }
    m_view.CreateCocosScene();

    char AppPtah[MAX_PATH];
    GetModuleFileName(NULL,AppPtah,MAX_PATH);
    string path(AppPtah);
    
    
    size_t n = path.rfind('\\');
    if (n != string::npos)
    {
        path.resize(n + 1);
    }
    path.append("path.txt");
    ifstream f;
    f.open(path.c_str());
    if(f)
    {
        f >> _rosurcePath;
    }
    if (_rosurcePath.empty())
    {
        MessageBoxA("加载path.txt路径失败.请重新添加.","错误", MB_OK);
    }else
    {
        _rosurcePath = ConvertPathFormatToUnixStyle(_rosurcePath);
        if (_rosurcePath[_rosurcePath.size() - 1] != '/')
        {
            _rosurcePath.append("/");
        }
    }
}

void EidtorFrame::OnFileMap()
{
    EditMap* map = EditMap::create();
    if (map && m_view.sceneRoot.Get())
    {
        MapClear();
        Node* root = m_view.sceneRoot.Get()->GetNode();
        root->addChild(map->GetNode());
        root->addChild(map->mapBound);
        m_view.map = Handler<EditMap>(map);

        int p = map->fileName.rfind('.');
        string effectFile(map->fileName.begin(), map->fileName.begin() + p);
        effectFile.append(".effect");
        Data data = FileUtils::getInstance()->getDataFromFile(effectFile);
        if (data.getSize() > 0)
        {
            int count = 0;
            string content((char*)data.getBytes(), data.getSize());
            istringstream is(content);
            is >> count;
            for (int i = 0; i < count; ++i)
            {
                EditSprite* eSprite = new EditSprite();
                eSprite->Load(is);
                map->GetNode()->addChild(eSprite->GetNode());
            }
        }
    }
}

void EidtorFrame::OnFileSave()
{
    if (m_view.map.Get() == NULL)
        return;
    
    int p = m_view.map.Get()->fileName.rfind('.');
    if (p != string::npos)
    {
        string effectFile(m_view.map.Get()->fileName.begin(), m_view.map.Get()->fileName.begin() + p);
        effectFile.append(".effect");

        const Vector<Node*>& childrens = m_view.map.Get()->GetNode()->getChildren();
        if (!childrens.empty())
        {
            Node* child =NULL;
            int count = 0;
            for (int i = 0; i < childrens.size(); ++i)
            {
                child = childrens.at(i);
                if (child->getUserData())
                {
                    EditClass* eNode = (EditClass*)child->getUserData();
                    if (eNode->GetType() == NodeType::SPRITE)
                    {
                        ++count;
                    }
                }
            }
            
            std::ofstream os;
            os.open(effectFile.c_str(), ios_base::binary);
            if (os)
            {
                os << count << ' ';
                for (int i = 0; i < childrens.size(); ++i)
                {
                    child = childrens.at(i);
                    if (child->getUserData())
                    {
                        EditClass* eNode = (EditClass*)child->getUserData();
                        if (eNode && eNode->GetType() == NodeType::SPRITE)
                        {
                            eNode->Save(os);
                        }
                    }
                }
                os.close();
            }
        }
    }
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


void EidtorFrame::OnNewEffect()
{
    CFile File;
    CString str = File.OpenFileDialog(0, OFN_FILEMUSTEXIST, _T("Open File"), 
            _T("plist files(*.plist)\0*.plist\0\0"), this);
    
    if (!str.IsEmpty())
    {
        str = ConvertPathFormatToUnixStyle(str);
        int pos = str.Find(_rosurcePath.c_str());
        if (pos < 0)
        {
            MessageBoxA("资源路径必须path.txt中配置的子目录路径.","添加失败", MB_OK);
            return;
        }

        EditSprite* effect =  EditSprite::create(str.c_str(), true);
        if (effect && m_view.map.Get() && m_view.sceneRoot.Get())
        {
            Size size = Director::getInstance()->getWinSize() * 0.5f;
            effect->GetNode()->setPosition(-m_view.sceneRoot.Get()->GetNode()->getPosition() + size);
            m_view.map.Get()->GetNode()->addChild(effect->GetNode(), 20);
            gSelectHepler.SetSelectNode(effect);
        }
    }
}

void EidtorFrame::OnNewSprite()
{
    CFile File;
    CString str = File.OpenFileDialog(0, OFN_FILEMUSTEXIST, _T("Open File"), 
        _T("png files(*.png)\0*.png\0jpeg files(*.jpg;*jpeg)\0*.jpg;*jpeg\0\0"), this);
   if (!str.IsEmpty())
   {
       str = ConvertPathFormatToUnixStyle(str);
       int pos = str.Find(_rosurcePath.c_str());
       if (pos < 0)
       {
           MessageBoxA("资源路径必须path.txt中配置的子目录路径.","添加失败", MB_OK);
           return;
       }
       EditSprite* effect =  EditSprite::create(str.c_str(), false);
       if (effect && m_view.map.Get() && m_view.sceneRoot.Get())
       {
           m_view.map.Get()->GetNode()->addChild(effect->GetNode(), 20);
       }
   }
}

void EidtorFrame::OnNodeSelect(EditClass* node)
{
    CDocker* pDocker = GetDockFromID(ID_DOCK_TREE_LIST1);
    if (pDocker)
    {
        if (node != m_view.sceneRoot.Get())
        {
            ((NodeAttributeDocker*)pDocker)->OnNodeSelect(node);
        }
    }
}

void EidtorFrame::OnNodeRemove(EditClass* node)
{
    if (node && m_view.map.Get())
    {
        m_view.map.Get()->GetNode()->removeChild(node->GetNode());
        Handler<EditClass> headler;
        headler.Delete(node);

        gSelectHepler.SetSelectNode(NULL);
        delete node;
    }
}

void EidtorFrame::MapClear()
{
    Node* root = m_view.sceneRoot.Get()->GetNode();
    RedoAndUndoMng::Instance().Clear();
    gSelectHepler.SetSelectNode(NULL);
    root->removeAllChildren();
}
