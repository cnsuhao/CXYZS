/********************************************************************
created:	2014/12/6
author:		LL
desc:		主界面UI：游戏主界面初始化和相关逻辑
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
    virtual bool HideMainPanelWhenSelfShow() { return false; }   // UI开启是否隐藏主界面
	void ChangeHero();			//主角信息改变
	void ChangeSelTarget();		//目标信息改变
	void PointClickCollect(int32 parid , int32 Itemid);	//点击采集物
    void SetMiniMap(int mapid); //设置小地图
    virtual void OnShow();      //>显示回调
    virtual bool TouchJoystickInRange(cc2d::Touch *touch);
	
	void HeroIcon_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//头像回调
	void Copy_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//副本回调
	void Recharge_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//充值回调
	void Mall_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//商城回调
	void Chest_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//宝箱回调
	void FirstRecharge_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);//首充回调
	void Automatic_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);	//挂机回调
	void Activity_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//活动回调
	void Reward_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//奖励回调
	void Chat_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//聊天回调
	void Skill1_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//技能1回调
	void Skill2_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//技能2回调
	void Skill3_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//技能3回调
	void Skill4_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//技能4回调
	void SkillXP_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//XP技能回调
	void Drug_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);			//药水回调
	void Horse_callback(cc2d::Ref* pSender , cc2dui::TouchEventType type);		//上下马回调

	cc2d::Node*	m_MainPanel;	//主面板

	class UIMiniMap*  m_pMiniMap;  //小地图

	cc2d::Node* m_roleAtt;		//人物属性
	cc2dui::ImageView* HeroIcon;		//主角头像
	cc2dui::Text* HeroName;				//主角名字
	cc2dui::LoadingBar* HeroHP;			//主角血条
	cc2dui::LoadingBar* HeroMP;			//主角蓝条
	cc2dui::Text* lev_text;				//等级
	cc2dui::TextAtlas* vip_text;		//vip等级
	cc2dui::LoadingBar* Exp;			//经验条

	cc2d::Node* m_targetAtt;	//目标栏背景
	cc2dui::ImageView* TargetIcon;		//目标头像
	cc2dui::Text* TargetName;			//目标名字
	cc2dui::LoadingBar* TargetHP;		//目标血条
	cc2dui::Text* TargetLevel;			//目标等级

	cc2d::Node*	m_BtPanel;		//右上角的BT
	cc2dui::CheckBox*	m_btCheck;		//按钮的checkbox

	cc2dui::ImageView*	m_infobg;		//系统信息背景
	cc2dui::Text*		m_infoContent;	//系统信息内容

	cc2d::Node* m_newEquipment; //新装备提示框
	cc2dui::Button*		m_neBtEquip;	//新装备提示框的装备BT
	cc2dui::Button*		m_neBtClose;	//新装备提示框的关闭BT

	cc2dui::ImageView*	m_taskTipbg;		//提示框背景
	cc2dui::Text*		m_taskTipContent;	//提示框内容

	cc2dui::ImageView*	m_barbg;		//（开箱子）进度条背景
	cc2dui::LoadingBar*	m_bar;			//进度条
	cc2dui::Text*		m_barWord;		//进度条文字

	cc2d::Node*	m_expInfo;		//经验提示
	cc2d::Node*	m_Control;		//控制杆
	cc2d::Node* m_taskTeamPanel;//任务与组队栏
	cc2dui::CheckBox*	m_cdTaskTeam;

	cc2dui::CheckBox*	m_cdTask;		//任务CHECKBOX
	cc2dui::CheckBox*	m_csTeam;		//队伍CHECKBOX
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
	
	cc2dui::ImageView* m_HeroBuff[MaxCountShowBuff];	//主角buff
	cc2dui::ImageView* m_TargetBuff[MaxCountShowBuff];	//目标buff

	//添加任务
	void AddTaskItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,  int32 taskid);
	//添加队友
	void AddTeamItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout, std::string itemName, int32 level, int32 hp, int32 maxHp, bool leader);
    void LoadTeamInfo();
    void UpdateTeamHp();


	map<int32,cc2dui::Layout*> m_mapTaskLayout;
	//点击任务栏
	void TaskClick(int32 taskid);
	//更新任务显示
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

	//点击NPC
	void	ClickNpc();

	//主角人物初始化
	void ProtagonistInit();
	//目标初始化
	void TargetInit();
	//主界面的任务栏
	void TaskInit();
	//经验条
	//void ExpInit();

	//更新聊天框
	void UpdateChat(int32 iChannel,string strName,string strContent);
	vector<MainMessage> m_vecMainMessage;

	cc2d::Node* m_chatPanel;


    enum MenuEnum
    {
        TeamJoin,  //>组队
        TeamKick,  //>踢人
        TeamChangeLeader,  //>队长转让
        TeamDimiss, //队伍解散
        TeamExit,   //退出队伍
        SingleChat, //>私聊
        SeeInfo,    //>查看信息
    };

    //>隐藏玩家菜单
    void HidePlayerMenu();
    //>弹出玩家菜单
    void PopPlayerMenu(const cc2d::Vec2& position, uint64 SelTagGuid);
    //>发起组队
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
//>技能相关
    void InitSkillPanel();  //>初始化技能面板
    void SetupTurntable();  //>设置支持旋转
    void ReloadSkill();     //>重新加载技能图标
    void Update(float deltaTime);   //>更新
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