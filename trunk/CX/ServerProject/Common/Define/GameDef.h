/********************************************************************
created:	2014/12/21
author:		LL
desc:		用于定义游戏中的基础类型及数值限定。客户端服务器共用此文件。
*********************************************************************/

#ifndef __GAMEDEF_H__
#define __GAMEDEF_H__

#if defined(_MSC_VER)
    typedef __int8     int8;
    typedef __int16    int16;
    typedef __int32    int32;
    typedef __int64    int64;

    typedef unsigned __int8     uint8;
    typedef unsigned __int16    uint16;
    typedef unsigned __int32    uint32;
    typedef unsigned __int64    uint64;
#else
    typedef char       int8;
    typedef short      int16;
    typedef int        int32;
    typedef long long  int64;

    typedef unsigned char       uint8;
    typedef unsigned short      uint16;
    typedef unsigned int        uint32;
    typedef unsigned long long  uint64; 
#endif

//安全释放指针函数
#define SAFE_DELETE(p)			{ if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p); (p) = NULL; } }

//直线移动速度
#define STRAIGHT_MOVE_SPEED 0.35f
//斜线移动速度
#define SLASH_MOVE_SPEED 0.5f
//单位移动速度(像素/每秒)
#define UNIT_MOVE_SPEED  192.0f
//一个格子像素大小
#define MAP_CELL_SIZE  64.0f
//玩家视野范围十个格子
#define PLAYER_VIEW_RANGE 768.0f

//寻找攻击目标最大距离（格）
#define FINDTARGETMAXDISTANCE 10

#define MIN_NAME_SIZE 6			//名字最小长度
#define MAX_NAME_SIZE 32		//名字最大长度

#define MAX_DESC_SIZE 4096		//描述字段最大长度

#define MIN_PASS_SIZE 6			//密码最小长度
#define MAX_PASS_SIZE 32		//密码最大长度

#define INVALID_ID  0           //无效的ID/TID(类型id)

#define	Monter_Att_Num	11		//怪物的属性数量

#define MAX_CHOSESKILL 8		//玩家可选择使用的技能最大数量


#define SKILL_ID_TO_TYPE(x)  ((x)/1000) 
#define SKILL_TYPE_AND_LEVEL_TO_ID(x, y) ((x * 1000) + y)
#define BAG_ITEMS_STREAM_SZIE     40960

enum RoleSex
{
    Sex_Woman,
    Sex_Man
};

//属性类型
enum AttributeType
{
	ATTRIBUTE_MONEY = 1,		//金币数(游戏币) v  --> v为值,p为百分比
	ATTRIBUTE_HPMAX,			//HP上限         v
	ATTRIBUTE_MPMAX,			//MP上限         v
	ATTRIBUTE_ATTACK,		    //攻击力 v
	ATTRIBUTE_DEFENSE,			//防御力 v
	ATTRIBUTE_HIT,				//命中值 v
	ATTRIBUTE_DODGE,			//闪避值 v
    ATTRIBUTE_TENACITY,         //韧性值 v
	ATTRIBUTE_CRIT,				//暴击值 v
	ATTRIBUTE_HP,				//HP v
	ATTRIBUTE_MP,				//MP v
	ATTRIBUTE_GOLD,				//元宝 v
	ATTRIBUTE_BINDGOLD,			//绑定元宝 v
	ATTRIBUTE_ARENAMONEY,		//竞技场货币数 v
	ATTRIBUTE_BATTLEFIELDMONEY,	//战场货币数 v
	ATTRIBUTE_XPMAX,			//XP值上限 v
	ATTRIBUTE_XP,				//XP值 v
	ATTRIBUTE_LEVEL,			//人物等级 v
	ATTRIBUTE_EXP,				//人物经验 v
	ATTRIBUTE_REALMLEVEL,		//境界等级 v
	ATTRIBUTE_REALMEXP,			//境界经验 v

    ATTRIBUTE_MOVESPEED_FACTOR,	//移动速度系数 p
    ATTRIBUTE_HUR_FACTOR,		//伤害状态系数 p
    ATTRIBUTE_ATK_FACTOR,       //攻击状态系数 p
    ATTRIBUTE_CURE_FACTOR,      //治疗状态系数 p
    ATTRIBUTE_CRIT_PROB,        //暴击率 p
    ATTRIBUTE_TENACITY_PROB,    //韧性率 p
    ATTRIBUTE_HIT_PROB,         //命中率 p
    ATTRIBUTE_DODGE_PROB,       //闪避率 p
    ATTRIBUTE_CRIT_FACTOR,      //暴击翻倍系数 p

