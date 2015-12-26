#include "UIDefaultServer.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "SceneManager.h"
#include "BaseFunction.h"
#include "CSVFileManager.h"
#include "NetManager.h"

using namespace cocostudio;


UIDefaultServer::UIDefaultServer()
{
}

UIDefaultServer::~UIDefaultServer()
{
}

bool UIDefaultServer::init()
{
	//初始化服务器名
	const TextInfo *textTemp;
	textTemp = g_CSVFileManager->GetTextByID(9);
	sprintf(m_szSerStateClose,"%s",textTemp->strContent.c_str());
	textTemp = g_CSVFileManager->GetTextByID(10);
	sprintf(m_szSerStateOld,"%s",textTemp->strContent.c_str());
	textTemp = g_CSVFileManager->GetTextByID(11);
	sprintf(m_szSerStateNew,"%s",textTemp->strContent.c_str());
	textTemp = g_CSVFileManager->GetTextByID(12);
	sprintf(m_szSerStateAnd,"%s",textTemp->strContent.c_str());

	textTemp = g_CSVFileManager->GetTextByID(35);
	sprintf(m_szSerRec,"%s",textTemp->strContent.c_str());
	textTemp = g_CSVFileManager->GetTextByID(36);
	sprintf(m_szSer1to10,"%s",textTemp->strContent.c_str());
	textTemp = g_CSVFileManager->GetTextByID(37);
	sprintf(m_szSer11to20,"%s",textTemp->strContent.c_str());


	bool bRet = false;
    do 
    {
        CC_BREAK_IF(!UIBase::init());

		//加载背景
		sp_canvas = Sprite::create("UI/Login/LoginCanvas.jpg");
		sp_canvas->setPosition(g_ScreenHalfSize.x, g_ScreenHalfSize.y);
		addChild(sp_canvas);

        defaultServerUI = CSLoader::createNode("UI/DefaultServer.csb");	
		selectServerUI = CSLoader::createNode("UI/SelectServer.csb");	
        if (defaultServerUI == NULL || selectServerUI == NULL)
            return false;
		addChild(defaultServerUI);
		addChild(selectServerUI);
		selectServerUI->setVisible(false);

		//右边服务器列表模板
		selectServer = static_cast<Layout*>(selectServerUI->getChildByName("Panel_8"));
		selectServer->setVisible(false);
		selectServerTemplate = static_cast<Layout*>(selectServerUI->getChildByName("Panel_11"));
		selectServerTemplate->setVisible(false);
		selectServer->setLayoutType(Layout::Type::VERTICAL);
		selectServer->forceDoLayout();

		//右边角色列表模板
		selectRole = static_cast<Layout*>(selectServerUI->getChildByName("Panel_9"));
		selectRole->setVisible(false);
		selectRoleTemplate = static_cast<Layout*>(selectServerUI->getChildByName("Panel_12"));
		selectRoleTemplate->setVisible(false);
		AddServerRole(selectRole,selectRoleTemplate->clone());
		AddServerRole(selectRole,selectRoleTemplate->clone());
		selectRole->setLayoutType(Layout::Type::VERTICAL);
		selectRole->forceDoLayout();

		std::string strServerid = UserDefault::getInstance()->getStringForKey("ServerID");
		if (strServerid == "")
		{
			m_vecServerList.clear();
			g_requestServerList->GetNewSerList(m_vecServerList);

			if (m_vecServerList.size() != 0)
			{
				m_currServer.iServerID = m_vecServerList[0].iServerID;
				sprintf(m_currServer.strServerName,"%s",m_vecServerList[0].strServerName);
				m_currServer.serverState = SERSTATE_NEW;
			}
		}
		else
		{
			tagServer *tempSer = g_requestServerList->GetSerByID(atoi(strServerid.c_str()));
			m_currServer.iServerID = tempSer->iServerID;
			sprintf(m_currServer.strServerName,"%s",tempSer->strServerName);
			m_currServer.serverState = SERSTATE_NEW;
		}
		
		//更新服务器名显示
		UpdateServerName();
		UpdateSelectServerName();


		Text* text = nullptr;
		Button* btn = nullptr;

		//默认选择服务器界面--------------------------------------------------------------------------------
        btn = static_cast<Button*>(defaultServerUI->getChildByName("Button_1"));	
        if (btn)
        {
            //登录游戏
            btn->addClickEventListener([this](Ref* _sender)
            {
				char regSerid[10];
				sprintf(regSerid,"%d",m_currServer.iServerID);
				UserDefault::getInstance()->setStringForKey("ServerID",regSerid);

                OnBtEnterGame();
            });
        }
		btn = static_cast<Button*>(defaultServerUI->getChildByName("Button_4"));	
        if (btn)
        {
            // 登陆游戏
            btn->addClickEventListener([this](Ref* _sender)
            {
                OnBtChangeServer();
            });
        }

		//默认选择服务器界面--------------------------------------------------------------------------------
		btn = static_cast<Button*>(selectServerUI->getChildByName("Button_26"));	
        if (btn)
        {
            // 返回到默认选择服务器界面
            btn->addClickEventListener([this](Ref* _sender)
            {
                OnBtSelectServertoDefault();
            });
        }


		//左边的服务器列表
		accountTemplate = static_cast<Layout*>(selectServerUI->getChildByName("Panel_10"));
		accountSV = static_cast<cc2dui::ScrollView*>(selectServerUI->getChildByName("ScrollView_5"));
		accountSV->setSwallowTouches(true);
		AddAcountItem(accountSV,accountTemplate->clone(), BaseFunction::a2u(m_szSerRec),0);
		AddAcountItem(accountSV,accountTemplate->clone(), BaseFunction::a2u(m_szSer1to10),1);
		AddAcountItem(accountSV,accountTemplate->clone(), BaseFunction::a2u(m_szSer11to20),2);
		accountSV->setLayoutType(Layout::Type::VERTICAL);
        accountSV->forceDoLayout();
        Size ContainerSize = accountSV->getInnerContainerSize();
        ContainerSize.height = accountSV->getChildrenCount() * accountTemplate->getContentSize().height;
        accountSV->setInnerContainerSize(ContainerSize);
        accountTemplate->setVisible(false);

		

		(static_cast<Text*>(selectServerUI->getChildByName("Text_64")))->setString(BaseFunction::a2u(m_szSerRec));

        bRet = true;
    } while (0);

    return bRet;
}

