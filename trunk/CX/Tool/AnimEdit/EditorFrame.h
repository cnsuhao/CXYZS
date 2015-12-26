#pragma once

#include "EditorView.h"


enum DockIDs
{
    ID_DOCK_TREE_LIST1 = 1,
    ID_DOCK_TREE_LIST2 = 2,
    ID_DOCK_TREE_LIST3 = 3
};

class EidtorFrame : public CFrame
{
public:
    EidtorFrame(void);
    ~EidtorFrame(void);
    virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
    virtual int		OnCreate(LPCREATESTRUCT pcs);
    virtual void	OnFileExit();
    virtual void	OnFileOpen();
    virtual void	OnFilePrint();
    virtual void	OnFileSave();
    virtual void	OnInitialUpdate();
    virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
    virtual void	PreCreate(CREATESTRUCT &cs);
    virtual void	SetupToolBar();
    virtual CDocker* NewDockerFromID(int nID);
    virtual BOOL    SaveRegistrySettings();
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    void   OnNewNode();
    void   OnNewSprite();

    void   OnNodeSelect(EditClass* node, int type);
    void   OnEditAnimNode(EditClass* node, int type);
    void   OnExitAnim();
    void   OnNodeMoveEnd(EditClass* node);
    
    void   UpdateTopology();
    void   RedrawAnim();
    void   ResetAttributeToStart();
    void   PlayAnim();
    unsigned char GetInterpolation();

    EditorView& GetView() { return m_view; }
private:
    void   UpdateTopology(EditClass* root);

    EditorView m_view;
};