	ATTRIBUTE_MAX
};
//属性ODDS初始值得
enum AttributeOdds
{
	ATTRIBUTE_RATIO_HIT_PROB = 1,		    //玩家初始命中率
	ATTRIBUTE_RATIO_DODGE_PROB,			    //玩家初始闪避率
	ATTRIBUTE_RATIO_CRIT_PROB,			    //玩家初始暴击率
	ATTRIBUTE_RATIO_TENACITY_PROB,		    //玩家初始韧性率

	ATTRIBUTE_RATIO_MOVESPEED_FACTOR,	    //玩家初始移动系数
    ATTRIBUTE_RATIO_HUR_FACTOR,             //玩家初始伤害状态系数
    ATTRIBUTE_RATIO_ATK_FACTOR,             //玩家初始攻击状态系数 
    ATTRIBUTE_RATIO_CURE_FACTOR,            //玩家初始治疗系数
    ATTRIBUTE_RATIO_CRIT_FACTOR,            //玩家初始暴击翻倍系数 

    ATTRIBUTE_RATIO_MONSTER_MOVESPEED_FACTOR,	//怪物初始移动系数
    ATTRIBUTE_RATIO_MONSTER_HUR_FACTOR,         //怪物初始伤害状态系数
    ATTRIBUTE_RATIO_MONSTER_ATK_FACTOR,         //怪物初始攻击状态系数 
    ATTRIBUTE_RATIO_MONSTER_CURE_FACTOR,        //怪物初始治疗系数
    ATTRIBUTE_RATIO_MONSTER_CRIT_FACTIR,        //怪物初始暴击翻倍系数 p

	ATTRIBUTE_RATIO_UPSTARUPINIT,				//升星强化初始系数
	ATTRIBUTE_RATIO_UPSTARUPADJUST1,			//升星强化调节系数1
	ATTRIBUTE_RATIO_UPSTARUPADJUST2,			//升星强化调节系数2
    ATTRIBUTE_RATIO_UPSTARUPDATE,				//升星强化修正系数
    ATTRIBUTE_RATIO_COMPOSEEXPEND,				//合成消耗系数
    ATTRIBUTE_RATIO_INHERITUPDATE,				//传承修正系数
    ATTRIBUTE_RATIO_INHERITEXPEND,				//传承消耗系数

    ATTRIBUTE_RATIO_HP_PUBLIC_CD,				//红药公共cd（s）
    ATTRIBUTE_RATIO_MP_PUBLIC_CD,				//蓝药公共cd（s）
    ATTRIBUTE_RATIO_SKILL_PUBLIC_CD,			//技能公共CD（S）

	ATTRIBUTE_RATIO_MAX
};
enum ObjectType
{
    OBJECT_NONE,        //空
	OBJECT_NPC,			//NPC
	OBJECT_PLAYER,		//玩家
	OBJECT_MONSTER,		//怪物
	OBJECT_RESOURCE		//采集资源
};

enum MostnerType
{
    MONSTER_NORMAL,
    MONSTER_BOSS
};

//物品分类
enum ItemType
{
	ITEMTYPE_EQUIP,		//装备
	ITEMTYPE_EXPEND,	//消耗品
	ITEMTYPE_MATERIAL,	//材料
	ITEMTYPE_WINGPIECE,	//翅膀碎片
	ITEMTYPE_TASK,		//任务物品
};

//物品2级分类
//装备分类
enum EquipType
{	
	EQUIPTYPE_HEAD,		//头饰
	EQUIPTYPE_BODY,		//衣服
	EQUIPTYPE_WEAPON,	//武器
	EQUIPTYPE_WING,		//翅膀
	EQUIPTYPE_HORSE,	//坐骑
	EQUIPTYPE_BELT,		//腰带
	EQUIPTYPE_SHOES,	//鞋子
	EQUIPTYPE_NECKLACE,	//项链
	EQUIPTYPE_RING,		//戒指
	EQUIPTYPE_ORNAMENTS,//饰品
	EQUIPTYPE_FASHION ,	//时装
	EQUIPTYPE_MAGICWEAPON,//法宝

	EQUIPTYPE_MAX
};

