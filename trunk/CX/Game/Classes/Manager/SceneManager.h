/********************************************************************
created:	2014/12/21
author:		LL
desc:		�����������ء���½��ս��������������������л���WaitScene�����ɡ�
*********************************************************************/
#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "ClientDef.h"
#include "CCLayer.h"
#include "CCScene.h"
#include "VersionController.h"
#include "CCTMXTiledMap.h"
#include "CCFastTMXTiledMap.h"

class JoystickListener
{
    // const Vec2& dirΪҡ�˵ĵ�ǰ������(�з���,�д�С).
public:
    virtual void OnMoveBegin() = 0;
    virtual void OnMoveEnd() = 0;
    virtual void OnLeft(const cc2d::Vec2& dir) = 0;           // ���Ƕ���(337.5', 22.5')����ô˺���
    virtual void OnUp(const cc2d::Vec2& dir) = 0;             // ���Ƕ���( 67.5',112.5')����ô˺���
    virtual void OnRight(const cc2d::Vec2& dir) = 0;          // ���Ƕ���(157.5',202.5')����ô˺���    
    virtual void OnDown(const cc2d::Vec2& dir) = 0;           // ���Ƕ���(247.5',292.5')����ô˺���
    virtual void OnLeftUp(const cc2d::Vec2& dir) = 0;         // ���Ƕ���( 22.5', 67.5')����ô˺��� 
    virtual void OnRightUp(const cc2d::Vec2& dir) = 0;        // ���Ƕ���(112.5',157.5')����ô˺���
    virtual void OnLeftDown(const cc2d::Vec2& dir) = 0;       // ���Ƕ���(292.5',337.5')����ô˺���
    virtual void OnRightDown(const cc2d::Vec2& dir) = 0;      // ���Ƕ���(202.5',247.5')����ô˺���
};

class ListenerLayer : public cc2d::Layer
{
public:
	virtual void onEnter();
	virtual void onExit();
	void onKeyPressed(cc2d::EventKeyboard::KeyCode keyCode, cc2d::Event* event);
	void onKeyReleased(cc2d::EventKeyboard::KeyCode keyCode, cc2d::Event* event);
};

// http �����汾
class HttpUpdateLayer : public cc2d::Layer , public VersionController::Lister 
{
public:
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float delta);

    virtual void OnChechFinished(bool result);  //>���汾�б����
    virtual void OnUpdateFinished(bool result); //>����һ���汾���
protected:
    void SetCurrentVersion();
    void SetLastVersion();
    void SetDownloadInfo();
    class UIHttpUpdate* m_UIupdate;
    bool  m_Waitting;
    float m_DelayTime;
    static const int UpdateDelayTime = 2; 
};

// ��¼����
class LoginLayer : public cc2d::Layer 
{
public:
	virtual void onEnter();
	virtual void onExit();

	cc2d::Sprite * background; //����
};

// ������ɫ����
class CreateRoleLayer : public cc2d::Layer
{
public:
    virtual void onEnter();
    virtual void onExit();
};

class LoadLayer : public cc2d::Layer
{
public:
	virtual void onEnter();
	virtual void onExit();
	virtual void update(float delta);

	bool GetExitFileList();
	bool GetExitFileName();
	void CallBackGetVersion(cc2d::Node *sender ,void *data);	//ȡ����汾��
	void CallBackGetFileList(cc2d::Node *sender ,void *data);	//ȡ�汾�����ļ��б�
	void CallBackGetFile(cc2d::Node *sender ,void *data);		//ȡ�汾�����ļ�
	std::vector<std::string> m_VersionList;
	std::vector<std::string> m_FileList;
	std::string m_CurrVersion;
	bool m_LoadEnd;		//�����Ƿ����
	float m_VersionNum;	//�ܰ汾����
	float m_FileNum;		//�����汾�ļ�����
	cc2d::Sprite* m_lvse;	//������
	cc2d::Sprite *back_1;
	cc2d::Sprite *back_2;
	float temp;
};

class FightLayer : public cc2d::Layer
{
public:
	virtual void onEnter();
	virtual void onExit();
	virtual void update(float delta);
	int copyid;	//����id
	
};

