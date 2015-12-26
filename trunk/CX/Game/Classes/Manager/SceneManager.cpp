#include "SceneManager.h"

USING_NS_CC;

#include "LogicManager.h"
#include "NetManager.h"
#include "BaseFunction.h"
#include "ResManager.h"
#include "UIManager.h"
#include "UIMain.h"
#include "CSVFileManager.h"
#include "cocos-ext.h"
#include "CocoStudio.h"
#include "NPC.h"
#include "Hero.h"
#include "Monster.h"
#include "AStar.h"
#include "CCArmatureDataManager.h"
#include "CCArmature.h"
#include "PostRenderNode.h"
#include "GreyScaleFilter.h"
#include "BlurFilter.h"
#include "FollowEx.h"
#include "CtrlJoystick.h"
#include "UIHttpUpdate.h"
#include "UIWaitting.h"
#include "UIFriends.h"
#include "UIMiniMap.h"
#include "Collect.h"

using namespace cc2dui;

using namespace cocostudio;
using namespace cocostudio::timeline;

class NPC;

SceneManager *g_SceneManager = NULL;
Vec2 g_ScreenSize;
Vec2 g_ScreenHalfSize;
//Director::getInstance()->getVisibleSize();
//Director::getInstance()->getVisibleOrigin();
void ListenerLayer::onEnter()
{
	Layer::onEnter();

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(ListenerLayer::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(ListenerLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void ListenerLayer::onExit()
{

	Layer::onExit();
}

void ListenerLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	//log("Key with keycode %d pressed", keyCode);
}

void ListenerLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_BREAK:
	case EventKeyboard::KeyCode::KEY_ESCAPE:
	//case EventKeyboard::KeyCode::KEY_BACKSPACE:
		{
			g_UIManager->ShowMsgBox(1,false,MSGBOX_EVENT_EXIT, MSGBOX_STYLE_CONFIRMANDCANCEL);
		}
		break;
	case EventKeyboard::KeyCode::KEY_MENU:
		{
			//g_UIManager->ShowMsgBox("KEY_MENU");
		}
		break;
	default:
		break;
	}	
}

void HttpUpdateLayer::onEnter()
{
    Layer::onEnter();

    VersionController::GetInstance().SetLister(this);
    VersionController::GetInstance().CheckVersion();
    scheduleUpdate();

    g_UIManager->OpenUI(UI_HTTP_UPDATE);
    m_UIupdate = (UIHttpUpdate*)g_UIManager->GetUI(UI_HTTP_UPDATE);

    SetCurrentVersion();
    SetLastVersion();
    SetDownloadInfo();

    m_DelayTime = 0;
    g_UIManager->OpenUI(UI_WAITTING);
    UIWaitting* pUiWait= (UIWaitting*)g_UIManager->GetUI(UI_WAITTING);
    if (pUiWait)
    {
        pUiWait->SetTextID(40);
    }
    m_Waitting = true;
}

void HttpUpdateLayer::onExit()
{
    g_UIManager->RemoveUI(UI_HTTP_UPDATE);
}

void HttpUpdateLayer::update(float delta)
{

    m_DelayTime += delta;
    if (m_DelayTime > UpdateDelayTime)
    {
        if (m_Waitting)
        {
            g_UIManager->HideUI(UI_WAITTING);
            m_Waitting = false;
        }
        
        VersionController::GetInstance().Update(delta);
        SetDownloadInfo();
    }
}

void HttpUpdateLayer::OnChechFinished(bool result)
{
    if (result)
    {
        SetLastVersion();
        int local = VersionController::GetInstance().GetLocalVersion();
        int next = VersionController::GetInstance().GetNextVerion(local);
        if (next != local)
        {
            VersionController::GetInstance().UpdateToVersion(next);
        }
        else
        {
            // 已经是最新版本
            g_SceneManager->HttpUpdateToLogin();
        }
    }
    else
    {
        // 链接服务器失败......
        log("连接服务器失败......");
        g_UIManager->ShowMsgBox(38);
    }
}

void HttpUpdateLayer::OnUpdateFinished(bool result)
{
    if (result)
    {
        int local = VersionController::GetInstance().GetLocalVersion();
        int next = VersionController::GetInstance().GetNextVerion(local);
        if (next != local)
        {
            VersionController::GetInstance().UpdateToVersion(next);
        }
        else
        {
            // 已经是最新版本
            g_SceneManager->HttpUpdateToLogin();
        }
    }
    else
    {
        // 更新失败......
        log("更新失败,文件中断......");
        g_UIManager->ShowMsgBox(39);
    }
}

void HttpUpdateLayer::SetCurrentVersion()
{
    int local = VersionController::GetInstance().GetLocalVersion();
    int major = local / 10000;
    int minor = (local % 10000) / 100;
    log("cur major=%d minor=%d", major, minor);
    if(m_UIupdate != NULL)
        m_UIupdate->SetCurrentVersion(StringUtils::format("%d.%02d", major, minor));
}

void HttpUpdateLayer::SetLastVersion()
{
    int last = VersionController::GetInstance().GetLastVerion();
    int major = last / 10000;
    int minor = (last % 10000) / 100;
    log("last major=%d minor=%d", major, minor);
    if(m_UIupdate != NULL)
        m_UIupdate->SetLastVerstion(StringUtils::format("%d.%02d", major, minor));
}

