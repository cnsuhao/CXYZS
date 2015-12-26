/********************************************************************
created:	2014/12/21
author:		LL
desc:		UI�������ڸ���ui���Ĵ�������ʾ���л�
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
//��½
	UI_AUTOLOGIN = 0,			//���ٵ�½
	UI_DEFAULTSERVER = 1,		//ѡ��
	UI_LOGIN = 2,				//��½
	UI_REGISTER = 3,			//ע��
	UI_AGREEMENT = 4,			//ע��Э��
	UI_ROLECREATE = 5,			//��ɫ����
//������
	UI_MAIN = 6,				//������
    UI_MENU = 7,		        //�ؿ�����
    UI_FABAO = 8,               //����ҳ��
	UI_PLAYERINFO = 9,			//��ɫ��Ϣ
	UI_CHAT = 10,		        //�������
	UI_GUILD = 11,			    //���ɽ���
	UI_BAG = 12,		        //��������
	UI_EQUIP_UPGRADE = 13,		//��������
	UI_WARRIORSKILL = 14,		//�佫����
	UI_FRIENDS = 15,	        //���ѽ���
	UI_TEAM = 16,	            //�������
    UI_EMAIL = 17,		        //�ʼ�
	UI_TASK = 18,			    //����
	UI_AUCTION = 19,		    //����
	UI_GONGFA = 20,		        //����
	UI_SHOP = 21,		        //�̵�
	UI_SETTING = 22,			//����
	UI_STATE = 23,				//����
	UI_WING = 24,				//���
	UI_LIST = 25,				//���а�
	UI_CONSIGN = 26,			//������
    UI_ITEM_INFO = 27,          //��Ʒ��ϸ��Ϣ
	UI_TALK = 28,				//̸������
	UI_RECEIVETASK = 29,		//��ȡ�������
	UI_APPLYGANG = 30,			
	UI_UICHEST = 31,			//������
	UI_UICHESTITEM = 32,		//�����ӽ���
	//UI_DEFAULTSERVER = 23,		//ѡ�������
	//UI_FRIENDS = 23,			//����
	//UI_SELECT_SERVER = 23,		//ѡ��
	//UI_CHAT = 24,				//����
	//UI_MISSION = 25,			//����
	//UI_SETTLEMENT = 26,			//���ؽ���
	//UI_SWEEP = 27,				//ɨ������
	//UI_WAREHOUSE = 28,			//�ֿ�
	//UI_EMBATTLE = 29,			//����
	//UI_EMBATTLELIST = 30,		//�����б�
	//UI_PUB = 31,				//�ƹ�
    UI_WAITTING,                //�ȴ�UI
    UI_HTTP_UPDATE,             //Http����
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
	void OpenUI(UIType type);	//��ui������ui����
	void HideUI(UIType type);	//����ui������ui����
	void RemoveUI(UIType type);	//�Ƴ�ui������ui����
	void RemoveAllUI();			//�Ƴ�ui��һ���ڳ����л�ʱ����
	UIBase *GetUI(UIType type);	//ȡuiʵ��������ui����
    bool IsOpened(UIType type); //�Ƿ��Ѿ��ȴ�
    
    void DisabledJoystick();    //>����ҡ��
    void EnabledJoystick();     //>����ҡ��
    bool TouchJoystickInRange(cc2d::Touch *touch);  //>���һ��touch���Ƿ�����ҡ�˷�Χ
    void CheckHideMainPannel(); //>�������������
    void CheckShowMainPannel(UIBase* pannel); //>�����ʾ������
	void AddChestItem(CliItemInfo* pItem);

    void Notify(const std::string& notice, const cc2d::Color3B& color); //>��ʱ��һ������֪ͨ.
    void NotifyAnimaCallback(cc2d::Label* pLabel);  //>�����ص�
	
	UIMsgBox* ShowMsgBox(const char* msg, bool isutf8 = true, MsgBoxEventType eventtype = MSGBOX_EVENT_NORMAL, MsgBoxStyle style = MSGBOX_STYLE_CONFIRM, long long value = -1);

	UIMsgBox* ShowMsgBox(int id, bool isutf8 = false, MsgBoxEventType eventtype = MSGBOX_EVENT_NORMAL, MsgBoxStyle style = MSGBOX_STYLE_CONFIRM, long long value = -1);

	UIType m_CurrUI;		//��ǰUI
	//����UI���ʵ��
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