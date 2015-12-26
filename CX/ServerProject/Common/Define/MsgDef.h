/********************************************************************
created:	2014/7/15
author:		LL
desc:		定义协议号，及协议相关类型。。 客户端服务器共用此文件
//游戏客户端（Client），登陆服务器（Login），网关服务器（Gate），游戏服务器（Game），数据库服务器（DB）
网关服务器只处理游戏服务器的极个别消息，其余只负责转发。
*********************************************************************/

#ifndef __MSGDEF_H__
#define __MSGDEF_H__

#pragma warning(disable:4996)

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
	ID_GATE2GAME,							//网关服务器->游戏服务器
	ID_MAX
};

//通用消息类型
enum MsgType
{
	Msg_GameServer_Register,		//游戏服务器到网关服务器注册	
	Msg_Notify,						//消息通知对应NotifyType枚举
    Msg_GetGlobalGuid,              //获取全局guid
	
//登陆
	Msg_Version_Check = 10,			//版本验证
	Msg_Regiser_Account,			//账号注册
	Msg_Temp_Login,					//游客登录
	Msg_Account_Login,				//登录账号
	Msg_ServerList,					//服务器列表   
	Msg_SelServer,					//选择服务器
//游戏 进入
	Msg_Random_RoleName = 30,		//角色名随机获取
	Msg_Player_Enter_GameServer,	//玩家进入游戏服务器	
	Msg_Player_Leave_GameServer,	//玩家离开游戏服务器
	Msg_Player_Remove_GameAndDBServer,//玩家离线踢出服务器
	Msg_CreateRole,					//创建角色
	Msg_Role_BaseInfo,				//角色基础信息
    Msg_Role_DetailInfo,            //角色详细信息
    Msg_Role_EnterMap,              //角色进入地图
	Msg_Role_DetailAttribute,		//角色信息属性
    Msg_OtherRole_BaseInfo,         //地图其他角色基础信息
    Msg_OtherRole_DetailInfo,       //其他角色详细信息
    Msg_OtherRole_DetailInfoQuery,  //其他角色详细信息查询
    Msg_OtherRole_Leave,            //其他角色离开视野
	Msg_Role_BaseInfo_Update,		//修改玩家信息
//活动
	Msg_Chest = 60,					//抽箱子
//
//角色相关
	Msg_Object_Move = 80,		    //玩家移动
    Msg_Object_Move_Stop,           //玩家停止移动
    Msg_Object_Move_kick,           //玩家移动踢回
	Msg_Player_state,				//服务器同步玩家状态(表)
	Msg_Player_UpdateLevel,			//服务器同步玩家升级