void HttpUpdateLayer::SetDownloadInfo()
{
    if(m_UIupdate != NULL)
    {
        int cur = VersionController::GetInstance().GetDownloadInfo();
        int max = VersionController::GetInstance().GetMaxDownloadInfo();
        log("info cur=%d max=%d", cur, max);
        m_UIupdate->SetDownloadSize(cur, max);
        m_UIupdate->SetDownloadPercent(cur);
    }
}

////////////////////////////////////////////////////////////////
void LoginLayer::onEnter()
{
	Layer::onEnter();

    g_UIManager->OpenUI(UI_LOGIN);

	//g_UIManager->OpenUI(UI_DEFAULTSERVER);

    //g_ResManager->StopAllSound();
	//g_ResManager->PlaySound(1);

	/*background = Sprite::create("UI/splash.jpg");
	addChild(background);
	background->setPosition(Point(g_ScreenSize.x/2,g_ScreenSize.y/2));*/
	//background->setColor(Color3B(80,80,80));

	//todowj
	//auto label = LabelTTF::create();
	//label->setString(BaseFunction::getImei());
	//label->setPosition(Point(g_ScreenSize.x/2, g_ScreenSize.y-50));
	//label->setColor(ccRED);
	//label->setFontSize(35);
	//this->addChild(label);
}
void LoginLayer::onExit()
{
	 Director::getInstance()->getTextureCache()->removeUnusedTextures();

	 Layer::onExit();
}

void CreateRoleLayer::onEnter()
{
    Layer::onEnter();

    g_UIManager->OpenUI(UI_ROLECREATE);
}

void CreateRoleLayer::onExit()
{
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
    Layer::onExit();
}


////////////////////////////////////////////////////////////////

void LoadLayer::onEnter()
{
	Layer::onEnter();	

	m_LoadEnd = false;
	m_lvse = NULL;
	m_VersionNum = 0;
	m_FileNum = 0;
	temp = 0.0f;
	scheduleUpdate();
}
void LoadLayer::onExit()
{
	Layer::onExit();
}

void LoadLayer::update(float delta)
{	
	
}

bool LoadLayer::GetExitFileList()
{	
	/*if (m_VersionList.size() > 0)
	{
		std::vector<std::string>::iterator it = m_VersionList.end() - 1;
		char Url[256];
		if (m_CurrVersion.size() > 0)
		{
			g_CSVFileManager->m_UserDate.version = atoi(m_CurrVersion.c_str());
		}
		m_CurrVersion = it->c_str();
		sprintf(Url, "http://www.vrmk.com/txwl/loading/%s/fileList.xml", it->c_str());
		extension::HttpRequest* request = new extension::HttpRequest();  
		request->setUrl(Url);  
		request->setRequestType(extension::HttpRequest::kHttpPost);  
		request->setTag(it->c_str());
		const char* postData = "1";  
		request->setRequestData(postData ,strlen(postData));  
		request->setResponseCallback(this, callfuncND_selector(LoadScene::CallBackGetFileList)); 	 
		extension::CCHttpClient::getInstance()->send(request);  
		request->release();
		m_VersionList.erase(it);
	}
	else
	{
		if (m_CurrVersion.size() > 0)
		{
			g_CSVFileManager->m_UserDate.version = atoi(m_CurrVersion.c_str());
		}
		return false;
	}*/
	return true;
}

bool LoadLayer::GetExitFileName()
{
	/*std::vector<std::string>::iterator it = m_FileList.begin();
	if (it != m_FileList.end())
	{
		char Url[256];
		sprintf(Url, "http://www.vrmk.com/txwl/loading/%s/%s", m_CurrVersion.c_str(), it->c_str());
		extension::HttpRequest* request = new extension::HttpRequest();  
		request->setUrl(Url);  
		request->setRequestType(extension::HttpRequest::kHttpPost);  
		request->setTag(it->c_str());
		const char* postData = "1";  
		request->setRequestData(postData ,strlen(postData));  
		request->setResponseCallback(this, callfuncND_selector(LoadScene::CallBackGetFile)); 	 
		extension::CCHttpClient::getInstance()->send(request);  
		request->release();
		m_FileList.erase(it);
	}
	else if (!GetExitFileList())
	{
		return false;
	}*/
	return true;
}
void LoadLayer::CallBackGetVersion(cocos2d::Node *sender ,void *data)
{	
}

void LoadLayer::CallBackGetFileList(cocos2d::Node *sender ,void *data)
{
}

void LoadLayer::CallBackGetFile(cocos2d::Node *sender ,void *data)
{
}


////////////////////////////////////////////////////////////////

void FightLayer::onEnter()
{
	Layer::onEnter();
	g_LogicManager->EnterCopy(1);
	scheduleUpdate();
}
void FightLayer::onExit()
{
	Director::getInstance()->getTextureCache()->removeUnusedTextures();

	Layer::onExit();
}

void FightLayer::update(float delta)
{		
}


