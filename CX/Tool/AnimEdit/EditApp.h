#pragma once

#include "EditorFrame.h"

class EditApp : public CWinApp
{
public:
    EditApp(void);
    ~EditApp(void);

    virtual BOOL InitInstance();

    EidtorFrame m_Frame;
};

extern EditApp* GetMyApp();
