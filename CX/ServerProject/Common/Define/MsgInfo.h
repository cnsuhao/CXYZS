	/********************************************************************
created:	2014/7/15
author:		LL
desc:		消息传输结构定义，消息结构对应MsgDef.h中的协议号。 客户端服务器共用此文件

通讯协议说明：
1. 以结构体形式实现C/S间数据交换
2. 协议结构必须是1字节对齐方式
3. 如结构体中有指针，其指向的内容紧挨着所在结构后放置，指针本事建议置零
*********************************************************************/

#ifndef _MESSAGE_DEFINE__H_
#define _MESSAGE_DEFINE__H_
#include "MessageIdentifiers.h"
#include "MsgDef.h"
#include "GameDef.h"

#define GATE_CONNECTION_PASSWORD	 "~Game@9494~"	//网关服务器连接密码

#pragma pack(1)

//所有协议的父类
struct MsgBase
{
    uint8           SysID;		//即包的第一个字节,系统定义的id
	uint16          MsgID;		//自定义协议号
	uint64          OBJID;		//用于转发消息时使用，寻找转发对象。客户端和登陆服务器通讯使用LinkID、和游戏服务器通讯使用AccountId。
	uint16          Len;		//消息长度，考虑有变长消息，所以消息体的长度以Len为准。
////以下函数用于变长消息
//	void AddByte(char in);		//加入Byte
//	void AddShort(short in);	//加入short
//	void AddInt(int in){;(*this)+=4;}		//加入int
//	void AddLong(long long in);	//加入64位int
//	void AddStr(char* in, int num);	//加入字符串
//	void AddBool(bool in);		//加入bool	
//	void AddFloat(float in);	//加入float
//	void AddDouble(double in);	//加入double
//
//	void AddNull(int num);		//加入指定长度空字节
};

//无参数的消息通知
struct MsgNotify :public MsgBase
{
    MsgNotify(SysMsgType sysmsg, NotifyType notifytype){SysID = sysmsg; MsgID = Msg_Notify; Type = notifytype; Len = sizeof(*this);}
	unsigned char Type;		//对应NotifyType
};

//注册、登陆
struct MsgRegiser_Login :public MsgBase
{
	MsgRegiser_Login(char* name, char* password, uint64 touristid, MsgType msgid){SysID = ID_C2L; MsgID = msgid; Len = sizeof(*this); 
	strncpy(Name, name, MAX_NAME_SIZE); strncpy(Password, password, MAX_PASS_SIZE); TouristID = touristid;}
	char Name[MAX_NAME_SIZE];			//账号名
	char Password[MAX_PASS_SIZE];		//密码
	uint64 TouristID;	                //游客id
};
 
struct ServerListIt
{
	uint16 ServerID;						//服务器id
	char ServerName[MAX_NAME_SIZE];			//服务器名
	int32 ServerState;						//服务器类型 1老服 2新服 3合服
	bool isStart;							//服务器是否开启
};
//服务器列表,变长数据需要手动设置Len
struct MsgServerList :public MsgBase
{
    MsgServerList(){SysID = ID_L2C; MsgID =  Msg_ServerList;}
	int32 ServerNum;					//服务器数量
	ServerListIt ServerListInfo;	//变长数据								
};

//选择服务器
struct MsgSelServer :public MsgBase
{
	MsgSelServer(int16 serverid){SysID = ID_C2L; MsgID = Msg_SelServer; ServerID = serverid; Len = sizeof(*this);}
	uint16 ServerID;					//服务器id
};

//创建角色 C2G、G2D
struct MsgCreateRole :public MsgBase
{
	MsgCreateRole(const char* name, int8 sex, int16 headid){SysID = ID_C2G; MsgID =  Msg_CreateRole; strncpy(Name, name, MAX_NAME_SIZE); Sex = sex; HeadID = headid; Len = sizeof(*this);}
	char Name[MAX_NAME_SIZE];		//角色名
	int8 Sex;								//性别 0女 1男
    int16 HeadID;							//头像id
};
//游戏服务器到网关和登陆去注册
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

	int16 ServerID;								//服务器id
	char ServerName[MAX_NAME_SIZE];				//服务器名
};