////////////////////////////////////////////////////////////////
MainLayer::MainLayer():m_TileMap(NULL),m_MapId(0)
{
	isSceneChangeSelected = false;
    m_MuitiTouchCount = 0;
	loginstep = 0;
    m_joystickTouch = false;
    m_TeamInfoDelay = 0;
    m_PostRender = NULL;
}
MainLayer::~MainLayer()
{
}

void MainLayer::onEnter()
{
	_currScale = 1.0f;
	Layer::onEnter();
	//增加多点触摸事件回调
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesMoved = CC_CALLBACK_2(MainLayer::OnMuitiTouchesMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	//增加触摸事件回调
	auto listener1 = EventListenerTouchOneByOne::create();
	listener1->onTouchBegan = CC_CALLBACK_2(MainLayer::OnTouchesBegan, this);
	listener1->onTouchMoved = CC_CALLBACK_2(MainLayer::OnTouchesMoved, this);
	listener1->onTouchEnded = CC_CALLBACK_2(MainLayer::OnTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);

#ifdef _WIN32
	auto listenerMouse = EventListenerMouse::create();
	listenerMouse->onMouseScroll = CC_CALLBACK_1(MainLayer::OnMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerMouse, this);
#endif // _WIN32 

	setTouchEnabled(true);
	setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
    
    if (g_SceneManager->m_joystick == NULL)
        g_SceneManager->CreateJoystick();
 
    if (g_SceneManager->m_joystick)
    {
        g_SceneManager->m_joystick->SetListener(this);
        g_SceneManager->m_joystick->SetEnabled(true);
    }
	
    scheduleUpdate();
}
                                                                                           
void MainLayer::onExit()
{
	Layer::onExit();
}
bool MainLayer::OnTouchesBegan(cocos2d::Touch* touch, cocos2d::Event *event)
{
	return true;
}

void MainLayer::OnTouchesMoved(cocos2d::Touch* touch, cocos2d::Event *event)
{

}

void MainLayer::OnTouchesEnded(cocos2d::Touch* touch, cocos2d::Event *event)
{
    if (m_MuitiTouchCount-- > 0)
        return;

	if (!g_LogicManager->m_Hero)
	{
		return;
	}

    if (!(g_SceneManager->m_joystick == NULL || (g_SceneManager->m_joystick && g_SceneManager->m_joystick->GetEnabled()) )) //不可以移动,不可交互
    {
        return;
    }

    if (g_SceneManager->m_joystick->GetJoyDir() == -1 && m_joystickTouch)
    {
        m_joystickTouch = false;
        return;
    }

    if (((UIMain*)g_UIManager->GetUI(UI_MAIN)) != NULL)
    {
        ((UIMain*)g_UIManager->GetUI(UI_MAIN))->HidePlayerMenu();   //隐藏弹出菜单
    }

	isSceneChangeSelected = false; //复位场景切换选择标志

	Vec2 touchEndedPosition = touch->getLocation();
	touchEndedPosition = touchEndedPosition - m_TileMap->getPosition();

    Vec2 Pos;
	//遍历Player队列,设置当前选中的玩家
	for (vector<Player *>::iterator iter = g_LogicManager->m_PlayerList.begin();iter != g_LogicManager->m_PlayerList.end();iter++)
	{
		if ((*iter)->m_AnimCompoenet->IsSelection(touch->getLocation()))
		{
			g_LogicManager->m_Hero->m_SelTarget = *iter;
			((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
			return;
		}
	}

	//遍历Npc队列,设置当前选中的npc
	for (vector<NPC*>::iterator iter = g_LogicManager->m_NPCList.begin();iter != g_LogicManager->m_NPCList.end();iter++)
	{		
		if ((*iter)->m_AnimCompoenet->IsSelection(touch->getLocation()))
		{			
			g_LogicManager->m_Hero->m_SelTarget = *iter;
			((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
			return;			
		}
	}
	//遍历怪物队列,设置当前选中的怪物
	//for (vector<Monster *>::iterator iter = g_LogicManager->m_MonsterList.begin();iter != g_LogicManager->m_MonsterList.end();iter++)
	//{
	//	if ((*iter)->IsSelection(touch->getLocation()))
	//	{
	//		g_LogicManager->m_Hero->m_SelTarget = *iter;
	//		((UIMain*)g_UIManager->GetUI(UI_MAIN))->ChangeSelTarget();
	//		return;
	//	}
	//}

	//遍历采集物队列
	//for (vector<Collect *>::iterator iter = g_LogicManager->m_Collect.begin();iter != g_LogicManager->m_Collect.end();iter++)
	//{
	//	if ((*iter)->IsSelection(touch->getLocation()))
	//	{
	//		//g_LogicManager->m_Hero->m_SelTarget = *iter;

	//		//遍历任务表
	//		map<int32,TaskInfo>::iterator itt;
	//		for(itt = g_CSVFileManager->m_TaskInfoMap.begin(); itt != g_CSVFileManager->m_TaskInfoMap.end() ; itt++)
	//		{
	//			if (itt->second.iTermsType == TASK_TERMS_COLLECT)
	//			{
	//				TaskInfoIt *tempTaskInfoIt = g_LogicManager->m_Hero->GetTaskInfoItByID(itt->first);
	//				if (tempTaskInfoIt)
	//				{
	//					if (tempTaskInfoIt->state == TASK_STATE_HAVETASK)
	//					{
	//						
	//						((UIMain*)g_UIManager->GetUI(UI_MAIN))->PointClickCollect((*iter)->m_GUID,itt->second.iTermsParam[0]);
	//					}
	//				}
	//			}
	//		}
	//		return;
	//	}
	//}

	Vec2 tagPos = m_TileMap->convertTouchToNodeSpace(touch);
	//选中传送点
	for (std::vector<Sprite*>::iterator it = m_ConveyList.begin(); it != m_ConveyList.end(); ++it)
	{	
		if ((*it)->getBoundingBox().containsPoint(tagPos))		
		{
			isSceneChangeSelected = true;
			Rect rect= g_LogicManager->m_Hero->getBoundingBox();

			/*for (int i = 0; i < CONVEY_NUM_MAX; i++)
			{
				if (m_Convey[i] && rect.intersectsRect(m_Convey[i]->getBoundingBox()))
				{
					isSceneChangeSelected = false;						
					break;
				}
			}*/
			break;
		}
	}


    if (g_LogicManager->m_Hero->IsSkilling())
    {
        g_UIManager->Notify(BaseFunction::a2u("技能尚未结束!"), Color3B::RED);
    }
    else
    {
        //点地板
        Vec2 tagTile = Pos2Tile(tagPos);
        if (g_LogicManager->m_Hero->FindPath(tagPos))
        {
            g_ResManager->ShowSprite(5, m_TileMap, tagTile.y, tagPos);
        }
    }
}

void MainLayer::OnMuitiTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event)
{
	if(touches.size() >= 2)
	{
		SetScaleFromScreenPoint(*touches[0], *touches[1]);
        m_MuitiTouchCount = touches.size();
	}
}

#ifdef _WIN32
void MainLayer::OnMouseScroll(cc2d::Event* event)
{	
	float delta = 0.0f;
	cc2d::EventMouse* mouseEvent = static_cast<cc2d::EventMouse*>(event);
	if (mouseEvent->getScrollY() > 0)
	{
		delta = 0.9f;
	}
	else if (mouseEvent->getScrollY() < 0)
	{
		delta = 1.1f;
	}
	float newScale = _currScale * delta;
	if (newScale < 0.5f)
	{
		newScale = 0.5f;
		delta = newScale / _currScale;
	}
	else if (newScale > 2.0f)
	{
		newScale = 2.0f;
		delta = newScale / _currScale;
	}

	// 设置缩放
	MapScale(newScale);	
}
#endif // _WIN32

// 设置缩放根据屏幕上两个的手指
void MainLayer::SetScaleFromScreenPoint(const cocos2d::Touch& touche1, const cocos2d::Touch& touche2)
{
	if (NULL == m_TileMap)
		return;

	//得到当前两触摸点
	auto point1 =  touche1.getLocation();
	auto point2 =  touche2.getLocation();	
	// 计算两点之间得距离
	auto currDistance = point1.distance(point2);
	//计算两触摸点上一时刻之间得距离
	auto prevDistance = touche1.getPreviousLocation().distance(touche2.getPreviousLocation());

	//控制缩放比例
	auto delta = currDistance / prevDistance;
	auto newScale = _currScale * delta;
	auto currentPos = getPosition();
	if (newScale < 0.5f)
	{
		newScale = 0.5f;
		delta = newScale / _currScale;
	}
	else if (newScale > 2.0f)
	{
		newScale = 2.0f;
		delta = newScale / _currScale;
	}

	if(_currScale == newScale)
		return;

	// 设置缩放
	MapScale(newScale);	
}

void MainLayer::MapScale(float newScale)
{
	setScale(newScale);
	auto delta = newScale / _currScale;
	_currScale = newScale;

    follow->SetRectScale(newScale);
}

void MainLayer::LoadMap(int mapid)
{
	if (m_MapId == mapid)
	{
		return;
	}
	//地图初始化
	const MapInfo* map = g_CSVFileManager->GetMapByID(mapid);
	if (map)
	{
		m_MapId = mapid;
		//加载地图背景
		if (map->strBackImage != "0")
		{
			Sprite* back = Sprite::create(map->strBackImage, Rect(0,0,g_ScreenSize.x,g_ScreenSize.y));
			back->setPosition(g_ScreenHalfSize);
			g_SceneManager->m_BackLayer->addChild(back, LAYER_BACKGROUND);
		}

        //创建主角
        if (g_LogicManager->m_Hero == NULL)
        {
            g_LogicManager->m_Hero = new Hero();
            g_LogicManager->m_Hero->retain(); //>手动释放
            g_LogicManager->m_Hero->Init();
            g_LogicManager->m_Hero->LoadBaseAttribute(g_LogicManager->m_BaseInfo);
        }
        else
        {
            g_LogicManager->m_Hero->stopAllActions();
            g_LogicManager->m_Hero->Idle();
            g_LogicManager->m_Hero->removeFromParentAndCleanup(false);
        }

		//加载地图
		if (m_TileMap != NULL)
		{
			//移除场景切换点
			m_ConveyList.clear();			

			g_ResManager->StopBgMusic();
			m_TileMap->removeFromParentAndCleanup(true);
			m_TileMap = NULL;
			g_LogicManager->m_PlayerList.clear();
			//g_LogicManager->m_MonsterList.clear();
			//g_LogicManager->m_NPCList.clear();
			Director::getInstance()->getTextureCache()->removeUnusedTextures();
		}
		m_TileMap = TMXTiledMap::create(map->strMapPath); 

        // 创建一个场景post render
        m_PostRender = PostRenderNode::create();
        m_PostRender->addChild(m_TileMap, LAYER_MAP);
        
        // 灰化滤镜
        //GreyScaleFilter* filter = GreyScaleFilter::create();
        //postRenderNode->UseFilter(filter);

        // 模糊滤镜
        //BlurFilter* filter = BlurFilter::create();
        //filter->SetResolution(Size(g_ScreenSize));
        //filter->SetBlurRadius(2.0f);
        //filter->SetSampleNum(5.0f);
        //postRenderNode->UseFilter(filter);

		addChild(m_PostRender, LAYER_MAP); 
		g_ResManager->PlaySound(map->iMusicId);

		//初始化地图阻挡
		TMXLayer *layer = (TMXLayer *)m_TileMap->getLayer("zd");
		if (layer)
		{
			layer->setVisible(false);
			Vec2 pos;
			int index = 0;
			for (int ii = 0; ii < m_TileMap->getMapSize().height; ii++)
			{
				for (int jj = 0; jj < m_TileMap->getMapSize().width; jj++)
				{
					pos.x = jj; pos.y = ii;
					index = ii * m_TileMap->getMapSize().width + jj;
					if (NULL != layer->getTileAt(pos))
					{
						g_astar.m_MapBlockList[ii][jj] = true;
					}
					else
					{
						g_astar.m_MapBlockList[ii][jj] = false;
					}
				}
			}
		}
		//初始化地图遮罩
		TMXLayer *layer2 = (TMXLayer *)m_TileMap->getLayer("zz");
		if (layer2)
		{
			layer2->setVisible(false);
			Vec2 pos;
			int index = 0;
			for (int ii = 0; ii < m_TileMap->getMapSize().height; ii++)
			{
				for (int jj = 0; jj < m_TileMap->getMapSize().width; jj++)
				{
					pos.x = jj; pos.y = ii;
					index = ii * m_TileMap->getMapSize().width + jj;
					if (NULL != layer2->getTileAt(pos))
					{
						g_astar.m_MapMaskList[ii][jj] = true;
					}
					else
					{
						g_astar.m_MapMaskList[ii][jj] = false;
					}
				}
			}
		}
		
		//关闭抗锯齿	
		auto nodelist = m_TileMap->getChildren();
		for (auto i = nodelist.begin(); i != nodelist.end(); i++)
		{
			TMXLayer *child = (TMXLayer*) (*i);
			child->getTexture()->setAliasTexParameters();		
		}
	
        g_LogicManager->m_Hero->SetMapPosition(Vec2(g_LogicManager->m_BaseInfo.posx, g_LogicManager->m_BaseInfo.posy));

        // 添加地图特效
        LoadMapEffect(map);
		m_TileMap->addChild(g_LogicManager->m_Hero);

		//获得初始化任务
		MsgTasks Msg(g_LogicManager->m_Hero->m_GUID,0,0);	
		Msg.MsgID = Msg_Task_GetList;
		g_NetManager->SendMsg(Msg);

		////设置地图跟随主角
		follow = FollowEx::create(g_LogicManager->m_Hero,Rect(0, 0, GetMapSize().width, GetMapSize().height));
		m_TileMap->runAction(follow);	

        UIMain* mainUI = static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN));
        if (mainUI)
        {
			mainUI->m_pMiniMap->ClearAllNode();
            mainUI->SetMiniMap(m_MapId);
        }
        //>重置地图缩放比例
        MapScale(1.0f);
	}	
	loginstep = 1;
}


void MainLayer::LoadMapEffect(const struct MapInfo* map)
{
    int p = map->strMapPath.rfind('.');
    if (p == string::npos)
    {
        log("error : can't find '.' in path(%s) success.\n", map->strMapPath.c_str());
        return;
    }
    string effectFile(map->strMapPath.begin(), map->strMapPath.begin() + p);
    effectFile.append(".effect");
    log("open map effect file(%s).\n", effectFile.c_str());
    Data data = FileUtils::getInstance()->getDataFromFile(effectFile);
    if (data.getSize() > 0)
    {
        log("open map effect file(%s) success.\n", effectFile.c_str());
        int count = 0;
        string content((char*)data.getBytes(), data.getSize());
        istringstream is(content);
        string fileName;
        is >> count;
        for (int i = 0; i < count; ++i)
        {
            Sprite* sprite = Sprite::create();
            int plist, version;
            is >> version;
            is >> fileName >> plist;
            if (plist != 0)
            {
                int p = fileName.rfind('.');
                string fileImage(fileName.begin(), fileName.begin() + p);
                fileImage.append(".png");
                int p2 = fileName.rfind('/');
                string keyName(fileName.begin()+p2+1, fileName.begin() + p);
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fileName, fileImage);
                ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fileName);
                int FrameCount = dict["frames"].asValueMap().size();
                Vector<SpriteFrame*> animFrames(FrameCount);
                char str[64] = {0};
                for (int i = 0; i < FrameCount; i++)
                {
                    sprintf(str, "%s_%d.png", keyName.c_str(), i);
                    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
                    animFrames.pushBack(frame);		
                }
                Animation* animation = Animation::createWithSpriteFrames(animFrames, 1.0f/FrameCount);
                Animate* animate = Animate::create(animation);
                sprite->runAction(RepeatForever::create(animate));
            }
            else
            {
                sprite->setTexture(fileName);
            }

            Vec2 t2;
            int zOrder;
            float t;

            is >> t2.x >> t2.y; sprite->setAnchorPoint(t2);
            is >> t; sprite->setPositionX(t);
            is >> t; sprite->setPositionY(t);
            is >> t; sprite->setScaleX(t);
            is >> t; sprite->setScaleY(t);
            is >> t; sprite->setRotation(t);
            is >> t; sprite->setOpacity((GLubyte)(t * 255.0f));
            is >> zOrder;
            if (zOrder > 0)
            {
                zOrder = 10000 - (int)sprite->getPositionY() + zOrder;
            }
            m_TileMap->addChild(sprite, zOrder);
        }
    }
}

