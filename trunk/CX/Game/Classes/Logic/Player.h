/********************************************************************
created:	2014/6/10
author:		LL
desc:		����ࣺ�����������ࡣ
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
	virtual void ReleaseSelf(cc2d::Node *sender, long data);	//�ͷ�����,һ�㶼��Ϊ�ص������Զ�����

    inline bool IsSkilling();               //>����״̬

//>�������
	virtual void Idle();					//����
	virtual void Die();						//����
	virtual void Ride(bool ride);			//������0��1��
	virtual bool GetRide() {return m_IsRide;}//��ȡ�Ƿ�����

//>���
    cc2d::Node* CreateFaceCopyNode();
    static void EquipIdToFaceId(int equipId[EQUIPTYPE_MAX], int outFaceId[EQUIPTYPE_MAX]);
	bool ReloadFace(int face[EQUIPTYPE_MAX], bool asyncLoad = true);	    //�������


//>����/��ʾ����ӿ�
    virtual float   AC_GetMoveSpeed();
    virtual void    GetFaces(int face[EQUIPTYPE_MAX]);
    virtual int     GetSex();
    virtual void    OnAttcakAnimEnd();

//>�ƶ�����ӿ�
    virtual float   MC_GetMoveSpeed();
    virtual void    OnChangeDirection(DirectionType direction);
    virtual void    OnMoveByTarget(const cc2d::Vec2 &tagPos);
    virtual void    OnMoveByPath(std::vector<cc2d::Vec2>& paths, const cc2d::Vec2 &tagPos);
    virtual void    OnMoveEnd();

//>ս������ӿ�
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
//����
	bool m_IsRide;				//�Ƿ�����
	int m_HeadIconIndex;		//ͷ������
	int m_Sex;					//�Ա�0Ů 1��
    int m_Level;                //�ȼ�
 	int m_Face[EQUIPTYPE_MAX];	//���id
private:
    int m_Hp;                   //hp
    int m_HpMax;                //hp����
    bool m_IsSkilling;          //>�ͷż�����
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
