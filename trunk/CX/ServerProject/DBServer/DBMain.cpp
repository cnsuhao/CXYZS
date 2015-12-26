#include "stdafx.h"
#include "DBServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//������ID
	__int16 ServerID = 0;
	if (argc > 1)
	{
		ServerID = _tstoi(argv[1]);
	}

	DBServer *g_DBServer = new DBServer;
	if (!g_DBServer)
	{
		MessageBoxA(NULL,"�ڴ����ʧ��(new DBServer)", "����", MB_OK);
		return -1;
	}

	//BaseServerManager::instance()->RegistMapServer(g_login);

	g_DBServer->Start(ServerID);

	return 0;
}
