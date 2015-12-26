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
#include "GameDef.h"

#define GATE_CONNECTION_PASSWORD	 "~Game@9494~"	//���ط�������������

#pragma pack(1)

//����Э��ĸ���
struct MsgBase
{
    uint8           SysID;		//�����ĵ�һ���ֽ�,ϵͳ�����id
	uint16          MsgID;		//�Զ���Э���
	uint64          OBJID;		//����ת����Ϣʱʹ�ã�Ѱ��ת�����󡣿ͻ��˺͵�½������ͨѶʹ��LinkID������Ϸ������ͨѶʹ��AccountId��
	uint16          Len;		//��Ϣ���ȣ������б䳤��Ϣ��������Ϣ��ĳ�����LenΪ׼��
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
	MsgRegiser_Login(char* name, char* password, uint64 touristid, MsgType msgid){SysID = ID_C2L; MsgID = msgid; Len = sizeof(*this); 
	strncpy(Name, name, MAX_NAME_SIZE); strncpy(Password, password, MAX_PASS_SIZE); TouristID = touristid;}
	char Name[MAX_NAME_SIZE];			//�˺���
	char Password[MAX_PASS_SIZE];		//����
	uint64 TouristID;	                //�ο�id
};
 
struct ServerListIt
{
	uint16 ServerID;						//������id
	char ServerName[MAX_NAME_SIZE];			//��������
	int32 ServerState;						//���������� 1�Ϸ� 2�·� 3�Ϸ�
	bool isStart;							//�������Ƿ���
};
//�������б�,�䳤������Ҫ�ֶ�����Len
struct MsgServerList :public MsgBase
{
    MsgServerList(){SysID = ID_L2C; MsgID =  Msg_ServerList;}
	int32 ServerNum;					//����������
	ServerListIt ServerListInfo;	//�䳤����								
};

//ѡ�������
struct MsgSelServer :public MsgBase
{
	MsgSelServer(int16 serverid){SysID = ID_C2L; MsgID = Msg_SelServer; ServerID = serverid; Len = sizeof(*this);}
	uint16 ServerID;					//������id
};

//������ɫ C2G��G2D
struct MsgCreateRole :public MsgBase
{
	MsgCreateRole(const char* name, int8 sex, int16 headid){SysID = ID_C2G; MsgID =  Msg_CreateRole; strncpy(Name, name, MAX_NAME_SIZE); Sex = sex; HeadID = headid; Len = sizeof(*this);}
	char Name[MAX_NAME_SIZE];		//��ɫ��
	int8 Sex;								//�Ա� 0Ů 1��
    int16 HeadID;							//ͷ��id
};
//��Ϸ�����������غ͵�½ȥע��
struct MsgGameServer_Register :public MsgBase
{
	MsgGameServer_Register(uint16 serverid, const char* name)
	{
		SysID = ID_GAME2GATE;
		MsgID = Msg_GameServer_Register;
		Len = sizeof(*this);
		ServerID = serverid;
		strncpy(ServerName, name, MAX_NAME_SIZE);
	}

	int16 ServerID;								//������id
	char ServerName[MAX_NAME_SIZE];				//��������
};

//��ȡȫ��guid
struct MsgGetGlobalGuid : public MsgBase
{
    MsgGetGlobalGuid()
    {
        SysID = ID_G2D;
        MsgID = Msg_GetGlobalGuid;
		Len = sizeof(*this);
    }
    uint64 ItemGuid;
};

//��½������֪ͨ��Ϸ��������ҽ���
struct MsgPlayer_Enter_GameServer :public MsgBase
{
	MsgPlayer_Enter_GameServer(uint64 accountid, uint64 linkid){SysID = ID_L2G;	MsgID = Msg_Player_Enter_GameServer; Len = sizeof(*this); AccountID = accountid; LinkID = linkid;}
	uint64 AccountID;								//�˺�id
	uint64 LinkID;									//����id
};

struct MsgPlayer_Leave_GameServer :public MsgBase
{
	MsgPlayer_Leave_GameServer(uint64 accountid){SysID = ID_L2G;	MsgID = Msg_Player_Leave_GameServer; Len = sizeof(*this); AccountID = accountid;}
	uint64 AccountID;								//�˺�id
};

struct MsgRole_BaseInfo :public MsgBase
{
	MsgRole_BaseInfo(uint64 accountid){SysID = ID_D2G;	MsgID = Msg_Role_BaseInfo; Len = sizeof(*this); OBJID = accountid;}
	BaseInfoIt BaseInfo;				//������Ϣ
};

struct MsgRole_DetailAttribute : public MsgBase
{
	MsgRole_DetailAttribute(uint64 accountid){SysID = ID_D2G;	MsgID = Msg_Role_DetailAttribute; Len = sizeof(*this); OBJID = accountid;}
	DetailAttribute Attribute;
};