void MainLayer::ChangeScene(int mapid,int pos)
{
}

void MainLayer::update(float delta)
{	
	//!如果选中了传送点，判断是否碰撞
	if (isSceneChangeSelected)
	{
		Rect rect= g_LogicManager->m_Hero->getBoundingBox();
		for (std::vector<Sprite*>::iterator it = m_ConveyList.begin(); it != m_ConveyList.end(); ++it)
		{	
			if (rect.intersectsRect((*it)->getBoundingBox()))
			{
				const MapInfo* map = g_CSVFileManager->GetMapByID(m_MapId);
				if (map)
				{
					isSceneChangeSelected = false;
					const ConveyInfo* info = g_CSVFileManager->GetConveyByID((*it)->getTag());
					if (info)
					{
                        //g_LogicManager->m_Hero->MoveEnd();
                        g_LogicManager->m_Hero->m_SelTarget = NULL;
                        //>发送消息
                        MsgRole_EnterMap enterMapMsg;
                        enterMapMsg.mapId = info->iTargetMapid;
                        enterMapMsg.conveyId = info->id;
                        g_NetManager->SendMsg(enterMapMsg);
					}					
					break;
				}
			}
		}
	}

    UpdateTeamInfo(delta);

	switch (loginstep)
	{
	case 1:
		{
            g_UIManager->OpenUI(UI_MAIN);

			//初始化传送点			
			std::map<int32, ConveyInfo>::iterator it = g_CSVFileManager->m_ConveyMap.begin();
			for (;it != g_CSVFileManager->m_ConveyMap.end(); ++it)
			{
				if (it->second.iMapid == m_MapId)
				{
                    Vec2 position = Tile2Pos(it->second.pConvey);
					Sprite * scene_sprite = g_ResManager->CreateSprite(4, position);
					scene_sprite->setAnchorPoint(Vec2(0.5f,0.1f));
					scene_sprite->setTag(it->first);
					m_TileMap->addChild(scene_sprite, 10000 - (int)position.y - 1);
					m_ConveyList.push_back(scene_sprite);

                    auto uiMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
                    if (uiMain && uiMain->m_pMiniMap)
                    {
                        uiMain->m_pMiniMap->AddNode(scene_sprite, 3);
                    }
				}
			}
			loginstep++;
		}
		break;
	case 2:
		{		
			loginstep++;
		}
		break;
	case 3:
		{
			//加载npc到地图
			std::map<int, NPCInfo>::iterator it = g_CSVFileManager->m_NPCInfoMap.begin();
			for (; it != g_CSVFileManager->m_NPCInfoMap.end(); it++)
			{			
				if (it->second.iMapid == m_MapId)
				{
					g_LogicManager->AddNPCToMap(&(it->second));
				}
			}

			////加载采集物到地图
			//std::map<int,ResInfo>::iterator itres = g_CSVFileManager->m_ResInfoMap.begin();
			//g_LogicManager->m_Collect.clear();
			//for (; itres != g_CSVFileManager->m_ResInfoMap.end(); itres++)
			//{
			//	if (itres->second.iMapid == m_MapId)
			//	{
			//		g_LogicManager->AddCollectToMap(&(itres->second));
			//	}
			//}

			loginstep++;
		}
		break;
	case 4:
		{
			loginstep++;
		}
		break;
	default:		
		break;
	}
}
//cocos2d-x坐标系转换为tilemap坐标系
Vec2 MainLayer::Pos2Tile(const Vec2 &pos)
{
	float x = pos.x / m_TileMap->getTileSize().width;
	float y = (m_TileMap->getMapSize().height - (pos.y / m_TileMap->getTileSize().height));
	return Vec2(floor(x), floor(y));
}
//tilemap坐标系转换为cocos2d-x坐标系
Vec2 MainLayer::Tile2Pos(const Vec2 &tile)
{
	float x = tile.x*m_TileMap->getTileSize().width + m_TileMap->getTileSize().width/2;
	float y = (m_TileMap->getMapSize().height * m_TileMap->getTileSize().height) - (tile.y + 0.5f)*m_TileMap->getTileSize().height;
	return Vec2(x, y);
}

