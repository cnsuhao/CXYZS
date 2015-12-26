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
#include "ServerDef.h"
#include <string.h>
#include <assert.h>



#pragma pack(1)

//所有协议的父类
struct MsgBase
{
    unsigned char SysID;		//即包的第一个字节,系统定义的id
	unsigned char MsgID;		//自定义协议号
	__int64 OBJID;				//用于转发消息时使用，寻找转发对象。客户端和登陆服务器通讯使用LinkID、和游戏服务器通讯使用AccountId。
	unsigned __int16 Len;		//消息长度，考虑有变长消息，所以消息体的长度以Len为准。
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
	MsgRegiser_Login(char* name, char* password, __int64 touristid, unsigned char msgid){SysID = ID_C2L; MsgID = msgid; Len = sizeof(*this); 
	strcpy(Name, name); strcpy(Password, password); TouristID = touristid;}
	char Name[MAX_REGISER_NAME_SIZE];			//账号名
	char Password[MAX_REGISER_PASS_SIZE];		//密码
	__int64 TouristID;	//游客id
};
 
struct ServerListIt
{
	__int16 ServerID;							//服务器id
	char ServerName[MAX_REGISER_NAME_SIZE];	//服务器名
	int ServerState;						//服务器类型 1老服 2新服 3合服
	bool isStart;							//服务器是否开启
};
//服务器列表,变长数据需要手动设置Len
struct MsgServerList :public MsgBase
{
    MsgServerList(){SysID = ID_L2C; MsgID =  Msg_ServerList;}
	int ServerNum;					//服务器数量
	ServerListIt ServerListInfo;	//变长数据								
};

//选择服务器
struct MsgSelServer :public MsgBase
{
	MsgSelServer(__int16 serverid){SysID = ID_C2L; MsgID = Msg_SelServer; ServerID = serverid; Len = sizeof(*this);}
	__int16 ServerID;					//服务器id
};

//创建角色 C2G、G2D
struct MsgCreateRole :public MsgBase
{
	MsgCreateRole(char* name, __int8 sex, __int16 headid){SysID = ID_C2G; MsgID =  Msg_CreateRole; strcpy(Name, name); Sex = sex; HeadID = headid; Len = sizeof(*this);}
	char Name[MAX_REGISER_NAME_SIZE];		//角色名
	__int8 Sex;								//性别 0女 1男
	__int16 HeadID;							//头像id
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

	__int16 ServerID;								//服务器id
	char ServerName[MAX_REGISER_NAME_SIZE];		//服务器名
};

struct MsgPlayer_Enter_GameServer :public MsgBase
{
	MsgPlayer_Enter_GameServer(__int64 accountid, __int64 linkid){SysID = ID_L2G;	MsgID = Msg_Player_Enter_GameServer; Len = sizeof(*this); AccountID = accountid; LinkID = linkid;}
	__int64 AccountID;								//账号id
	__int64 LinkID;									//连接id
};

struct MsgPlayer_Leave_GameServer :public MsgBase
{
	MsgPlayer_Leave_GameServer(__int64 accountid){SysID = ID_L2G;	MsgID = Msg_Player_Leave_GameServer; Len = sizeof(*this); AccountID = accountid;}
	__int64 AccountID;								//账号id
};

#pragma pack()
#endif