//消耗品分类
enum ExpendType
{
	EXPENDTYPE_CONTINUED,			//持续回血药
	EXPENDTYPE_MOMENT,				//瞬间回血药
	EXPENDTYPE_CONTINUEDMP,			//持续回蓝药
	EXPENDTYPE_MOMENTMP,			//瞬间回蓝药
	EXPENDTYPE_BUFF,				//获得BUFF
	EXPENDTYPE_EXP,					//人物经验丹
	EXPENDTYPE_SKILLTREEEXP,		//功法经验丹
	EXPENDTYPE_MAGICWEAPONEXP,		//法宝经验丹
	EXPENDTYPE_RealmExp,			//境界经验丹
	EXPENDTYPE_SKILLBOOK,			//功法书
	EXPENDTYPE_MAGICWEAPONBOOK,		//法宝书
	EXPENDTYPE_SPIRITSBOOK,			//器灵书
	EXPENDTYPE_PACKAGE,				//礼包
	EXPENDTYPE_MONEY,				//游戏币兑换
	EXPENDTYPE_GOLD,				//充值货币兑换
	EXPENDTYPE_BINDGOLD,			//绑定货币兑换
	EXPENDTYPE_ARENAMONEY,			//竞技场货币兑换
	EXPENDTYPE_BATTLEFIELDMONEY,	//战场货币兑换

	EXPENDTYPE_MAX
};

//材料分类
enum MaterialType
{
	MATERIALTYPE_REALMODDS,	//境界概率
	MATERIALTYPE_STARODDS,	//升星概率	
	MATERIALTYPE_REALM,		//境界突破材料
	MATERIALTYPE_STARM,		//装备升星材料
	MATERIALTYPE_WORLDCHAT,	//世界喊话道具

	MATERIALTYPE_MAX
};

//翅膀碎片分类
enum WingPieceType
{
	WINGPIECETYPE_0,	//翅膀孔位0
	WINGPIECETYPE_1,	//翅膀孔位1
	WINGPIECETYPE_2,	//翅膀孔位2
	WINGPIECETYPE_3,	//翅膀孔位3
	WINGPIECETYPE_4,	//翅膀孔位4
	WINGPIECETYPE_5,	//翅膀孔位5
	WINGPIECETYPE_6,	//翅膀孔位6
	WINGPIECETYPE_7,	//翅膀孔位7
	WINGPIECETYPE_8,	//翅膀孔位8
	WINGPIECETYPE_9,	//翅膀孔位9

	WINGPIECETYPE_MAX,			   
};

//颜色品质
enum QualityType
{
	QUALITY_WHITE,	//白色
	QUALITY_GREEN,	//绿色
	QUALITY_BLUE,	//蓝色
	QUALITY_PURPLE,	//紫色
	QUALITY_ORANGE,	//橙色

	QUALITY_MAX
};

//地图类型
enum MapType
{
	MAPTYPE_NORMAL = 1,	//场景地图
	MAPTYPE_BOOS,		//世界boos地图
	MAPTYPE_COPY,		//副本地图
	MAPTYPE_PVP,		//竞技场地图
	MAPTYPE_BATTLEFIELD,//战场地图

	MAPTYPE_MAX
};

//地图pk类型
enum MapPKType
{
	MAPPKTYPE_PEACE = 1,	//和平地图
	MAPPKTYPE_TEAM,			//组队战地图
	MAPPKTYPE_GUILD,		//公会战地图

	MAPPKTYPE_MAX,
};

//副本类型
enum CopyType
{
	COPY_SELF,				//单人副本
	COPY_TEAM,				//组队副本（5人）
	COPY_GUILD,				//公会战地图
	COPY_TOWER,				//爬塔副本

	COPY_MAX
};

//攻击类型
enum AttackType
{
	AttackType_NORRMAL,		//普通
	AttackType_CRIT,		//暴击
	AttackType_MISS,		//闪避
	AttackType_SUNDER,		//破甲
	AttackType_PENETRATE,	//穿透
	AttackType_NOHIT,		//未命中
	AttackType_BUFF			//BUFF,不掉血
};

//任务类型
enum TaskType
{
	TASK_TYPE_MAIN,		//主线任务
	TASK_TYPE_BRANCH,	//支线任务
	TASK_TYPE_DAILY,	//日常任务
	TASK_TYPE_FACTION,	//公会任务

	TASK_TYPE_MAX
};

//任务完成条件
enum TaskTermsType
{
	TASK_TERMS_TALK,		//对话
	TASK_TERMS_KILLMONSTER,	//杀怪
	TASK_TERMS_USEITEM,		//使用道具
	TASK_TERMS_DELIVERYITEM,//交付物品
	TASK_TERMS_GETITEM,		//物品收集
	TASK_TERMS_COPY,		//通关副本
	TASK_TERMS_ESCORT,		//护送
	TASK_TERMS_COLLECT,		//物品采集
	TASK_TERMS_ATTRIBUTE,	//属性提升（经验、技能、战斗力、等级等）