Size MainLayer::GetMapSize()
{
    if (m_TileMap)
    {
        return m_TileMap->getContentSize();
    }
    return Size::ZERO;
}

void MainLayer::EnableGrey(bool enable)
{
    // 灰化滤镜
    if (enable)
    {
        GreyScaleFilter* filter = GreyScaleFilter::create();
        m_PostRender->UseFilter(filter);
    }else
    {
        m_PostRender->UseFilter(NULL);
    }
}

Vec2 MainLayer::TileId2Tile(int posid)
{
	//128是地图的最大格子数
	Vec2 pos(0,0);
	if (posid != 0)
	{
		pos.x = posid / 128;
		pos.y = posid % 128;
	}
	
	return pos;
}

int MainLayer::Tile2TileId(const Vec2 &pos)	
{
	//128是地图的最大格子数	
	return (int)pos.x * 128 + pos.y;
}

void MainLayer::DoCameraShake(float duration, int count, float xRang, float yRang)
{
    if (getNumberOfRunningActions() > 0)
        return;
    
    float step = duration / (float)count;
    float halfstep = step * 0.5f;
    Vector<FiniteTimeAction*> actions;
    for (float i = 0; i < duration; i += step)
    {
        Vec2 offset(random(-xRang, xRang), random(-yRang, yRang));
        MoveBy* action = MoveBy::create(halfstep, offset);
        actions.pushBack(
            action
            );
        actions.pushBack(
            action->reverse()
            );
    }

    runAction(Sequence::create(actions));
    // 模糊效果
    //PostRenderNode* postRenderNode = static_cast<PostRenderNode*>(m_TileMap->getParent());
    //runAction(BlurAction::create(duration, postRenderNode));
}

