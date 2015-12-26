#pragma once


class AnimAttributeView : public CDialog
{
public:
    AnimAttributeView(UINT nResID);
    virtual ~AnimAttributeView(void);

    virtual HWND Create(CWnd* pParent);
    virtual BOOL OnInitDialog();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnOK();

    void OnNodeSelect(EditClass* node, int type);
    void OnMoveEnd(EditClass* node);
    unsigned char GetInterpolationIndex();
protected:
    class Text : public CStatic {};
    class EditText : public CEdit {};
    class ExitEdit : public CButton {};

    Text m_text1;
    Text m_text2;
    Text m_text3;
    EditText m_editText1;
    EditText m_editText2;
    EditText m_editText3;
    ExitEdit m_exit;
    CListBox m_interpolations;
};