    Msg_Object_SyncAttribute,		//>对象的属性
    Msg_Object_AddHp,               //>对象加血
    Msg_Object_DropHp,              //>对象掉血
    Msg_Object_BuffList,            //>对象buff队列
    Msg_Object_AddBuff,				//>对象添加buff效果
    Msg_Object_DelBuff,				//>玩家结束buff效果

//战斗相关
	Msg_Player_UseSkill = 100,		//玩家或怪物使用技能
	Msg_Player_ReliveData,			//服务器通知客户端原地复活所需要的信息，玩家死亡时若允许玩家原地复活发此协议，否则直接发Msg_Player_Relive
	Msg_Player_Relive,				//玩家复活
	Msg_Player_BuffHurt,			//负面buff产生的掉血

//怪物相关
	Msg_Monster_Baseinfo = 140,		//广播怪物信息，怪物出现在视野时广播，与其它玩家一样
	Msg_Monster_Move,				//广播怪物移动
	Msg_Monster_Del,				//广播移除怪物，用于怪物死亡和移除有时效性的怪物和
	Msg_Monster_Goto,				//广播传送怪物，用于出了追踪范围的怪物。

//物品相关
    Msg_Item_BagList = 200,         //背包清单
    Msg_Item_Add_Set,               //添加物品集合
    Msg_Item_Del_Set,               //删除物品集合
    Msg_Item_Updata_Set,            //更新物品集合
    Msg_Item_ShopBuy,               //商店购买
    Msg_Item_ShopSell,              //商店卖出
    Msg_Item_Compose,               //物品合成
    Msg_Item_UpStar,                //装备升星
    Msg_Item_Inherit,               //装备传承
    Msg_Item_Use,                   //物品使用
    Msg_Equip_List,                 //装备穿戴清单
    Msg_Equip_Update,               //装备穿戴更新
    Msg_Equip_ShowUpdate,           //装备变化显示更新

//技能相关
    Msg_Skill_List = 300,       //技能树清单
    Msg_Skill_Add,              //添加技能树
    Msg_SKill_Update,           //更新技能树
    Msg_Skill_Activate,         //修炼主技能树
    Msg_Skill_ChoseSkill,       //选择ui使用技能

//任务相关
	Msg_Task_Take = 340,			//接受任务
	Msg_Task_Make,
	Msg_Task_Submit,    			//完成任务
	Msg_Task_GetList,				//获得任务列表
	Msg_Task_Notify,				//任务的通知消息
//好友管理
	Msg_Add_Friends = 400,			//添加好友/黑名单
	Msg_Del_Friends,				//删除好友/黑名单
	Msg_Get_FriendsList,			//获得好友/黑名单列表
	Msg_Get_PlayInfo,				//获得好友/黑名单信息
	Msg_Fri_Notify,					//好友的通知消息

//队伍管理
	Msg_Team_Apply = 450,			//申请组队
	Msg_Team_ReplyApply,			//回复申请
    Msg_Team_Created,               //队伍成功创建
	Msg_Team_NewMember,				//加入新成员
	Msg_Team_MemberExit,		    //成员离开
	Msg_Team_ChangeLeader,			//队长更变
	Msg_Team_KickMemberReq,			//提出队伍
	Msg_Team_Notify,				//队伍的通知消息

//聊天管理
	Msg_Chat_POINT = 500,			//点对点聊天
	Msg_Chat_CHANNEL,				//频道消息
	Msg_Chat_Notify,				//聊天的通知消息

//帮派管理
	Msg_Create_Gang = 520,			//创建帮派
	Msg_Apply_Gang,					//申请加入帮派
	Msg_Reply_ApplyGang,			//回复申请入帮
	Msg_Get_GangApplyList,			//获得申请列表
	Msg_Leave_Gang,					//离开帮派
	Msg_Put_Gang,					//提出帮派
	Msg_Transfer_GangHeader,		//转让帮主
	Msg_Get_GangList,				//获得帮派列表
	Msg_Get_GangAnnounce,
	Msg_Edit_GangAnnounce,
	Msg_Send_GangEmail,
	Msg_Add_GangContribution,
	Msg_Get_GangContribution,
	Msg_Gang_Appoint,
	Msg_Gang_Transfer,
	Msg_Gang_UpGrade,
	Msg_Get_GangLevel,
	Msg_Get_GangInfo,
	Msg_Get_ApplyInfo,

	Msg_Gang_Notify,				//帮派的通知消息

//副本相关
	Msg_Apply_Copy = 600,			// 申请进入副本
	Msg_Copy_Finish,				// 副本完成
	Msg_Player_AttackType,			//战斗状态 临时放这

	Msg_Task_Collect,				//采集任务

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
	//角色
	Notify_S2C_CreateRole,			//服务器要求客户端创建角色
	Notify_S2C_CreateRole_NameRepeat,//角色名已存在
	Notify_S2C_CreateRole_OK,		//角色创建成功
    Notify_S2C_RoleExist,           //已经创建过角色了
    Notify_S2C_PlayerOffline,       //玩家不在线
	//任务
	Notify_S2C_TakeTask_OK = 30,			//任务接取成功 
	Notify_S2C_TakeTask_LevelError,	//等级达不到接取任务条件
	Notify_S2C_TakeTask_RealmError, //接取任务境界错误
	Notify_S2C_TakeTask_TaskExists, //任务已经接过
	Notify_S2C_TakeTask_ReqTaskError,//接取任务前置条件错误
	Notify_S2C_TaskFinish,			//任务已经完成 
	Notify_S2C_TakeTask_NoTask,  	//任务不存在
	Notify_S2C_TakeTask_ItemError,	//物品错误
	NotiFy_S2C_Task_BagFull,		//背包已满
	Notify_S2C_MakeTask_ItemError,	//物品错误
	NotiFy_S2C_MakeTask_DisNpcError,
	Notify_S2C_SubmitTaskError,		//提交任务失败
	Notify_S2C_SubmitTaskFinish,	//提交任务成功
	Notify_S2C_GetTaskList_OK,		//获取任务列表成功