struct MsgUpdate_BaseInfo : public MsgBase
{
	MsgUpdate_BaseInfo(uint64 accountid){{SysID = ID_G2D; MsgID = Msg_Role_BaseInfo_Update; Len = sizeof(*this); OBJID = accountid;}}
	int32 attributeType;			//��Ҹı����������
	float attributeValue;				//��Ҹı��������͵�ֵ
};
struct MsgRole_EnterMap : public MsgBase
{
    MsgRole_EnterMap() { SysID = ID_C2G; MsgID = Msg_Role_EnterMap; Len = sizeof(*this);  }

    uint32 mapId;       //>�ͻ�����������mapId.(�������жϽ�ɫ�Ѵ���ĳ�ŵ�ͼ����mapIdΪҪ�л��ĵ�ͼ,�������ʹ���Լ�Ĭ�ϵĵ�ͼ)
    uint32 conveyId;    //>���͵�Id
};

//>DB����Game��ɫ������Ϣ��
struct MsgRole_Detail_Info : public MsgBase
{
    MsgRole_Detail_Info() {}

    int32 SkillTreeNum;		//��������
	int32 WingNum;			//�������
    int32 MagicWeaponNum;	//��������
    int32 MountNum;			//��������
	int32 CopyNum;			//��������
	int32 DailyNum;			//�ճ�����
	int32 TaskNum;			//��������
	int32 EquipNum;			//����װ������
	int32 BagNum;			//������Ʒ����
	int32 FriendsListNum;	//�����б�����
	int32 BlackListNum;		//�������б�����
	int32 ChoseSkill;		//����Ѿ�ѡ��ļ�������
};


//>��������ڵ�ͼ�ϻ�����Ϣ
struct MsgOtherRole_BaseInfo : public MsgBase
{
    MsgOtherRole_BaseInfo(uint64 accountid){SysID = ID_G2C;	MsgID = Msg_OtherRole_BaseInfo; Len = sizeof(*this); OBJID = accountid;}
   
    uint64  guid;		                //Ψһid	
    char    playerName[MAX_NAME_SIZE];	//����
    int32   sex;                        //�Ա�
    int32   equip[EQUIPTYPE_MAX];       //����
    float   posX;                       //λ��x
    float   posY;                       //λ��y
    int32   hp;					        //Ѫ
    int32   maxHp;                      //���Ѫ��
    int32   level;                      //�ȼ�
    int32   headIconIndex;              //ͷ������
};

struct MsgOtherRole_Leave : public MsgBase
{
    MsgOtherRole_Leave(uint64 accountid) {SysID = ID_G2C;	MsgID = Msg_OtherRole_Leave; Len = sizeof(*this); OBJID = accountid; }
    uint64  LeaverID;           //�뿪��
};


//>������ɫ��ϸ��Ϣ
struct MsgOtherRole_DetailInfo : public MsgBase
{
    MsgOtherRole_DetailInfo() 
    {
        SysID = ID_G2C;
        MsgID = Msg_OtherRole_DetailInfo;
        Len = sizeof(*this);
    }

    int64   guid;                       //>��ɫΨһid
    char    name[MAX_NAME_SIZE];        //>��ɫ��
    int32   level;                      //>��ɫ�ȼ�
    int     sex;                        //>�Ա�

    int     battlePower;                //>ս����
    char    gangName[MAX_NAME_SIZE];    //>������
    int     hp;                         //>Ѫ��
    int     maxHp;                      //>���Ѫ��
    int     mp;                         //>��
    int     maxMp;                      //>�����
    int     attack;                     //>������
    int     defence;                    //>������
    int     hit;                        //>����ֵ
    int     dodge;                      //>����ֵ
    int     crit;                       //>����ֵ
    int     tenacity;                   //>����ֵ
    float   hitProb;                    //>������
    float   critProb;                   //>������

    ItemInfoIt equipments[EQUIPTYPE_MAX];   //>װ����Ϣ
};

//>������ɫ��ϸ��Ϣ��ѯ
struct MsgOtherRole_DetailInfoQuery : public MsgBase
{
    MsgOtherRole_DetailInfoQuery() 
    {
        SysID = ID_C2G;
        MsgID = Msg_OtherRole_DetailInfoQuery;
        Len = sizeof(*this);
    }
    
    int64   queryGuid;          //>��ɫΨһid
};

