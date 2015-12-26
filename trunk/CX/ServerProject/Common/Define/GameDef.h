/********************************************************************
created:	2014/12/21
author:		LL
desc:		���ڶ�����Ϸ�еĻ������ͼ���ֵ�޶����ͻ��˷��������ô��ļ���
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

//��ȫ�ͷ�ָ�뺯��
#define SAFE_DELETE(p)			{ if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p); (p) = NULL; } }

//ֱ���ƶ��ٶ�
#define STRAIGHT_MOVE_SPEED 0.35f
//б���ƶ��ٶ�
#define SLASH_MOVE_SPEED 0.5f
//��λ�ƶ��ٶ�(����/ÿ��)
#define UNIT_MOVE_SPEED  192.0f
//һ���������ش�С
#define MAP_CELL_SIZE  64.0f
//�����Ұ��Χʮ������
#define PLAYER_VIEW_RANGE 768.0f

//Ѱ�ҹ���Ŀ�������루��
#define FINDTARGETMAXDISTANCE 10

#define MIN_NAME_SIZE 6			//������С����
#define MAX_NAME_SIZE 32		//������󳤶�

#define MAX_DESC_SIZE 4096		//�����ֶ���󳤶�

#define MIN_PASS_SIZE 6			//������С����
#define MAX_PASS_SIZE 32		//������󳤶�

#define INVALID_ID  0           //��Ч��ID/TID(����id)

#define	Monter_Att_Num	11		//�������������

#define MAX_CHOSESKILL 8		//��ҿ�ѡ��ʹ�õļ����������


#define SKILL_ID_TO_TYPE(x)  ((x)/1000) 
#define SKILL_TYPE_AND_LEVEL_TO_ID(x, y) ((x * 1000) + y)
#define BAG_ITEMS_STREAM_SZIE     40960

enum RoleSex
{
    Sex_Woman,
    Sex_Man
};

//��������
enum AttributeType
{
	ATTRIBUTE_MONEY = 1,		//�����(��Ϸ��) v  --> vΪֵ,pΪ�ٷֱ�
	ATTRIBUTE_HPMAX,			//HP����         v
	ATTRIBUTE_MPMAX,			//MP����         v
	ATTRIBUTE_ATTACK,		    //������ v
	ATTRIBUTE_DEFENSE,			//������ v
	ATTRIBUTE_HIT,				//����ֵ v
	ATTRIBUTE_DODGE,			//����ֵ v
    ATTRIBUTE_TENACITY,         //����ֵ v
	ATTRIBUTE_CRIT,				//����ֵ v
	ATTRIBUTE_HP,				//HP v
	ATTRIBUTE_MP,				//MP v
	ATTRIBUTE_GOLD,				//Ԫ�� v
	ATTRIBUTE_BINDGOLD,			//��Ԫ�� v
	ATTRIBUTE_ARENAMONEY,		//������������ v
	ATTRIBUTE_BATTLEFIELDMONEY,	//ս�������� v
	ATTRIBUTE_XPMAX,			//XPֵ���� v
	ATTRIBUTE_XP,				//XPֵ v
	ATTRIBUTE_LEVEL,			//����ȼ� v
	ATTRIBUTE_EXP,				//���ﾭ�� v
	ATTRIBUTE_REALMLEVEL,		//����ȼ� v
	ATTRIBUTE_REALMEXP,			//���羭�� v

    ATTRIBUTE_MOVESPEED_FACTOR,	//�ƶ��ٶ�ϵ�� p
    ATTRIBUTE_HUR_FACTOR,		//�˺�״̬ϵ�� p
    ATTRIBUTE_ATK_FACTOR,       //����״̬ϵ�� p
    ATTRIBUTE_CURE_FACTOR,      //����״̬ϵ�� p
    ATTRIBUTE_CRIT_PROB,        //������ p
    ATTRIBUTE_TENACITY_PROB,    //������ p
    ATTRIBUTE_HIT_PROB,         //������ p
    ATTRIBUTE_DODGE_PROB,       //������ p
    ATTRIBUTE_CRIT_FACTOR,      //��������ϵ�� p

	ATTRIBUTE_MAX
};
//����ODDS��ʼֵ��
enum AttributeOdds
{
	ATTRIBUTE_RATIO_HIT_PROB = 1,		    //��ҳ�ʼ������
	ATTRIBUTE_RATIO_DODGE_PROB,			    //��ҳ�ʼ������
	ATTRIBUTE_RATIO_CRIT_PROB,			    //��ҳ�ʼ������
	ATTRIBUTE_RATIO_TENACITY_PROB,		    //��ҳ�ʼ������

	ATTRIBUTE_RATIO_MOVESPEED_FACTOR,	    //��ҳ�ʼ�ƶ�ϵ��
    ATTRIBUTE_RATIO_HUR_FACTOR,             //��ҳ�ʼ�˺�״̬ϵ��
    ATTRIBUTE_RATIO_ATK_FACTOR,             //��ҳ�ʼ����״̬ϵ�� 
    ATTRIBUTE_RATIO_CURE_FACTOR,            //��ҳ�ʼ����ϵ��
    ATTRIBUTE_RATIO_CRIT_FACTOR,            //��ҳ�ʼ��������ϵ�� 

    ATTRIBUTE_RATIO_MONSTER_MOVESPEED_FACTOR,	//�����ʼ�ƶ�ϵ��
    ATTRIBUTE_RATIO_MONSTER_HUR_FACTOR,         //�����ʼ�˺�״̬ϵ��
    ATTRIBUTE_RATIO_MONSTER_ATK_FACTOR,         //�����ʼ����״̬ϵ�� 
    ATTRIBUTE_RATIO_MONSTER_CURE_FACTOR,        //�����ʼ����ϵ��
    ATTRIBUTE_RATIO_MONSTER_CRIT_FACTIR,        //�����ʼ��������ϵ�� p

	ATTRIBUTE_RATIO_UPSTARUPINIT,				//����ǿ����ʼϵ��
	ATTRIBUTE_RATIO_UPSTARUPADJUST1,			//����ǿ������ϵ��1
	ATTRIBUTE_RATIO_UPSTARUPADJUST2,			//����ǿ������ϵ��2
    ATTRIBUTE_RATIO_UPSTARUPDATE,				//����ǿ������ϵ��
    ATTRIBUTE_RATIO_COMPOSEEXPEND,				//�ϳ�����ϵ��
    ATTRIBUTE_RATIO_INHERITUPDATE,				//��������ϵ��
    ATTRIBUTE_RATIO_INHERITEXPEND,				//��������ϵ��

    ATTRIBUTE_RATIO_HP_PUBLIC_CD,				//��ҩ����cd��s��
    ATTRIBUTE_RATIO_MP_PUBLIC_CD,				//��ҩ����cd��s��
    ATTRIBUTE_RATIO_SKILL_PUBLIC_CD,			//���ܹ���CD��S��

	ATTRIBUTE_RATIO_MAX
};
enum ObjectType
{
    OBJECT_NONE,        //��
	OBJECT_NPC,			//NPC
	OBJECT_PLAYER,		//���
	OBJECT_MONSTER,		//����
	OBJECT_RESOURCE		//�ɼ���Դ
};

enum MostnerType
{
    MONSTER_NORMAL,
    MONSTER_BOSS
};

//��Ʒ����
enum ItemType
{
	ITEMTYPE_EQUIP,		//װ��
	ITEMTYPE_EXPEND,	//����Ʒ
	ITEMTYPE_MATERIAL,	//����
	ITEMTYPE_WINGPIECE,	//�����Ƭ
	ITEMTYPE_TASK,		//������Ʒ
};

//��Ʒ2������
//װ������
enum EquipType
{	
	EQUIPTYPE_HEAD,		//ͷ��
	EQUIPTYPE_BODY,		//�·�
	EQUIPTYPE_WEAPON,	//����
	EQUIPTYPE_WING,		//���
	EQUIPTYPE_HORSE,	//����
	EQUIPTYPE_BELT,		//����
	EQUIPTYPE_SHOES,	//Ь��
	EQUIPTYPE_NECKLACE,	//����
	EQUIPTYPE_RING,		//��ָ
	EQUIPTYPE_ORNAMENTS,//��Ʒ
	EQUIPTYPE_FASHION ,	//ʱװ
	EQUIPTYPE_MAGICWEAPON,//����

	EQUIPTYPE_MAX
};

//����Ʒ����
enum ExpendType
{
	EXPENDTYPE_CONTINUED,			//������Ѫҩ
	EXPENDTYPE_MOMENT,				//˲���Ѫҩ
	EXPENDTYPE_CONTINUEDMP,			//��������ҩ
	EXPENDTYPE_MOMENTMP,			//˲�����ҩ
	EXPENDTYPE_BUFF,				//���BUFF
	EXPENDTYPE_EXP,					//���ﾭ�鵤
	EXPENDTYPE_SKILLTREEEXP,		//�������鵤
	EXPENDTYPE_MAGICWEAPONEXP,		//�������鵤
	EXPENDTYPE_RealmExp,			//���羭�鵤
	EXPENDTYPE_SKILLBOOK,			//������
	EXPENDTYPE_MAGICWEAPONBOOK,		//������
	EXPENDTYPE_SPIRITSBOOK,			//������
	EXPENDTYPE_PACKAGE,				//���
	EXPENDTYPE_MONEY,				//��Ϸ�Ҷһ�
	EXPENDTYPE_GOLD,				//��ֵ���Ҷһ�
	EXPENDTYPE_BINDGOLD,			//�󶨻��Ҷһ�
	EXPENDTYPE_ARENAMONEY,			//���������Ҷһ�
	EXPENDTYPE_BATTLEFIELDMONEY,	//ս�����Ҷһ�

	EXPENDTYPE_MAX
};

//���Ϸ���
enum MaterialType
{
	MATERIALTYPE_REALMODDS,	//�������
	MATERIALTYPE_STARODDS,	//���Ǹ���	
	MATERIALTYPE_REALM,		//����ͻ�Ʋ���
	MATERIALTYPE_STARM,		//װ�����ǲ���
	MATERIALTYPE_WORLDCHAT,	//���纰������

	MATERIALTYPE_MAX
};

//�����Ƭ����
enum WingPieceType
{
	WINGPIECETYPE_0,	//����λ0
	WINGPIECETYPE_1,	//����λ1
	WINGPIECETYPE_2,	//����λ2
	WINGPIECETYPE_3,	//����λ3
	WINGPIECETYPE_4,	//����λ4
	WINGPIECETYPE_5,	//����λ5
	WINGPIECETYPE_6,	//����λ6
	WINGPIECETYPE_7,	//����λ7
	WINGPIECETYPE_8,	//����λ8
	WINGPIECETYPE_9,	//����λ9

	WINGPIECETYPE_MAX,			   
};

//��ɫƷ��
enum QualityType
{
	QUALITY_WHITE,	//��ɫ
	QUALITY_GREEN,	//��ɫ
	QUALITY_BLUE,	//��ɫ
	QUALITY_PURPLE,	//��ɫ
	QUALITY_ORANGE,	//��ɫ

	QUALITY_MAX
};

//��ͼ����
enum MapType
{
	MAPTYPE_NORMAL = 1,	//������ͼ
	MAPTYPE_BOOS,		//����boos��ͼ
	MAPTYPE_COPY,		//������ͼ
	MAPTYPE_PVP,		//��������ͼ
	MAPTYPE_BATTLEFIELD,//ս����ͼ

	MAPTYPE_MAX
};

//��ͼpk����
enum MapPKType
{
	MAPPKTYPE_PEACE = 1,	//��ƽ��ͼ
	MAPPKTYPE_TEAM,			//���ս��ͼ
	MAPPKTYPE_GUILD,		//����ս��ͼ

	MAPPKTYPE_MAX,
};

//��������
enum CopyType
{
	COPY_SELF,				//���˸���
	COPY_TEAM,				//��Ӹ�����5�ˣ�
	COPY_GUILD,				//����ս��ͼ
	COPY_TOWER,				//��������

	COPY_MAX
};

//��������
enum AttackType
{
	AttackType_NORRMAL,		//��ͨ
	AttackType_CRIT,		//����
	AttackType_MISS,		//����
	AttackType_SUNDER,		//�Ƽ�
	AttackType_PENETRATE,	//��͸
	AttackType_NOHIT,		//δ����
	AttackType_BUFF			//BUFF,����Ѫ
};

//��������
enum TaskType
{
	TASK_TYPE_MAIN,		//��������
	TASK_TYPE_BRANCH,	//֧������
	TASK_TYPE_DAILY,	//�ճ�����
	TASK_TYPE_FACTION,	//��������

	TASK_TYPE_MAX
};

//�����������
enum TaskTermsType
{
	TASK_TERMS_TALK,		//�Ի�
	TASK_TERMS_KILLMONSTER,	//ɱ��
	TASK_TERMS_USEITEM,		//ʹ�õ���
	TASK_TERMS_DELIVERYITEM,//������Ʒ
	TASK_TERMS_GETITEM,		//��Ʒ�ռ�
	TASK_TERMS_COPY,		//ͨ�ظ���
	TASK_TERMS_ESCORT,		//����
	TASK_TERMS_COLLECT,		//��Ʒ�ɼ�
	TASK_TERMS_ATTRIBUTE,	//�������������顢���ܡ�ս�������ȼ��ȣ�

	TASK_TERMS_MAX
};

enum TaskState
{
	TASK_STATE_NOTASK,		//����δ��
	TASK_STATE_HAVETASK,	//�����ѽ�δ���
	TASK_STATE_SUBMITTASK,	//�������δ����
	TASK_STATE_FINISHTASK,	//��������ѽ���

	TASK_STATE_MAX
};

//��������
enum SkillClass
{
	SKILL_CLASS_SKILL = 1,		//����
	SKILL_CLASS_NORMALATTACK,	//��ͨ����
	SKILL_CLASS_XPSKILL,		//XP����
	SKILL_CLASS_PASSIVESKILL,	//��������

	SKILL_CLASS_MAX
};

//��������
enum SkillType
{
	SKILL_TYPE_NORMAL = 1,	//��ͨ����
	SKILL_TYPE_POSMOVE,		//λ��
	SKILL_TYPE_DISPEL,		//��ɢ����Ч
	SKILL_TYPE_POUNCE,		//���
	SKILL_TYPE_BACKSTAB,	//����
	SKILL_TYPE_TAUNT,		//����

	SKILL_TYPE_MAX
};

//�����˺�����
enum SkillHurtType
{
	SKILL_HURT_PHYSICS = 1,	//������
	SKILL_HURT_MAGICE,		//ħ������
	SKILL_HURT_TREAT,		//����
	SKILL_HURT_BUFF,		//BUFF

	SKILL_HURT_MAX
};

//��Ҳ���ö��
enum PlayerState_Operation 
{
	Oper_Move,				// ����Ѱ·
	Oper_Hang,				// �һ�
	Oper_MountHorse,		// ����
	Oper_Dismount,			// ����
	Oper_TriggerNPC,		// ����NPC
	Oper_ChangeMap,			// ��ͼ��ת
	Oper_Attack,			// ����
	Oper_Skill,				// ����
	Oper_XPSkill,			// XP����
	Oper_ChangeSkill,		// �л�����
	Oper_UseMedicine,		// ʹ��ҩƷ
	Oper_ChangeEqu,			// ����װ��
	Oper_Team,				// ���
	Oper_Collect,			// �ɼ�
	Oper_ArenaEnter,		// ����������
	Oper_BattlefieldLine,	// ս���Ŷ�
	Oper_BattlefieldEnter,	// ս������
	Oper_CopyLine,			// �����Ŷ�
	Oper_CopyEnter,			// ��������
	Oper_Max
};

//���״̬ö��
enum PlayerState_States 
{	
	State_Moving,			// Ѱ·״̬(�ƶ�״̬)
	State_ChangeMap,		// ��ͼ����
	State_Die,				// ����״̬
	State_Fight,			// ս��״̬	
	State_Riding,			// ���״̬
	State_Team,				// ���״̬
	State_BattlefieldLine,	// ս���Ŷ�״̬
	State_CopyLine,			// �����Ŷ�״̬
	State_Collect,			// �ɼ�״̬
	State_Hang,				// �һ�״̬
	State_Arena,			// ������״̬
	State_Battlefield,		// ս��״̬
	State_Copy,				// ����״̬
	State_Silence,			// ��Ĭ״̬
	State_Nomove,			// ����״̬
	State_Vertigo,			// ѣ��״̬
	State_Invincible,		// �޵�״̬

	State_Max
};

//��ҹ�ϵ
enum Role_Relation
{
	Relation_Friends = 0,
	Relation_BlackList = 1,

	Relation_Max,
};

//����Ƶ��
enum Chat_Channel
{
	Channel_World = 1,      //>����Ƶ��
    Channel_SingleChat,     //>˽��Ƶ��
	Channel_Near,           //>����Ƶ��
	Channel_Guild,          //>����Ƶ��
    Channel_Team,           //>����Ƶ��
    Channel_System,         //>ϵͳƵ��
};

//>���ܹ���
enum SkillFunction
{
    Skill_Damage = 1,       //>�˺�
    Skill_Cure,             //>����
    Skill_BeHurtAddBuff,    //>�ܻ����buff
    Skill_QuChuYouYi,       //>������˺�
    Skill_JianShe,          //>����
    Skill_FanTanBuff,       //>����buff
    Skill_ChaoFeng,         //>����
    Skill_XiXue,            //>��Ѫ
    Skill_ZhenShiShangHai,  //>��ʵ�˺�, �˴ι�����Ŀ��ķ�����������0���ټ����˺����㱩���ģ�
    Skill_ChongSheng,       //>����
    Skill_MianYiKongZhi,    //>���߿���
};

//>���ܴ���
enum SkillTriggerType
{
    SklTrg_NormalAttack = 1,    //>��ͨ��������
    SklTrg_BeHurt,          //>�����ܻ�
    SklTrg_HpPercent,       //>Hp�ٷֱȴ���
    SklTrg_AddBuffType,     //>����ƶ�����buff����
};

enum BuffType
{
    BuffType_YouYi = 1,
    BuffType_YouHai = 2,
    BuffType_KongZhi = 3,
};

//>����buff���˺�,������ɱ������ܴ���
enum BuffFunctionType
{
    Buff_ChenMo = 1,//>��Ĭ, ���ͷ���������
    Buff_FengXue,   //>��Ѩ, ����������,
    Buff_XuanYun,   //>ѣ��, ���ܹ���,�����ƶ�
    Buff_HunShui,   //>��˯, ���ܹ���,�����ƶ�, �ܱ�����
    Buff_HunLuan,   //>����, ����������Լ�������Ŀ��, ������ҿ���
    Buff_Wudi,      //>�޵�, ���������˺�, �����к�buff.���ܿ���buff

    Buff_DisableAddBuff,//>����, ����ָ�����͵�buff
    Buff_HuDun,         //>����,����һ���˺���ֵ
    Buff_HuDunPct,      //>����,��������Ѫ�����ްٷֱ���ֵ
    Buff_HuDunMp2Hp,    //>����,���ٷ�֮x��mpת����ֵ
    Buff_DamagePctToHp, //>��Ѫ,�˺�ֵ�İٷֱȱ��hp
    Buff_FanShang,      //>����,�����˺�ֵ�İٷֱ�

    Buff_HuoQiang,      //>��ǽ
    Buff_ZhaoHuang,     //>�ٻ�

    Buff_DropHp,        //>������Ѫ(�̶�ֵ)
    Buff_DropCurHpPct,  //>������Ѫ(��ǰѪ���ٷֱ�)
    Buff_DropHpMaxPct,  //>������Ѫ(���Ѫ���ٷֱ�)

    Buff_AddHpMax,      //>���Hp����ֵ
    Buff_AddHpMaxPct,   //>���Hp���ްٷֱ�

    Buff_AddMpMax,      //>���Mp����
    Buff_AddMpMaxPct,   //>���Mp���ްٷֱ�

    Buff_AddMoveSpeed,      //>����ƶ��ٶ�

    Buff_AddAttackPower,    //>��ӹ�����
    Buff_AddAttackPowerPct, //>��ӹ������ٷֱ�
    Buff_AddDefencePower,   //>��ӷ�����
    Buff_AddDefencePowerPct,//>��ӷ������ٷֱ�
    Buff_AddCrit,           //>��ӱ���ֵ
    Buff_AddCritPct,        //>��ӱ���ֵ�ٷֱ�
    Buff_AddHit,            //>�������ֵ
    Buff_AddHitPct,         //>�������ֵ�ٷֱ�
    Buff_AddTenacity,       //>�������ֵ
    Buff_AddTenacityPct,    //>�������ֵ�ٷֱ�
    Buff_AddDodge,          //>�������ֵ
    Buff_AddDodgePct,       //>�������ֵ�ٷֱ�

    Buff_AddAtkFactor,      //>��ӹ���ϵ��
    Buff_AddDefFactor,      //>��ӷ���ϵ��
    Buff_AddCritFactor,     //>��ӱ���ϵ��
};


enum SkillTargetType
{
    SklTag_SingleTarget = 1,   //>����Ŀ��   
    SklTag_OnlySelf,           //>�Լ�
    SklTag_EnemyInSelfRang,    //>���Լ�Ϊ���ĵĵж�Ŀ��
    SklTag_TeamPlayerInSelfRang, //>���Լ�Ϊ���ĵĶ����Ա
    SklTag_EnemyInTargetRang,  //>��Ŀ��Ϊ���ĵĶ����Ա
};

/////////////////////////////////////////////////////////////////////////////////

//>������Ϣ(���ݿⱣ��)
struct SkillInfoIt
{
    int skillType;      //>��������
    int level;          //>���ܵȼ�
};

//�����Ϣ(���ݿⱣ��)
struct WingInfoIt
{
	int id;				//����ID 
	int level;			//���׵ȼ� 
	int exp;			//���׾���
};

//������Ϣ(���ݿⱣ��)
struct MagicWeaponInfoIt
{
	int id;				//����ID
	int level;			//�����ȼ�
	int exp;			//��������
	int growthPlus;		//�����ɳ�ϵ��
};

//������Ϣ(���ݿⱣ��)
struct MountInfoIt
{
	int id;				//����ID 
	int time;			//����ʱ��
};

//������Ϣ(���ݿⱣ��)
struct CopyInfoIt
{
	int id;				//����id	
	int enterNum;		//�������
	int layer;			//����
};

//�ճ���Ϣ(���ݿⱣ��)
struct DailyInfoIt
{
	int id;				//�ճ�id	
	int progress;		//��ɽ���
	bool isReward;		//�Ƿ��콱
};

//������Ϣ
struct TaskInfoIt
{
	int id;				//����id	
	int progress;		//��ɽ���
	int state;			//����״̬��0δ�� 1�ѽ� 2�ɽ��� 3�����
};

//������Ϣ(���ݿⱣ��)
struct ItemInfoIt
{
	uint64 guid;		//Ψһid	
	int id;			    //��ƷID��ģ�壩
	int prefixID;		//ǰ׺ID
	int starNum;		//����
	bool isBind;		//�Ƿ��
	int num;			//����
	bool isLock;        //�Ƿ�����
    int time;			//����ʱ��
};

//��һ�����Ϣ
struct BaseInfoIt
{
	uint64 playerID;	//���id���˺�id�����
	char playerName[MAX_NAME_SIZE];	//����
	int8 sex;					//�Ա� 0Ů 1��	
	int16 headID;				//ͷ��ID
	int64 offlineTime;		//����ʱ�䣬��������ʱ�ж��Ƿ�ˢ��ÿ�մ���
	int32 level;				//�ȼ�
	int32 exp;				//����ֵ
	int32 money;				//��ǰ�����
	int32 gold;				//Ԫ������
	int32 bindGold;			//��Ԫ������
	int32 rechargGoldSum;		//�ܳ�ֵԪ��
	int32 arenaScore;			//����������
	int32 arenaMoney;			//������������	
	int32 battlefieldScore;	//��ǰս������	
	int32 battlefieldMoney;	//ս��������
	int32 mapID;				//���ڵ�ͼid
	int32 posx;				//����λ��x
	int32 posy;				//����λ��y
	int32 hp;					//Ѫ
    int32 hpMax;                //Ѫ����
	int32 mp;					//��
    int32 mpMax;                //������
	int32 xp;					//ŭ��
    int32 xpMax;                //ŭ������
	int64 societyID;			//����id
	int32 realmLevel;			//����ȼ�	
	int32 realmExp;			//���羭��
	int32 activeSkillType;  //�������ļ��������� 
};

struct DetailAttribute
{
	int32 hpMax;			//HP����         v					
	int32 mpMax;			//MP����         v
	int32 attack;		    //������ v
	int32 defense;			//������ v
	int32 hit;				//����ֵ v
	int32 dodge;			//����ֵ v
	int32 crit;				//����ֵ v
	int32 tenacity;			//����ֵ v
    int32 moveSpeedFactor;  //�ƶ��ٶ�ϵ��   v
    float hitProb;          //������
    float critProb;         //������
	int32 battlePower;		//ս����
};

struct GuidInfoIt
{
	uint64 itemId;
};

struct GangMember
{
	int64		id;							//����ID
	char		szName[MAX_NAME_SIZE];		//��������
	int32		iLevel;						//�ȼ�
	char		szApplyTime[50];			//����ʱ��
	char		szPosition[MAX_NAME_SIZE];	//ְ��
	int32		iContribution;				//����
	char		szLeaveTime[50];			//�뿪ʱ��
};

struct FriendsInfo
{
	uint64	roleId;					//��ɫID
	char	szName[MAX_NAME_SIZE];	//��������
	bool	bIsLine;				//�Ƿ�����
	int		iLevel;					//�ȼ�
	int		iFight;					//ս����
};

struct AddHpMode
{
    enum Mode
    {//>�Ժ�ҩ,���Ƽ���,Buff��Ѫ,��Ѫ,����
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
    {//>��ͨ,����,����,����,��������buff,�޵�����buff,����buff,������ѪBuff
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