#pragma once

#include "EditorView.h"

class EditClass;
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
    virtual void	OnFileMap();
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
    void   OnNewEffect();
    void   OnNewSprite();
    
    void   OnNodeSelect(EditClass* node); 
    void   OnNodeRemove(EditClass* node);

    EditorView& GetView() { return m_view; }
    const std::string& GetResourcePath() { return _rosurcePath; }
    void  MapClear();
private:
    bool   _mapLoaded;
    EditorView m_view;
    std::string _rosurcePath;
};