//����ƶ�
struct MsgObject_Move :public MsgBase
{
	MsgObject_Move(){ SysID = ID_D2G;	MsgID = Msg_Object_Move; Len = sizeof(*this);}
	uint64 MoverID;         //�ƶ���
    int    MoverType;       //�ƶ�������
    float MoverPosX;        //�ƶ��ߵĵ�ǰλ��
    float MoverPosY;        //�ƶ��ߵĵ�ǰλ��
    float TargetPosX;       //Ŀ�ĵ�λ��
    float TargetPosY;       //Ŀ�ĵ�λ��
    uint16 PathNum;			//·������
    //int MovePath;		    //·���б��䳤�ֶ�
};

struct MsgObject_Move_Stop : public MsgBase
{
    MsgObject_Move_Stop(){SysID = ID_D2G;	MsgID = Msg_Object_Move_Stop; Len = sizeof(*this);}
    uint64 MoverID;         //�ƶ���
    int    MoverType;       //�ƶ�������
    float MoverPosX;        //�ƶ��ߵĵ�ǰλ��
    float MoverPosY;        //�ƶ��ߵĵ�ǰλ��
};

struct MsgObject_Move_Kick : public MsgBase
{
    MsgObject_Move_Kick(){SysID = ID_G2C;	MsgID = Msg_Object_Move_kick; Len = sizeof(*this);}
    uint64 MoverID;         //�ƶ���
    int    MoverType;       //�ƶ�������
    float MoverPosX;        //�ƶ��ߵĵ�ǰλ��
    float MoverPosY;        //�ƶ��ߵĵ�ǰλ��
};


//>���������
struct MsgObject_SyncAttribute :public MsgBase
{
	MsgObject_SyncAttribute(){SysID = ID_G2C; MsgID = Msg_Object_SyncAttribute; Len = sizeof(*this);}
	uint64 OwnerID;             //>�����ID
    ObjectType OwnerType;       //>���������
    AttributeType Property;		//>�������������
	float Value;				//>���Ե�ֵ
};
struct MsgPlayer_Property_Level : public MsgBase
{
	MsgPlayer_Property_Level(){SysID = ID_G2C; MsgID = Msg_Player_UpdateLevel; Len = sizeof(*this);}
	int level;
	int hpMax;
	int mpMax;
	int hp;
	int mp;
	int ability;
};

//>�����Ѫ
struct MsgObject_AddHp : public MsgBase
{
    MsgObject_AddHp() { SysID = ID_G2C; MsgID = Msg_Object_AddHp; Len = sizeof(*this); }

    uint64 OwnerID;             //>�����ID
    ObjectType OwnerType;       //>���������
    int32  LastHp;              //>ʣ��Hp
    AddHpMode Mode;             //>��Ѫģʽ
};

//>�����Ѫ
struct MsgObject_DropHp : public MsgBase
{
    MsgObject_DropHp() { SysID = ID_G2C; MsgID = Msg_Object_DropHp; Len = sizeof(*this); }

    uint64 OwnerID;             //>�����ID
    ObjectType OwnerType;       //>���������
    int32  LastHp;              //>ʣ��Hp
    DropHpMode Mode;            //>��Ѫģʽ
};

//����buff����
struct MsgObject_BuffList : public MsgBase
{
    MsgObject_BuffList() { SysID = ID_G2C; MsgID = Msg_Object_BuffList; Len = sizeof(*this); }

    uint64 OwnerID;             //>�����ID
    ObjectType OwnerType;       //>���������
    int32  buffNumber;          //>buff����

    //int   buffId;				//>id
    //int   buffInstanceId      //>buffʵ��id
    //float lastTime;			//>ʣ��ʱ��s 0��ʱ�Ƴ�buff
};

//>�������buffЧ��
struct MsgObject_AddBuff :public MsgBase
{
    MsgObject_AddBuff(){ SysID = ID_G2C; MsgID = Msg_Object_AddBuff; Len = sizeof(*this); }
    int64 OwnerID;			    //>�����ID
    ObjectType OwnerType;		//>���������
    int   buffId;				//>id
    int   buffInstanceId;       //>buffʵ��id
    float lastTime;				//>ʣ��ʱ��s 0��ʱ�Ƴ�buff
};

//>��ҽ���buffЧ��
struct MsgObject_DelBuff :public MsgBase
{
    MsgObject_DelBuff(){ SysID = ID_G2C; MsgID = Msg_Object_DelBuff; Len = sizeof(*this); }

    int64 OwnerID;			    //>�����ID
    ObjectType OwnerType;		//>���������
    int   buffInstanceId;		//>buffʵ��id
};


//������ͬ�����״̬(��)
struct MsgPlayer_state :public MsgBase
{
	MsgPlayer_state(int64 guid, bool *state){SysID = ID_G2C; MsgID = Msg_Player_state; Len = sizeof(*this); OBJID = guid; for (int i = 0; i < State_Max; i++){State[i] = state[i];}}
	bool State[State_Max];
};