	//副本
	Notify_S2C_ApplyCopy_TimeError = 50,//未到副本开启时间
	Notify_S2C_ApplyCopy_LevelError,//等级未达到副本要求
	Notify_S2C_ApplyCopy_EnterNumError,//超过进入次数
	Notify_S2C_ApplyCopy_PlayerNumError,//人数未达到进入条件
	Notify_S2C_ApplyCopy_ItemError,	//未发现进入副本所需要的道具
	Notify_S2C_ApplyCopy_OK,		//进入副本成功
	Notify_S2C_Copy_FinishANDNext,	//爬塔 进入下一层
	Notify_S2C_Copy_Finish,			//副本完成

	//货币提示
	Notify_S2C_No_Money,
	Notify_S2C_No_Gold,

    //物品
    Notify_S2C_EquipmentUpStar_Failture = 80,//物品升星失败.
    Notify_S2C_ItemUse_RepeatedAddSkillTree,//技能树重复添加.
    Notify_S2C_ItemUse_SkillTreeExpFull,    //技能树经验上限
    Notify_S2C_ItemUse_CD_Cooling,          //物品使用CD冷却中         

	//好友管理
	Notify_S2C_ADDFRI_OK = 100,			//好友添加成功
	Notify_S2C_ADDEDFRI_OK,				//好友被添加成功
	Notify_S2C_ADDBLACK_OK,				//黑名单添加成功
	Notify_S2C_ADDFRI_FAILURE,			//好友添加失败
	Notify_S2C_ADDBLACK_FAILURE,		//黑名单添加失败
	Notify_S2C_FRINULL,					//人物ID错误
	Notify_S2C_DELFRI_OK,				//删除好友成功
	Notify_S2C_DELBLACK_OK,				//删除黑名单成功
	Notify_S2C_DELFRI_FAILURE,			//删除好友失败
	Notify_S2C_DELBLACK_FAILURE,		//删除黑名单失败
	Notify_S2C_GETFRILIST_OK,			//获取好友列表成功
	Notify_S2C_GETBLACKLIST_OK,			//获取黑名单成功
	Notify_S2C_GETFRI_OK,				//获取好友信息成功

	//队伍管理
	Notify_S2C_CRATE_OK = 150,			//创建队伍成功
	Notify_S2C_APPLY_OK,				//申请队伍成功
	Notify_S2C_APPLY_HEADER,			//提示队长，有人申请加队
	Notify_S2C_APPLY_FAILURE,			//申请队伍失败（对方不是队长）
	Notify_S2C_INVITE_OK,				//邀请玩家成功
	Notify_S2C_INVITE_HEADER,			//提示玩家，有人邀请加队
	Notify_S2C_INVITE_FAILURE,			//邀请失败（对方已是队长）
	Notify_S2C_TeamFull,				//队伍人数已满
	Notify_S2C_ADDTEAM_OK_HEADER,		//加队成功_队长
	Notify_S2C_ADDTEAM_OK,				//加队成功_队员
	Notify_S2C_ADDTEAM_FAILURE_HEADER,	//加队失败_队长
	Notify_S2C_ADDTEAM_FAILURE,			//加队失败_队员
	Notify_S2C_LEAVETEAM_HEADER,		//离开队伍（队长）
	Notify_S2C_LEAVETEAM,				//离开队伍（队员）
	Notify_S2C_DISSOLVE,				//解散队伍
	Notify_S2C_PUTTEAM_HEADER,			//提出队伍（队长）
	Notify_S2C_PUTTEAM,					//提出队伍（队员）
	Notify_S2C_GET_TEAMLIST,			//获得队伍列表

	//聊天管理
	Notify_S2C_Recive_Point = 200,		//接收点消息
	Notify_S2C_Recive_Channel,			//接收频道消息

