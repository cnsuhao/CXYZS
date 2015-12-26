#include "stdafx.h"
#include "DBServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//·şÎñÆ÷ID
	__int16 ServerID = 0;
	if (argc > 1)
	{
		ServerID = _tstoi(argv[1]);
	}

	DBServer *g_DBServer = new DBServer;
	if (!g_DBServer)
	{
		MessageBoxA(NULL,"ÄÚ´æ·ÖÅäÊ§°Ü(new DBServer)", "´íÎó", MB_OK);
		return -1;
	}

	//BaseServerManager::instance()->RegistMapServer(g_login);

	g_DBServer->Start(ServerID);

	return 0;
}