	TASK_TERMS_MAX
};

enum TaskState
{
	TASK_STATE_NOTASK,		//任务未接
	TASK_STATE_HAVETASK,	//任务已接未完成
	TASK_STATE_SUBMITTASK,	//任务完成未交付
	TASK_STATE_FINISHTASK,	//任务完成已交付

	TASK_STATE_MAX
};

//技能种类
enum SkillClass
{
	SKILL_CLASS_SKILL = 1,		//技能
	SKILL_CLASS_NORMALATTACK,	//普通攻击
	SKILL_CLASS_XPSKILL,		//XP技能
	SKILL_CLASS_PASSIVESKILL,	//被动技能

	SKILL_CLASS_MAX
};

//技能类型
enum SkillType
{
	SKILL_TYPE_NORMAL = 1,	//普通类型
	SKILL_TYPE_POSMOVE,		//位移
	SKILL_TYPE_DISPEL,		//驱散负面效
	SKILL_TYPE_POUNCE,		//冲锋
	SKILL_TYPE_BACKSTAB,	//背刺
	SKILL_TYPE_TAUNT,		//嘲讽

	SKILL_TYPE_MAX
};

//技能伤害类型
enum SkillHurtType
{
	SKILL_HURT_PHYSICS = 1,	//物理攻击
	SKILL_HURT_MAGICE,		//魔法攻击
	SKILL_HURT_TREAT,		//治疗
	SKILL_HURT_BUFF,		//BUFF

	SKILL_HURT_MAX
};

//玩家操作枚举
enum PlayerState_Operation 
{
	Oper_Move,				// 进入寻路
	Oper_Hang,				// 挂机
	Oper_MountHorse,		// 上马
	Oper_Dismount,			// 下马
	Oper_TriggerNPC,		// 触发NPC
	Oper_ChangeMap,			// 地图跳转
	Oper_Attack,			// 攻击
	Oper_Skill,				// 技能
	Oper_XPSkill,			// XP技能
	Oper_ChangeSkill,		// 切换技能
	Oper_UseMedicine,		// 使用药品
	Oper_ChangeEqu,			// 更换装备
	Oper_Team,				// 组队
	Oper_Collect,			// 采集
	Oper_ArenaEnter,		// 竞技场进入
	Oper_BattlefieldLine,	// 战场排队
	Oper_BattlefieldEnter,	// 战场加入
	Oper_CopyLine,			// 副本排队
	Oper_CopyEnter,			// 副本进入
	Oper_Max
};

//玩家状态枚举
enum PlayerState_States 
{	
	State_Moving,			// 寻路状态(移动状态)
	State_ChangeMap,		// 切图保护
	State_Die,				// 死亡状态
	State_Fight,			// 战斗状态	
	State_Riding,			// 骑乘状态
	State_Team,				// 组队状态
	State_BattlefieldLine,	// 战场排队状态
	State_CopyLine,			// 副本排队状态
	State_Collect,			// 采集状态
	State_Hang,				// 挂机状态
	State_Arena,			// 竞技场状态
	State_Battlefield,		// 战场状态
	State_Copy,				// 副本状态
	State_Silence,			// 沉默状态
	State_Nomove,			// 定身状态
	State_Vertigo,			// 眩晕状态
	State_Invincible,		// 无敌状态

	State_Max
};

//玩家关系
enum Role_Relation
{
	Relation_Friends = 0,
	Relation_BlackList = 1,

	Relation_Max,
};

//聊天频道
enum Chat_Channel
{
	Channel_World = 1,      //>世界频道
    Channel_SingleChat,     //>私聊频道
	Channel_Near,           //>附近频道
	Channel_Guild,          //>帮派频道
    Channel_Team,           //>队伍频道
    Channel_System,         //>系统频道
};

//>技能功能
enum SkillFunction
{
    Skill_Damage = 1,       //>伤害
    Skill_Cure,             //>治疗
    Skill_BeHurtAddBuff,    //>受击添加buff
    Skill_QuChuYouYi,       //>会计算伤害
    Skill_JianShe,          //>溅射
    Skill_FanTanBuff,       //>反弹buff
    Skill_ChaoFeng,         //>嘲讽
    Skill_XiXue,            //>吸血
    Skill_ZhenShiShangHai,  //>真实伤害, 此次攻击将目标的防御和闪避清0，再计算伤害（算暴击的）
    Skill_ChongSheng,       //>重生
    Skill_MianYiKongZhi,    //>免疫控制
};

