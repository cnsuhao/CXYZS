#include "stdafx.h"
#include "EditApp.h"

#pragma comment(lib, "comctl32.lib")


int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Start Win32++
    EditApp MyApp;

    // Run the application
    return MyApp.Run();
}