#include "TaskManager.h"
#include "CSVFileManager.h"
#include "MsgInfo.h"
#include "LogManage.h"
#include "GameServer.h"
#include "MapManager.h"
#include "StackStream.h"
#include "Monster.h"
TaskManager *g_TaskManager = NULL;

TaskManager::TaskManager()
{		
}

TaskManager::~TaskManager()
{
}

void TaskManager::Init()
{
	//killMonsterCount = 0;
}
//接取任务
void TaskManager::OnGCAskActiveTask(Player* player,MsgBase* msg)
{

	MsgTask_TakeTask* pTakeTask = (MsgTask_TakeTask*)msg;
	const TaskInfo* taskInfo = g_CSVFileManager->GetTaskByID(pTakeTask->taskId);  
	//是否找到任务
	if (!taskInfo)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_NoTask,pTakeTask->taskId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		gLog.AddLog(LOG_LEVEL_ERROR, "GetMapByID(%d)", pTakeTask->taskId);
		return;
	}
	//判断任务需求等级是否符合
	if(player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI() < taskInfo->iLevel)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_LevelError,pTakeTask->taskId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	//判断玩家境界等级是否符合
	if(player->m_Attribute[ATTRIBUTE_REALMLEVEL].GetValueI() < taskInfo->iRealmID)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_RealmError,pTakeTask->taskId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	//判断玩家前置任务是否完成
	if(pTakeTask->iReqTaskID != taskInfo->iReqTaskID)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_ReqTaskError,pTakeTask->taskId,0); 
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	//查询此任务是否已存在
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->id == pTakeTask->taskId)
		{
			MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_TaskExists,pTakeTask->taskId,0);  
			ttt.OBJID = msg->OBJID;
			g_GameServer->SendToClient(&ttt);
			return;
		}
	}
	//判断任务接取时是否有任务物品
	if(taskInfo->iTermsType == TASK_TERMS_DELIVERYITEM && taskInfo->iTermsType == TASK_TERMS_USEITEM)
	{
		g_ItemManager->AddItem(player,ItemOpreate_TakeTaskItem,taskInfo->iTermsParam[0],taskInfo->iTermsParam[1]);
	}                              
	TaskInfoIt taskInfoIt;
	taskInfoIt.id = pTakeTask->taskId;
	taskInfoIt.progress = 0;
	//！！！临时
	if(taskInfo->iTermsType == TASK_TERMS_TALK )
	{
		taskInfoIt.state = 2;
		taskInfoIt.progress = taskInfo->iTermsParam[1];
	}
		
	else
		taskInfoIt.state = 1;
	player->m_TaskList.push_back(taskInfoIt);
	//通知客户端任务接取成功
	MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_OK,pTakeTask->taskId,taskInfoIt.progress);  
	ttt.OBJID = msg->OBJID;
	g_GameServer->SendToClient(&ttt);

	MsgTask_TakeTask addTask=*pTakeTask;
	StackStream<> addTaskStream;
	addTask.SysID = ID_G2D;
	addTaskStream.Wirte(&addTask, sizeof(addTask));

	addTaskStream.Wirte(&taskInfoIt.id, sizeof(int));
	addTaskStream.Wirte(&taskInfoIt.progress, sizeof(int));
	addTaskStream.Wirte(&taskInfoIt.state, sizeof(int));
	//通知服务器
	MsgTask_TakeTask* pAddTask = (MsgTask_TakeTask*)addTaskStream.Get();
	pAddTask->Len = addTaskStream.GetSize();
	g_GameServer->SendToDB(pAddTask);
}

