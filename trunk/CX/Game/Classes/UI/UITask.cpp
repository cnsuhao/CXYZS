#include "UITask.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "CSVFileManager.h"
#include "NetManager.h"
#include "SceneManager.h"

using namespace cocostudio;

UITask::UITask()
{

}

UITask::~UITask()
{
}

bool UITask::init()
{
    bool bRet = false;
    do 
    {
        Size winSize = Director::getInstance()->getVisibleSize();

		//载入各种面板
		if (!LoadNode())	return false;

		m_iCurrSelectTask = 0;
		ShowTaskDetailed();
		m_currModelSelect = false;
		m_bCurrSelect = true;

        bRet = true;
    } while (0);

    return bRet;
}

//加载各种面板
bool UITask::LoadNode()
{
	//加载主面板
	m_taskPanel = CSLoader::createNode("UI/Task.csb");
	if (m_taskPanel == NULL)
		return false;
	addChild(m_taskPanel);

	Button* btn;
	btn = static_cast<Button*>(m_taskPanel->getChildByName("bt_close"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref*)
		{
			g_UIManager->HideUI(UI_TASK);
		});
	}

	btn = static_cast<Button*>(m_taskPanel->getChildByName("bt_rectask"));
	if (btn)
	{
		btn->addClickEventListener([this](Ref*)
		{
			BtClick();
		});
	}

	//已接任务
	m_haveTask =  static_cast<cc2dui::CheckBox*>(m_taskPanel->getChildByName("cb_task"));
	if (m_haveTask)
	{
		m_haveTask->setSelected(true);
		m_haveTask->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			m_haveTask->setSelected(true);
			m_acceptTask->setSelected(false);
			m_currModelSelect = false;
			UpdateTaskList();
		});
	}
	//可接任务
	m_acceptTask =  static_cast<cc2dui::CheckBox*>(m_taskPanel->getChildByName("cb_rec"));
	if (m_acceptTask)
	{
		m_acceptTask->setSelected(false);
		m_acceptTask->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			m_haveTask->setSelected(false);
			m_acceptTask->setSelected(true);
			m_currModelSelect = true;
			UpdateTaskList();
		});
	}

	//模板
	m_Template = static_cast<Layout*>(m_taskPanel->getChildByName("panel_temp"));
	m_Template->setVisible(false);

	//右边滚动条
	m_taskSV = static_cast<cc2dui::ScrollView*>(m_taskPanel->getChildByName("sv_task"));
	m_taskSV ->setSwallowTouches(true);

	//物品图标
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward1"))->setVisible(false);
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item1"))->setVisible(false);
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward2"))->setVisible(false);
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item2"))->setVisible(false);
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward3"))->setVisible(false);
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item3"))->setVisible(false);
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward4"))->setVisible(false);
	static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item4"))->setVisible(false);

	return true;
}

