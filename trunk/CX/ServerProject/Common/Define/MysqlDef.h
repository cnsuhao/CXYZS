/********************************************************************
created:	2014/7/20
author:		LL
desc:		���ڶ������ݿ�ṹ���������͡�
*********************************************************************/
#ifndef _MysqlDef__H_
#define _MysqlDef__H_
#include "ServerDef.h"
//���ݿ�ִ�����
enum eMysqlQueryType
{
	eMysqlNULL = -1,
	eMysqlSelect = 0,
	eMysqlInsert,
	eMysqlUpdate,
	eMysqlDelete,
	eMysqlInsertUpdate,
	eMysqlProcedure,
	eMysqlNoRecord,
};

#include <vector>

enum MYSQL_DATATYPE
{
	MYSQL_INT8,
	MYSQL_INT16,
	MYSQL_INT32,
	MYSQL_INT64,
	MYSQL_UINT8,	
	MYSQL_UINT16,
	MYSQL_UINT32,
	MYSQL_UINT64,
	MYSQL_FLOAT,
	MYSQL_BIN,
	MYSQL_STR,
	MYSQL_ZIP,
	MYSQL_DATETIME,
};


#define MAX_NAME 32


struct mysqlCol
{
	const char * name;		//�ֶ�����
	int mysqlType;			//MYSQL��������
	unsigned int dataSize;	//���ݴ�С	
};

static mysqlCol SQL_account[] =
{	
	{"accountId", MYSQL_UINT64, sizeof(unsigned __int64)},
	//{"accountName", MYSQL_STR, MAX_NAME},
	/*{"pwd", MYSQL_STR, MAX_NAME},
	{"userName", MYSQL_STR, MAX_NAME},
	{"idCard", MYSQL_STR, 20},
	{"mobile", MYSQL_STR, 20},
	{"email", MYSQL_STR, 255},*/
	{"lastServerId", MYSQL_UINT16, sizeof(unsigned __int16)},
	{NULL, 0, 0}
};

static mysqlCol SQL_player[] =
{
	{"playerID", MYSQL_UINT64, sizeof(unsigned __int64)},//���id���˺�id�����
	{"playerName", MYSQL_STR, MAX_NAME},				//����
	{"sex", MYSQL_INT8, sizeof(__int8)},				//�Ա� 0Ů 1��	
	{"headID", MYSQL_INT16, sizeof(__int16)},			//ͷ��ID
	{"offlineTime", MYSQL_INT64, sizeof(__int64)},		//����ʱ�䣬��������ʱ�ж��Ƿ�ˢ��ÿ�մ���
	{"level", MYSQL_INT32, sizeof(__int32)},			//�ȼ�
	{"exp", MYSQL_INT32, sizeof(__int32)},				//����ֵ
	{"money", MYSQL_INT32, sizeof(__int32)},			//��ǰ�����
	{"gold", MYSQL_INT32, sizeof(__int32)},				//��Ԫ����
	{"bindGold", MYSQL_INT32, sizeof(__int32)},			//Ԫ������
	{"rechargGoldSum", MYSQL_INT32, sizeof(__int32)},	//�ܳ�ֵԪ��
	{"arenaScore", MYSQL_INT32, sizeof(__int32)},		//����������
	{"arenaMoney", MYSQL_INT32, sizeof(__int32)},		//������������	
	{"battlefieldScore", MYSQL_INT32, sizeof(__int32)},	//��ǰս������	
	{"battlefieldMoney", MYSQL_INT32, sizeof(__int32)},	//ս��������
	{"mapID", MYSQL_INT32, sizeof(__int32)},			//���ڵ�ͼid
	{"posx", MYSQL_INT32, sizeof(__int32)},				//����λ��x
	{"posy", MYSQL_INT32, sizeof(__int32)},				//����λ��y
	{"hp", MYSQL_INT32, sizeof(__int32)},				//Ѫ
	{"mp", MYSQL_INT32, sizeof(__int32)},				//��
	{"xp", MYSQL_INT32, sizeof(__int32)},				//ŭ��
	{"societyID", MYSQL_INT32, sizeof(__int32)},		//����id
	{"realmLevel", MYSQL_UINT32, sizeof(__int32)},		//����ȼ�	
	{"realmExp", MYSQL_INT32, sizeof(__int32)},			//���羭��
	{"skillInfo", MYSQL_STR, 255},						//������Ϣ	"����ID �����ȼ� ��������"
	{"wingInfo", MYSQL_STR, 255},						//�����Ϣ	"���ȼ� ���飩*10"
	{"magicWeaponInfo", MYSQL_STR, 255},				//������Ϣ	"����ID �����ȼ� �������� �����ɳ�ϵ��"
	{"mountInfo", MYSQL_STR, 255},						//������Ϣ	"����ID	����ʱ��"
	{"buffInfo", MYSQL_STR, 255},						//buff��Ϣ	"buffID	����ʱ��"
	{"copy", MYSQL_STR, 255},							//������Ϣ	"����id ������� ����"(���ḱ���ڹ�����Ϣ��)
	{"daily", MYSQL_STR, 255},							//�ճ���Ϣ	""
	{"task", MYSQL_STR, 255},							//������Ϣ	""
	{"equip", MYSQL_STR, 255},							//����װ�� "guID ��ƷID ǰ׺ID ���� �Ƿ�� ���� ����ʱ��"
	{"bag", MYSQL_STR, 255},							//������Ʒ	"guID ��ƷID ǰ׺ID ���� �Ƿ�� ���� ����ʱ��"
	{"friendsList", MYSQL_STR, 255},					//�����б�	"���ID"
	{"blackList", MYSQL_STR, 255},						//�������б�"���ID"
	//!ǰ4���ֽڱ�ʾ���ȣ��䳤�������ݶ�255���ȣ�������Ҫ�޸�

	{NULL, 0, 0}
};

