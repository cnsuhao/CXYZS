// LoginMain.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "LoginServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	LoginServer *g_LoginServer = new LoginServer;
	if (!g_LoginServer)
	{
		MessageBoxA(NULL,"�ڴ����ʧ��(new LoginServer)", "����", MB_OK);
		return -1;
	}

	//BaseServerManager::instance()->RegistMapServer(g_login);

	g_LoginServer->Start();

	return 0;
}

