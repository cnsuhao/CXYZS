/********************************************************************
created:	2015/11/28
author:		lishihai
desc:		玩家类,存放、更改玩家的数据
*********************************************************************/

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Object.h"
#include <list>

#include "ItemManager.h"
#include "MoveComponent.h"
#include "BattleComponent.h"
#include "Skill.h"

struct MsgBase;

struct SkillInfoEx;
class Buff;


class Player: public Object, public MoveComponent::Interface, public BattleComponent::Interface
{
public:
	Player();
	virtual ~Player();
	virtual bool Init(uint64 guid);

    void Update(float deltaTime);
    virtual void OnChangeBlock(int oldBlock, int newBlock);
    virtual void OnChangeTile(const Vec2& oldTile, const Vec2& newTile);

    void LoadBaseAttribute(BaseInfoIt& baseInfo);   //>加载基础属性
	void LoadOtherAttributes();                     //>加载配置属性 
	void CalcPlayerOddsAttribute(AttributeType attributeOdds,AttributeType oddsValues);
	void LoadPlayerOtherAttribute();
	void CalcPlayerBattlePower();
    void CalcSuitAttribute();                       //>计算套装属性

    void SendBaseAttribute();                       //>发送基础属性到客户端
	void SendDetailAttribute();						//>发送其他属性到客户端
    void SendSkillList();                           //>发送技能清单
	void SendUdpAttributeToDB(AttributeType attributeType);						//>发送基础信息到DB

    virtual bool AddHp(const AddHpMode& mode);          //>加血 
    virtual bool DropHP(const DropHpMode& mode);		//>掉血
    virtual void Idle();                                //>待机
    virtual void Die();					                //>死亡
	void Relive(int8 type);						//复活类型 0回城 1原地

	void Ride(bool ride);			//上下马0下1上
	bool GetRide() {return m_IsRide;}//获取是否骑马

    void UpdateViewObjectSet();     //更新可见视野对象集合
    void ActivateSkill(int skillType);  //>激活主修技能 
    void SetChoseSkill(int32 index, int32 skillId); //>设置选择使用的技能
	void Chest(AttributeType type, int num);	//抽箱子

    void OnOnline();                //上线
    void OnOffline();               //离线

    void OnEnterMap();              //地图通知玩家进入地图
    void OnLeaveMap();              //地图通知玩家离开地图

    void OnObjEnterView(Object* obj); //触发某单位进入视野
    void OnObjLeaveView(Object* obj); //触发某单位离开视野

    void OnChangeEquipment();        //装备改变
    void OnEquipmentUpstart(SvrItemInfo* pItem);    //装备升星成功
    void OnEquipmentCompose(SvrItemInfo* pItem);    //装备合成成功
    void OnEquipmentInherit();                      //装备传承成功

    void SyncCreate(Object* obj);   //>通知客户端创建对象
    void SyncDelete(Object* obj);   //>通知客户端删除对象
    void SyncMove(Object* obj);     //>通知客户对象移动
    void SyncBuff(Object* obj); //>通知客户端buff

	virtual void BroadcastMsg(MsgBase* msg, bool isBroadcastSelf = true);		//广播消息

	bool IsTeamMembers(int64 guid);					//是否队伍成员

    void StartHpCD();       //>开始HP cd
    void StartMpCD();       //>开始MP cd
    bool IsHpCDCooled();    //>HP cd是否冷却完毕
    bool IsMpCDCooled();    //>MP cd是否冷却完毕
    void HpCDCallback(int); //>cd回调
    void MpCDCallback(int); //>cd回调


    void AddExp(int exp);  //>添加经验
    void Upgrade();         //>升级