void UITask::AddTaskItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,int pid)
{
	const TaskInfo* taskTemp = g_CSVFileManager->GetTaskByID(pid);

	cc2dui::Layout* newLayout = static_cast<cc2dui::Layout*>(layout);
	newLayout->setVisible(true);
	newLayout->setTouchEnabled(true);

	CheckBox *cb;
	cb =  static_cast<cc2dui::CheckBox*>(newLayout->getChildByName("cb_task"));
	if (cb)
	{
		cb->setSelected(m_bCurrSelect);
		m_iCurrSelectTask = pid;
		m_bCurrSelect = false;
		ShowTaskDetailed();

		m_mapChecks.insert(pair<int32,CheckBox*>(pid,cb));
		cb->addEventListener([this](Ref* _sender,CheckBox::EventType type)
		{
			if (type == CheckBox::EventType::SELECTED || type == CheckBox::EventType::UNSELECTED)
			{
				CheckBox* tcb =  static_cast<CheckBox*>(_sender);
				map<int32,CheckBox*>::iterator it =  m_mapChecks.begin();
				for (; it != m_mapChecks.end(); it++)
				{
					it->second->setSelected(false);
					if (it->second == tcb)
						m_iCurrSelectTask = it->first;
				}
				tcb->setSelected(true);
				ShowTaskDetailed();
			}
		});
	}
	
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_taskname"))->setString(BaseFunction::a2u(taskTemp->strName));

	const TextInfo *textTemp;
	if (taskTemp->iType == 0)
	{
		textTemp = g_CSVFileManager->GetTextByID(720);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_tasktype"))->setString(BaseFunction::a2u(textTemp->strContent));
	}
	else if (taskTemp->iType == 1)
	{
		textTemp = g_CSVFileManager->GetTextByID(721);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_tasktype"))->setString(BaseFunction::a2u(textTemp->strContent));
	}
	else if (taskTemp->iType == 2)
	{
		textTemp = g_CSVFileManager->GetTextByID(722);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_tasktype"))->setString(BaseFunction::a2u(textTemp->strContent));
	}
	else if (taskTemp->iType == 3)
	{
		textTemp = g_CSVFileManager->GetTextByID(723);
		static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_tasktype"))->setString(BaseFunction::a2u(textTemp->strContent));
	}

	textTemp = g_CSVFileManager->GetTextByID(724);
	char szTemp[MAX_NAME_SIZE];
	sprintf(szTemp,"%d%s",taskTemp->iLevel,textTemp->strContent.c_str());
	static_cast<cc2dui::Text*>(newLayout->getChildByName("tx_tasklevel"))->setString(BaseFunction::a2u(szTemp));

	//是否完成图标
	//模式是关闭
	static_cast<cc2dui::ImageView*>(newLayout->getChildByName("img_completed"))->setVisible(false);

	Vector<TaskInfoIt>::iterator it;
	for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
	{
		if (it->id == pid)
		{
			if (it->state == TASK_STATE_SUBMITTASK)
			{
				static_cast<cc2dui::ImageView*>(newLayout->getChildByName("img_completed"))->setVisible(true);
			}
		}
	}

	sv->addChild(newLayout);
}

