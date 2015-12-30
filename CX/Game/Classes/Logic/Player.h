/********************************************************************
created:	2014/6/10
author:		LL
desc:		玩家类：派生出主角类。
*********************************************************************/

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "ClientDef.h"
#include "Object.h"
#include "MoveComponent.h"
#include "AnimComponent.h"
#include "BattleComponent.h"

class Player: public MapObj , 
              public MoveComponent::Interface, 
              public PlayerAnimComp::Interface,
              public BattleComponent::Interface
{
public:
	Player();
	virtual ~Player();

	bool Init(uint64 guid,int sex, const char* name, int face[EQUIPTYPE_MAX], bool initDead = false);

	virtual void Update(float delta);
	virtual void ReleaseSelf(cc2d::Node *sender, long data);	//释放自身,一般都作为回调函数自动调用

    inline bool IsSkilling();               //>技能状态

//>动作相关
	virtual void Idle();					//待机
	virtual void Die();						//死亡
	virtual void Ride(bool ride);			//上下马0下1上
	virtual bool GetRide() {return m_IsRide;}//获取是否骑马

//>外观
    cc2d::Node* CreateFaceCopyNode();
    static void EquipIdToFaceId(int equipId[EQUIPTYPE_MAX], int outFaceId[EQUIPTYPE_MAX]);
	bool ReloadFace(int face[EQUIPTYPE_MAX], bool asyncLoad = true);	    //更新外观


//>动画/显示组件接口
    virtual float   AC_GetMoveSpeed();
    virtual void    GetFaces(int face[EQUIPTYPE_MAX]);
    virtual int     GetSex();
    virtual void    OnAttcakAnimEnd();

//>移动组件接口
    virtual float   MC_GetMoveSpeed();
    virtual void    OnChangeDirection(DirectionType direction);
    virtual void    OnMoveByTarget(const cc2d::Vec2 &tagPos);
    virtual void    OnMoveByPath(std::vector<cc2d::Vec2>& paths, const cc2d::Vec2 &tagPos);
    virtual void    OnMoveEnd();

//>战斗组件接口
    virtual void    OnSkillBegin(int skillId, MapObj* target);
    virtual void    OnSkillEnd();
    virtual void    OnInitBuffList();
    virtual void    OnAddBuff(const BattleComponent::BuffData& buffData);
    virtual void    OnRemoveBuff(const BattleComponent::BuffData& buffData);
    
public:
    virtual void    SetHp(int hp);
    inline int      GetHp() const;
    virtual void    SetHpMax(int hpMax);
    inline int      GetHpMax() const;
    virtual void    SetMoveSpeed(float speed);
    inline float    GetMoveSpeed() const;
private:    
    void            InitDie();
public:
//属性
	bool m_IsRide;				//是否骑马
	int m_HeadIconIndex;		//头像索引
	int m_Sex;					//性别，0女 1男
    int m_Level;                //等级
 	int m_Face[EQUIPTYPE_MAX];	//外观id
private:
    int m_Hp;                   //hp
    int m_HpMax;                //hp上限
    bool m_IsSkilling;          //>释放技能中
    float m_moveSpeed;          //>
};

bool Player::IsSkilling()
{
    return m_IsSkilling;
}

inline int  Player::GetHp() const
{
    return m_Hp;
}

inline int  Player::GetHpMax() const
{
    return m_HpMax;
}

inline float Player::GetMoveSpeed() const
{
    return m_moveSpeed;
}


#endif	//__H_PLAYER__