	//帮派管理
	Notity_S2C_Create_OK = 210,			//创建成功
	Notity_S2C_Create_Fail_InGang,		//创建失败，已存在帮派之中
	Notity_S2C_Create_Fail_RepeatName,	//创建失败，帮派重名
	Notity_S2C_Get_GangInfo,			//获得帮会信息成功
	Notity_S2C_Apply_OK,				//申请成功
	Notity_S2C_Apply_Fail_InGang,		//申请失败
	Notity_S2C_Apply_Fail_NoGang,		//帮派不存在
	Notity_S2C_Apply_Fail_Repeat,		//申请重复
	Notity_S2C_Reply_OK,				//申请成功
	Notity_S2C_Reply_Refused,			//申请拒绝
	Notity_S2C_Reply_OK_INGang,			//申请成功,但对方已经有帮派
	Notity_S2C_GETAPPLYLIST,			//获得帮派申请列表
	Notity_S2C_LEAVE_GANG,				//离开帮派
	Notity_S2C_LEAVE_GANG_HEADER,		//帮主离开帮派
	Notity_S2C_PUT_GANG,				//被提出帮派
	Notity_S2C_GET_GANGLIST,			//获得帮派列表
	Notity_S2C_GET_Announce,
	Notity_S2C_EDIT_Announce_OK,
	Notity_S2C_TransferHeader_OK,		//转让帮主成功

	Notify_S2C_MakeTask_OK,

	Notify_MAX
};

//物品的操作类型(增、删、改)
enum ItemOperateType
{
    ItemOperate_BagLoading,			//登录时加载
    ItemOperate_EquipLoading,       //穿戴装备加载
    ItemOperate_BagReset,			//整理背包
    ItemOperate_ItemUsed,			//消耗品使用
    ItemOperate_WearEquipment,      //穿上装备
    ItemOperate_EquipGetOff,		//脱下装备
    ItemOperate_EquipUpLevel,		//装备强化
    ItemOperate_EquipUpUid,         //装备打造
    ItemOperate_EquipStone,         //装备镶嵌
    ItemOperate_EquipStoneRemove,	//拆除装备镶嵌宝石
    ItemOperate_ItemMerge,			//物品(宝石)合成
    ItemOperate_TaskReward,         //任务奖励
	ItemOpreate_TakeTaskItem,		//接取任务时的任务物品
	ItemOperate_DoTakeItem,			//交付任务物品
    ItemOperate_BuyMallItem,		//购买商城里面的物品
    ItemOperate_SellBagItem,		//出售背包里面的物品
    ItemOperate_BuyShopItem,		//npc商店购买
    ItemOperate_EquipInherite,      //装备传承
	ItemOperate_ItemDrop,			//战斗掉落
	ItemOperate_BuyChest,			//购买抽箱子
	ItemOperate_Chest,				//抽箱子送

    ItemOperate_Max
};
//装备属性类型
enum EquipmentAttributeType
{
	Attribute_Equipment_HpMaxValue = 1,		//装备增加的HP上限值
	Attribute_Equipment_HpMaxPercent,		//装备增加的HP上限百分比
	Attribute_Equipment_MpMaxValue,			//装备增加的ＭＰ上限值
	Attribute_Equipment_MpMaxPercent,		//装备增加的MP上限百分比
	Attribute_Equipment_AttackValue,		//装备增加的攻击力
	Attribute_Equipment_AttackPercent,		//装备增加的攻击力百分比
	Attribute_Equipment_DefenseValue,		//装备增加的防御力
	Attribute_Equipment_DefensePercent,		//装备增加的防御力百分比
	Attribute_Equipment_HitValue,			//装备增加的命中值
	Attribute_Equipment_HitPercent,			//装备增加的命中百分比
	Attribute_Equipment_DodgeValue,			//装备增加的闪避值
	Attribute_Equipment_DodgePercent,		//装备增加的闪避百分比
	Attribute_Equipment_CritValue,			//装备增加的暴击值
	Attribute_Equipment_CritPercent,		//装备增加的暴击百分比
	Attribute_Equipment_TenaCityValue,		//装备增加的韧性值
	Attribute_Equipment_TenaCityPercent,	//装备增加的韧性百分比
	Attribute_Equipment_HitRatio,			//装备增加的命中率
	Attribute_Equipment_DodgeRatio,			//装备增加的闪避率
	Attribute_Equipment_CritRatio,			//装备增加的暴击率
	Attribute_Equipment_TenaCityRatio,		//装备增加的韧性率
	Attribute_Equipment_MoveSpeedOdds,		//装备增加的移动系数
	Attribute_Equipment_HurtStateOdds,		//装备增加的伤害状态系数
	Attribute_Equipment_AttackStateOdds,	//装备增加的攻击状态系数
	Attribute_Equipment_TreatStateOdds,		//装备增加的治疗状态系数
	Attribute_Equipment_DoubleCritOdds		//装备增加的暴击翻倍系数

};

#endif	//__MSGDEF_H__