void TaskManager::OnMonsterKill(Monster* monster,Player* player)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK)
		{
			const TaskInfo* taskInfo =  g_CSVFileManager->GetTaskByID(it->id);
			if(taskInfo->iTermsType == TASK_TERMS_KILLMONSTER && taskInfo->iTermsParam[0] == monster->m_MonsterInfo->id)
			{
				//修改任务进度  
				it->progress += 1;
				//当进度完成时通知客户端任务可以交付
				if(it->progress >= taskInfo->iTermsParam[1])
				{
					it->state = TASK_STATE_SUBMITTASK;
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);		
				}
				MsgTask_Progress taskPro;
				taskPro.OBJID = player->m_GUID;
				taskPro.taskInfo.id =  it->id;
				taskPro.taskInfo.progress = it->progress;
				taskPro.taskInfo.state = it->state;
				
				g_GameServer->SendToDB(&taskPro);
				if(it->progress < taskInfo->iTermsParam[1])
				{
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_MakeTask_OK,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);	
				}
				break;
			}
		}

	}
}

void TaskManager::OnGetItem(uint32 itemId,Player* player)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK)
		{
			const TaskInfo* taskInfo =  g_CSVFileManager->GetTaskByID(it->id);
			if(taskInfo->iTermsType == TASK_TERMS_GETITEM && taskInfo->iTermsParam[0] == itemId)
			{
				//修改任务进度
				it->progress = g_ItemManager->GetTotalNumber(player,itemId);
				//当进度完成时通知客户端任务可以交付
				if(it->progress >= taskInfo->iTermsParam[1])
				{
					it->state = TASK_STATE_SUBMITTASK;
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);		
				}
				MsgTask_Progress taskPro;
				taskPro.OBJID = player->m_GUID;
				taskPro.taskInfo.id =  it->id;
				taskPro.taskInfo.progress = it->progress;
				taskPro.taskInfo.state = it->state;

				g_GameServer->SendToDB(&taskPro);
				if(it->progress < taskInfo->iTermsParam[1])
				{
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_MakeTask_OK,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);	
				}
				break;
			}
		}

	}
}

//使用道具
void TaskManager::OnUseItem(Player* player,uint32 itemId,int32 useCount /* = 1 */)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK)
		{
			const TaskInfo* taskInfo =  g_CSVFileManager->GetTaskByID(it->id);
			if(taskInfo->iTermsType == TASK_TERMS_USEITEM && taskInfo->iTermsParam[0] == itemId)
			{
				//修改任务进度
				it->progress += useCount;
				//当进度完成时通知客户端任务可以交付
				if(it->progress >= taskInfo->iTermsParam[1])
				{
					it->state = TASK_STATE_SUBMITTASK;
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);		
				}
				MsgTask_Progress taskPro;
				taskPro.OBJID = player->m_GUID;
				taskPro.taskInfo.id =  it->id;
				taskPro.taskInfo.progress = it->progress;
				taskPro.taskInfo.state = it->state;

				g_GameServer->SendToDB(&taskPro);
				if(it->progress < taskInfo->iTermsParam[1])
				{
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_MakeTask_OK,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);	
				}
				break;
			}
		}

	}
}

void TaskManager::OnCopyPass(uint32 copyId,Player* player)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK)
		{
			const TaskInfo* taskInfo =  g_CSVFileManager->GetTaskByID(it->id);
			if(taskInfo->iTermsType == TASK_TERMS_COPY && taskInfo->iTermsParam[0] == copyId)
			{
				//修改任务进度
				it->progress += 1;
				//当进度完成时通知客户端任务可以交付
				if(it->progress >= taskInfo->iTermsParam[1])
				{
					it->state = TASK_STATE_SUBMITTASK;
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress); 
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);		
				}
				MsgTask_Progress taskPro;
				taskPro.OBJID = player->m_GUID;
				taskPro.taskInfo.id =  it->id;
				taskPro.taskInfo.progress = it->progress;
				taskPro.taskInfo.state = it->state;

				g_GameServer->SendToDB(&taskPro);
				if(it->progress < taskInfo->iTermsParam[1])
				{
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_MakeTask_OK,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);	
				}
				break;
			}
		}

	}
}

