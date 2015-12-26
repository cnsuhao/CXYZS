//////////////////////////////////////////////////////////////////////////
//    File Name:      RequestServerList.h
//    Description:    请求服务器列表
//    Creator:        zp
//    Version:        2015.04.27
//////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <vector>


//服务器状态
enum ServerState
{
	SERSTATE_OLD = 0,
	SERSTATE_NEW,
	SERSTATE_AND,
	SERSTATE_CLOSE,
};

struct tagServer
{
	int			iServerID;		
	char		strServerName[20];
	ServerState serverState;
	int			serverPos;

	tagServer()
	{
		serverState = SERSTATE_CLOSE;
		iServerID = 0;
		memset(strServerName, 0, 20);
		serverPos = 0;
	}
};

typedef std::vector<tagServer> vecServerList;

class RequestServerList
{
public:
	RequestServerList();
	~RequestServerList();

	bool GetServerList(vecServerList &vecList,int iStart,int iend);			//取固定区域的服务器
	bool GetNewSerList(vecServerList &vecList);								//取新开服的服务器

	tagServer* GetSerByID(int iserid);										//根据ID取服务器

	void AddServerList(tagServer parServer);

	void ClearServerList();

	//静态类
	static RequestServerList	*Instance();
	static void			UnInstance();
	//静态对象
	static RequestServerList	*_Instance;
private:
	vecServerList m_vecServerList;
};

#define g_requestServerList RequestServerList::Instance()