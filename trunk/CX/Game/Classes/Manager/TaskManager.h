/********************************************************************
created:	2015/01/8
author:		LL
desc:		�������
*********************************************************************/
#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__

class TaskManager
{
public:
	TaskManager();
	~TaskManager();

	void Init();				//��ʼ��
	void Release();				//�ͷ�
	void Update(float delta);	//����
	
};

#endif