void MainLayer::UpdateTeamInfo(float deltaTime)
{
    m_TeamInfoDelay += deltaTime;
    if(m_TeamInfoDelay > 1.0f)
    {
        if (g_LogicManager->m_Hero)
        {
            if (g_LogicManager->m_Hero->m_team.m_Members.size() > 0)
            {
                if(g_UIManager->GetUI(UI_MAIN))
                {
                    static_cast<UIMain*>(g_UIManager->GetUI(UI_MAIN))->UpdateTeamHp();
                }
            }
        }

        m_TeamInfoDelay = 0.0f;
    }
}

void MainLayer::OnMoveBegin()
{
    m_joystickTouch = true;
}

void MainLayer::OnMoveEnd()
{
    if (g_LogicManager->m_Hero && g_LogicManager->m_Hero->IsMoveActionType())
    {
        g_LogicManager->m_Hero->StopMove();
    }
}

void MainLayer::OnLeft(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 0 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
		{
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_LEFT);
           /* static Vec2 modifyVec(-1.0f, 0);
            JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(0);
        }
	}
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}		
	}
}

void MainLayer::OnUp(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 1 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
		{
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_UP);
           /* static Vec2 modifyVec(0, -1.0f);
            JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(1);
        }        
	}
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}	
	}
}

void MainLayer::OnRight(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 2 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
		{
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_RIGHT);
           /* static Vec2 modifyVec(1.0f, 0);
            JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(2);
        }
	}
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}	
	}
}

void MainLayer::OnDown(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 3 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
		{
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_DOWN);
           /* static Vec2 modifyVec(0, 1.0f);
            JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(3);
        }
    }
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}	
	}
}

void MainLayer::OnLeftUp(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 4 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
		{
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_LEFT_UP);
           /* static Vec2 modifyVec(-1.0, -1.0f);
            JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(4);
        }
	}
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}	
	}
}

void MainLayer::OnRightUp(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 5 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
        {
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_RIGHT_UP);
          /*  static Vec2 modifyVec(1.0, -1.0f);
            JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(5);
        }
	}
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}	
	}
}

void MainLayer::OnLeftDown(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 6 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
        {
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_LEFT_DOWN);
			/* static Vec2 modifyVec(-1.0, 1.0f);
			JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(6);
        }
	}
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}	
	}
}

void MainLayer::OnRightDown(const Vec2& dir)
{
    if (dir.length() > 0.3f) // 忽略内圈的移动范围.
    {
        if (g_SceneManager->m_joystick->GetJoyDir() != 7 && (g_LogicManager->m_Hero->IsIdleActionType() || g_LogicManager->m_Hero->IsMoveActionType()))
        {
			g_LogicManager->m_Hero->FindPathByDirection(DIRECTION_RIGHT_DOWN);
				/*  static Vec2 modifyVec(1.0, 1.0f);
				JoystickMove(modifyVec);*/
            g_SceneManager->m_joystick->SetJoyDir(7);
        }
	}
	else
	{
		if (g_SceneManager->m_joystick->GetJoyDir() != -1)
		{
			g_LogicManager->m_Hero->StopMove();
			g_SceneManager->m_joystick->SetJoyDir(-1);
		}	
	}
}
/*
void MainLayer::JoystickMove(const Vec2& modifyTile)
{
    Vec2 tagTile = Pos2Tile(g_LogicManager->m_Hero->getPosition());
    tagTile += modifyTile;
    g_LogicManager->m_Hero->FindPath(tagTile);
}
*/
//////////////////////////////////////////////////////////////
	
