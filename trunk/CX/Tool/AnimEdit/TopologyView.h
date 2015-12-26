#pragma once

#include "treeview.h"

class TopologyView : public CDialog
{
public:
    TopologyView(UINT nResID);
    ~TopologyView(void);

    virtual HWND Create(CWnd* pParent);
    virtual BOOL OnInitDialog();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnOK() { }
    void UpdateByNode(EditClass* root) { tree.UpdateByNode(root); }

    class TopologyTree : public CTreeView 
    {
    public:
        TopologyTree();
        virtual ~TopologyTree();
        void UpdateByNode(EditClass* root);
    protected:
        virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void AddNode(HTREEITEM hParent, EditClass* node);
        virtual HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR sxText, int iImage);
    };
    
    TopologyTree tree;
};

class TopologyDocker : public CDocker
{
public:
    TopologyDocker(void);
    ~TopologyDocker(void);

    virtual void OnInitialUpdate();
    void UpdateTopology(EditClass* root);

    TopologyView topologyView;
};