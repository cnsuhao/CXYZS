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
//��ȡ����
void TaskManager::OnGCAskActiveTask(Player* player,MsgBase* msg)
{

	MsgTask_TakeTask* pTakeTask = (MsgTask_TakeTask*)msg;
	const TaskInfo* taskInfo = g_CSVFileManager->GetTaskByID(pTakeTask->taskId);  
	//�Ƿ��ҵ�����
	if (!taskInfo)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_NoTask,pTakeTask->taskId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		gLog.AddLog(LOG_LEVEL_ERROR, "GetMapByID(%d)", pTakeTask->taskId);
		return;
	}
	//�ж���������ȼ��Ƿ����
	if(player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI() < taskInfo->iLevel)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_LevelError,pTakeTask->taskId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	//�ж���Ҿ���ȼ��Ƿ����
	if(player->m_Attribute[ATTRIBUTE_REALMLEVEL].GetValueI() < taskInfo->iRealmID)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_RealmError,pTakeTask->taskId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	//�ж����ǰ�������Ƿ����
	if(pTakeTask->iReqTaskID != taskInfo->iReqTaskID)
	{
		MsgTaskNotify ttt(ID_G2C,Notify_S2C_TakeTask_ReqTaskError,pTakeTask->taskId,0); 
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	//��ѯ�������Ƿ��Ѵ���
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
	//�ж������ȡʱ�Ƿ���������Ʒ
	if(taskInfo->iTermsType == TASK_TERMS_DELIVERYITEM && taskInfo->iTermsType == TASK_TERMS_USEITEM)
	{
		g_ItemManager->AddItem(player,ItemOpreate_TakeTaskItem,taskInfo->iTermsParam[0],taskInfo->iTermsParam[1]);
	}                              
	TaskInfoIt taskInfoIt;
	taskInfoIt.id = pTakeTask->taskId;
	taskInfoIt.progress = 0;
	//��������ʱ
	if(taskInfo->iTermsType == TASK_TERMS_TALK )
	{
		taskInfoIt.state = 2;
		taskInfoIt.progress = taskInfo->iTermsParam[1];
	}
		
	else
		taskInfoIt.state = 1;
	player->m_TaskList.push_back(taskInfoIt);
	//֪ͨ�ͻ��������ȡ�ɹ�
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
	//֪ͨ������
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
				//�޸��������  
				it->progress += 1;
				//���������ʱ֪ͨ�ͻ���������Խ���
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
				//�޸��������
				it->progress = g_ItemManager->GetTotalNumber(player,itemId);
				//���������ʱ֪ͨ�ͻ���������Խ���
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

//ʹ�õ���
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
				//�޸��������
				it->progress += useCount;
				//���������ʱ֪ͨ�ͻ���������Խ���
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
				//�޸��������
				it->progress += 1;
				//���������ʱ֪ͨ�ͻ���������Խ���
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
				//�޸��������
				it->progress += attributeCount;
				//���������ʱ֪ͨ�ͻ���������Խ���
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
				//�޸��������
			it->progress += taskInfo->iTermsParam[1];
			//�޸�����״̬
			it->state = TASK_STATE_SUBMITTASK;
			//֪ͨ�ͻ�������ɽ���
			MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress);   
			ttt.OBJID = player->m_GUID;
			g_GameServer->SendToClient(&ttt);	
			//֪ͨDB������
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
				//�޸��������
				it->progress += 1;
				//���������ʱ֪ͨ�ͻ���������Խ���
				if(it->progress >= taskInfo->iTermsParam[1])
				{
					it->state = TASK_STATE_SUBMITTASK;
					MsgTaskNotify ttt(ID_G2C,Notify_S2C_TaskFinish,it->id,it->progress); 
					ttt.OBJID = player->m_GUID;
					g_GameServer->SendToClient(&ttt);		
				}
				//֪ͨDB������
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

//�ύ����
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
		//����û����� ֪ͨ�ͻ���
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
		//��ȡ����ID
		int32 iReferDropID = taskInfo->iReferDropID;
		//�������������
		g_ItemManager->DropItems(player,iReferDropID,ItemOperate_TaskReward);
		player->ChangePlayerAttribute((AttributeType)taskInfo->iRewardType,(float)taskInfo->iRewardNum);
		player->ChangePlayerAttribute(ATTRIBUTE_EXP,(float)taskInfo->iPerserExp);
		player->ChangePlayerAttribute(ATTRIBUTE_REALMEXP,(float)taskInfo->iRealmExp);
		//�������֪ͨ�ͻ��˽�������
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
		//֪ͨ���ݿ������
		MsgTask_SubmitTask* pSubmitTask = (MsgTask_SubmitTask*)submitTaskStream.Get();
		pSubmitTask->Len = submitTaskStream.GetSize();
		g_GameServer->SendToDB(pSubmitTask);
	}
}



//��������б�
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