class MainLayer : public cc2d::Layer, public JoystickListener
{
public:
	MainLayer();
	~MainLayer();	
	virtual void onEnter();
	virtual void onExit();
	virtual void update(float delta);
	bool OnTouchesBegan(cocos2d::Touch* touch, cocos2d::Event *event);
	void OnTouchesMoved(cocos2d::Touch* touch, cocos2d::Event *event);
	void OnTouchesEnded(cocos2d::Touch* touch, cocos2d::Event *event);
	void OnMuitiTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
#ifdef _WIN32
	void OnMouseScroll(cc2d::Event* event);
#endif // _WIN32
	// �������Ÿ�����Ļ����������ָ
	void SetScaleFromScreenPoint(const cocos2d::Touch& touche1, const cocos2d::Touch& touche2);
	void MapScale(float newScale);
	void LoadMap(int mapid);				//���ص�ͼ
    void LoadMapEffect(const struct MapInfo* map);       //���ص�ͼ��Ч
	void ChangeScene(int mapid,int pos=0);	//�л�������������
	cc2d::Size GetMapSize();                      //��ͼ��С
    void EnableGrey(bool enable);           //��ͼ�һ�

//����ת��
	cc2d::Vec2 Pos2Tile(const cc2d::Vec2 &pos);			//��ͼ����ת��ͼ����
	cc2d::Vec2 Tile2Pos(const cc2d::Vec2 &tile);		//��ͼ����ת��ͼ����
	cc2d::Vec2 TileId2Tile(int posid);			//��ͼ����Idת����
	int Tile2TileId(const cc2d::Vec2 &pos);	    //��ͼ����ת����Id

// ҡ�˼���
    virtual void OnMoveBegin();
    virtual void OnMoveEnd();
    virtual void OnLeft(const cc2d::Vec2& dir);
    virtual void OnUp(const cc2d::Vec2& dir);
    virtual void OnRight(const cc2d::Vec2& dir);
    virtual void OnDown(const cc2d::Vec2& dir);
    virtual void OnLeftUp(const cc2d::Vec2& dir);
    virtual void OnRightUp(const cc2d::Vec2& dir);
    virtual void OnLeftDown(const cc2d::Vec2& dir);
    virtual void OnRightDown(const cc2d::Vec2& dir);
    //void JoystickMove(const Vec2& modifyTile);
    bool m_joystickTouch;

	//Layer* m_MapLayer;	//��ͼ��ͼ�㣬�������ŵ�ͼ
	float _currScale;	//��ͼ����
	cc2d::TMXTiledMap *m_TileMap;				//tiled��ͼ
	int m_MapId;						//��ͼid
	std::vector<cc2d::Sprite*> m_ConveyList;	//���͵�
	bool isSceneChangeSelected;			//�Ƿ�ѡ�д��͵�
    class FollowEx* follow;
 
    int m_MuitiTouchCount;

    // ���������, ʱ��, ����, x��Χ, y��Χ
    void DoCameraShake(float duration, int count, float xRang, float yRang);

    void UpdateTeamInfo(float deltaTime);
    float m_TeamInfoDelay;

    class PostRenderNode* m_PostRender;

	//!��ʱ�õ�½����
	int loginstep;
};

class SceneManager : public cc2d::Scene
{
public: 
	SceneManager();
	~SceneManager();	
	virtual void update(float delta);
	void Init();	//��ʼ��
	void Release();	//�ͷ�

	void CreateJoystick(); //����ҡ��
    void DestoryJoystick();

	//ͼ��֮ǰ���л�
	void Main2Login();	//����������½	
	void Login2Main();	//��¼�л���������
    void Login2CreateRole(); // ��¼��������ɫ
    void CheckHttpUpdate();     //>���http����
    void HttpUpdateToLogin();   //>http���µ���½

	cc2d::Layer *m_BackLayer;	//�����㣬��ͬ��ͼ����в�ͬ�ı���
	//ͼ��ʵ��
    HttpUpdateLayer* m_HttpUpdateLayer; //http����
	LoginLayer *m_LoginLayer;	//��¼ͼ��
	LoadLayer *m_LoadLayer;		//����ͼ��
	MainLayer *m_MainLayer;		//������ͼ��
	FightLayer *m_FightLayer;	//ս��ͼ��
    CreateRoleLayer* m_CreateRoleLayer; // ������ɫ
	cc2d::Layer *m_CurrLayer;		//��¼��ǰͼ��
    class CtrlJoystick* m_joystick; // ҡ��
};


extern SceneManager* g_SceneManager;
extern cc2d::Vec2 g_ScreenSize;		//��Ļ�ߴ�
extern cc2d::Vec2 g_ScreenHalfSize;	//����
#endif  // __SCENEMANAGER_H__