void UIDefaultServer::OnBtEnterGame()
{
	MsgSelServer msg(m_currServer.iServerID);
	g_NetManager->SendMsg(msg);
}

void UIDefaultServer::OnBtChangeServer()
{
	ReplaceUI(defaultServerUI,selectServerUI);
}

void UIDefaultServer::OnTxSelectServer()
{
	//ReplaceUI(selectServerUI,selectRoleUI);
}

//选择角色界面中方法
void UIDefaultServer::OnBtSelectRole()
{
	//ReplaceUI(selectRoleUI,defaultServerUI);
}
void UIDefaultServer::OnBtSelectServertoDefault()
{
	ReplaceUI(selectServerUI,defaultServerUI);
}



void UIDefaultServer::ReplaceUI(cc2d::Node* oldUI, cc2d::Node* newUI)
{
	if (oldUI)
    {
        oldUI->setVisible(false);
    }
    if (newUI)
    {
        newUI->setVisible(true);
        newUI->setScale(0.7f, 0.7f);
        newUI->runAction(EaseElasticOut::create(ScaleTo::create(1.0f, 1.0f, 1.0f)));
		//更新服务器名显示
		UpdateServerName();
		UpdateSelectServerName();
    }
}

void UIDefaultServer::AddAcountItem(cc2dui::ScrollView* sv, cc2dui::Widget* widget, std::string itemName, int iPos)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(widget);
    newLayout->setVisible(true);
    newLayout->setTouchEnabled(true);

	Text* text =  static_cast<cc2dui::Text*>(newLayout->getChildByName("Text_99"));
    if (text)
    {
        text->setString(itemName);
    }

	CheckBox* cb =  static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("CheckBox_18"));
    if (cb)
    {
		if (iPos == 0)
		{
			//刚载入时界面
			cb->setSelected(true);

			m_vecServerList.clear();
			selectServer->removeAllChildren();
			g_requestServerList->GetNewSerList(m_vecServerList);
			int length = m_vecServerList.size();
			for (int i = 0; i < length; i += 2)
			{
				selectServer->setVisible(true);
				if( i + 1 >=  length)
				{
					tagServer tagTemp;
					AddServerName(selectServer,selectServerTemplate->clone(),m_vecServerList[i] ,tagTemp);
				}
				else
					AddServerName(selectServer,selectServerTemplate->clone(),m_vecServerList[i] ,m_vecServerList[i+1]);
			}

		}
		m_mapcheckboxs.insert(pair<int , CheckBox*>(iPos,cb));
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			int iCurrSeleted = 0;
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				map<int, CheckBox*>::iterator it =  m_mapcheckboxs.begin();
				for (; it != m_mapcheckboxs.end(); it++)
				{
					if (it->second == tcb)
						iCurrSeleted = it->first;
					it->second->setSelected(false);
				}

				tcb->setSelected(true);

				selectServer->setVisible(true);
				selectRole->setVisible(false);
			}

			if (iCurrSeleted == 0)
				(static_cast<Text*>(selectServerUI->getChildByName("Text_64")))->setString(BaseFunction::a2u(m_szSerRec));
			else if (iCurrSeleted == 1)
				(static_cast<Text*>(selectServerUI->getChildByName("Text_64")))->setString(BaseFunction::a2u(m_szSer1to10));
			else if (iCurrSeleted == 2)
				(static_cast<Text*>(selectServerUI->getChildByName("Text_64")))->setString(BaseFunction::a2u(m_szSer11to20));

			m_vecServerList.clear();
			selectServer->removeAllChildren();
			//向服务器请求数据,获得服务器列表
			if (iCurrSeleted == 0)
				g_requestServerList->GetNewSerList(m_vecServerList);
			else
				g_requestServerList->GetServerList(m_vecServerList,(iCurrSeleted - 1)  * 10 ,iCurrSeleted * 10 - 1);
			int length = m_vecServerList.size();
			for (int i = 0; i < length; i += 2)
			{
				if( i + 1 >=  length)
				{
					tagServer tagTemp;
					AddServerName(selectServer,selectServerTemplate->clone(),m_vecServerList[i] ,tagTemp);
				}
				else
					AddServerName(selectServer,selectServerTemplate->clone(),m_vecServerList[i] ,m_vecServerList[i+1]);
			}
		});
    }
	sv->addChild(newLayout);
}

