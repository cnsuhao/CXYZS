/********************************************************************
created:	2014/7/15
author:		LL
desc:		��Ϣ����ṹ���壬��Ϣ�ṹ��ӦMsgDef.h�е�Э��š� �ͻ��˷��������ô��ļ�

ͨѶЭ��˵����
1. �Խṹ����ʽʵ��C/S�����ݽ���
2. Э��ṹ������1�ֽڶ��뷽ʽ
3. ��ṹ������ָ�룬��ָ������ݽ��������ڽṹ����ã�ָ�뱾�½�������
*********************************************************************/

#ifndef _MESSAGE_DEFINE__H_
#define _MESSAGE_DEFINE__H_
#include "MessageIdentifiers.h"
#include "MsgDef.h"
#include "ServerDef.h"
#include <string.h>
#include <assert.h>



#pragma pack(1)

//����Э��ĸ���
struct MsgBase
{
    unsigned char SysID;		//�����ĵ�һ���ֽ�,ϵͳ�����id
	unsigned char MsgID;		//�Զ���Э���
	__int64 OBJID;				//����ת����Ϣʱʹ�ã�Ѱ��ת�����󡣿ͻ��˺͵�½������ͨѶʹ��LinkID������Ϸ������ͨѶʹ��AccountId��
	unsigned __int16 Len;		//��Ϣ���ȣ������б䳤��Ϣ��������Ϣ��ĳ�����LenΪ׼��
////���º������ڱ䳤��Ϣ
//	void AddByte(char in);		//����Byte
//	void AddShort(short in);	//����short
//	void AddInt(int in){;(*this)+=4;}		//����int
//	void AddLong(long long in);	//����64λint
//	void AddStr(char* in, int num);	//�����ַ���
//	void AddBool(bool in);		//����bool	
//	void AddFloat(float in);	//����float
//	void AddDouble(double in);	//����double
//
//	void AddNull(int num);		//����ָ�����ȿ��ֽ�
};

//�޲�������Ϣ֪ͨ
struct MsgNotify :public MsgBase
{
    MsgNotify(SysMsgType sysmsg, NotifyType notifytype){SysID = sysmsg; MsgID = Msg_Notify; Type = notifytype; Len = sizeof(*this);}
	unsigned char Type;		//��ӦNotifyType
};


//ע�ᡢ��½
struct MsgRegiser_Login :public MsgBase
{
	MsgRegiser_Login(char* name, char* password, __int64 touristid, unsigned char msgid){SysID = ID_C2L; MsgID = msgid; Len = sizeof(*this); 
	strcpy(Name, name); strcpy(Password, password); TouristID = touristid;}
	char Name[MAX_REGISER_NAME_SIZE];			//�˺���
	char Password[MAX_REGISER_PASS_SIZE];		//����
	__int64 TouristID;	//�ο�id
};
 
struct ServerListIt
{
	__int16 ServerID;							//������id
	char ServerName[MAX_REGISER_NAME_SIZE];	//��������
	int ServerState;						//���������� 1�Ϸ� 2�·� 3�Ϸ�
	bool isStart;							//�������Ƿ���
};
//�������б�,�䳤������Ҫ�ֶ�����Len
struct MsgServerList :public MsgBase
{
    MsgServerList(){SysID = ID_L2C; MsgID =  Msg_ServerList;}
	int ServerNum;					//����������
	ServerListIt ServerListInfo;	//�䳤����								
};

//ѡ�������
struct MsgSelServer :public MsgBase
{
	MsgSelServer(__int16 serverid){SysID = ID_C2L; MsgID = Msg_SelServer; ServerID = serverid; Len = sizeof(*this);}
	__int16 ServerID;					//������id
};

//������ɫ C2G��G2D
struct MsgCreateRole :public MsgBase
{
	MsgCreateRole(char* name, __int8 sex, __int16 headid){SysID = ID_C2G; MsgID =  Msg_CreateRole; strcpy(Name, name); Sex = sex; HeadID = headid; Len = sizeof(*this);}
	char Name[MAX_REGISER_NAME_SIZE];		//��ɫ��
	__int8 Sex;								//�Ա� 0Ů 1��
	__int16 HeadID;							//ͷ��id
};

struct MsgGameServer_Register :public MsgBase
{
	MsgGameServer_Register(__int16 serverid, std::string &name)
	{
		SysID = ID_GAME2GATE;
		MsgID = Msg_GameServer_Register;
		Len = sizeof(*this);
		ServerID = serverid;
		if (name.size() < MAX_REGISER_NAME_SIZE)
		{
			strcpy_s(ServerName, MAX_REGISER_NAME_SIZE, name.c_str());
		}
	}

	__int16 ServerID;								//������id
	char ServerName[MAX_REGISER_NAME_SIZE];		//��������
};

struct MsgPlayer_Enter_GameServer :public MsgBase
{
	MsgPlayer_Enter_GameServer(__int64 accountid, __int64 linkid){SysID = ID_L2G;	MsgID = Msg_Player_Enter_GameServer; Len = sizeof(*this); AccountID = accountid; LinkID = linkid;}
	__int64 AccountID;								//�˺�id
	__int64 LinkID;									//����id
};

struct MsgPlayer_Leave_GameServer :public MsgBase
{
	MsgPlayer_Leave_GameServer(__int64 accountid){SysID = ID_L2G;	MsgID = Msg_Player_Leave_GameServer; Len = sizeof(*this); AccountID = accountid;}
	__int64 AccountID;								//�˺�id
};

#pragma pack()
#endif