//获取全局guid
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

//登陆服务器通知游戏服务器玩家进入
struct MsgPlayer_Enter_GameServer :public MsgBase
{
	MsgPlayer_Enter_GameServer(uint64 accountid, uint64 linkid){SysID = ID_L2G;	MsgID = Msg_Player_Enter_GameServer; Len = sizeof(*this); AccountID = accountid; LinkID = linkid;}
	uint64 AccountID;								//账号id
	uint64 LinkID;									//连接id
};

struct MsgPlayer_Leave_GameServer :public MsgBase
{
	MsgPlayer_Leave_GameServer(uint64 accountid){SysID = ID_L2G;	MsgID = Msg_Player_Leave_GameServer; Len = sizeof(*this); AccountID = accountid;}
	uint64 AccountID;								//账号id
};

struct MsgRole_BaseInfo :public MsgBase
{
	MsgRole_BaseInfo(uint64 accountid){SysID = ID_D2G;	MsgID = Msg_Role_BaseInfo; Len = sizeof(*this); OBJID = accountid;}
	BaseInfoIt BaseInfo;				//基础信息
};

struct MsgRole_DetailAttribute : public MsgBase
{
	MsgRole_DetailAttribute(uint64 accountid){SysID = ID_D2G;	MsgID = Msg_Role_DetailAttribute; Len = sizeof(*this); OBJID = accountid;}
	DetailAttribute Attribute;
};

struct MsgUpdate_BaseInfo : public MsgBase
{
	MsgUpdate_BaseInfo(uint64 accountid){{SysID = ID_G2D; MsgID = Msg_Role_BaseInfo_Update; Len = sizeof(*this); OBJID = accountid;}}
	int32 attributeType;			//玩家改变的属性类型
	float attributeValue;				//玩家改变属性类型的值
};
struct MsgRole_EnterMap : public MsgBase
{
    MsgRole_EnterMap() { SysID = ID_C2G; MsgID = Msg_Role_EnterMap; Len = sizeof(*this);  }

    uint32 mapId;       //>客户端请求进入的mapId.(服务器判断角色已存在某张地图中则mapId为要切换的地图,否则服务使用自己默认的地图)
    uint32 conveyId;    //>传送点Id
};

//>DB传给Game角色数据信息流
struct MsgRole_Detail_Info : public MsgBase
{
    MsgRole_Detail_Info() {}

    int32 SkillTreeNum;		//功法数量
	int32 WingNum;			//翅膀数量
    int32 MagicWeaponNum;	//法宝数量
    int32 MountNum;			//器灵数量
	int32 CopyNum;			//副本数量
	int32 DailyNum;			//日常数量
	int32 TaskNum;			//任务数量
	int32 EquipNum;			//身上装备数量
	int32 BagNum;			//背包物品数量
	int32 FriendsListNum;	//好友列表数量
	int32 BlackListNum;		//黑名单列表数量
	int32 ChoseSkill;		//玩家已经选择的技能数量
};


//>其他玩家在地图上基础信息
struct MsgOtherRole_BaseInfo : public MsgBase
{
    MsgOtherRole_BaseInfo(uint64 accountid){SysID = ID_G2C;	MsgID = Msg_OtherRole_BaseInfo; Len = sizeof(*this); OBJID = accountid;}
   
    uint64  guid;		                //唯一id	
    char    playerName[MAX_NAME_SIZE];	//名字
    int32   sex;                        //性别
    int32   equip[EQUIPTYPE_MAX];       //穿戴
    float   posX;                       //位置x
    float   posY;                       //位置y
    int32   hp;					        //血
    int32   maxHp;                      //最大血量
    int32   level;                      //等级
    int32   headIconIndex;              //头像索引
};

