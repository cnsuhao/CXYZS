#pragma once

#include "platform/win32/CCApplication-win32.h"

class EditorView : public CWnd, public cc2d::Application
{
public:
    EditorView(void);
    ~EditorView(void);

    virtual void OnInitialUpdate();
    virtual void PreCreate(CREATESTRUCT& cs);
    virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnWindowPosChanged(WPARAM wParam, LPARAM lParam);
            LRESULT OnTime(WPARAM wParam, LPARAM lParam);
            LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
            LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
            LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
            LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
            LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void OnDestroy();

    virtual bool applicationDidFinishLaunching() { return true; }

    virtual void applicationDidEnterBackground()  {}

    virtual void applicationWillEnterForeground() {}

    void UpdateTopology();
public:
    void CreateCocosScene();
    EditRoot* sceneRoot;
protected:
    LARGE_INTEGER nLast;
    LARGE_INTEGER nNow;
    float mx;
    float my;
    int mlb;
};

