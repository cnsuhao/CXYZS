#include "stdafx.h"
#include "GateServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	g_GateServer = new GateServer;
	if (!g_GateServer)
	{
		MessageBoxA(NULL,"ÄÚ´æ·ÖÅäÊ§°Ü(new GateServer)", "´íÎó", MB_OK);
		return -1;
	}

	//BaseServerManager::instance()->RegistMapServer(g_login);

	g_GateServer->Start();
	delete g_GateServer;
	g_GateServer = NULL;
	return 0;
}
