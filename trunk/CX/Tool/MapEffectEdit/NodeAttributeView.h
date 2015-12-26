#pragma once

#include "docking.h"
#include "EditClass.h"

class TopologyView;
class SpriteAttributeView;
class AnimAttributeView;

class NodeAttributeView : public CDialog
{
public:
    NodeAttributeView(UINT nResID);
    virtual ~NodeAttributeView();

    virtual HWND Create(CWnd* pParent);
    virtual BOOL OnInitDialog();
    virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnOK();

    class CEditName : public CEdit {};
    class CEditCoord : public CEdit {};
    class CEditAnchor : public CEdit {};
    class CEditScale : public CEdit {};

    void OnNodeSelect(EditClass* node);
    void OnMoveEnd(EditClass* node);

    void SetName(const char* name);
    void SetCoordX(float v);
    void SetCoordY(float v);
    void SetAnchorX(float v);
    void SetAnchorY(float v);
    void SetScaleX(float v);
    void SetScaleY(float v);
    void SetRotation(float v);
    void SetOpacity(GLubyte v);
    void SetRenderLevel(int level);

protected:
    CEditName editName;
    CEditCoord editCoord[2];
    CEditAnchor editAnchro[2];
    CEditAnchor editScale[2];
};


class NodeAttributeDocker : public CDocker
{
public:
    NodeAttributeDocker(void);
    ~NodeAttributeDocker(void);


    virtual void OnInitialUpdate();
    void OnNodeSelect(EditClass* node);
    //void OnEditAnimNode(EditClass* node, int type);
    //void OnMoveEnd(EditClass* node);
    //void ResetToStart();
    //unsigned char GetInterpolation();
protected:
    CTab m_view;
    NodeAttributeView* nodeView;
    SpriteAttributeView* spriteView;
    AnimAttributeView* animView;
};