    int  GetMoney();                    //>拥有金币
    void AddMoney(int money);           //>添加金币
    bool SpendMoney(int money);         //>消耗金币
    int  GetGold();                     //>拥有元宝
    void AddGold(int gold);             //>添加元宝
    bool SpendGold(int gold);           //>消耗元宝
    int  GetBindGold();                 //>拥有绑定元宝
    void AddBindGold(int gold);         //>添加绑定元宝
    bool SpendBindGold(int gold);       //>消耗绑定元宝
    int  GetArenaMoney();               //>拥有战场货币
    void AddArenaMoney(int money);      //>添加战场货币
    bool SpendArenaMoney(int money);    //>消耗战场货币
    int  GetBattlefieldMoney();         //>拥有竞技场货币
    void AddBattlefieldMoney(int money);//>添加竞技场货币
    bool SpendBattlefieldMoney(int money);//>消耗竞技场货币


	void ChangePlayerAttribute(AttributeType attributeType,float attributeValue);
//属性

	
	char m_Name[MAX_NAME_SIZE];			//玩家名字
	bool m_IsRide;						//是否骑马
	int m_HeadIconIndex;				//头像索引
	int m_Sex;							//性别，0女 1男
	int m_VIPLevel;						//vip等级
	int m_BattlePower;				    //战斗力
	int64 m_SocietyID;					//公会id
    int64 m_OfflineTime;                //离线时间，用于上线时判断是否刷新每日次数
	int32 m_activeSkillType;		    //激活的技能类型
    uint32 m_teamId;                    //队伍


	std::vector<MagicWeaponInfoIt> m_MagicWeaponList;	//法宝信息
	std::vector<MountInfoIt> m_MountList;				//器灵信息
	std::vector<CopyInfoIt> m_CopyList;					//副本信息
	std::vector<DailyInfoIt> m_DailyList;				//日常信息
	std::vector<TaskInfoIt> m_TaskList;					//任务信息
	std::vector<SvrItemInfo*> m_BagList;				//背包信息
	SvrItemInfo* m_Equip[EQUIPTYPE_MAX];				//身上所穿装备
	WingInfoIt m_WingList[WINGPIECETYPE_MAX];			//翅膀孔信息
	int32      m_ChoseSkill[MAX_CHOSESKILL];			//玩家已选择的技能

    std::list<Object*> m_ViewObjSet;                    //视野对象
    Vec2               m_Tile4LastUpdateViwe;           //>最后一次更新视野的tile

	std::vector<FriendsInfo>	m_vecFriendsList;		//好友列表
	std::vector<FriendsInfo>	m_vecBlackList;			//黑名单列表

    bool m_HpCooling;
    bool m_MpCooling;

//状态表  用法：主角做PlayerState_Operation操作之前调用CheckOperInState函数判断操作是否允许执行，若返回true则在调用ChangeState后执行PlayerState_Operation的具体操作。
	bool m_State[State_Max];								//当前状态信息
	bool CheckOperInState(PlayerState_Operation _oper);		//检查该操作在当前状态下是否可进行
	bool ChangeState(PlayerState_Operation _oper);			//根据操作改变状态
	bool GetState(PlayerState_States _state);				//是否处于某种状态下
private:
	void AddState(PlayerState_States _state);		//添加状态	×××非必要情况别使用此函数
	void CancelState(PlayerState_States _state);	//取消状态	×××非必要情况别使用此函数

public:
    //>技能
    std::vector<SvrSkillInfo> m_SkillList; 
    bool   AddSkill(int skillType);
    bool   UpgradeSkill(int skillType);
public:

//>移动组件接口
    virtual void    OnMoveByTarget(const Vec2& moveTarget);
    virtual void    OnMoveByPath(std::vector<int>& paths, const Vec2& moveTarget);
    virtual void    OnMoveEnd();

//>战斗组件接口
    virtual bool  IsNormalSkill(int skillId);
    virtual bool  HasSkill(int skillId);
    virtual void  OnUseSkill(int skillId, Object* target);
    virtual void  OnAddBuff(Buff* pBuff);
    virtual void  OnRemoveBuff(Buff* pBuff);

    virtual void  GetSkillsForInterestNormalSkill(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestBeHurt(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestHpPct(std::vector<const SkillInfo*>& skills);
    virtual void  GetSkillsForInterestAddBuff(std::vector<const SkillInfo*>& skills);
};

#endif	//__H_PLAYER__
