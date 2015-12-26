/********************************************************************
created:	2015/11/28
author:		lishihai
desc:		�����,��š�������ҵ�����
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

    void LoadBaseAttribute(BaseInfoIt& baseInfo);   //>���ػ�������
	void LoadOtherAttributes();                     //>������������ 
	void CalcPlayerOddsAttribute(AttributeType attributeOdds,AttributeType oddsValues);
	void LoadPlayerOtherAttribute();
	void CalcPlayerBattlePower();
    void CalcSuitAttribute();                       //>������װ����

    void SendBaseAttribute();                       //>���ͻ������Ե��ͻ���
	void SendDetailAttribute();						//>�����������Ե��ͻ���
    void SendSkillList();                           //>���ͼ����嵥
	void SendUdpAttributeToDB(AttributeType attributeType);						//>���ͻ�����Ϣ��DB

    virtual bool AddHp(const AddHpMode& mode);          //>��Ѫ 
    virtual bool DropHP(const DropHpMode& mode);		//>��Ѫ
    virtual void Idle();                                //>����
    virtual void Die();					                //>����
	void Relive(int8 type);						//�������� 0�س� 1ԭ��

	void Ride(bool ride);			//������0��1��
	bool GetRide() {return m_IsRide;}//��ȡ�Ƿ�����

    void UpdateViewObjectSet();     //���¿ɼ���Ұ���󼯺�
    void ActivateSkill(int skillType);  //>�������޼��� 
    void SetChoseSkill(int32 index, int32 skillId); //>����ѡ��ʹ�õļ���
	void Chest(AttributeType type, int num);	//������

    void OnOnline();                //����
    void OnOffline();               //����

    void OnEnterMap();              //��ͼ֪ͨ��ҽ����ͼ
    void OnLeaveMap();              //��ͼ֪ͨ����뿪��ͼ

    void OnObjEnterView(Object* obj); //����ĳ��λ������Ұ
    void OnObjLeaveView(Object* obj); //����ĳ��λ�뿪��Ұ

    void OnChangeEquipment();        //װ���ı�
    void OnEquipmentUpstart(SvrItemInfo* pItem);    //װ�����ǳɹ�
    void OnEquipmentCompose(SvrItemInfo* pItem);    //װ���ϳɳɹ�
    void OnEquipmentInherit();                      //װ�����гɹ�

    void SyncCreate(Object* obj);   //>֪ͨ�ͻ��˴�������
    void SyncDelete(Object* obj);   //>֪ͨ�ͻ���ɾ������
    void SyncMove(Object* obj);     //>֪ͨ�ͻ������ƶ�
    void SyncBuff(Object* obj); //>֪ͨ�ͻ���buff

	virtual void BroadcastMsg(MsgBase* msg, bool isBroadcastSelf = true);		//�㲥��Ϣ

	bool IsTeamMembers(int64 guid);					//�Ƿ�����Ա

    void StartHpCD();       //>��ʼHP cd
    void StartMpCD();       //>��ʼMP cd
    bool IsHpCDCooled();    //>HP cd�Ƿ���ȴ���
    bool IsMpCDCooled();    //>MP cd�Ƿ���ȴ���
    void HpCDCallback(int); //>cd�ص�
    void MpCDCallback(int); //>cd�ص�


    void AddExp(int exp);  //>��Ӿ���
    void Upgrade();         //>����

    int  GetMoney();                    //>ӵ�н��
    void AddMoney(int money);           //>��ӽ��
    bool SpendMoney(int money);         //>���Ľ��
    int  GetGold();                     //>ӵ��Ԫ��
    void AddGold(int gold);             //>���Ԫ��
    bool SpendGold(int gold);           //>����Ԫ��
    int  GetBindGold();                 //>ӵ�а�Ԫ��
    void AddBindGold(int gold);         //>��Ӱ�Ԫ��
    bool SpendBindGold(int gold);       //>���İ�Ԫ��
    int  GetArenaMoney();               //>ӵ��ս������
    void AddArenaMoney(int money);      //>���ս������
    bool SpendArenaMoney(int money);    //>����ս������
    int  GetBattlefieldMoney();         //>ӵ�о���������
    void AddBattlefieldMoney(int money);//>��Ӿ���������
    bool SpendBattlefieldMoney(int money);//>���ľ���������


	void ChangePlayerAttribute(AttributeType attributeType,float attributeValue);
//����

	
	char m_Name[MAX_NAME_SIZE];			//�������
	bool m_IsRide;						//�Ƿ�����
	int m_HeadIconIndex;				//ͷ������
	int m_Sex;							//�Ա�0Ů 1��
	int m_VIPLevel;						//vip�ȼ�
	int m_BattlePower;				    //ս����
	int64 m_SocietyID;					//����id
    int64 m_OfflineTime;                //����ʱ�䣬��������ʱ�ж��Ƿ�ˢ��ÿ�մ���
	int32 m_activeSkillType;		    //����ļ�������
    uint32 m_teamId;                    //����


	std::vector<MagicWeaponInfoIt> m_MagicWeaponList;	//������Ϣ
	std::vector<MountInfoIt> m_MountList;				//������Ϣ
	std::vector<CopyInfoIt> m_CopyList;					//������Ϣ
	std::vector<DailyInfoIt> m_DailyList;				//�ճ���Ϣ
	std::vector<TaskInfoIt> m_TaskList;					//������Ϣ
	std::vector<SvrItemInfo*> m_BagList;				//������Ϣ
	SvrItemInfo* m_Equip[EQUIPTYPE_MAX];				//��������װ��
	WingInfoIt m_WingList[WINGPIECETYPE_MAX];			//������Ϣ
	int32      m_ChoseSkill[MAX_CHOSESKILL];			//�����ѡ��ļ���

    std::list<Object*> m_ViewObjSet;                    //��Ұ����
    Vec2               m_Tile4LastUpdateViwe;           //>���һ�θ�����Ұ��tile

	std::vector<FriendsInfo>	m_vecFriendsList;		//�����б�
	std::vector<FriendsInfo>	m_vecBlackList;			//�������б�

    bool m_HpCooling;
    bool m_MpCooling;

//״̬��  �÷���������PlayerState_Operation����֮ǰ����CheckOperInState�����жϲ����Ƿ�����ִ�У�������true���ڵ���ChangeState��ִ��PlayerState_Operation�ľ��������
	bool m_State[State_Max];								//��ǰ״̬��Ϣ
	bool CheckOperInState(PlayerState_Operation _oper);		//���ò����ڵ�ǰ״̬���Ƿ�ɽ���
	bool ChangeState(PlayerState_Operation _oper);			//���ݲ����ı�״̬
	bool GetState(PlayerState_States _state);				//�Ƿ���ĳ��״̬��
private:
	void AddState(PlayerState_States _state);		//���״̬	�������Ǳ�Ҫ�����ʹ�ô˺���
	void CancelState(PlayerState_States _state);	//ȡ��״̬	�������Ǳ�Ҫ�����ʹ�ô˺���

public:
    //>����
    std::vector<SvrSkillInfo> m_SkillList; 
    bool   AddSkill(int skillType);
    bool   UpgradeSkill(int skillType);
public:

//>�ƶ�����ӿ�
    virtual void    OnMoveByTarget(const Vec2& moveTarget);
    virtual void    OnMoveByPath(std::vector<int>& paths, const Vec2& moveTarget);
    virtual void    OnMoveEnd();

//>ս������ӿ�
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
