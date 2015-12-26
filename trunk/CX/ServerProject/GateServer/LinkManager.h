/********************************************************************
created:	2014/7/15
author:		LL
desc:		���ӹ������ڹ������ط���������������
*********************************************************************/

#ifndef __LINKMANAGER_H__
#define __LINKMANAGER_H__
#include "RakPeerInterface.h"
#include <iostream>
#include <hash_map>
#include <map>
#include "GameDef.h"

enum OPERATION_RESULT
{
	LINK_EXISTS,					//�ظ�������
	ADD_OK,							//������ӳɹ�
	REMOVE_LINK_SUCCESS,			//�Ƴ��ɹ�
	LINK_NO_EXISTS,					//���Ӳ�����
};

struct Link_Info
{	
	unsigned long LinkID;				//����id
	RakNet::SystemAddress Address;		//��ַ
	std::string name;					//����
	__int64 AccountId;					//�û����ʺ�ID	,��������ID
	char State;							//״̬	0δ֪��1��������
	char LinkType;						//��������	0δ֪��1��Ϸ��������2�ͻ���
	uint16 lastServerId;				//����½������
	uint64	loginTime;					//����½ʱ��
};

class LinkManager
{
public:
	LinkManager(void);
	~LinkManager(void);

	OPERATION_RESULT AddLink(RakNet::SystemAddress &address);		//��������
	OPERATION_RESULT DelLink(RakNet::SystemAddress &address);		//ɾ������
	void AddClient(uint16 serverid, uint64 linkid, uint64 accountid);		//���ӿͻ���
	void DelClient(uint64 accountid);		//!ɾ���ͻ���	�������״̬Ϊ�뿪��������ɾ����ʱ������ʱ�䵽��һ����뿪״̬����ɾ����
	void GameServerRegister(uint64 linkid, uint16 serverid, char* name);	//ע����Ϸ������


	void RemoveByAccountID(uint64 accountid);						//�Ƴ��ͻ��ˣ������˺�id
	void onCheckPlayerLogin(void* plinkInfo);
	RakNet::SystemAddress* GetAddressByAccountID(uint64 accountid);//��ȡ�ͻ��˵�ַ�������˺�id

	Link_Info* GetLinkByLinkID(uint64 linkid);					//��ȡ������Ϣ
	Link_Info* GetLinkByAccountId(uint64 accountId);					//��ȡ�ͻ���
	Link_Info* GetLinkByServerId(uint16 serverid);						//��ȡ������

	//bool ClientList::Exists(std::string& accountName);				//�˺����Ƿ�����
	//bool ClientList::Exists(__int64 accountId);						//�˺�id�Ƿ�����

private:	
	std::map<uint64, Link_Info*> m_LinkList;				//�����б�
	std::map<unsigned __int16, Link_Info*> m_ServerList;						//��Ϸ�������б�
	stdext::hash_map<__int16, stdext::hash_map<__int64, Link_Info*>> m_ServerClientList;	//�������ͻ��˶��У���Ӧÿ������������пͻ���
	stdext::hash_map<__int64, stdext::hash_map<__int64, stdext::hash_map<__int64, Link_Info*>>> m_ServerGuildClientList; //����������ͻ��˶���
	std::map<unsigned long, Link_Info*> m_ClientList;				//�ͻ����б�����id����ֵ
	std::map<uint64, Link_Info*> m_ClientListByID;					//�ͻ����б����id����ֵ
	std::map<std::string, Link_Info*> m_ClientListByName;			//�ͻ����б����������ֵ
};

#endif	//__LINKMANAGER_H__