void TaskManager::OnAttriButeChange(int32 attributeId,int32 attributeCount,Player* player)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK)
		{
			const TaskInfo* taskInfo =  g_CSVFileManager->GetTaskByID(it->id);
			if(taskInfo->iTermsType == TASK_TERMS_ATTRIBUTE && taskInfo->iTermsParam[0] == attributeId)
			{
				//修改任务进度
				it->progress += attributeCount;
				//当进度完成时通知客户端任务可以交付
				if(it->progress >= taskInfo->iTermsParam[1])
				{
					it->state = TASK_STATE_SUBMITTASK;
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress); 
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);		
				}
				MsgTask_Progress taskPro;
				taskPro.OBJID = player->m_GUID;
				taskPro.taskInfo.id =  it->id;
				taskPro.taskInfo.progress = it->progress;
				taskPro.taskInfo.state = it->state;

				g_GameServer->SendToDB(&taskPro);
				if(it->progress < taskInfo->iTermsParam[1])
				{
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_MakeTask_OK,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);	
				}
				break;
			}
		}

	}
}
void TaskManager::OnDoTaskCheckItem(const TaskInfo* taskInfo,Player* player)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK && it->id == taskInfo->id)
		{
			bool rs = g_ItemManager->RemoveItem(player,ItemOperate_DoTakeItem,(uint32)taskInfo->iTermsParam[0],taskInfo->iTermsParam[1]);
			if(!rs)
			{
				MsgTaskNotify ttt(ID_G2C,Notify_S2C_MakeTask_ItemError,it->id,it->progress);  
				ttt.OBJID = player->m_GUID;
				g_GameServer->SendToClient(&ttt);
				return;
			}
				//修改任务进度
			it->progress += taskInfo->iTermsParam[1];
			//修改任务状态
			it->state = TASK_STATE_SUBMITTASK;
			//通知客户端任务可交付
			MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress);   
			ttt.OBJID = player->m_GUID;
			g_GameServer->SendToClient(&ttt);	
			//通知DB服务器
			MsgTask_Progress taskPro;
			taskPro.OBJID = player->m_GUID;
			taskPro.taskInfo.id =  it->id;
			taskPro.taskInfo.progress = it->progress;
			taskPro.taskInfo.state = it->state;

			g_GameServer->SendToDB(&taskPro);
			break;

		}
	}
}

void TaskManager::OnDoTaskCollect(int32 itemId,Player* player)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK)
		{
			const TaskInfo* taskInfo =  g_CSVFileManager->GetTaskByID(it->id);
			if(taskInfo->iTermsType == TASK_TERMS_COLLECT && taskInfo->iTermsParam[0] == itemId)
			{
				//修改任务进度
				it->progress += 1;
				//当进度完成时通知客户端任务可以交付
				if(it->progress >= taskInfo->iTermsParam[1])
				{
					it->state = TASK_STATE_SUBMITTASK;
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress); 
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);		
				}
				//通知DB服务器
				MsgTask_Progress taskPro;
				taskPro.OBJID = player->m_GUID;
				taskPro.taskInfo.id =  it->id;
				taskPro.taskInfo.progress = it->progress;
				taskPro.taskInfo.state = it->state;

				g_GameServer->SendToDB(&taskPro);
				if(it->progress < taskInfo->iTermsParam[1])
				{
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_MakeTask_OK,it->id,it->progress);   
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);	
				}
				break;
			}
		}
		
	}
}

void TaskManager::OnEscortItems(uint32 taskId,uint32 npcId,Player* player)
{
	vector<TaskInfoIt>::iterator it;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->state == TASK_STATE_HAVETASK && it->id == taskId)
		{
			const TaskInfo* taskInfo = g_CSVFileManager->GetTaskByID(taskId);
		}
	}
}
void TaskManager::OnGCAskSubmitTask(MsgBase* msg)
{
	MsgTask_SubmitTask* pSubmitTask=(MsgTask_SubmitTask*)msg;
	Player* player = g_MapManager->GetPlayerByID(pSubmitTask->OBJID);
	const TaskInfo* taskInfo = g_CSVFileManager->GetTaskByID(pSubmitTask->taskId);  
	if(taskInfo != NULL){
		OnSubmitTask(pSubmitTask,taskInfo,player);
	}
	else
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_NoTask,pSubmitTask->taskId,0);  
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
	}

}