struct MsgOtherRole_Leave : public MsgBase
{
    MsgOtherRole_Leave(uint64 accountid) {SysID = ID_G2C;	MsgID = Msg_OtherRole_Leave; Len = sizeof(*this); OBJID = accountid; }
    uint64  LeaverID;           //离开者
};


//>其他角色详细信息
struct MsgOtherRole_DetailInfo : public MsgBase
{
    MsgOtherRole_DetailInfo() 
    {
        SysID = ID_G2C;
        MsgID = Msg_OtherRole_DetailInfo;
        Len = sizeof(*this);
    }

    int64   guid;                       //>角色唯一id
    char    name[MAX_NAME_SIZE];        //>角色名
    int32   level;                      //>角色等级
    int     sex;                        //>性别

    int     battlePower;                //>战斗力
    char    gangName[MAX_NAME_SIZE];    //>帮派名
    int     hp;                         //>血量
    int     maxHp;                      //>最大血量
    int     mp;                         //>蓝
    int     maxMp;                      //>最大蓝
    int     attack;                     //>攻击力
    int     defence;                    //>防御力
    int     hit;                        //>命中值
    int     dodge;                      //>闪避值
    int     crit;                       //>暴击值
    int     tenacity;                   //>韧性值
    float   hitProb;                    //>命中率
    float   critProb;                   //>暴击率

    ItemInfoIt equipments[EQUIPTYPE_MAX];   //>装备信息
};

//>其他角色详细信息查询
struct MsgOtherRole_DetailInfoQuery : public MsgBase
{
    MsgOtherRole_DetailInfoQuery() 
    {
        SysID = ID_C2G;
        MsgID = Msg_OtherRole_DetailInfoQuery;
        Len = sizeof(*this);
    }
    
    int64   queryGuid;          //>角色唯一id
};

//玩家移动
struct MsgObject_Move :public MsgBase
{
	MsgObject_Move(){ SysID = ID_D2G;	MsgID = Msg_Object_Move; Len = sizeof(*this);}
	uint64 MoverID;         //移动者
    int    MoverType;       //移动者类型
    float MoverPosX;        //移动者的当前位置
    float MoverPosY;        //移动者的当前位置
    float TargetPosX;       //目的地位置
    float TargetPosY;       //目的地位置
    uint16 PathNum;			//路径数量
    //int MovePath;		    //路径列表，变长字段
};

struct MsgObject_Move_Stop : public MsgBase
{
    MsgObject_Move_Stop(){SysID = ID_D2G;	MsgID = Msg_Object_Move_Stop; Len = sizeof(*this);}
    uint64 MoverID;         //移动者
    int    MoverType;       //移动者类型
    float MoverPosX;        //移动者的当前位置
    float MoverPosY;        //移动者的当前位置
};

struct MsgObject_Move_Kick : public MsgBase
{
    MsgObject_Move_Kick(){SysID = ID_G2C;	MsgID = Msg_Object_Move_kick; Len = sizeof(*this);}
    uint64 MoverID;         //移动者
    int    MoverType;       //移动者类型
    float MoverPosX;        //移动者的当前位置
    float MoverPosY;        //移动者的当前位置
};


//>对象的属性
struct MsgObject_SyncAttribute :public MsgBase
{
	MsgObject_SyncAttribute(){SysID = ID_G2C; MsgID = Msg_Object_SyncAttribute; Len = sizeof(*this);}
	uint64 OwnerID;             //>对象的ID
    ObjectType OwnerType;       //>对象的类型
    AttributeType Property;		//>对象的属性类型
	float Value;				//>属性的值
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

//>对象加血
struct MsgObject_AddHp : public MsgBase
{
    MsgObject_AddHp() { SysID = ID_G2C; MsgID = Msg_Object_AddHp; Len = sizeof(*this); }

    uint64 OwnerID;             //>对象的ID
    ObjectType OwnerType;       //>对象的类型
    int32  LastHp;              //>剩余Hp
    AddHpMode Mode;             //>掉血模式
};

//>对象掉血
struct MsgObject_DropHp : public MsgBase
{
    MsgObject_DropHp() { SysID = ID_G2C; MsgID = Msg_Object_DropHp; Len = sizeof(*this); }

