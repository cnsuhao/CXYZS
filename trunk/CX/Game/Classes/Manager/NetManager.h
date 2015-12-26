/********************************************************************
created:	2015/3/20
author:		LL
desc:		�������������������˵�ͨѶ�����ӡ��Ͽ����ӡ�����Ϣ������Ϣ
*********************************************************************/
#ifndef __NETMANAGER_H__
#define __NETMANAGER_H__

#include "ClientDef.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "CCLayer.h"

using namespace std;

struct MsgBase;
struct Show_GameSrv;
class UIMsgBox;

class NetManager: public cocos2d::Layer
{
public:
	NetManager();
	~NetManager();
	void Init();					//��ʼ��	
	void Release();					//�ͷ�
	void Update(float delta);		//���£�����ѭ����ȡ����������

	bool Connect();					//���ӷ�����
	int SendMsg(MsgBase &msg);		//������Ϣ	
	
	RakNet::RakPeerInterface* m_Net;				//�����ʵ�����������ӷ��������շ�����
	RakNet::SystemAddress 	m_ServerAddress;		//��������ַ

	UIMsgBox* m_CutTip;				//��������
	UIMsgBox* m_ReturnLoginTip;		//�������Ͽ������ص�½��ʾ

    const std::string& GetGateIP() const { return m_GateIP; }
    const std::string& GetGatePort() const { return m_GatePort; }
    const std::string& GetResIP() const { return m_ResIP; }
    const std::string& GetResPort() const { return m_ResPort; }
	//Show_GameSrv *m_showServer;

	char m_SendBuff[1024];	//���ͻ��棬���ڷ��ͱ䳤����
private:
	int  OnRecv(RakNet::Packet* pData);		//�յ���������Ϸ����

    bool LoadConfing();

    std::string m_GateIP;
    std::string m_GatePort;

    std::string m_ResIP;
    std::string m_ResPort;

public:
    struct MsgTeam_Apply* m_LastTeamInfo;
    UIMsgBox* m_LastApplyMsgBox;
//////////////////////////////////////////////////////////////////////////////////////////
//���¿ռ������������ݴ�����������
public:
	void OnNotify(MsgBase* msg);

    void OnPlayerBaseInfo(MsgBase* msg);        //��һ�����Ϣ
    void OnObjectMove(MsgBase* msg);            //��������ƶ���Ϣ
    void OnObjectMoveKick(MsgBase* msg);        //����������ƶ��߻�
    void OnObjectMoveStop(MsgBase* msg);        //���ֹͣ�ƶ�
    void OnOtherPlayerEnterView(MsgBase* msg);  //������ҽ�����Ұ
    void OnOtherPlayerLeaveView(MsgBase* msg);  //��������뿪��Ұ
	void OnObjectSyncAttribute(MsgBase* msg);	//������ͬ����������
    void OnObjectAddHp(MsgBase* msg);           //�����Ѫ
    void OnObjectDropHp(MsgBase* msg);          //�����Ѫ
	void OnPlayerstate(MsgBase* msg);			//������ͬ�����״̬(��)
	void OnPlayerUseSkill(MsgBase* msg);		//��һ����ʹ�ü���
	void OnPlayerReliveData(MsgBase* msg);		//������֪ͨ�ͻ���ԭ�ظ�������Ҫ����Ϣ
	void OnPlayerRelive(MsgBase* msg);			//��Ҹ���
    void OnPlayerChangeEquipment(MsgBase* msg); //װ���ı�
	void OnPlayerDetailAttribute(MsgBase* msg);	//�������������Ϣ���繥�����ȣ�
    void OnPlayerOtherDetailInfo(MsgBase* msg); //������ҵ���ϸ��Ϣ

    void OnAddItem(MsgBase* msg);               //�����Ʒ
    void OnDelItem(MsgBase* msg);               //ɾ����Ʒ
    void OnUpdateItem(MsgBase* msg);            //������Ʒ
    void OnEquipList(MsgBase* msg);             //������Ʒ�嵥
    void OnEquipUpdate(MsgBase* msg);           //���������

    void OnMonsterEnterView(MsgBase* msg);      //�������
    void OnMonsterLeaveView(MsgBase* msg);      //����ɾ��
    void OnMonsterMoving(MsgBase* msg);         //�����ƶ�
	void OnMonsterGoto(MsgBase* msg);			//���ﴫ��

    void OnSkillList(MsgBase* msg);             //�����嵥
    void OnAddSkill(MsgBase* msg);              //��Ӽ���
    void OnUpdateSkill(MsgBase* msg);           //���¼���

    void OnObjBuffList(MsgBase* msg);           //>buff�б�
    void OnObjAddBuff(MsgBase* msg);            //>buff���
    void OnObjDelBuff(MsgBase* msg);            //>buffɾ��
    
	void OnRecvFriendsNotify(MsgBase* msg);     //��ú���֪ͨ

	void OnRecvTaskNotify(MsgBase* msg);		//�������֪ͨ

	void OnRecvGangNotify(MsgBase* msg);		//��ù���֪ͨ

    void OnChatMsg(MsgBase* msg);               //������Ϣ

    void OnTeamApply(MsgBase* msg);             //>�������
    void OnTeamReplyApply(MsgBase* msg);        //>��ӻ�Ӧ
    void OnTeamCreated(MsgBase* msg);           //>��������
    void OnTeamNewMember(MsgBase* msg);         //>�����³�Ա
    void OnTeamMemberExit(MsgBase* msg);        //>��Ա�뿪
    void OnTeamChangeLeader(MsgBase* msg);      //>�ӳ�����
};

extern NetManager* g_NetManager;
#endif  // __NETMANAGER_H__