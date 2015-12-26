/********************************************************************
created:	2015/7/27
author:		LL
desc:		�������,����������������߼�
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

	void Init();				//��ʼ��
	void OnGCAskActiveTask(Player* player,MsgBase* msg);//�ͻ��������ȡ����
	void OnGCAskSubmitTask(MsgBase* msg);	//�ͻ��������ύ����

	void OnMonsterKill(Monster* monster,Player* player); //֪ͨɱ������
	void OnGetItem(uint32 itemId,Player* player);//֪ͨ��ȡ��Ʒ
	void OnUseItem(Player* player,uint32 itemId,int32 useCount = 1);
	void OnCopyPass(uint32 copyId,Player* player); //֪ͨ����ͨ��
	void OnAttriButeChange(int32 attributeId,int32 attributeCount,Player* player);// �����������
	void OnDoTaskCheckItem(const TaskInfo* taskInfo,Player* player);//��Ʒ�������ߵ���ʹ��
	void OnDoTaskCollect(int32 itemId,Player* player);
	void OnEscortItems(uint32 taskId,uint32 npcId,Player* player);
	void OnSubmitTask(MsgTask_SubmitTask* msg,const TaskInfo* taskInfo,Player* player);

	void OnGetTaskList(MsgBase* msg);
};
extern TaskManager* g_TaskManager;
#endif	//__TASKMANAGER_H__