    uint64 OwnerID;             //>对象的ID
    ObjectType OwnerType;       //>对象的类型
    int32  LastHp;              //>剩余Hp
    DropHpMode Mode;            //>加血模式
};

//对象buff队列
struct MsgObject_BuffList : public MsgBase
{
    MsgObject_BuffList() { SysID = ID_G2C; MsgID = Msg_Object_BuffList; Len = sizeof(*this); }

    uint64 OwnerID;             //>对象的ID
    ObjectType OwnerType;       //>对象的类型
    int32  buffNumber;          //>buff个数

    //int   buffId;				//>id
    //int   buffInstanceId      //>buff实例id
    //float lastTime;			//>剩余时间s 0秒时移除buff
};

//>对象添加buff效果
struct MsgObject_AddBuff :public MsgBase
{
    MsgObject_AddBuff(){ SysID = ID_G2C; MsgID = Msg_Object_AddBuff; Len = sizeof(*this); }
    int64 OwnerID;			    //>对象的ID
    ObjectType OwnerType;		//>对象的类型
    int   buffId;				//>id
    int   buffInstanceId;       //>buff实例id
    float lastTime;				//>剩余时间s 0秒时移除buff
};

//>玩家结束buff效果
struct MsgObject_DelBuff :public MsgBase
{
    MsgObject_DelBuff(){ SysID = ID_G2C; MsgID = Msg_Object_DelBuff; Len = sizeof(*this); }

