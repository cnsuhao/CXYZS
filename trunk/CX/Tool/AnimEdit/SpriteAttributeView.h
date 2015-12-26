#pragma once

class SpriteAttributeView : public CDialog
{
public:
    SpriteAttributeView(UINT nResID);
    virtual ~SpriteAttributeView();

    virtual HWND Create(CWnd* pParent);
    virtual BOOL OnInitDialog();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnOK() { }

    void OnSeletFileBtn();
    void OnNodeSelect(cc2d::Node* node);



    BOOL FileOpen(LPCTSTR szFilename);
    
    class SeleteFileBtn : public CButton {};
    class FilePath : public CEdit {};

    SeleteFileBtn   selBtn;
    FilePath        pathEdit;
};