//提交任务
void TaskManager::OnSubmitTask(MsgTask_SubmitTask* pSubmitTask,const TaskInfo* taskInfo,Player* player)
{
	bool isFinish = false;
	vector<TaskInfoIt>::iterator it;
	TaskInfoIt taskInfoit;
	for(it = player->m_TaskList.begin();it != player->m_TaskList.end();it++)
	{
		if(it->id == taskInfo->id)
		{
			if(it->progress >= taskInfo->iTermsParam[1])
			{
				taskInfoit.id = it->id;
				taskInfoit.progress = it->progress;
				it->state = TASK_STATE_FINISHTASK;
				taskInfoit.state = it->state;

				isFinish = true;
				break;
			}
		}
	}
	if(!isFinish)
	{
		//任务没有完成 通知客户端
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_SubmitTaskError,pSubmitTask->taskId,0);     
		ttt.OBJID = pSubmitTask->OBJID;
		g_GameServer->SendToClient(&ttt);
	}
	else
	{
		if(taskInfo->iTermsType == TASK_TERMS_GETITEM)
		{
			g_ItemManager->RemoveItem(player,ItemOperate_DoTakeItem,(uint32)taskInfo->iTermsParam[0],taskInfo->iTermsParam[1]);
		}
		//获取掉落ID
		int32 iReferDropID = taskInfo->iReferDropID;
		//给予玩家任务奖励
		g_ItemManager->DropItems(player,iReferDropID,ItemOperate_TaskReward);
		player->ChangePlayerAttribute((AttributeType)taskInfo->iRewardType,(float)taskInfo->iRewardNum);
		player->ChangePlayerAttribute(ATTRIBUTE_EXP,(float)taskInfo->iPerserExp);
		player->ChangePlayerAttribute(ATTRIBUTE_REALMEXP,(float)taskInfo->iRealmExp);
		//任务完成通知客户端交付任务
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_SubmitTaskFinish,pSubmitTask->taskId,0); 
		ttt.OBJID = pSubmitTask->OBJID;
		g_GameServer->SendToClient(&ttt);

		MsgTask_SubmitTask submitTask=*pSubmitTask;
		StackStream<> submitTaskStream;
		submitTask.SysID = ID_G2D;
		submitTask.money = player->m_Attribute[taskInfo->iRewardType].GetValueI();
		submitTaskStream.Wirte(&submitTask, sizeof(submitTask));

		submitTaskStream.Wirte(&taskInfoit.id, sizeof(int));
		submitTaskStream.Wirte(&taskInfoit.progress, sizeof(int));
		submitTaskStream.Wirte(&taskInfoit.state, sizeof(int));
		//通知数据库服务器
		MsgTask_SubmitTask* pSubmitTask = (MsgTask_SubmitTask*)submitTaskStream.Get();
		pSubmitTask->Len = submitTaskStream.GetSize();
		g_GameServer->SendToDB(pSubmitTask);
	}
}



//获得任务列表
void TaskManager::OnGetTaskList(MsgBase* msg)
{
	MsgTasks* temp = (MsgTasks*)msg;
	Player *pplay = g_MapManager->GetPlayerByID(temp->iPar[0]);
	if (pplay)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_GetTaskList_OK,0,pplay->m_TaskList.size());
		ttt.OBJID = temp->iPar[0];
		StackStream<2048> taskListStream;
		taskListStream.Wirte(&ttt,sizeof(MsgTaskNotify));
		vector<TaskInfoIt>::iterator iVec;
		for (iVec = pplay->m_TaskList.begin();iVec != pplay->m_TaskList.end(); ++iVec)
		{
			taskListStream.Wirte(&iVec->id,sizeof(int));
			taskListStream.Wirte(&iVec->progress,sizeof(int));
			taskListStream.Wirte(&iVec->state,sizeof(int));
		}

		MsgTaskNotify* pAddMsg = (MsgTaskNotify*)taskListStream.Get();
		pAddMsg->Len = taskListStream.GetSize();
		g_GameServer->SendToClient(pAddMsg);
	}
}