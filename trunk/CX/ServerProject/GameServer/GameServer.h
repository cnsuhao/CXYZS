/********************************************************************
created:	2014/7/15
author:		LL
desc:		��Ϸ���������������ء���½�����ݿ������
*********************************************************************/

#ifndef _GameServer_
#define _GameServer_
#include <map>
#include "ServerDef.h"
#include "RakPeerInterface.h"
#include "TimerManage.h"

class Player;

class GameServer
{
public:
	GameServer(void);
	~GameServer(void);
	bool Start(uint16 ServerID);		//����id��������������������������������Ҫ���ӵ���Ӧ������
	void Run();							//ѭ����ȡ��Ϣ������֮
	bool Stop();						//ֹͣ���з�����
	bool m_StopFlag;					//�������Ƿ�ֹͣ����
	


//������Ϣ
	void OnRecvLogin(RakNet::Packet* p);		//�յ���½���������ݰ�
	void OnRecvClient(RakNet::Packet* p);		//�յ��ͻ������ݰ�	
	void OnRecvDB(RakNet::Packet* p);			//�յ����ݿ���������ݰ�	
	void OnRecvGate(RakNet::Packet* p);			//�յ����ط����������ݰ�
	bool SendToGate(MsgBase *msg);				//�������ݵ����ط�����
	bool SendToLogin(MsgBase *msg);				//�������ݵ���½������
	bool SendToDB(MsgBase *msg);				//�������ݵ����ݿ������
	bool SendToClient(MsgBase *msg);			//�������ݵ��ͻ���
	

	RakNet::RakPeerInterface* m_Net;			//�����ʵ���������շ���Ϣ	
	uint16 m_ServerID;							//������ID
	uint16	m_Port;					            //�˿�
	std::string	m_ServerName;					//��½��������
	std::string	m_LoginServerIP;				//��½������IP
	uint16 	m_LoginServerPort;		            //��½�������˿�
	RakNet::SystemAddress m_LoginAddress;		//��½��������ַ
	std::string	m_GateServerIP;					//���ط�����IP
	uint16 	m_GateServerPort;		            //���ط������˿�
	RakNet::SystemAddress 	m_GateAddress;		//���ط�������ַ
	std::string	m_DBServerIP;					//���ݿ������IP
	uint16 	m_DBServerPort;			            //���ݿ�������˿�
	RakNet::SystemAddress 	m_DBAddress;		//���ݿ�������ַ
	char m_SendBuff[1024];						//���ͻ��棬���ڷ��ͱ䳤����
	
//���պ���
    void OnGetGlobalGuid(MsgBase* msg);         //���ȫ��guid
	void OnPlayerEnterGameServer(MsgBase* msg);	//��ҽ��������
	void OnPlayerLeaveGameServer(MsgBase* msg);	//����뿪������
	void OnCreateRoleToDB(MsgBase* msg);			//������ɫ
	void OnRoleDetailInfo(MsgBase* msg);		//�������ݿ��ɫ������Ϣ
	void OnGangInfo(MsgBase* msg);				//�������ݿ������Ϣ
	void OnGangApplyInfo(MsgBase* msg);			
    void OnRoleRequestEnterMap(MsgBase* msg);   //��������ͼ
    void OnPlayerMove(MsgBase* msg);            //��ɫ�ƶ�
    void OnPlayerMoveStop(MsgBase* msg);        //��ɫ�ƶ�ֹͣ
	void OnPlayerUseSkill(MsgBase* msg);        //��ɫʹ�ü���
	void OnPlayerRelive(MsgBase* msg);			//��ɫ����
    
    void OnPlayerBagList(MsgBase* msg);         //������Ʒ�嵥
    void OnPlayerEquipList(MsgBase* msg);       //����װ���嵥
    void OnPlayerSkillList(MsgBase* msg);       //�����嵥
    void OnPlayerActivateSkill(MsgBase* msg);   //�ı����޼���
    void OnPlayerUIChoseSkill(MsgBase* msg);    //ѡ��ui��ѡ����
    void OnPlayerShopBuy(MsgBase* msg);         //�̵깺��
    void OnPlayerShopSell(MsgBase* msg);        //�̵����
    void OnPlayerItemCompose(MsgBase* msg);     //��Ʒ�ϳ�
    void OnPlayerEquipmentUpStar(MsgBase* msg); //װ������
    void OnPlayerEquipmentInherit(MsgBase* msg);//װ������
    void OnPlayerItemUse(MsgBase* msg);         //��Ʒʹ��
	void OnPlayerTakeTask(MsgBase* msg);		//��ȡ����
	void OnPlayerDoTask(MsgBase* msg);			//������

    void OnTeamApply(MsgBase* msg);             //>�������
    void OnTeamReplyApply(MsgBase* msg);        //>��Ӧ���
    void OnTeamChangeLeader(MsgBase* msg);      //>�ӳ�ת��
    void OnTeamKickMember(MsgBase* msg);        //>�߳���Ա
    void OnTeamMemberExit(MsgBase* msg);        //>��Ա�뿪

	void OnPlayerChest(MsgBase* msg);			//������
    void OnPlayerDetailInfoQuery(MsgBase* msg);       //>�����Ϣ��ѯ
private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);
};

extern GameServer *g_GameServer;

#endif	//_GameServer_