//>技能触发
enum SkillTriggerType
{
    SklTrg_NormalAttack = 1,    //>普通攻击触发
    SklTrg_BeHurt,          //>技能受击
    SklTrg_HpPercent,       //>Hp百分比触发
    SklTrg_AddBuffType,     //>添加制定类型buff触发
};

enum BuffType
{
    BuffType_YouYi = 1,
    BuffType_YouHai = 2,
    BuffType_KongZhi = 3,
};

//>所有buff的伤害,不在造成被动技能触发
enum BuffFunctionType
{
    Buff_ChenMo = 1,//>沉默, 不释放主动技能
    Buff_FengXue,   //>封穴, 不触发绝技,
    Buff_XuanYun,   //>眩晕, 不能攻击,不能移动
    Buff_HunShui,   //>昏睡, 不能攻击,不能移动, 能被打醒
    Buff_HunLuan,   //>混乱, 随机攻击非自己的任意目标, 不受玩家控制
    Buff_Wudi,      //>无敌, 不受任意伤害, 不受有害buff.不受控制buff

    Buff_DisableAddBuff,//>免疫, 不受指定类型的buff
    Buff_HuDun,         //>护盾,吸收一定伤害数值
    Buff_HuDunPct,      //>护盾,吸收自身血量上限百分比数值
    Buff_HuDunMp2Hp,    //>护盾,将百分之x的mp转护盾值
    Buff_DamagePctToHp, //>吸血,伤害值的百分比变成hp
    Buff_FanShang,      //>反伤,承受伤害值的百分比

    Buff_HuoQiang,      //>火墙
    Buff_ZhaoHuang,     //>召唤

    Buff_DropHp,        //>持续掉血(固定值)
    Buff_DropCurHpPct,  //>持续掉血(当前血量百分比)
    Buff_DropHpMaxPct,  //>持续掉血(最大血量百分比)

    Buff_AddHpMax,      //>添加Hp上限值
    Buff_AddHpMaxPct,   //>添加Hp上限百分比

    Buff_AddMpMax,      //>添加Mp上限
    Buff_AddMpMaxPct,   //>添加Mp上限百分比

    Buff_AddMoveSpeed,      //>添加移动速度

    Buff_AddAttackPower,    //>添加攻击力
    Buff_AddAttackPowerPct, //>添加攻击力百分比
    Buff_AddDefencePower,   //>添加防御力
    Buff_AddDefencePowerPct,//>添加防御力百分比
    Buff_AddCrit,           //>添加暴击值
    Buff_AddCritPct,        //>添加暴击值百分比
    Buff_AddHit,            //>添加命中值
    Buff_AddHitPct,         //>添加命中值百分比
    Buff_AddTenacity,       //>添加韧性值
    Buff_AddTenacityPct,    //>添加韧性值百分比
    Buff_AddDodge,          //>添加闪避值
    Buff_AddDodgePct,       //>添加闪避值百分比

    Buff_AddAtkFactor,      //>添加攻击系数
    Buff_AddDefFactor,      //>添加防御系数
    Buff_AddCritFactor,     //>添加暴击系数
};


enum SkillTargetType
{
    SklTag_SingleTarget = 1,   //>单个目标   
    SklTag_OnlySelf,           //>自己
    SklTag_EnemyInSelfRang,    //>以自己为中心的敌对目标
    SklTag_TeamPlayerInSelfRang, //>以自己为中心的队伍成员
    SklTag_EnemyInTargetRang,  //>以目标为中心的队伍成员
};

/////////////////////////////////////////////////////////////////////////////////

//>技能信息(数据库保存)
struct SkillInfoIt
{
    int skillType;      //>技能类型
    int level;          //>技能等级
};

//翅膀信息(数据库保存)
struct WingInfoIt
{
	int id;				//翅膀孔ID 
	int level;			//翅膀孔等级 
	int exp;			//翅膀孔经验
};

//法宝信息(数据库保存)
struct MagicWeaponInfoIt
{
	int id;				//法宝ID
	int level;			//法宝等级
	int exp;			//法宝经验
	int growthPlus;		//法宝成长系数
};

//器灵信息(数据库保存)
struct MountInfoIt
{
	int id;				//器灵ID 
	int time;			//创建时间
};

