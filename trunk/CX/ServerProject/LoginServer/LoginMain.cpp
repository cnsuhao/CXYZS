// LoginMain.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LoginServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	LoginServer *g_LoginServer = new LoginServer;
	if (!g_LoginServer)
	{
		MessageBoxA(NULL,"内存分配失败(new LoginServer)", "错误", MB_OK);
		return -1;
	}

	//BaseServerManager::instance()->RegistMapServer(g_login);

	g_LoginServer->Start();

	return 0;
}

