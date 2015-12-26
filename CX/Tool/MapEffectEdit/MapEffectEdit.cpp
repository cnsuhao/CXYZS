// MapEffectEdit.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MapEffectEdit.h"
#include "EditApp.h"

#pragma comment(lib, "comctl32.lib")

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // Start Win32++
    EditApp MyApp;

    // Run the application
    return MyApp.Run();
}
