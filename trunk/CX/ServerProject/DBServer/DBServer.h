/********************************************************************
created:	2014/7/15
author:		LL
desc:		���ݿ��������������Ϸ���ݿ�,����Ϸ����������
*********************************************************************/
#ifndef _DBServer_
#define _DBServer_
#include <map>
#include "ServerDef.h"
#include "RakPeerInterface.h"
#include "TimerManage.h"
#include "CMySql.h"

class DBServer :public TimerManage
{
public:
	DBServer(void);
	~DBServer(void);
	bool Start(uint16 ServerID);	//����������������������������������������Ҫ���ӵ���Ӧ������
	void Run();		//ѭ����ȡ��Ϣ������֮
	bool Stop();	//ֹͣ���з�����

	bool m_StopFlag;				//�������Ƿ�ֹͣ����	

//������Ϣ
	void OnRecvGame(RakNet::Packet* p);		//�յ���Ϸ���������ݰ�	
	bool SendToGame(MsgBase *msg);			//�������ݵ���Ϸ������	
	bool SendToClient(MsgBase *msg);		//�������ݵ��ͻ���	

	RakNet::RakPeerInterface* m_Net;			//�����ʵ���������շ���Ϣ
	uint16 m_ServerID;							//������ID
	uint16	m_Port;					            //�˿�
	std::string	m_GameServerIP;					//��Ϸ������IP
	uint16 	m_GameServerPort;		            //��Ϸ�������˿�
	RakNet::SystemAddress 	m_GameAddress;		//��Ϸ������ַ
	
//���ݿ���Ϣ	
	std::string	m_GameDBName;		//��Ϸ���ݿ���
	std::string	m_GameDBIP;			//��Ϸ���ݿ�IP
	uint16 m_GameDBPort;	        //��Ϸ���ݿ�˿�
	std::string	m_GameDBAccount;	//��Ϸ���ݿ��˺�
	std::string	m_GameDBPassword;	//��Ϸ���ݿ�����
	CMysql m_Mysql;			//���ڲ������ݿ�

	GuidInfoIt guidInfo;


/////////////////	
	std::map<uint64, SQL_player_info> m_PlayerInfo;	//!������ݿ���Ϣ

	std::map<uint64,SQL_gang_info>		m_GangInfo;
	std::map<uint64,SQL_apply_info>		m_ApplyInfo;
	void test(int i);
	void onUpdatePlayerinfo(int i);
	void getGuidInfo();
	void LoadGangInfo();

//���պ���	
	void OnGetGlobalGuid(MsgBase* msg);         //��ȡȫ��guid
	void OnGetGang(MsgBase* msg);				//��ȡ������Ϣ
    void OnPlayerEnterGameServer(MsgBase* msg);	//��ҽ��������
	void OnPlayerLeaveGameServer(MsgBase* msg);	//����뿪������
	void OnCreateRole(MsgBase* msg);			//������ɫ
    void OnAddItemSet(MsgBase* msg);            //>�����Ʒ
    void OnDelItemSet(MsgBase* msg);            //>ɾ����Ʒ
    void OnUpdateItemSet(MsgBase* msg);         //>������Ʒ
    void OnUpdateWearEquipment(MsgBase* msg);   //>���´���װ��

    void OnAddSkill(MsgBase* msg);              //>��Ӽ���
    void OnUpdateSkill(MsgBase* msg);           //>���¼���
    void OnActivateSkill(MsgBase* msg);         //>�����
    void OnChoseCanUseSkill(MsgBase* msg);      //>ѡ��ʹ�õļ���

	void OnAddFriends(MsgBase* msg);			//��Ӻ���
	void OnDelFriends(MsgBase* msg);			//ɾ������

	void OnCopyFinish(MsgBase* msg);			//������� ��Ҫ���������ĸ���
	void OnCopyApply (MsgBase* msg);			//��������ɹ� �������ݿ�

	void OnCreateGang(MsgBase* msg);			//��������
	void OnDelGang(MsgBase* msg);				//��ɢ����
	void OnApplyGang(MsgBase* msg);				//����������
	void OnDelApplyInfo(MsgBase* msg);			//ɾ�����������Ϣ
	void OnDelApplyInfo(__int64 applyId);		//������Ϣɾ��
	void OnApplyGangAgree(MsgBase* msg);		//ͬ�������˼������
	void OnQuitGang(MsgBase* msg);				//�˳�����
	void OnEditAnnounce(MsgBase* msg);			//�༭���ɹ���
	void OnChangeGangHeader(MsgBase* msg);		//ת�ð���

	void OnTakeTask(MsgBase* msg);
	

	bool OnFullSelectGangInfoMap(StoreQueryResult res);
	bool OnFullSelectApplyInfoMap(StoreQueryResult res);
	void OnMakeTask(MsgBase* msg);
	void OnSubmitTask(MsgBase* msg);
	void OnUpdateBaseInfo(MsgBase* msg);
private:
	bool Send(RakNet::SystemAddress &address, MsgBase *msg);
	void SendPlayerDetaiInfo(SQL_player_info* playerInfo);
    SkillInfoIt* FindPlayerSkill(SQL_player_info* playerInfo, int32 skillType);
};

#endif	//_DBServer_