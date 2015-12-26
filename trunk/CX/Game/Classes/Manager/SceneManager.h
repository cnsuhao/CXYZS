/********************************************************************
created:	2014/12/21
author:		LL
desc:		场景管理：加载、登陆、战斗、主场景。场景间的切换用WaitScene来过渡。
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
    // const Vec2& dir为摇杆的当前的向量(有方向,有大小).
public:
    virtual void OnMoveBegin() = 0;
    virtual void OnMoveEnd() = 0;
    virtual void OnLeft(const cc2d::Vec2& dir) = 0;           // 当角度在(337.5', 22.5')会调用此函数
    virtual void OnUp(const cc2d::Vec2& dir) = 0;             // 当角度在( 67.5',112.5')会调用此函数
    virtual void OnRight(const cc2d::Vec2& dir) = 0;          // 当角度在(157.5',202.5')会调用此函数    
    virtual void OnDown(const cc2d::Vec2& dir) = 0;           // 当角度在(247.5',292.5')会调用此函数
    virtual void OnLeftUp(const cc2d::Vec2& dir) = 0;         // 当角度在( 22.5', 67.5')会调用此函数 
    virtual void OnRightUp(const cc2d::Vec2& dir) = 0;        // 当角度在(112.5',157.5')会调用此函数
    virtual void OnLeftDown(const cc2d::Vec2& dir) = 0;       // 当角度在(292.5',337.5')会调用此函数
    virtual void OnRightDown(const cc2d::Vec2& dir) = 0;      // 当角度在(202.5',247.5')会调用此函数
};

class ListenerLayer : public cc2d::Layer
{
public:
	virtual void onEnter();
	virtual void onExit();
	void onKeyPressed(cc2d::EventKeyboard::KeyCode keyCode, cc2d::Event* event);
	void onKeyReleased(cc2d::EventKeyboard::KeyCode keyCode, cc2d::Event* event);
};

// http 升级版本
class HttpUpdateLayer : public cc2d::Layer , public VersionController::Lister 
{
public:
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float delta);

    virtual void OnChechFinished(bool result);  //>检测版本列表完毕
    virtual void OnUpdateFinished(bool result); //>更新一个版本完毕
protected:
    void SetCurrentVersion();
    void SetLastVersion();
    void SetDownloadInfo();
    class UIHttpUpdate* m_UIupdate;
    bool  m_Waitting;
    float m_DelayTime;
    static const int UpdateDelayTime = 2; 
};

// 登录场景
class LoginLayer : public cc2d::Layer 
{
public:
	virtual void onEnter();
	virtual void onExit();

	cc2d::Sprite * background; //背景
};

// 创建角色场景
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
	void CallBackGetVersion(cc2d::Node *sender ,void *data);	//取网络版本号
	void CallBackGetFileList(cc2d::Node *sender ,void *data);	//取版本更新文件列表
	void CallBackGetFile(cc2d::Node *sender ,void *data);		//取版本更新文件
	std::vector<std::string> m_VersionList;
	std::vector<std::string> m_FileList;
	std::string m_CurrVersion;
	bool m_LoadEnd;		//加载是否结束
	float m_VersionNum;	//总版本数量
	float m_FileNum;		//单个版本文件数量
	cc2d::Sprite* m_lvse;	//进度条
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
	int copyid;	//副本id
	
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
	// 设置缩放根据屏幕上两个的手指
	void SetScaleFromScreenPoint(const cocos2d::Touch& touche1, const cocos2d::Touch& touche2);
	void MapScale(float newScale);
	void LoadMap(int mapid);				//加载地图
    void LoadMapEffect(const struct MapInfo* map);       //加载地图特效
	void ChangeScene(int mapid,int pos=0);	//切换场景，带坐标
	cc2d::Size GetMapSize();                      //地图大小
    void EnableGrey(bool enable);           //地图灰化

//坐标转换
	cc2d::Vec2 Pos2Tile(const cc2d::Vec2 &pos);			//地图坐标转地图格子
	cc2d::Vec2 Tile2Pos(const cc2d::Vec2 &tile);		//地图格子转地图坐标
	cc2d::Vec2 TileId2Tile(int posid);			//地图格子Id转格子
	int Tile2TileId(const cc2d::Vec2 &pos);	    //地图格子转格子Id

// 摇杆监听
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

	//Layer* m_MapLayer;	//地图父图层，用于缩放地图
	float _currScale;	//地图缩放
	cc2d::TMXTiledMap *m_TileMap;				//tiled地图
	int m_MapId;						//地图id
	std::vector<cc2d::Sprite*> m_ConveyList;	//传送点
	bool isSceneChangeSelected;			//是否选中传送点
    class FollowEx* follow;
 
    int m_MuitiTouchCount;

    // 做相机抖动, 时间, 次数, x范围, y范围
    void DoCameraShake(float duration, int count, float xRang, float yRang);

    void UpdateTeamInfo(float deltaTime);
    float m_TeamInfoDelay;

    class PostRenderNode* m_PostRender;

	//!临时用登陆流程
	int loginstep;
};

class SceneManager : public cc2d::Scene
{
public: 
	SceneManager();
	~SceneManager();	
	virtual void update(float delta);
	void Init();	//初始化
	void Release();	//释放

	void CreateJoystick(); //创建摇杆
    void DestoryJoystick();

	//图层之前的切换
	void Main2Login();	//主场景到登陆	
	void Login2Main();	//登录切换到主场景
    void Login2CreateRole(); // 登录到创建角色
    void CheckHttpUpdate();     //>检测http更新
    void HttpUpdateToLogin();   //>http更新到登陆

	cc2d::Layer *m_BackLayer;	//背景层，不同的图层会有不同的背景
	//图层实例
    HttpUpdateLayer* m_HttpUpdateLayer; //http更新
	LoginLayer *m_LoginLayer;	//登录图层
	LoadLayer *m_LoadLayer;		//加载图层
	MainLayer *m_MainLayer;		//主场景图层
	FightLayer *m_FightLayer;	//战斗图层
    CreateRoleLayer* m_CreateRoleLayer; // 创建角色
	cc2d::Layer *m_CurrLayer;		//记录当前图层
    class CtrlJoystick* m_joystick; // 摇杆
};


extern SceneManager* g_SceneManager;
extern cc2d::Vec2 g_ScreenSize;		//屏幕尺寸
extern cc2d::Vec2 g_ScreenHalfSize;	//半屏
#endif  // __SCENEMANAGER_H__