//副本信息(数据库保存)
struct CopyInfoIt
{
	int id;				//副本id	
	int enterNum;		//进入次数
	int layer;			//层数
};

//日常信息(数据库保存)
struct DailyInfoIt
{
	int id;				//日常id	
	int progress;		//完成进度
	bool isReward;		//是否领奖
};

//任务信息
struct TaskInfoIt
{
	int id;				//任务id	
	int progress;		//完成进度
	int state;			//任务状态，0未接 1已接 2可交付 3已完成
};

//道具信息(数据库保存)
struct ItemInfoIt
{
	uint64 guid;		//唯一id	
	int id;			    //物品ID（模板）
	int prefixID;		//前缀ID
	int starNum;		//星数
	bool isBind;		//是否绑定
	int num;			//数量
	bool isLock;        //是否锁定
    int time;			//创建时间
};

//玩家基础信息
struct BaseInfoIt
{
	uint64 playerID;	//玩家id用账号id来填充
	char playerName[MAX_NAME_SIZE];	//名字
	int8 sex;					//性别 0女 1男	
	int16 headID;				//头像ID
	int64 offlineTime;		//离线时间，用于上线时判断是否刷新每日次数
	int32 level;				//等级
	int32 exp;				//经验值
	int32 money;				//当前金币数
	int32 gold;				//元宝数量
	int32 bindGold;			//绑定元宝数量
	int32 rechargGoldSum;		//总充值元宝
	int32 arenaScore;			//竞技场积分
	int32 arenaMoney;			//竞技场货币数	
	int32 battlefieldScore;	//当前战场积分	
	int32 battlefieldMoney;	//战场货币数
	int32 mapID;				//所在地图id
	int32 posx;				//所在位置x
	int32 posy;				//所在位置y
	int32 hp;					//血
    int32 hpMax;                //血上限
	int32 mp;					//蓝
    int32 mpMax;                //蓝上限
	int32 xp;					//怒气
    int32 xpMax;                //怒气上限
	int64 societyID;			//公会id
	int32 realmLevel;			//境界等级	
	int32 realmExp;			//境界经验
	int32 activeSkillType;  //所修炼的技能树类型 
};

struct DetailAttribute
{
	int32 hpMax;			//HP上限         v					
	int32 mpMax;			//MP上限         v
	int32 attack;		    //攻击力 v
	int32 defense;			//防御力 v
	int32 hit;				//命中值 v
	int32 dodge;			//闪避值 v
	int32 crit;				//暴击值 v
	int32 tenacity;			//韧性值 v
    int32 moveSpeedFactor;  //移动速度系数   v
    float hitProb;          //命中率
    float critProb;         //暴击率
	int32 battlePower;		//战斗力
};

struct GuidInfoIt
{
	uint64 itemId;
};

struct GangMember
{
	int64		id;							//人物ID
	char		szName[MAX_NAME_SIZE];		//人物名称
	int32		iLevel;						//等级
	char		szApplyTime[50];			//申请时间
	char		szPosition[MAX_NAME_SIZE];	//职务
	int32		iContribution;				//贡献
	char		szLeaveTime[50];			//离开时间
};

struct FriendsInfo
{
	uint64	roleId;					//角色ID
	char	szName[MAX_NAME_SIZE];	//好友名称
	bool	bIsLine;				//是否在线
	int		iLevel;					//等级
	int		iFight;					//战斗力
};

struct AddHpMode
{
    enum Mode
    {//>吃红药,治疗技能,Buff吸血,吸血,重生
        EatRad = 1,
        SkillCure,
        XiXueBuff,
        XiXue,
        Relive,
    };

    AddHpMode() {}
    AddHpMode(int hp, Mode mode){ this->hp = hp; this->mode = mode; }

    int32   hp;
    Mode    mode;
};

struct DropHpMode
{
    enum Mode
    {//>普通,暴击,溅射,闪避,护盾消除buff,无敌免疫buff,反伤buff,持续掉血Buff
        Normal = 1,
        Critical,
        JianShe,
        Dodge,
        HuDunXiShouBuff,
        WuDiMianYiBuff,
        DamageInverseBuff,
        DropHpBuff
    };

    DropHpMode(){}
    DropHpMode(uint64 attacker, int32 effectId, int32 hp, Mode mode){ this->attacker = attacker; this->effectId = effectId; this->hp = hp; this->mode = mode; }

    uint64  attacker;
    int32   effectId;
    int32   hp;
    Mode    mode;
};

#endif	//__GAMEDEF_H__