    int64 OwnerID;			    //>对象的ID
    ObjectType OwnerType;		//>对象的类型
    int   buffInstanceId;		//>buff实例id
};


//服务器同步玩家状态(表)
struct MsgPlayer_state :public MsgBase
{
	MsgPlayer_state(int64 guid, bool *state){SysID = ID_G2C; MsgID = Msg_Player_state; Len = sizeof(*this); OBJID = guid; for (int i = 0; i < State_Max; i++){State[i] = state[i];}}
	bool State[State_Max];
};

//客户端申请使用技能
struct MsgC2GPlayer_UseSkill :public MsgBase
{
	MsgC2GPlayer_UseSkill(){SysID = ID_C2G; MsgID = Msg_Player_UseSkill; Len = sizeof(*this);}
	int SkillID;					//技能id，对应技能表的技能ID
	ObjectType ObjType;				//首目标的类型
	uint64 target;					//首目标的唯一id
};

struct HurtData
{
	ObjectType ObjType;			//目标的类型
	uint64 guid;				//目标的唯一id
	AttackType hurtType;		//伤害类型
	int hp;						//血量变化值
};

//服务器通知玩家或怪物使用技能
struct MsgG2CPlayer_UseSkill :public MsgBase
{
	MsgG2CPlayer_UseSkill(){SysID = ID_G2C; MsgID = Msg_Player_UseSkill; Len = sizeof(*this);}
	int SkillID;				//技能id，对应技能表的技能ID
    uint64 attacker;            //攻击者的唯一id
	ObjectType attackerType;	//攻击者的类型
    uint64 target;              //目标的唯一id
    ObjectType targetType;	    //目标的类型
};

//服务器通知玩家或者怪物的buff伤害
struct MsgG2CPlayer_BuffHurt : public MsgBase
{
	MsgG2CPlayer_BuffHurt(){SysID = ID_G2C; MsgID = Msg_Player_BuffHurt; Len = sizeof(*this);}
	int SkillID;		//技能ID
	int BuffId;			//造成掉血的BUFFID
	uint64  AttactGuid;	//攻击方的对象唯一ID
	uint64  TargetGuid;		//受伤害对象的唯一ID
	float hp;				//掉血血量
};


struct  MsgG2CPlayer_AttackType : public MsgBase
{
	MsgG2CPlayer_AttackType(){SysID = ID_G2C; MsgID = Msg_Player_AttackType; Len = sizeof(*this);}
	AttackType attackType;
};

//服务器通知客户端原地复活所需要的信息
struct MsgPlayer_ReliveData :public MsgBase
{
	MsgPlayer_ReliveData(){SysID = ID_G2C; MsgID = Msg_Player_ReliveData; Len = sizeof(*this);}
	int Countdown;			//复活倒计时（秒），若倒计时结束玩家没选择复活类型则默认回城复活。
	int StayRelive;			//当前原地复活次数
	int RMB;				//本次原地复活所需付费（rmb）
};

//客户端选择复活类型
struct MsgC2GPlayer_Relive :public MsgBase
{
	MsgC2GPlayer_Relive(){SysID = ID_C2G; MsgID = Msg_Player_Relive; Len = sizeof(*this);}
	int8 relive_type;			//复活类型 0回城复活，1原地复活
};

//服务器通知玩家复活
struct MsgG2CPlayer_Relive :public MsgBase
{
	MsgG2CPlayer_Relive(){SysID = ID_G2C; MsgID = Msg_Player_Relive; Len = sizeof(*this);}
	int mapid;		//复活地图id
	int	posid;		//复活位置
};

//广播怪物信息
struct MsgMonster_Baseinfo :public MsgBase
{
	MsgMonster_Baseinfo(){SysID = ID_G2C; MsgID = Msg_Monster_Baseinfo; Len = sizeof(*this);}
	uint64 MonsterGuid;					//怪物唯一id;
	int MonsterId;						//怪物表id;
	int	hp;								//当前血量
	int	hp_max;							//血量上限;
	bool CanAttack;						//是否允许玩家攻击;	
    float   posX;                       //位置x
    float   posY;                       //位置y
};

//广播怪物移动
struct MsgMonster_Move :public MsgBase
{
	MsgMonster_Move(){SysID = ID_G2C; MsgID = Msg_Monster_Move; Len = sizeof(*this);}
	uint64 MonsterGuid;					//怪物唯一id;
	uint16 PathNum;		//路径数量
	int MovePath;		//路径列表，变长字段
};

//广播移除怪物
struct MsgMonster_Del :public MsgBase
{
	MsgMonster_Del(){SysID = ID_G2C; MsgID = Msg_Monster_Del; Len = sizeof(*this);}
	uint64 MonsterGuid;					//怪物唯一id;
};

//广播传送怪物
struct MsgMonster_Goto :public MsgBase
{
	MsgMonster_Goto(){SysID = ID_G2C; MsgID = Msg_Monster_Goto; Len = sizeof(*this);}
	uint64 MonsterGuid;					//怪物唯一id;
    float   posX;                       //位置x
    float   posY;                       //位置y
};

//请求背包清单
struct MsgItem_BagList : public MsgBase
{
    MsgItem_BagList() { SysID = ID_C2G; MsgID = Msg_Item_BagList; Len = sizeof(*this); }
};

//物品添加
struct MsgItem_Add_Set : public MsgBase
{
    MsgItem_Add_Set() { SysID = ID_G2D; MsgID = Msg_Item_Add_Set; Len = sizeof(*this); }

    uint32 operateType; //操作类型
    uint8  addNumItem;  //添加物品的数量

    //{   //花扩号内为变长数据的主体内容
    //    uint64 guid;		//唯一id (由DB服务器创建)
    //    int itemID;		//物品ID（模板）
    //    int prefixID;		//前缀ID
    //    int starNum;		//星数
    //    bool isBind;		//是否绑定
    //    int num;			//数量
    //    int time;			//创建时间(由DB服务器创建)
    //}
};

//物品删除
struct MsgItem_Del_Set : public MsgBase
{
    MsgItem_Del_Set() { SysID = ID_G2D; MsgID = Msg_Item_Del_Set; Len = sizeof(*this); }

    uint32 operateType; //操作类型
    uint8  delNumItem;  //删除物品的数量

