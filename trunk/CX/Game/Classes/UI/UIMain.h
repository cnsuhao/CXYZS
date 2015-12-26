/********************************************************************
created:	2014/12/6
author:		LL
desc:		������UI����Ϸ�������ʼ��������߼�
*********************************************************************/
#ifndef __UIMAIN_H__
#define __UIMAIN_H__

#include "UIBase.h"
#include "Turntable.h"
#include "GestureDetector.h"
#include "UITextAtlas.h"
#include "UILoadingBar.h"
#include "CCProgressTimer.h"

struct MainMessage
{
	string strChannel;
	string strContent;
    cc2d::Color3B channelColor;
};

class UIMain : public UIBase
{
public:
	UIMain();
	~UIMain();	
	virtual bool init();
	CREATE_FUNC(UIMain);
    virtual bool HideMainPanelWhenSelfShow() { return false; }   // UI�����Ƿ�����������
	void ChangeHero();			//������Ϣ�ı�
	void ChangeSelTarget();		//Ŀ����Ϣ�ı�
	void PointClickCollect(int32 parid , int32 Itemid);	//����ɼ���
    void SetMiniMap(int mapid); //����С��ͼ
    virtual void OnShow();      //>��ʾ�ص�
    virtual bool TouchJoystickInRange(cc2d::Touch *touch);
	