//�ͻ�������ʹ�ü���
struct MsgC2GPlayer_UseSkill :public MsgBase
{
	MsgC2GPlayer_UseSkill(){SysID = ID_C2G; MsgID = Msg_Player_UseSkill; Len = sizeof(*this);}
	int SkillID;					//����id����Ӧ���ܱ�ļ���ID
	ObjectType ObjType;				//��Ŀ�������
	uint64 target;					//��Ŀ���Ψһid
};

struct HurtData
{
	ObjectType ObjType;			//Ŀ�������
	uint64 guid;				//Ŀ���Ψһid
	AttackType hurtType;		//�˺�����
	int hp;						//Ѫ���仯ֵ
};

//������֪ͨ��һ����ʹ�ü���
struct MsgG2CPlayer_UseSkill :public MsgBase
{
	MsgG2CPlayer_UseSkill(){SysID = ID_G2C; MsgID = Msg_Player_UseSkill; Len = sizeof(*this);}
	int SkillID;				//����id����Ӧ���ܱ�ļ���ID
    uint64 attacker;            //�����ߵ�Ψһid
	ObjectType attackerType;	//�����ߵ�����
    uint64 target;              //Ŀ���Ψһid
    ObjectType targetType;	    //Ŀ�������
};

//������֪ͨ��һ��߹����buff�˺�
struct MsgG2CPlayer_BuffHurt : public MsgBase
{
	MsgG2CPlayer_BuffHurt(){SysID = ID_G2C; MsgID = Msg_Player_BuffHurt; Len = sizeof(*this);}
	int SkillID;		//����ID
	int BuffId;			//��ɵ�Ѫ��BUFFID
	uint64  AttactGuid;	//�������Ķ���ΨһID
	uint64  TargetGuid;		//���˺������ΨһID
	float hp;				//��ѪѪ��
};


struct  MsgG2CPlayer_AttackType : public MsgBase
{
	MsgG2CPlayer_AttackType(){SysID = ID_G2C; MsgID = Msg_Player_AttackType; Len = sizeof(*this);}
	AttackType attackType;
};

//������֪ͨ�ͻ���ԭ�ظ�������Ҫ����Ϣ
struct MsgPlayer_ReliveData :public MsgBase
{
	MsgPlayer_ReliveData(){SysID = ID_G2C; MsgID = Msg_Player_ReliveData; Len = sizeof(*this);}
	int Countdown;			//�����ʱ���룩��������ʱ�������ûѡ�񸴻�������Ĭ�ϻسǸ��
	int StayRelive;			//��ǰԭ�ظ������
	int RMB;				//����ԭ�ظ������踶�ѣ�rmb��
};

//�ͻ���ѡ�񸴻�����
struct MsgC2GPlayer_Relive :public MsgBase
{
	MsgC2GPlayer_Relive(){SysID = ID_C2G; MsgID = Msg_Player_Relive; Len = sizeof(*this);}
	int8 relive_type;			//�������� 0�سǸ��1ԭ�ظ���
};

//������֪ͨ��Ҹ���
struct MsgG2CPlayer_Relive :public MsgBase
{
	MsgG2CPlayer_Relive(){SysID = ID_G2C; MsgID = Msg_Player_Relive; Len = sizeof(*this);}
	int mapid;		//�����ͼid
	int	posid;		//����λ��
};

//�㲥������Ϣ
struct MsgMonster_Baseinfo :public MsgBase
{
	MsgMonster_Baseinfo(){SysID = ID_G2C; MsgID = Msg_Monster_Baseinfo; Len = sizeof(*this);}
	uint64 MonsterGuid;					//����Ψһid;
	int MonsterId;						//�����id;
	int	hp;								//��ǰѪ��
	int	hp_max;							//Ѫ������;
	bool CanAttack;						//�Ƿ�������ҹ���;	
    float   posX;                       //λ��x
    float   posY;                       //λ��y
};

//�㲥�����ƶ�
struct MsgMonster_Move :public MsgBase
{
	MsgMonster_Move(){SysID = ID_G2C; MsgID = Msg_Monster_Move; Len = sizeof(*this);}
	uint64 MonsterGuid;					//����Ψһid;
	uint16 PathNum;		//·������
	int MovePath;		//·���б��䳤�ֶ�
};

//�㲥�Ƴ�����
struct MsgMonster_Del :public MsgBase
{
	MsgMonster_Del(){SysID = ID_G2C; MsgID = Msg_Monster_Del; Len = sizeof(*this);}
	uint64 MonsterGuid;					//����Ψһid;
};