//更新任务列表
void UITask::UpdateTaskList()
{
	//m_iCurrSelectTask = 0;
	ShowTaskDetailed();
	m_mapChecks.clear();
	m_taskSV->removeAllChildrenWithCleanup(true);

	Vector<TaskInfoIt>::iterator it;
	//显示已接任务
	if (!m_currModelSelect)
	{
		for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
		{
			if (it->state == TASK_STATE_HAVETASK || it->state == TASK_STATE_SUBMITTASK)
			{
				AddTaskItem(m_taskSV,m_Template->clone(),it->id);
			}
		}
	}
	else//显示可接任务
	{
		map<int32,TaskInfo>::iterator itt;
		for(itt = g_CSVFileManager->m_TaskInfoMap.begin(); itt != g_CSVFileManager->m_TaskInfoMap.end() ; itt++)
		{
			//遍历所有任务
			bool bIsNull = true;
			for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
			{
				//查找是否列表中是否有可接任务
				if (it->id == itt->first)
				{
					bIsNull = false;
					if (it->state == TASK_STATE_NOTASK)
						bIsNull = true;
					break;
				}
			}
			if (bIsNull)
			{
				//检测等级
				if ( g_LogicManager->m_Hero->m_Level >= itt->second.iLevel)
				{
					//检测境界
					if (g_LogicManager->m_Hero->m_Attribute[ATTRIBUTE_REALMLEVEL] >= itt->second.iRealmID)
					{
						//检测前置任务是否完成
						if (itt->second.iReqTaskID == 0)
						{
							AddTaskItem(m_taskSV,m_Template->clone(),itt->first);
						}
						else
						{
							for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
							{
								//查找前置任务是否完成
								if (it->id == itt->second.iReqTaskID)
								{
									if (it->state == TASK_STATE_FINISHTASK)
									{
										AddTaskItem(m_taskSV,m_Template->clone(),itt->first);
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	m_taskSV->setLayoutType(Layout::Type::VERTICAL);
	m_taskSV->forceDoLayout();
	Size WeasonSize = m_taskSV->getInnerContainerSize();
	WeasonSize.height = m_taskSV->getChildrenCount() * m_Template->getContentSize().height;
	m_taskSV->setInnerContainerSize(WeasonSize);
}

//右边显示选择任务
void UITask::ShowTaskDetailed()
{
	if (m_iCurrSelectTask == 0)
	{
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_title"))->setString("");
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("value_profile"))->setString("");
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("value_purpose"))->setString("");
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_money"))->setString("");
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_exp"))->setString("");
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_magic_exp"))->setString("");

		//物品图标
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward1"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item1"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward2"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item2"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward3"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item3"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_reward4"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_item4"))->setVisible(false);
		static_cast<cc2dui::Button*>(m_taskPanel->getChildByName("bt_rectask"))->setVisible(false);

		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("Text_16"))->setVisible(false);
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("Text_16_Copy"))->setVisible(false);
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("Text_16_Copy_0"))->setVisible(false);
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("Text_16_Copy_1"))->setVisible(false);

		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_money"))->setVisible(false);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_exp"))->setVisible(false);
		static_cast<cc2dui::Button*>(m_taskPanel->getChildByName("img_magic_exp"))->setVisible(false);
	}
	else
	{
		m_SelectTaskState = TASK_STATE_NOTASK;
		char szTemp[50];
		const TaskInfo* taskTemp = g_CSVFileManager->GetTaskByID(m_iCurrSelectTask);
		Vector<TaskInfoIt>::iterator it;
		for (it = g_LogicManager->m_Hero->m_vecTaskList.begin() ; it != g_LogicManager->m_Hero->m_vecTaskList.end(); it++)
		{
			if (it->id == m_iCurrSelectTask)
			{
				m_SelectTaskState = (TaskState)it->state;
				break;
			}
		}

		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_money"))->setVisible(true);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_exp"))->setVisible(true);
		static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_magic_exp"))->setVisible(true);
		
		switch (taskTemp->iRewardType)
		{
		case ATTRIBUTE_MONEY:
			static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_magic_exp"))->loadTexture("Public/money.png");
			break;
		case ATTRIBUTE_GOLD:
			static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_magic_exp"))->loadTexture("Public/gold.png");
			break;
		case ATTRIBUTE_BINDGOLD:
			static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_magic_exp"))->loadTexture("Public/binggold.png");
			break;
		case ATTRIBUTE_ARENAMONEY:
			static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_magic_exp"))->loadTexture("Public/arenamoney.png");
			break;
		case ATTRIBUTE_BATTLEFIELDMONEY:
			static_cast<cc2dui::ImageView*>(m_taskPanel->getChildByName("img_magic_exp"))->loadTexture("Public/battlefieldmoney.png");
			break;
		default:
			break;
		}
		
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_title"))->setString(BaseFunction::a2u(taskTemp->strName));
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("value_profile"))->setString(BaseFunction::a2u(taskTemp->strTaskDesc));
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("value_purpose"))->setString(BaseFunction::a2u(taskTemp->strTargetDesc));
		sprintf(szTemp,"%d",taskTemp->iPerserExp);
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_exp"))->setString(szTemp);
		sprintf(szTemp,"%d",taskTemp->iRealmExp);
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_magic_exp"))->setString(szTemp);
		sprintf(szTemp,"%d",taskTemp->iRewardNum);
		static_cast<cc2dui::Text*>(m_taskPanel->getChildByName("tx_money"))->setString(szTemp);


		int32 dropid = taskTemp->iReceiveDropID;
		if (m_SelectTaskState != TASK_STATE_NOTASK)
			dropid = taskTemp->iReferDropID;
		
		if (dropid != 0)
		{
			int Num = 0;
			std::map<int32,std::vector<Drop>>::iterator it = g_CSVFileManager->m_DropMap.find(dropid);

			if(it != g_CSVFileManager->m_DropMap.end())
			{
				std::vector<Drop>::iterator itDrop;
				for(itDrop = it->second.begin();itDrop != it->second.end();itDrop++)
				{
					Num++;
					switch (Num)
					{
					case 1:
						{
							const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_reward1"))->setVisible(true);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item1"))->setVisible(true);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16"))->setVisible(true);
							sprintf(szTemp,"%d",itDrop->iItemNum);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item1"))->loadTexture(itemTemp->strDropIcon);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16"))->setString(szTemp);
						}
						break;
					case 2:
						{
							const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_reward2"))->setVisible(true);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item2"))->setVisible(true);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16_Copy"))->setVisible(true);
							sprintf(szTemp,"%d",itDrop->iItemNum);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item2"))->loadTexture(itemTemp->strDropIcon);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16_Copy"))->setString(szTemp);
						}
						break;
					case 3:
						{
							const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_reward3"))->setVisible(true);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item3"))->setVisible(true);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16_Copy_0"))->setVisible(true);
							sprintf(szTemp,"%d",itDrop->iItemNum);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item3"))->loadTexture(itemTemp->strDropIcon);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16_Copy_0"))->setString(szTemp);
						}
						break;
					case 4:
						{
							const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_reward4"))->setVisible(true);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item4"))->setVisible(true);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16_Copy_1"))->setVisible(true);
							sprintf(szTemp,"%d",itDrop->iItemNum);
							static_cast<ImageView*>(m_taskPanel->getChildByName("img_item4"))->loadTexture(itemTemp->strDropIcon);
							static_cast<Text*>(m_taskPanel->getChildByName("Text_16_Copy_1"))->setString(szTemp);
						}
						break;
					default:
						break;
					}
				}
			}
		}

		//按钮
		static_cast<cc2dui::Button*>(m_taskPanel->getChildByName("bt_rectask"))->setVisible(true);
		switch (m_SelectTaskState)
		{
		case TASK_STATE_NOTASK:
			static_cast<cc2dui::Button*>(m_taskPanel->getChildByName("bt_rectask"))->setTitleText(BaseFunction::a2u("领取任务"));
			break;
		case TASK_STATE_HAVETASK:
			static_cast<cc2dui::Button*>(m_taskPanel->getChildByName("bt_rectask"))->setTitleText(BaseFunction::a2u("开始任务"));
			break;
		case TASK_STATE_SUBMITTASK:
			static_cast<cc2dui::Button*>(m_taskPanel->getChildByName("bt_rectask"))->setTitleText(BaseFunction::a2u("交付任务"));
			break;
		case TASK_STATE_FINISHTASK:
			break;
		case TASK_STATE_MAX:
			break;
		default:
			break;
		}
	}
}

