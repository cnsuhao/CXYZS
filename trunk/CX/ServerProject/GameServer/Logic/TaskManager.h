/********************************************************************
created:	2015/7/27
author:		LL
desc:		任务管理,处理任务及任务相关逻辑
*********************************************************************/

#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__

#include "GameDef.h"
#include "Monster.h"
#include "Player.h"
#include "MsgInfo.h"
struct TaskInfo;

class TaskManager
{
public:
	TaskManager();
	~TaskManager();

	void Init();				//初始化
	void OnGCAskActiveTask(Player* player,MsgBase* msg);//客户端请求接取任务
	void OnGCAskSubmitTask(MsgBase* msg);	//客户端请求提交任务

	void OnMonsterKill(Monster* monster,Player* player); //通知杀死怪物
	void OnGetItem(uint32 itemId,Player* player);//通知获取物品
	void OnUseItem(Player* player,uint32 itemId,int32 useCount = 1);
	void OnCopyPass(uint32 copyId,Player* player); //通知副本通关
	void OnAttriButeChange(int32 attributeId,int32 attributeCount,Player* player);// 玩家属性增加
	void OnDoTaskCheckItem(const TaskInfo* taskInfo,Player* player);//物品交付或者道具使用
	void OnDoTaskCollect(int32 itemId,Player* player);
	void OnEscortItems(uint32 taskId,uint32 npcId,Player* player);
	void OnSubmitTask(MsgTask_SubmitTask* msg,const TaskInfo* taskInfo,Player* player);

	void OnGetTaskList(MsgBase* msg);
};
extern TaskManager* g_TaskManager;
#endif	//__TASKMANAGER_H__