//�㲥���͹���
struct MsgMonster_Goto :public MsgBase
{
	MsgMonster_Goto(){SysID = ID_G2C; MsgID = Msg_Monster_Goto; Len = sizeof(*this);}
	uint64 MonsterGuid;					//����Ψһid;
    float   posX;                       //λ��x
    float   posY;                       //λ��y
};

//���󱳰��嵥
struct MsgItem_BagList : public MsgBase
{
    MsgItem_BagList() { SysID = ID_C2G; MsgID = Msg_Item_BagList; Len = sizeof(*this); }
};

//��Ʒ���
struct MsgItem_Add_Set : public MsgBase
{
    MsgItem_Add_Set() { SysID = ID_G2D; MsgID = Msg_Item_Add_Set; Len = sizeof(*this); }

    uint32 operateType; //��������
    uint8  addNumItem;  //�����Ʒ������

    //{   //��������Ϊ�䳤���ݵ���������
    //    uint64 guid;		//Ψһid (��DB����������)
    //    int itemID;		//��ƷID��ģ�壩
    //    int prefixID;		//ǰ׺ID
    //    int starNum;		//����
    //    bool isBind;		//�Ƿ��
    //    int num;			//����
    //    int time;			//����ʱ��(��DB����������)
    //}
};

//��Ʒɾ��
struct MsgItem_Del_Set : public MsgBase
{
    MsgItem_Del_Set() { SysID = ID_G2D; MsgID = Msg_Item_Del_Set; Len = sizeof(*this); }

    uint32 operateType; //��������
    uint8  delNumItem;  //ɾ����Ʒ������

    //{   //��������Ϊ�䳤���ݵ���������
    //    uint64 guid;		//Ψһid (��DB����������)
    //}
};

//��Ʒ����
struct MsgItem_Update_Set : public MsgBase
{
    MsgItem_Update_Set() { SysID = ID_G2D; MsgID = Msg_Item_Updata_Set; Len = sizeof(*this); }
    
    uint32 operateType; //��������
    uint8  updateNumItem;  //�����Ʒ������

    //{   //��������Ϊ�䳤���ݵ���������
    //    uint64 guid;		//Ψһid (��DB����������)
    //    int itemID;		//��ƷID��ģ�壩
    //    int prefixID;		//ǰ׺ID
    //    int starNum;		//����
    //    bool isBind;		//�Ƿ��
    //    int num;			//����
    //    int time;			//����ʱ��(��DB����������)
    //}
};

//�̵깺����Ʒ
struct MsgItem_ShopBuy : public MsgBase
{
    MsgItem_ShopBuy() { SysID = ID_C2G, MsgID = Msg_Item_ShopBuy; Len = sizeof(*this); }

    int32 shopId;
    int32 goodesId;
};

//�̵������Ʒ
struct MsgItem_ShopSell : public MsgBase
{

    MsgItem_ShopSell() { SysID = ID_C2G, MsgID = Msg_Item_ShopSell; Len = sizeof(*this); }

    uint64 itemGuid;
    int32 number;
};

//��Ʒ�ϳ�
struct MsgItem_Compose : public MsgBase
{
    MsgItem_Compose() { SysID = ID_C2G, MsgID = Msg_Item_Compose; Len = sizeof(*this); }

    uint64 mainStuffGuid;   //>����Ʒ
    uint64 Stuff1Guid;      //>������Ʒ1
    uint64 Stuff2Guid;      //>������Ʒ2
    uint64 Stuff3Guid;      //>������Ʒ3
    uint64 Stuff4Guid;      //>������Ʒ4
};

//װ������
struct MsgItem_UpStar : public MsgBase
{
    MsgItem_UpStar() { SysID = ID_C2G, MsgID = Msg_Item_UpStar; Len = sizeof(*this); }

    uint64 equipmentGuid;   //>���ǵ�װ��
};

//װ������
struct MsgItem_Inherit : public MsgBase
{
    MsgItem_Inherit() { SysID = ID_C2G, MsgID = Msg_Item_Inherit; Len = sizeof(*this); }

    uint64 sourceEquipmentGuid; //>����װ��
    uint64 targetEquipmentGuid; //>�̳�װ��
};

//��Ʒʹ��
struct MsgItem_Use : public MsgBase
{
    MsgItem_Use() { SysID = ID_C2G, MsgID = Msg_Item_Use; Len = sizeof(*this); }

    uint32 operateType; //>��������
    uint64 useItemGuid; //>ʹ����ƷΨһid
};

struct MsgEquip_List : public MsgBase
{
    MsgEquip_List() { SysID = ID_G2C, MsgID = Msg_Equip_List; Len = sizeof(*this); }

    uint32 operateType; //>��������
    ItemInfoIt equipments[EQUIPTYPE_MAX];
};