    //{   //花扩号内为变长数据的主体内容
    //    uint64 guid;		//唯一id (由DB服务器创建)
    //}
};

//物品更新
struct MsgItem_Update_Set : public MsgBase
{
    MsgItem_Update_Set() { SysID = ID_G2D; MsgID = Msg_Item_Updata_Set; Len = sizeof(*this); }
    
    uint32 operateType; //操作类型
    uint8  updateNumItem;  //添加物品的数量

    //{   //花扩号内为变长数据的主体内容
    //    uint64 guid;		//唯一id (由DB服务器创建)
    //    int itemID;		//物品ID（模板）
    //    int prefixID;		//前缀ID
    //    int starNum;		//星数
    //    bool isBind;		//是否绑定
    //    int num;			//数量
    //    int time;			//创建时间(由DB服务器创建)
    //}
};

//商店购买物品
struct MsgItem_ShopBuy : public MsgBase
{
    MsgItem_ShopBuy() { SysID = ID_C2G, MsgID = Msg_Item_ShopBuy; Len = sizeof(*this); }

    int32 shopId;
    int32 goodesId;
};

//商店出售物品
struct MsgItem_ShopSell : public MsgBase
{

    MsgItem_ShopSell() { SysID = ID_C2G, MsgID = Msg_Item_ShopSell; Len = sizeof(*this); }

    uint64 itemGuid;
    int32 number;
};

//物品合成
struct MsgItem_Compose : public MsgBase
{
    MsgItem_Compose() { SysID = ID_C2G, MsgID = Msg_Item_Compose; Len = sizeof(*this); }

    uint64 mainStuffGuid;   //>主物品
    uint64 Stuff1Guid;      //>材料物品1
    uint64 Stuff2Guid;      //>材料物品2
    uint64 Stuff3Guid;      //>材料物品3
    uint64 Stuff4Guid;      //>材料物品4
};

//装备升星
struct MsgItem_UpStar : public MsgBase
{
    MsgItem_UpStar() { SysID = ID_C2G, MsgID = Msg_Item_UpStar; Len = sizeof(*this); }

    uint64 equipmentGuid;   //>升星的装备
};

//装备传承
struct MsgItem_Inherit : public MsgBase
{
    MsgItem_Inherit() { SysID = ID_C2G, MsgID = Msg_Item_Inherit; Len = sizeof(*this); }

    uint64 sourceEquipmentGuid; //>传承装备
    uint64 targetEquipmentGuid; //>继承装备
};

//物品使用
struct MsgItem_Use : public MsgBase
{
    MsgItem_Use() { SysID = ID_C2G, MsgID = Msg_Item_Use; Len = sizeof(*this); }

    uint32 operateType; //>操作类型
    uint64 useItemGuid; //>使用物品唯一id
};

struct MsgEquip_List : public MsgBase
{
    MsgEquip_List() { SysID = ID_G2C, MsgID = Msg_Equip_List; Len = sizeof(*this); }

    uint32 operateType; //>操作类型
    ItemInfoIt equipments[EQUIPTYPE_MAX];
};

//穿戴装备更新
struct MsgEquip_Update : public MsgBase
{
    MsgEquip_Update() { SysID = ID_G2D, MsgID = Msg_Equip_Update; Len = sizeof(*this); }

    uint32 operateType;    //>操作类型
    uint32 equipmentType;  //>装备的类型(脱下装备的时候并不提供物品结构)

    ItemInfoIt equipment;
};

//装备显示更新
struct MsgEquip_ShowUpdate : public MsgBase
{
    MsgEquip_ShowUpdate() { SysID = ID_G2C, MsgID = Msg_Equip_ShowUpdate; Len = sizeof(*this); }

    uint64 whoGuid;
    uint32 equip[EQUIPTYPE_MAX];
};

//技能树清单
struct MsgSkill_List : public MsgBase
{
    MsgSkill_List() { SysID = ID_G2C, MsgID = Msg_Skill_List; Len = sizeof(*this); }

    uint32 numberSkillTree;

