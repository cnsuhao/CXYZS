/********************************************************************
created:	2014/7/15
author:		LL
desc:		��½�����������ڵ�½ע�ᡣ�������ط��������˺����ݿ⣬����Ϸ����������
*********************************************************************/

#ifndef _LoginServer_
#define _LoginServer_
#include <map>
#include "ServerDef.h"
#include "RakPeerInterface.h"
#include "TimerManage.h"
#include "windows.h"
#include "CMySql.h"

struct GameServerInfo
{
	uint16 ServerID;						//������id
	std::string ServerName;					//��������
	RakNet::SystemAddress ServerAddress;	//��������ַ
	int ServerState;						//���������� 1�Ϸ� 2�·� 3�Ϸ�
	bool isStart;							//�������Ƿ���
};

struct LoginPlayerInfo
{
	uint64 accountId;				//�˺�id
	uint64 LinkID;					//����id
	char accountName[MAX_NAME];		//����
	char state;						//״̬ 0����, 1��½��2��Ϸ��
	uint16 lastServerId;			//����½������id,������Ϸʱ��Ҫ���´�id
};

class LoginServer
{
public:
	LoginServer(void);
	~LoginServer(void);
	bool Start();	//���ݶ˿�����������������������������Ҫ���ӵ���Ӧ������
	void Run();		//ѭ����ȡ��Ϣ������֮
	bool Stop();	//ֹͣ���з�����

//������Ϣ
	void OnRecvClient(RakNet::Packet* p);				//�յ��ͻ����Զ������ݰ�
	void OnRecvGame(RakNet::Packet* p);					//�յ���Ϸ�������Զ������ݰ�
	bool SendToClient(uint64 &objid, MsgBase *msg);	//�������ݵ��ͻ���
	bool SendToGame(uint16 &serverid, MsgBase *msg);		//�������ݵ���Ϸ������	

	RakNet::RakPeerInterface* m_Net;				//�����ʵ���������շ���Ϣ
	uint16 m_Port;						            //�˿�
	std::string	m_GateServerIP;						//���ط�����IP
	uint16	m_GateServerPort;			            //���ط������˿�
	RakNet::SystemAddress m_GateAddress;			//���ط�������ַ,��ip+�˿����
	
//��Ϸ������
	void AddServerLink(RakNet::SystemAddress &address);		//������Ϸ����������
	void DelServerLink(RakNet::SystemAddress &address);		//ɾ����Ϸ����������	
	GameServerInfo* GetServerInfo(uint16 id);					//��ȡ��Ϸ��������Ϣ������id

	std::map<uint16, GameServerInfo>	m_GameServerList;					//���ӵ�������������Ϸ�������б�

//���
	void AddLoginPlayer(uint64 linkid, uint64 accountid, char* accountname, uint16 lastserverid);			//���ӵ�½���
	void DelLoginPlayer(uint64 accountid);						//ɾ����½���
	LoginPlayerInfo* GetLoginPlayer(uint64 accountid);			//��ȡ��½���
	LoginPlayerInfo* GetLoginPlayerByLinkID(uint64 linkid);	//��ȡ��½���,��������id

	std::map<uint64, LoginPlayerInfo*> m_LoginPlayerListByID;	//�ͻ����б����id����ֵ

//���ݿ���Ϣ	
	std::string	m_AccountDBName;		//�˺����ݿ���
	std::string	m_AccountDBIP;			//�˺����ݿ�IP
	uint16 m_AccountDBPort;	            //�˺����ݿ�˿�
	std::string	m_AccountDBAccount;		//�˺����ݿ��˺�
	std::string	m_AccountDBPassword;	//�˺����ݿ�����
	CMysql m_Mysql;				//���ڲ������ݿ�
//
	bool m_StopFlag;					//�������Ƿ�ֹͣ����
	TimerManage m_TimerManage;			//��ʱ������

	
	
//��Ϣ������
	void Regiser(MsgBase* msg);			//ע��
	void Login(MsgBase* msg);			//��½
	void SelServer(MsgBase* msg);		//ѡ�������


private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);

	char m_SendBuff[1024];	//���ͻ��棬���ڷ��ͱ䳤����
//�ڲ���Ϣ������
	
};

#endif	//_LoginServer_