//����װ������
struct MsgEquip_Update : public MsgBase
{
    MsgEquip_Update() { SysID = ID_G2D, MsgID = Msg_Equip_Update; Len = sizeof(*this); }

    uint32 operateType;    //>��������
    uint32 equipmentType;  //>װ��������(����װ����ʱ�򲢲��ṩ��Ʒ�ṹ)

    ItemInfoIt equipment;
};

//װ����ʾ����
struct MsgEquip_ShowUpdate : public MsgBase
{
    MsgEquip_ShowUpdate() { SysID = ID_G2C, MsgID = Msg_Equip_ShowUpdate; Len = sizeof(*this); }

    uint64 whoGuid;
    uint32 equip[EQUIPTYPE_MAX];
};

//�������嵥
struct MsgSkill_List : public MsgBase
{
    MsgSkill_List() { SysID = ID_G2C, MsgID = Msg_Skill_List; Len = sizeof(*this); }

    uint32 numberSkillTree;

    //{ //�䳤���ݵ���������
    //   int skillType;      //>��������
    //   int level;          //>���ܵȼ�
    //}.set
};

//�������
struct MsgSkill_Add : public MsgBase
{
    MsgSkill_Add() { SysID = ID_G2C, MsgID = Msg_Skill_Add; Len = sizeof(*this); }

    SkillInfoIt addSkill;
};

//���ܸ���
struct MsgSkill_Update : public MsgBase
{
    MsgSkill_Update() { SysID = ID_G2C, MsgID = Msg_SKill_Update; Len = sizeof(*this); }

    SkillInfoIt   updateSkill;
};

struct MsgSkill_Activate : public MsgBase
{
    MsgSkill_Activate() { SysID = ID_C2G, MsgID = Msg_Skill_Activate; Len = sizeof(*this); }

    int32 ActivateSkillType;    //>���ı��������ܵ�����
};

struct MsgSkill_ChoseSkill : public MsgBase
{
    MsgSkill_ChoseSkill() { SysID = ID_C2G, MsgID = Msg_Skill_ChoseSkill, Len = sizeof(*this); }

    int32 setIndex;     //>���õ�λ������
    int32 skillId;      //>����id
};

struct MsgTask_TakeTask : public MsgBase
{
	MsgTask_TakeTask(){SysID = ID_C2G,MsgID = Msg_Task_Take;Len = sizeof(*this);}
	int32 taskId;
	int32 iReqTaskID;

	int32 itemId;
};
//������ȸ���
struct MsgTask_Progress : public MsgBase
{
	MsgTask_Progress(){SysID = ID_G2D,MsgID = Msg_Task_Make;Len = sizeof(*this);}
	TaskInfoIt taskInfo;
};

struct MsgTask_MakeTask : public MsgBase
{
	MsgTask_MakeTask(){SysID = ID_C2G,MsgID = Msg_Task_Make;Len = sizeof(*this);}
	int taskId;
};


struct MsgTask_SubmitTask : public MsgBase
{
	MsgTask_SubmitTask(){SysID = ID_C2G,MsgID = Msg_Task_Submit;Len = sizeof(*this);money = 0;}
	int32 taskId;
	int32 money;
	
};

struct  MsgCopy_Apply : public MsgBase
{
	MsgCopy_Apply(){SysID = ID_C2G,MsgID = Msg_Apply_Copy;Len = sizeof(*this);}
	int32 copyId;
};
struct  MsgCopy_Finish : public MsgBase
{
	MsgCopy_Finish(){SysID = ID_G2D,MsgID = Msg_Copy_Finish;Len = sizeof(*this);}
	CopyInfoIt copyInfoIt;
};
struct MsgTasks : public MsgBase
{
	MsgTasks(int32 ipar1,int32 ipar2,int8 itype)
	{
		SysID = ID_C2G;
		MsgID = Msg_Task_GetList;
		iPar[0] = ipar1;
		iPar[1] = ipar2;
		iType = itype;
		Len = sizeof(*this);
	}
	int32	iPar[2];
	int8	iType;
};

//��Ӻ��ѵ���Ϣ֪ͨ
struct MsgTaskNotify :public MsgBase
{
	MsgTaskNotify(SysMsgType sysmsg, NotifyType notifytype,int32 taskid,int8 inum)
	{
		SysID = sysmsg; 
		MsgID = Msg_Task_Notify; 
		Type = notifytype;
		taskId = taskid;
		iNum = inum;
		Len = sizeof(*this);
	}
	int32	Type;		//��ӦNotifyType
	int32	taskId;
	int8		iNum;
};


