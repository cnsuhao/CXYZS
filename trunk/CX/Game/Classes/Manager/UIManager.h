/********************************************************************
created:	2014/12/21
author:		LL
desc:		UI管理：用于各个ui面板的创建、显示、切换
*********************************************************************/
#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#include "ClientDef.h"
#include "CCLayer.h"
#include "UIMsgBox.h"

#include <queue>


using namespace std;
struct CliItemInfo;

enum UIType
{
//登陆
	UI_AUTOLOGIN = 0,			//快速登陆
	UI_DEFAULTSERVER = 1,		//选服
	UI_LOGIN = 2,				//登陆
	UI_REGISTER = 3,			//注册
	UI_AGREEMENT = 4,			//注册协议
	UI_ROLECREATE = 5,			//角色创建
//主界面
	UI_MAIN = 6,				//主界面
    UI_MENU = 7,		        //关卡介绍
    UI_FABAO = 8,               //法宝页面
	UI_PLAYERINFO = 9,			//角色信息
	UI_CHAT = 10,		        //聊天界面
	UI_GUILD = 11,			    //帮派界面
	UI_BAG = 12,		        //背包界面
	UI_EQUIP_UPGRADE = 13,		//武器升级
	UI_WARRIORSKILL = 14,		//武将技能
	UI_FRIENDS = 15,	        //好友界面
	UI_TEAM = 16,	            //队伍界面
    UI_EMAIL = 17,		        //邮件
	UI_TASK = 18,			    //任务
	UI_AUCTION = 19,		    //寄售
	UI_GONGFA = 20,		        //功法
	UI_SHOP = 21,		        //商店
	UI_SETTING = 22,			//设置
	UI_STATE = 23,				//境界
	UI_WING = 24,				//翅膀
	UI_LIST = 25,				//排行榜
	UI_CONSIGN = 26,			//寄售行
    UI_ITEM_INFO = 27,          //物品详细信息
	UI_TALK = 28,				//谈话界面
	UI_RECEIVETASK = 29,		//领取任务界面
	UI_APPLYGANG = 30,			
	UI_UICHEST = 31,			//抽箱子
	UI_UICHESTITEM = 32,		//抽箱子结算
	//UI_DEFAULTSERVER = 23,		//选择服务器
	//UI_FRIENDS = 23,			//好友
	//UI_SELECT_SERVER = 23,		//选区
	//UI_CHAT = 24,				//聊天
	//UI_MISSION = 25,			//任务
	//UI_SETTLEMENT = 26,			//闯关结算
	//UI_SWEEP = 27,				//扫荡结算
	//UI_WAREHOUSE = 28,			//仓库
	//UI_EMBATTLE = 29,			//布阵
	//UI_EMBATTLELIST = 30,		//布阵列表
	//UI_PUB = 31,				//酒馆
    UI_WAITTING,                //等待UI
    UI_HTTP_UPDATE,             //Http更新
	UI_MAX
};

class UIManager : public cc2d::Layer
{
public:
	UIManager();
	~UIManager();	
	
	void Init();
	void Release();
    void Update(float delta);
	void OpenUI(UIType type);	//打开ui，根据ui类型
	void HideUI(UIType type);	//隐藏ui，根据ui类型
	void RemoveUI(UIType type);	//移除ui，根据ui类型
	void RemoveAllUI();			//移除ui，一般在场景切换时调用
	UIBase *GetUI(UIType type);	//取ui实例，根据ui类型
    bool IsOpened(UIType type); //是否已经先打开
    
    void DisabledJoystick();    //>禁用摇杆
    void EnabledJoystick();     //>启用摇杆
    bool TouchJoystickInRange(cc2d::Touch *touch);  //>检测一个touch点是否属于摇杆范围
    void CheckHideMainPannel(); //>检测隐藏主界面
    void CheckShowMainPannel(UIBase* pannel); //>检测显示主界面
	void AddChestItem(CliItemInfo* pItem);

    void Notify(const std::string& notice, const cc2d::Color3B& color); //>临时的一个弹屏通知.
    void NotifyAnimaCallback(cc2d::Label* pLabel);  //>动画回调
	
	UIMsgBox* ShowMsgBox(const char* msg, bool isutf8 = true, MsgBoxEventType eventtype = MSGBOX_EVENT_NORMAL, MsgBoxStyle style = MSGBOX_STYLE_CONFIRM, long long value = -1);

	UIMsgBox* ShowMsgBox(int id, bool isutf8 = false, MsgBoxEventType eventtype = MSGBOX_EVENT_NORMAL, MsgBoxStyle style = MSGBOX_STYLE_CONFIRM, long long value = -1);

	UIType m_CurrUI;		//当前UI
	//各个UI面板实例
	UIBase * m_UIList[UI_MAX];


    void CreatePlayerInfo() { CreateUI(UI_PLAYERINFO); }
private:
	bool CreateUI(UIType type);
	void ShowUI(UIType type);

    struct NoticeInfo
    {
        NoticeInfo(const std::string content_, const cc2d::Color3B& color_):content(content_), color(color_)
        { }

        std::string content;
        cc2d::Color3B color;
    };

    std::queue<NoticeInfo*> m_NoticeLabels;
    std::queue<cc2d::Label*> m_FreeLabels;
    cc2d::Vec2    m_Start;
    float   m_FontSize;
    float   m_ElapseTime;
    float   m_DelayTime;
};

extern UIManager* g_UIManager;

#endif