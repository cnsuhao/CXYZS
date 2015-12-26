/********************************************************************
created:	2015/01/8
author:		LL
desc:		任务管理
*********************************************************************/
#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__

class TaskManager
{
public:
	TaskManager();
	~TaskManager();

	void Init();				//初始化
	void Release();				//释放
	void Update(float delta);	//更新
	
};

#endif