void UIDefaultServer::AddServerName(cc2dui::Layout* sv, cc2dui::Widget* layout, tagServer& serverLeft,tagServer& serverRight)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
    newLayout->setVisible(true);
    newLayout->setTouchEnabled(true);

	if (serverLeft.iServerID != 0)
	{
		//服务器ID
		char szServerID[5];
		sprintf(szServerID,"%d",serverLeft.iServerID);
		(static_cast<Text*>(newLayout->getChildByName("Left_Num")))->setString(BaseFunction::a2u(szServerID));

		//服务器名
		Button* btn = static_cast<Button*>(newLayout->getChildByName("Left_Bt"));
		btn->setTitleText(BaseFunction::a2u(serverLeft.strServerName));
		btn->setUserData(&serverLeft.serverPos);
		if(static_cast<Button*>(newLayout->getChildByName("Left_Bt")))
		{
			static_cast<Button*>(newLayout->getChildByName("Left_Bt"))->addClickEventListener([this](Ref* _sender)
			{
				Button* btn = static_cast<Button*>(_sender);
				int serverPos = *(int*)(btn->getUserData());
				if (m_vecServerList[serverPos].serverState != SERSTATE_CLOSE)
				{
					m_currServer = m_vecServerList[serverPos];
					OnBtSelectServertoDefault();
				}
			});
		}

		//服务器状态
		switch (serverLeft.serverState)
		{
		case SERSTATE_OLD:
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setString(BaseFunction::a2u(m_szSerStateOld));
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setTextColor(Color4B(253,82,0,0));
			static_cast<Text*>(newLayout->getChildByName("Left_Num"))->setTextColor(Color4B(253,82,0,0));
			static_cast<Button*>(newLayout->getChildByName("Left_Bt"))->setTitleColor(Color3B(253,82,0));
			break;
		case SERSTATE_NEW:
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setString(BaseFunction::a2u(m_szSerStateNew));
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setTextColor(Color4B(253,202,0,0));
			static_cast<Text*>(newLayout->getChildByName("Left_Num"))->setTextColor(Color4B(253,202,0,0));
			static_cast<Button*>(newLayout->getChildByName("Left_Bt"))->setTitleColor(Color3B(253,202,0));
			break;
		case SERSTATE_AND:
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setString(BaseFunction::a2u(m_szSerStateAnd));
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setTextColor(Color4B(54,254,0,0));
			static_cast<Text*>(newLayout->getChildByName("Left_Num"))->setTextColor(Color4B(54,254,0,0));
			static_cast<Button*>(newLayout->getChildByName("Left_Bt"))->setTitleColor(Color3B(54,254,0));
			break;
		case SERSTATE_CLOSE:
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setString(BaseFunction::a2u(m_szSerStateClose));
			static_cast<Text*>(newLayout->getChildByName("Left_State"))->setTextColor(Color4B(255,255,255,0));
			static_cast<Text*>(newLayout->getChildByName("Left_Num"))->setTextColor(Color4B(255,255,255,0));
			static_cast<Button*>(newLayout->getChildByName("Left_Bt"))->setTitleColor(Color3B(255,255,255));
			break;
		default:
			break;
		}
	}

	if (serverRight.iServerID != 0)
	{
		//服务器ID
		char szServerID[5];
		sprintf(szServerID,"%d",serverRight.iServerID);
		(static_cast<Text*>(newLayout->getChildByName("Right_Num")))->setString(BaseFunction::a2u(szServerID));

		//服务器名
		Button* btn = static_cast<Button*>(newLayout->getChildByName("Right_Bt"));
		btn->setTitleText(BaseFunction::a2u(serverRight.strServerName));
		btn->setUserData(&serverRight.serverPos);
		if(static_cast<Button*>(newLayout->getChildByName("Right_Bt")))
		{
			static_cast<Button*>(newLayout->getChildByName("Right_Bt"))->addClickEventListener([this](Ref* _sender)
			{
				Button* btn = static_cast<Button*>(_sender);
				int serverPos = *(int*)(btn->getUserData());
				if (m_vecServerList[serverPos].serverState != SERSTATE_CLOSE)
				{
					m_currServer = m_vecServerList[serverPos];
					OnBtSelectServertoDefault();
				}
			});
		}
		//服务器状态
		switch (serverRight.serverState)
		{
		case SERSTATE_OLD:
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setString(BaseFunction::a2u(m_szSerStateOld));
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setTextColor(Color4B(253,82,0,0));
			static_cast<Text*>(newLayout->getChildByName("Right_Num"))->setTextColor(Color4B(253,82,0,0));
			static_cast<Button*>(newLayout->getChildByName("Right_Bt"))->setTitleColor(Color3B(253,82,0));
			break;
		case SERSTATE_NEW:
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setString(BaseFunction::a2u(m_szSerStateNew));
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setTextColor(Color4B(253,202,0,0));
			static_cast<Text*>(newLayout->getChildByName("Right_Num"))->setTextColor(Color4B(253,202,0,0));
			static_cast<Button*>(newLayout->getChildByName("Right_Bt"))->setTitleColor(Color3B(253,202,0));
			break;
		case SERSTATE_AND:
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setString(BaseFunction::a2u(m_szSerStateAnd));
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setTextColor(Color4B(54,254,0,0));
			static_cast<Text*>(newLayout->getChildByName("Right_Num"))->setTextColor(Color4B(54,254,0,0));
			static_cast<Button*>(newLayout->getChildByName("Right_Bt"))->setTitleColor(Color3B(54,254,0));
			break;
		case SERSTATE_CLOSE:
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setString(BaseFunction::a2u(m_szSerStateClose));
			static_cast<Text*>(newLayout->getChildByName("Right_State"))->setTextColor(Color4B(255,255,255,0));
			static_cast<Text*>(newLayout->getChildByName("Right_Num"))->setTextColor(Color4B(255,255,255,0));
			static_cast<Button*>(newLayout->getChildByName("Right_Bt"))->setTitleColor(Color3B(255,255,255));
			break;
		default:
			break;
		}
	}
	else
	{
		static_cast<Text*>(newLayout->getChildByName("Right_Num"))->setVisible(false);
		static_cast<Button*>(newLayout->getChildByName("Right_Bt"))->setVisible(false);
		static_cast<Text*>(newLayout->getChildByName("Right_State"))->setVisible(false);
	}

	sv->addChild(newLayout);
}
void UIDefaultServer::AddServerRole(cc2dui::Layout* sv, cc2dui::Widget* layout)
{
	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
    newLayout->setVisible(true);
    newLayout->setTouchEnabled(true);

	Button* bt = NULL;
	bt =  static_cast<cc2dui::Button*>(newLayout->getChildByName("Button_15"));
    if (bt)
    {
		bt->addClickEventListener([this](Ref* _sender)
		{
			sprintf(m_currServer.strServerName,"%s","servername");
			OnBtSelectServertoDefault();
			
		});

    }
	sv->addChild(newLayout);
}

