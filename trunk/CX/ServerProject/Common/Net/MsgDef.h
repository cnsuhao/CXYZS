/********************************************************************
created:	2014/7/15
author:		LL
desc:		定义协议号，及协议相关类型。。 客户端服务器共用此文件
//游戏客户端（Client），登陆服务器（Login），网关服务器（Gate），游戏服务器（Game），数据库服务器（DB）
网关服务器只处理游戏服务器的极个别消息，其余只负责转发。
*********************************************************************/

#ifndef __MSGDEF_H__
#define __MSGDEF_H__


//系统消息类型扩展，用于标识消息的收发方
enum SysMsgType
{
//c/s通讯
	ID_C2L = ID_USER_PACKET_ENUM + 11,		//客户端->登陆服务器
	ID_L2C,									//登陆服务器->客户端
	ID_C2G,									//客户端->游戏服务器
	ID_G2C,									//游戏服务器->客户端	
	ID_D2C,									//数据库服务器->游戏服务器->客户端	

//内部服务器通讯
	ID_G2L,									//游戏服务器->登陆服务器
	ID_L2G,									//登陆服务器->游戏服务器
	ID_G2D,									//游戏服务器->数据库服务器
	ID_D2G,									//数据库服务器->游戏服务器
	ID_GAME2GATE,							//游戏服务器->网关服务器

	ID_MAX
};

//通用消息类型
enum MsgType
{
	Msg_GameServer_Register,		//游戏服务器到网关服务器注册	
	Msg_Notify,						//消息通知对应NotifyType枚举
//登陆
	Msg_Version_Check,				//版本验证
	Msg_Regiser_Account,			//账号注册
	Msg_Temp_Login,					//游客登录
	Msg_Account_Login,				//登录账号
	Msg_ServerList,					//服务器列表   
	Msg_SelServer,					//选择服务器
//游戏 进入
	Msg_Random_RoleName,			//角色名随机获取
	Msg_Player_Enter_GameServer,	//玩家进入游戏服务器	
	Msg_Player_Leave_GameServer,	//玩家离开游戏服务器
	Msg_CreateRole,					//创建角色

	Msg_MAX
};


//通知消息类型
enum NotifyType
{
	//注册
	Notify_S2C_Regiser_NameLen,		//注册账号名长度不对
	Notify_S2C_Regiser_PasswordLen,	//注册账号密码长度不对
	Notify_S2C_Regiser_NameShield,	//注册账号名存在屏蔽字
	Notify_S2C_Regiser_NameRepeat,	//注册账号已存在
	Notify_S2C_Regiser_OK,			//注册成功
	//登陆
	Notify_S2C_Login_NamePassError,	//账号或密码错误
	Notify_S2C_Login_OK,			//登陆成功
	Notify_C2S_Login_ServerList,	//查询服务器列表
	Notify_S2C_Login_ServerNotOpen,	//服务器未开启
	Notify_S2C_Login_ReLogin,		//服务器要求客户端重新登陆
	Notify_S2C_Login_HeLogin,		//！账号在别处登陆
	//角色
	Notify_S2C_CreateRole,			//服务器要求客户端创建角色
	Notify_S2C_CreateRole_NameRepeat,//角色名已存在
	Notify_S2C_CreateRole_OK,		//角色创建成功

	Notify_MAX
};

//物品的操作类型(增、删、改)
enum ItemOperateType
{
    ItemOperate_LoginLoad,			//登录时加载
    ItemOperate_BagReset,			//整理背包
    ItemOperate_ItemUsed,			//消耗品使用
    ItemOperate_EquipGetOff,		//脱下装备
    ItemOperate_EquipUpLevel,		//装备强化
    ItemOperate_EquipUpUid,         //装备打造
    ItemOperate_EquipStone,         //装备镶嵌
    ItemOperate_EquipStoneRemove,	//拆除装备镶嵌宝石
    ItemOperate_ItemMerge,			//物品(宝石)合成
    ItemOperate_TaskReward,         //任务奖励
    ItemOperate_BuyMallItem,		//购买商城里面的物品
    ItemOperate_SellBagItem,		//出售背包里面的物品
    ItemOperate_BuyShopItem,		//npc商店购买
    
    ItemOperate_Max
};

#endif	//__MSGDEF_H__