SceneManager::SceneManager():m_LoginLayer(NULL),m_LoadLayer(NULL),m_MainLayer(NULL),m_FightLayer(NULL),m_CurrLayer(NULL),m_BackLayer(NULL),m_CreateRoleLayer(NULL)
{
    m_joystick = NULL;
}

SceneManager::~SceneManager()
{
	Release();	
}

void SceneManager::Init()
{	

	//初始化全局对象
	FileUtils::getInstance()->addSearchPath(FileUtils::getInstance()->getWritablePath().c_str(), true);

	g_LogicManager = new LogicManager;
	g_UIManager = new UIManager;
	g_CSVFileManager = new CSVFileManager;
	g_ResManager = new ResManager;
	g_NetManager = new NetManager;

	g_LogicManager->Init();
	g_CSVFileManager->Init();
	g_UIManager->Init();
	g_NetManager->Init();
    g_ResManager->Init();
	addChild(g_UIManager, LAYER_UI);
	//添加监听图层
	auto layer = new ListenerLayer();
	addChild(layer, LAYER_LISTENER);
	layer->release();

	m_BackLayer = Layer::create();
	addChild(m_BackLayer, LAYER_BACKGROUND);


    CheckHttpUpdate();  //http更新
	scheduleUpdate();	//开启update
}