////（选择服务器界面）右边服务器列表点击
//void UIDefaultServer::OnBtServerClick(string itemNameLeft)
//{
//	
//}

//更改默认登录界面的服务器名
void UIDefaultServer::UpdateServerName()
{
	//服务器ID
	char szServerID[5];
	sprintf(szServerID,"%d",m_currServer.iServerID);
	(static_cast<Text*>(defaultServerUI->getChildByName("Text_4")))->setString(BaseFunction::a2u(szServerID));

	//服务器名
	(static_cast<Text*>(defaultServerUI->getChildByName("Text_1")))->setString(BaseFunction::a2u(m_currServer.strServerName));

	//服务器状态
	switch (m_currServer.serverState)
	{
	case SERSTATE_OLD:
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setString(BaseFunction::a2u(m_szSerStateOld));
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setTextColor(Color4B(253,82,2,0));
		break;
	case SERSTATE_NEW:
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setString(BaseFunction::a2u(m_szSerStateNew));
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setTextColor(Color4B(253,202,0,0));
		break;
	case SERSTATE_AND:
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setString(BaseFunction::a2u(m_szSerStateAnd));
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setTextColor(Color4B(54,254,0,0));
		break;
	case SERSTATE_CLOSE:
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setString(BaseFunction::a2u(m_szSerStateClose));
		static_cast<Text*>(defaultServerUI->getChildByName("Text_2"))->setTextColor(Color4B(255,255,255,0));
		break;
	default:
		break;
	}

}