    //{ //变长数据的主体内容
    //   int skillType;      //>技能类型
    //   int level;          //>技能等级
    //}.set
};

//技能添加
struct MsgSkill_Add : public MsgBase
{
    MsgSkill_Add() { SysID = ID_G2C, MsgID = Msg_Skill_Add; Len = sizeof(*this); }

    SkillInfoIt addSkill;
};

//技能更新
struct MsgSkill_Update : public MsgBase
{
    MsgSkill_Update() { SysID = ID_G2C, MsgID = Msg_SKill_Update; Len = sizeof(*this); }

    SkillInfoIt   updateSkill;
};

struct MsgSkill_Activate : public MsgBase
{
    MsgSkill_Activate() { SysID = ID_C2G, MsgID = Msg_Skill_Activate; Len = sizeof(*this); }

    int32 ActivateSkillType;    //>所改变修炼技能的类型
};

struct MsgSkill_ChoseSkill : public MsgBase
{
    MsgSkill_ChoseSkill() { SysID = ID_C2G, MsgID = Msg_Skill_ChoseSkill, Len = sizeof(*this); }

    int32 setIndex;     //>设置的位置索引
    int32 skillId;      //>技能id
};

struct MsgTask_TakeTask : public MsgBase
{
	MsgTask_TakeTask(){SysID = ID_C2G,MsgID = Msg_Task_Take;Len = sizeof(*this);}
	int32 taskId;
	int32 iReqTaskID;

	int32 itemId;
};
//任务进度更新
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

//添加好友的消息通知
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
	int32	Type;		//对应NotifyType
	int32	taskId;
	int8		iNum;
};


//副本的消息通知
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
	int32	Type;		//对应NotifyType
	int32 copyId;
	uint64 copyGuid;
};



//好友 C2G、G2D
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

//添加好友的消息通知
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
	int32	Type;		//对应NotifyType
	int64	iRoleID;
	int		iNumFriend;
	
	//int64	roleId;					//角色ID
	//char	szName[MAX_NAME_SIZE];	//好友名称
	//bool	bIsLine;				//是否在线
	//int		iLevel;					//等级
	//int		iFight;					//战斗力
};

//队伍 C2G、G2D
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


//>邀请组队
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
    int32   iResult;    //>0.同意组队
                        //>1.对方已经离线
                        //>2.对方已有队伍,组队失败
                        //>3.对方拒绝加入邀请者的队伍
                        //>4.对方队伍拒绝邀请者加入
                        //>5.对方队伍已满员
};

//>组队创建成功
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

//>组队加入新成员
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

//>成员离开组队
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

//>组队队长转移
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

//>组长踢队员
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


//队伍的消息通知
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
	unsigned char Type;		//对应NotifyType
	int64	iRoleID;
	char	szPar[255];
};

//聊天 C2G、G2D
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
    int8	        iChannel;		//>频道类型
	int64			iSendRoleID;    //>发送者
    char            SenderName[MAX_NAME_SIZE];     //>发送者名字
    int16           msgSize;        //>消息内容大小
    int16           itemCount;      //>携带物品个数
    //char	szMsg[msgSize];		//>消息内容
    //int64 itemsGuid;          //>携带的物品guid
};

//聊天的消息通知
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

	int8	iChannel;		//>频道类型
    int64	iSenderID;	    //>发送者
    char    SenderName[MAX_NAME_SIZE];     //>发送者名字
    int16   msgSize;        //>消息内容大小
    int16   itemCount;      //>携带物品个数
	//char	szMsg[msgSize];		//>消息内容
    //int64 itemsGuid;          //>携带的物品guid
};

//帮派 C2G、G2D
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
	unsigned char Type;		//对应NotifyType
	int64	iPar[3];		
	char	szPar[255];		//发送的数据
};

//抽箱子，客户端服务器共用此协议
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
	AttributeType MoneyType;	//货币类型
	int Num;		//抽取次数
	int Itemid;		//物品id,变长
};


#pragma pack()
#endif