void SceneManager::Release()
{
    DestoryJoystick();
	/*delete g_NetManager;
	delete g_LogicManager;
	delete g_UIManager;
	delete g_CSVFileManager;
	delete g_ResManager;*/
#ifdef WIN32
	Sleep(1000);	//等待
#endif
}

void SceneManager::CreateJoystick()
{
    m_joystick = CtrlJoystick::create("UI2/Main/Joystick_bg.png","UI2/Main/Joystick_bt.png");
    if (m_joystick){
        m_joystick->retain();
        g_UIManager->addChild(m_joystick, 100);
    }
}

void SceneManager::DestoryJoystick()
{
    if (m_joystick){
        g_UIManager->removeFromParent();
        m_joystick->release();
    }
}

void SceneManager::update(float delta)
{
	if (g_NetManager)
	{
		g_NetManager->Update(delta);
	}
	if (g_LogicManager)
	{
		g_LogicManager->Update(delta);
	}
    if (g_UIManager)
    {
        g_UIManager->Update(delta);
    }
}

void SceneManager::Main2Login()
{
	if (m_MainLayer)
	{
		g_UIManager->RemoveAllUI();
		m_MainLayer->removeAllChildrenWithCleanup(true);
		m_MainLayer= NULL;	
	}
	if (!m_LoginLayer)
	{
		m_LoginLayer = new LoginLayer();		
	}
	m_CurrLayer = m_LoginLayer;
	addChild(m_CurrLayer, LAYER_BACKGROUND);

	Director::getInstance()->getTextureCache()->removeUnusedTextures(); 
}

void SceneManager::Login2Main()
{
    // 关闭创建角色
    g_UIManager->RemoveUI(UI_ROLECREATE);
	g_UIManager->RemoveUI(UI_DEFAULTSERVER);
	if (m_LoginLayer)
	{		
		m_LoginLayer->removeAllChildrenWithCleanup(true);
		m_LoginLayer= NULL;
	}
	if (!m_MainLayer)
	{
		m_MainLayer = new MainLayer();	
		m_CurrLayer = m_MainLayer;
		addChild(m_CurrLayer, LAYER_BACKGROUND);
	}

    if (m_MainLayer)
    {
        //载人地图
        m_MainLayer->LoadMap(g_LogicManager->m_BaseInfo.mapID);
    }

	Director::getInstance()->getTextureCache()->removeUnusedTextures(); 
}

void SceneManager::Login2CreateRole()
{
    if (m_LoginLayer)
    {		
        m_LoginLayer->removeAllChildrenWithCleanup(true);
        m_LoginLayer= NULL;
    }
    if (!m_CreateRoleLayer)
    {
        m_CreateRoleLayer = new CreateRoleLayer();
        
    }
    m_CurrLayer = m_CreateRoleLayer;
    addChild(m_CurrLayer, LAYER_BACKGROUND);

    g_UIManager->RemoveUI(UI_DEFAULTSERVER);
}

void SceneManager::CheckHttpUpdate()
{
    m_HttpUpdateLayer = new HttpUpdateLayer();
    m_CurrLayer = m_HttpUpdateLayer;
    addChild(m_CurrLayer, LAYER_BACKGROUND);
}

void SceneManager::HttpUpdateToLogin()
{
    if (m_HttpUpdateLayer)
    {
        m_HttpUpdateLayer->removeAllChildrenWithCleanup(true);
        removeChild(m_HttpUpdateLayer);
        m_HttpUpdateLayer= NULL;
    }

    m_LoginLayer = new LoginLayer();
    m_CurrLayer = m_LoginLayer;
    addChild(m_CurrLayer, LAYER_BACKGROUND);
}