struct SQL_player_info
{
	unsigned __int64 playerID;	//���id���˺�id�����
	char playerName[MAX_NAME];	//����
	__int8 sex;					//�Ա� 0Ů 1��	
	__int16 headID;				//ͷ��ID
	__int64 offlineTime;		//����ʱ�䣬��������ʱ�ж��Ƿ�ˢ��ÿ�մ���
	__int32 level;				//�ȼ�
	__int32 exp;				//����ֵ
	__int32 money;				//��ǰ�����
	__int32 gold;				//��Ԫ����
	__int32 bindGold;			//Ԫ������
	__int32 rechargGoldSum;		//�ܳ�ֵԪ��
	__int32 arenaScore;			//����������
	__int32 arenaMoney;			//������������	
	__int32 battlefieldScore;	//��ǰս������	
	__int32 battlefieldMoney;	//ս��������
	__int32 mapID;				//���ڵ�ͼid
	__int32 posx;				//����λ��x
	__int32 posy;				//����λ��y
	__int32 hp;					//Ѫ
	__int32 mp;					//��
	__int32 xp;					//ŭ��
	__int32 societyID;			//����id
	__int32 realmLevel;			//����ȼ�	
	__int32 realmExp;			//���羭��	
	__int32 activeSkillType;    //ѡ�м���
	std::vector<SkillInfoIt> skillInfo;	//������Ϣ
	std::map<int, WingInfoIt> wingInfo;			//�����Ϣ	"���ȼ� ���飩*10"
	std::map<int, MagicWeaponInfoIt> realmupInfo;//������Ϣ	"����ID �����ȼ� �������� �����ɳ�ϵ��"
	std::map<int, MountInfoIt> mountInfo;		//������Ϣ	"����ID	ʣ��ʱ��"
	std::map<int, CopyInfoIt> copy;				//������Ϣ	"����id	������� ����"
	std::map<int, DailyInfoIt> daily;			//�ճ���Ϣ	"�ճ�id	��ɽ��� �Ƿ��콱"
	std::map<int, TaskInfoIt> task;				//������Ϣ	"����id	��ɽ��� ����״̬��0δ�� 1�ѽ� 2�����"
	ItemInfoIt equip[EQUIPTYPE_MAX];			//����װ�� "guID ��ƷID ǰ׺ID ���� �Ƿ�� ���� ʣ��ʱ��"
	std::map<uint64, ItemInfoIt> bag;			//������Ʒ	"guID ��ƷID ǰ׺ID ���� �Ƿ�� ���� ʣ��ʱ��"
	std::vector<FriendsInfo> vecFriendsList;	//�����б�	"���ID,���ID"
	std::vector<FriendsInfo> vecBlackList;		//�������б�"���ID,���ID"
	__int32   choseSkill[MAX_CHOSESKILL];		//���ѡ�еļ�����Ϣ

};

struct SQL_gang_info
{
	__int64		id;							//����ID
	char		szGangName[MAX_NAME];		//��������
	__int32		iGangLevel;					//���ɵȼ�
	__int32		iGangShopLevel;				//�����̵�ȼ�
	__int64		iHeaderid;					//����ID
	char		szHeaderName[MAX_NAME];		//��������
	__int32		iGangActive;				//��Ծ��
	char		szGangAnnounce[255];		//���ɹ���
	__int64		iViceHeaderid[2];			//�������б�
	__int64		iStarFlex[5];				//�����б�
	char		szCreateTime[50];			//����ʱ��
	__int32		iCopyid;					//����ID
	char		szCopyStartTime[50];		//������ʼʱ��
	std::vector<GangMember> vecMembersList;	//���ɳ�Ա�б�
	char		szBackupInfo[255];			//��ע

	SQL_gang_info()
	{
		id = 0;
		memset(szGangName,0,32);
		iGangLevel = 0;
		iGangShopLevel = 0;
		iHeaderid = 0;
		memset(szHeaderName,0,32);
		iGangActive = 0;
		memset(szGangAnnounce,0,255);
		iViceHeaderid[0] = 0;
		iViceHeaderid[1] = 0;
		iStarFlex[0] = 0;
		iStarFlex[1] = 0;
		iStarFlex[2] = 0;
		iStarFlex[3] = 0;
		iStarFlex[4] = 0;
		memset(szCreateTime,0,50);
		iCopyid = 0;
		memset(szCopyStartTime,0,50);
		vecMembersList.clear();
		memset(szBackupInfo,0,255);
	}
};

struct SQL_apply_info
{
	__int32		id;							//����ID
	__int64		iApplyid;					//������
	__int32		iApplyType;					//�������
	__int64		iTargetid;					//Ŀ��ID
	char		szApplyTime[50];			//����ʱ��
	char		szBackupInfo[255];			//��ע

	SQL_apply_info()
	{
		id = 0;
		iApplyid = 0;
		iApplyType = 0;
		iTargetid = 0;
		memset(szApplyTime,0,50);
		memset(szBackupInfo,0,255);
	}
};

#endif	//_MysqlDef__H_