//按钮事件
void UITask::BtClick()
{
	const TaskInfo* taskTemp = g_CSVFileManager->GetTaskByID(m_iCurrSelectTask);
	switch (m_SelectTaskState)
	{
	case TASK_STATE_NOTASK:
		{
			//可接任务
			const NPCInfo *npcTemp = g_CSVFileManager->GetNPCInfoByID(taskTemp->iTaskNpcID);
			g_LogicManager->m_Hero->m_iFindPathClickNpc = taskTemp->iTaskNpcID;
			g_LogicManager->m_Hero->FindPath(g_SceneManager->m_MainLayer->Tile2Pos(Vec2(npcTemp->pPos.x,npcTemp->pPos.y)));
		}
		break;
	case TASK_STATE_HAVETASK:
		{
			//开始任务
			g_LogicManager->m_Hero->m_iFindPathClickNpc = 0;
			g_LogicManager->m_Hero->FindPath(g_SceneManager->m_MainLayer->Tile2Pos(Vec2(taskTemp->pTargetPoint.x,taskTemp->pTargetPoint.y)));
		}
		break;
	case TASK_STATE_SUBMITTASK:
		{
			//提交任务
			g_LogicManager->m_Hero->m_iFindPathClickNpc = taskTemp->iReferNpcID;
			const NPCInfo *npcTemp = g_CSVFileManager->GetNPCInfoByID(taskTemp->iReferNpcID);
			g_LogicManager->m_Hero->FindPath(g_SceneManager->m_MainLayer->Tile2Pos(Vec2(npcTemp->pPos.x,npcTemp->pPos.y)));
		}
		break;
	case TASK_STATE_FINISHTASK:
		break;
	case TASK_STATE_MAX:
		break;
	default:
		break;
	}
}