//��������Ϣ֪ͨ
struct MsgCopyNotify :public MsgBase
{
	MsgCopyNotify(SysMsgType sysmsg, NotifyType notifytype,int32 copyid,uint64 copyguid)
	{
		SysID = sysmsg; 
		MsgID = Msg_Task_Notify; 
		Type = notifytype;
		copyId = copyid;
		copyGuid = copyguid;
		Len = sizeof(*this);
	}
	int32	Type;		//��ӦNotifyType
	int32 copyId;
	uint64 copyGuid;
};



//���� C2G��G2D
struct MsgFriends :public MsgBase
{
	MsgFriends(int64 irequestroleid, int64 iaddroleid, int8 itype,char *szname)
	{
		SysID = ID_C2G; 
		MsgID =  Msg_Add_Friends; 
		iRequestRoleID = irequestroleid; 
		iAddRoleID = iaddroleid; 
		iType = itype; 
		strncpy(szName,szname, MAX_NAME_SIZE);
		Len = sizeof(*this);
	}
	int64			iRequestRoleID;
	int64			iAddRoleID;
	int8			iType;
	char			szName[MAX_NAME_SIZE];
};

//��Ӻ��ѵ���Ϣ֪ͨ
struct MsgFriNotify :public MsgBase
{
	MsgFriNotify(SysMsgType sysmsg, NotifyType notifytype,int64 iroleid,int inumfriend)
	{
		SysID = sysmsg; 
		MsgID = Msg_Fri_Notify; 
		Type = notifytype; 
		iRoleID = iroleid;
		iNumFriend = inumfriend;
		Len = sizeof(*this);
	}
	int32	Type;		//��ӦNotifyType
	int64	iRoleID;
	int		iNumFriend;
	
	//int64	roleId;					//��ɫID
	//char	szName[MAX_NAME_SIZE];	//��������
	//bool	bIsLine;				//�Ƿ�����
	//int		iLevel;					//�ȼ�
	//int		iFight;					//ս����
};

//���� C2G��G2D
struct MsgTeam_Apply :public MsgBase
{
	MsgTeam_Apply(uint64 requester, const char* name, int level, uint64 partner)
	{
		SysID = ID_C2G; 
		MsgID =  Msg_Team_Apply;
		iRequesterId = requester;
        iRequesterlevel = level;
        strncpy(szRequesterName, name, sizeof(char)*MAX_NAME_SIZE);
		iPartnerId = partner;
		Len = sizeof(*this);
	}

	uint64			iRequesterId;
    char	        szRequesterName[MAX_NAME_SIZE];
    int32           iRequesterlevel;
	uint64			iPartnerId;
};


//>�������
struct MsgTeam_ReplyApply : public MsgBase
{
    MsgTeam_ReplyApply(uint64 requester, const char* requestName, uint64 partnerId, const char* partnerName, int result)
    {
        SysID = ID_G2C;
        MsgID = Msg_Team_ReplyApply;

        iRequesterId = requester;
        strncpy(szRequesterName, requestName, MAX_NAME_SIZE);
        iPartnerId = partnerId;
        strncpy(szPartnerName, partnerName, MAX_NAME_SIZE);
        iResult = result;
        Len = sizeof(*this);
    }

    uint64			iRequesterId;
    char	        szRequesterName[MAX_NAME_SIZE];
    uint64	        iPartnerId;
    char	        szPartnerName[MAX_NAME_SIZE];
    int32   iResult;    //>0.ͬ�����
                        //>1.�Է��Ѿ�����
                        //>2.�Է����ж���,���ʧ��
                        //>3.�Է��ܾ����������ߵĶ���
                        //>4.�Է�����ܾ������߼���
                        //>5.�Է���������Ա
};

//>��Ӵ����ɹ�
struct MsgTeam_Created : public MsgBase
{
    MsgTeam_Created()
    {
        SysID = ID_G2C;
        MsgID = Msg_Team_Created;
        Len = sizeof(*this);
    }

    int memberNumber;

    //int64	memberId;
    //int32 memberAttribute;
    //char	szName[MAX_NAME_SIZE];
    //int32   memberLevel;
    //int32   Hp;
    //int32   MaxHp;
};

//>��Ӽ����³�Ա
struct MsgTeam_NewMember : public MsgBase
{
    MsgTeam_NewMember()
    {
        SysID = ID_G2C;
        MsgID = Msg_Team_NewMember;
        Len = sizeof(*this);
    }

    uint64	memberId;
    int32   memberAttribute;
    char	szName[MAX_NAME_SIZE];
    int32   memberLevel;
    int32   Hp;
    int32   MaxHp;
};

//>��Ա�뿪���
struct MsgTeam_MemberExit : public MsgBase
{
    MsgTeam_MemberExit()
    {
        SysID = ID_G2C;
        MsgID = Msg_Team_MemberExit;
        Len = sizeof(*this);
    }