//更新选择服务器界面的服务器名
void UIDefaultServer::UpdateSelectServerName()
{
	char szTemp[20];
	sprintf(szTemp,"%d %s",m_currServer.iServerID,m_currServer.strServerName);
	static_cast<Button*>(selectServerUI->getChildByName("Button_26"))->setTitleText(BaseFunction::a2u(szTemp));

	//服务器状态
	switch (m_currServer.serverState)
	{
	case SERSTATE_OLD:
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setString(BaseFunction::a2u(m_szSerStateOld));
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setTextColor(Color4B(252,83,0,0));
		static_cast<Button*>(selectServerUI->getChildByName("Button_26"))->setTitleColor(Color3B(252,83,0));
		break;
	case SERSTATE_NEW:
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setString(BaseFunction::a2u(m_szSerStateNew));
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setTextColor(Color4B(253,202,0,0));
		static_cast<Button*>(selectServerUI->getChildByName("Button_26"))->setTitleColor(Color3B(253,202,0));
		break;
	case SERSTATE_AND:
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setString(BaseFunction::a2u(m_szSerStateAnd));
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setTextColor(Color4B(54,254,0,0));
		static_cast<Button*>(selectServerUI->getChildByName("Button_26"))->setTitleColor(Color3B(54,254,0));
		break;
	case SERSTATE_CLOSE:
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setString(BaseFunction::a2u(m_szSerStateClose));
		static_cast<Text*>(selectServerUI->getChildByName("Text_100"))->setTextColor(Color4B(255,255,255,0));
		static_cast<Button*>(selectServerUI->getChildByName("Button_26"))->setTitleColor(Color3B(255,255,255));
		break;
	default:
		break;
	}
}