//////////////////////////////////////////////////////////////////////////
//    File Name:      RequestServerList.h
//    Description:    ����������б�
//    Creator:        zp
//    Version:        2015.04.27
//////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <vector>


//������״̬
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

	bool GetServerList(vecServerList &vecList,int iStart,int iend);			//ȡ�̶�����ķ�����
	bool GetNewSerList(vecServerList &vecList);								//ȡ�¿����ķ�����

	tagServer* GetSerByID(int iserid);										//����IDȡ������

	void AddServerList(tagServer parServer);

	void ClearServerList();

	//��̬��
	static RequestServerList	*Instance();
	static void			UnInstance();
	//��̬����
	static RequestServerList	*_Instance;
private:
	vecServerList m_vecServerList;
};

#define g_requestServerList RequestServerList::Instance()