    uint64	memberId;
};

//>��Ӷӳ�ת��
struct MsgTeam_ChangeLeader : public MsgBase
{
    MsgTeam_ChangeLeader()
    {
        SysID = ID_G2C;
        MsgID = Msg_Team_ChangeLeader;
        Len = sizeof(*this);
    }

    uint64 memberId;
    int32 memberAttribute;
};

//>�鳤�߶�Ա
struct MsgTeam_KickMemberReq : public MsgBase
{
    MsgTeam_KickMemberReq()
    {
        SysID = ID_C2G;
        MsgID = Msg_Team_KickMemberReq;
        Len = sizeof(*this);
    }

    uint64 badGuyId;
};


//�������Ϣ֪ͨ
struct MsgTeamNotify :public MsgBase
{
	MsgTeamNotify(SysMsgType sysmsg, NotifyType notifytype,int64 iroleid,const char *szpar)
	{
		SysID = sysmsg; 
		MsgID = Msg_Team_Notify; 
		Type = notifytype; 
		iRoleID = iroleid;
		strncpy(szPar, szpar, 255);
		Len = sizeof(*this);
	}
	unsigned char Type;		//��ӦNotifyType
	int64	iRoleID;
	char	szPar[255];
};

//���� C2G��G2D
struct MsgChat :public MsgBase
{
    MsgChat(){ }
	MsgChat(SysMsgType sysmsg, int64 senderId,const char *senderName, int8 channel)
	{
		SysID = ID_C2G; 
		MsgID =  Msg_Chat_CHANNEL; 
		iSendRoleID = senderId;
        iChannel = channel;
		strncpy(SenderName, senderName, MAX_NAME_SIZE);
		Len = sizeof(*this);
	}
    int8	        iChannel;		//>Ƶ������
	int64			iSendRoleID;    //>������
    char            SenderName[MAX_NAME_SIZE];     //>����������
    int16           msgSize;        //>��Ϣ���ݴ�С
    int16           itemCount;      //>Я����Ʒ����
    //char	szMsg[msgSize];		//>��Ϣ����
    //int64 itemsGuid;          //>Я������Ʒguid
};

//�������Ϣ֪ͨ
struct MsgChatNotify :public MsgBase
{
	MsgChatNotify(SysMsgType sysmsg,int64 senderId, char* senderName, int8 channel)
	{
		SysID = sysmsg; 
		MsgID = Msg_Chat_Notify; 
		iSenderID = senderId;
        strncpy(SenderName, senderName, MAX_NAME_SIZE);
		iChannel = channel;
        msgSize = 0;
        itemCount = 0;
		Len = sizeof(*this);
	}

	int8	iChannel;		//>Ƶ������
    int64	iSenderID;	    //>������
    char    SenderName[MAX_NAME_SIZE];     //>����������
    int16   msgSize;        //>��Ϣ���ݴ�С
    int16   itemCount;      //>Я����Ʒ����
	//char	szMsg[msgSize];		//>��Ϣ����
    //int64 itemsGuid;          //>Я������Ʒguid
};

//���� C2G��G2D
struct MsgGang :public MsgBase
{
	MsgGang(int64 ipar1, int64 ipar2, int64 ipar3,const char *szpar)
	{
		SysID = ID_C2G; 
		MsgID =  Msg_Create_Gang; 
		iPar[0] = ipar1;
		iPar[1] = ipar2;
		iPar[2] = ipar3;
		strncpy(szPar, szpar, 255);
		Len = sizeof(*this);
	}
	int64			iPar[3];
	char			szPar[255];
};

struct MsgGangNotify :public MsgBase
{
	MsgGangNotify(SysMsgType sysmsg, NotifyType notifytype,int64 ipar1,int64 ipar2,int64 ipar3,const char *szpar)
	{
		SysID = sysmsg; 
		MsgID = Msg_Gang_Notify; 
		Type = notifytype; 
		iPar[0] = ipar1;
		iPar[1] = ipar2;
		iPar[2] = ipar3;
		strncpy(szPar, szpar, 255);
		Len = sizeof(*this);
	}
	unsigned char Type;		//��ӦNotifyType
	int64	iPar[3];		
	char	szPar[255];		//���͵�����
};

//�����ӣ��ͻ��˷��������ô�Э��
struct MsgChest :public MsgBase
{
	MsgChest(uint8 sysid, AttributeType moneytype, int num)
	{
		SysID = sysid;
		MsgID = Msg_Chest; 
		MoneyType = moneytype;
		Num = num;
		Len = sizeof(*this);
	}
	AttributeType MoneyType;	//��������
	int Num;		//��ȡ����
	int Itemid;		//��Ʒid,�䳤
};


#pragma pack()
#endif