	void HeroIcon_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//ͷ��ص�
	void Copy_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//�����ص�
	void Recharge_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//��ֵ�ص�
	void Mall_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//�̳ǻص�
	void Chest_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//����ص�
	void FirstRecharge_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);//�׳�ص�
	void Automatic_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);	//�һ��ص�
	void Activity_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//��ص�
	void Reward_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//�����ص�
	void Chat_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//����ص�
	void Skill1_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//����1�ص�
	void Skill2_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//����2�ص�
	void Skill3_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//����3�ص�
	void Skill4_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//����4�ص�
	void SkillXP_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//XP���ܻص�
	void Drug_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//ҩˮ�ص�
	void Horse_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//������ص�

	cc2d::Node*	m_MainPanel;	//�����

	class UIMiniMap*  m_pMiniMap;  //С��ͼ

	cc2d::Node* m_roleAtt;		//��������
	cc2dui::ImageView* HeroIcon;		//����ͷ��
	cc2dui::Text* HeroName;				//��������
	cc2dui::LoadingBar* HeroHP;			//����Ѫ��
	cc2dui::LoadingBar* HeroMP;			//��������
	cc2dui::Text* lev_text;				//�ȼ�
	cc2dui::TextAtlas* vip_text;		//vip�ȼ�
	cc2dui::LoadingBar* Exp;			//������

	cc2d::Node* m_targetAtt;	//Ŀ��������
	cc2dui::ImageView* TargetIcon;		//Ŀ��ͷ��
	cc2dui::Text* TargetName;			//Ŀ������
	cc2dui::LoadingBar* TargetHP;		//Ŀ��Ѫ��
	cc2dui::Text* TargetLevel;			//Ŀ��ȼ�

	cc2d::Node*	m_BtPanel;		//���Ͻǵ�BT
	cc2dui::CheckBox*	m_btCheck;		//��ť��checkbox

	cc2dui::ImageView*	m_infobg;		//ϵͳ��Ϣ����
	cc2dui::Text*		m_infoContent;	//ϵͳ��Ϣ����

	cc2d::Node* m_newEquipment; //��װ����ʾ��
	cc2dui::Button*		m_neBtEquip;	//��װ����ʾ���װ��BT
	cc2dui::Button*		m_neBtClose;	//��װ����ʾ��Ĺر�BT

	cc2dui::ImageView*	m_taskTipbg;		//��ʾ�򱳾�
	cc2dui::Text*		m_taskTipContent;	//��ʾ������

	cc2dui::ImageView*	m_barbg;		//�������ӣ�����������
	cc2dui::LoadingBar*	m_bar;			//������
	cc2dui::Text*		m_barWord;		//����������

	cc2d::Node*	m_expInfo;		//������ʾ
	cc2d::Node*	m_Control;		//���Ƹ�
	cc2d::Node* m_taskTeamPanel;//�����������
	cc2dui::CheckBox*	m_cdTaskTeam;

	cc2dui::CheckBox*	m_cdTask;		//����CHECKBOX
	cc2dui::CheckBox*	m_csTeam;		//����CHECKBOX
	cc2d::Vector<cc2dui::CheckBox*> m_Checkboxs;

	cc2dui::ScrollView*	m_svPanel;
	cc2dui::Layout* m_TaskTemplate;
	cc2dui::Layout* m_TeamTemplate;

    enum
    {
        MaxCountShowBuff = 6,
    };


    void LoadHeroBuffIcon();
    void LoadTargetBuffIcon();
	
	cc2dui::ImageView* m_HeroBuff[MaxCountShowBuff];	//����buff
	cc2dui::ImageView* m_TargetBuff[MaxCountShowBuff];	//Ŀ��buff

	//�������
	void AddTaskItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,  int32 taskid);
	//��Ӷ���
	void AddTeamItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout, std::string itemName, int32 level, int32 hp, int32 maxHp, bool leader);
    void LoadTeamInfo();
    void UpdateTeamHp();


	map<int32,cc2dui::Layout*> m_mapTaskLayout;
	//���������
	void TaskClick(int32 taskid);
	//����������ʾ
	void UpdateTask();


    void UpdateCD(float dt);
    float m_CDInterval;

	cc2d::Sprite*		m_disable;
	cc2d::ProgressTimer* m_progressRed;
	bool	m_bRedDisable;
	float	m_fRedNum;
	float	m_fRedLevel;
	void	RedCD(float dt); 

	cc2d::ProgressTimer* m_progressBlue;
	bool	m_bBlueDisable;
	float	m_fBlueNum;
	float	m_fBlueLevel;
	void	BlueCD(float dt); 

	cc2d::Sprite*		m_sprSkillcd;
	cc2d::ProgressTimer* m_progressSkill1;
	bool	m_bSkill1Disable;
	float	m_fSkill1Num;
	float	m_fSkill1Level;
	void	Skill1CD(float dt); 

	cc2d::ProgressTimer* m_progressSkill2;
	bool	m_bSkill2Disable;
	float	m_fSkill2Num;
	float	m_fSkill2Level;
	void	Skill2CD(float dt); 

	cc2d::ProgressTimer* m_progressSkill3;
	bool	m_bSkill3Disable;
	float	m_fSkill3Num;
	float	m_fSkill3Level;
	void	Skill3CD(float dt); 

	cc2d::ProgressTimer* m_progressSkillxp;
	bool	m_bSkillxpDisable;
	float	m_fSkillxpNum;
	float	m_fSkillxpLevel;
	void	SkillxpCD(float dt); 

	cc2d::Sprite*	m_sprProcd;
	cc2d::ProgressTimer* m_progressCollect;
	bool	m_bCollectDisable;
	float	m_fCollectNum;
	float	m_fCollectLevel;
	void	CollectCD(float dt); 
	void	StopCollectCD(); 
	bool	m_bIsCollect;

	void	SetProgress(cc2d::ProgressTimer* pPro,float fSec);

	int32   m_iCollectItem;

	//���NPC
	void	ClickNpc();

	//���������ʼ��
	void ProtagonistInit();
	//Ŀ���ʼ��
	void TargetInit();
	//�������������
	void TaskInit();
	//������
	//void ExpInit();

	//���������
	void UpdateChat(int32 iChannel,string strName,string strContent);
	vector<MainMessage> m_vecMainMessage;

	cc2d::Node* m_chatPanel;


    enum MenuEnum
    {
        TeamJoin,  //>���
        TeamKick,  //>����
        TeamChangeLeader,  //>�ӳ�ת��
        TeamDimiss, //�����ɢ
        TeamExit,   //�˳�����
        SingleChat, //>˽��
        SeeInfo,    //>�鿴��Ϣ
    };

    //>������Ҳ˵�
    void HidePlayerMenu();
    //>������Ҳ˵�
    void PopPlayerMenu(const cc2d::Vec2& position, uint64 SelTagGuid);
    //>�������
    void OnTeamApply();
    void OnTeamKick();
    void OnTeamChangeLeader();
    void OnTeamDimiss();
    void OnTeamExit();
    void OnSingleChat();
    void OnSeeInfo();
    cc2d::Node* m_PlayerMenuPnl;
    cc2dui::ImageView* m_MenuImgBg;
    cc2dui::Button* m_MenuButton;
    ccWidgetClickCallback m_MenuCallback;
    uint64 m_PopMenuTagGuid;
//////////////////////////////////////////////////////////////////////////
//>�������
    void InitSkillPanel();  //>��ʼ���������
    void SetupTurntable();  //>����֧����ת
    void ReloadSkill();     //>���¼��ؼ���ͼ��
    void Update(float deltaTime);   //>����
protected:
    cc2dui::ImageView*  m_comSkillIcon;
    cc2dui::Button*     m_comSkillbtn;
    cc2dui::ImageView*  m_xpSkillIcon;
    cc2dui::Button*     m_xpSkillbtn;
    cc2dui::ImageView*  m_SkillIcons[4];
    cc2dui::Button*     m_SkillFrames[4];
    cc2dui::Layout*     m_TouchPanel;
    int32       m_nSkillCount;
    Turntable   m_Turntable;
    GestureDetector m_GetstureDetector;
//////////////////////////////////////////////////////////////////////////

};

#endif	//__UIMAIN_H__