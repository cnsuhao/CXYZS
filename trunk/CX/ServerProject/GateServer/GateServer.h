/********************************************************************
created:	2014/7/15
author:		LL
desc:		���ط�����������ӿڣ�����ת����Ϣ�����ͻ��ˡ���½����Ϸ����������
*********************************************************************/
#ifndef _GateServer_
#define _GateServer_
#include <map>
#include "LinkManager.h"
#include "ServerDef.h"
#include "TimerManage.h"



class GateServer :public LinkManager
{
public:
	GateServer(void);
	~GateServer(void);
	bool Start();	//���ݶ˿�����������������������������Ҫ���ӵ���Ӧ������
	void Run();		//ѭ����ȡ��Ϣ������֮
	bool Stop();	//ֹͣ���з�����
	
	void OnRecvGame(RakNet::Packet* p);			//�յ���Ϸ���������ݰ�
	bool SendToClientByLinkID(MsgBase *msg);	//�������ݵ��ͻ���
	bool SendToClientByAccountID(MsgBase *msg);	//�������ݵ��ͻ���
	bool SendToGame(uint16 &serverid, MsgBase *msg);		//�������ݵ���Ϸ������
	bool SendToLogin(MsgBase *msg);	//�������ݵ���½������

//������Ϣ
	RakNet::RakPeerInterface*	m_Net;				//�����ʵ���������շ���Ϣ
	uint16	m_Port;						            //�˿�
	std::string	m_LoginServerIP;					//��½������IP
	uint16	m_LoginServerPort;			            //��½�������˿�
	RakNet::SystemAddress m_LoginAddress;			//��½��������ַ
	/////////////////
	bool m_StopFlag;					//�������Ƿ�ֹͣ����	
	//TimerManage m_TimerManage;
private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);



	void OnGameServerRegister(RakNet::Packet* msg);		//��Ϸ������ע��
	void OnPlayerEnterGameServer(RakNet::Packet* msg);	//��ҽ�����Ϸ������
};
extern GateServer* g_GateServer;
#endif	//_GateServer_