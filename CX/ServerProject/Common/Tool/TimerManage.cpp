#include "TimerManage.h"
#include <windows.h>

TimerManage *g_TimerManage = NULL;

TimerManage::TimerManage(void)
{	
}

TimerManage::~TimerManage(void)
{
}

bool TimerManage::AddTimer(int type, unsigned int TimeMax, CallbackBase* callback, int CountDown, void* pUserData)
{
	TimerIt *temp = new TimerIt;
	temp->Type = type;
	temp->TimeMax = TimeMax;
	temp->CountDown = CountDown;
	temp->Callback = callback;
	temp->UserData = pUserData;
	temp->Next = NULL;	
	return AddTimer(temp, timeGetTime());
}

/*
bool TimerManage::AddTimer(Link_Info linkInfo, unsigned int TimeMax, CallbackBase* callback, int CountDown)
{
	TimerIt *temp = new TimerIt;
	temp->Type = 1;
	temp->TimeMax = TimeMax;
	temp->CountDown = CountDown;
	temp->Callback = callback;
	temp->Next = NULL;	
	return AddTimer(temp, timeGetTime());
}
*/
bool TimerManage::AddTimer(TimerIt* timer, unsigned int time)
{
	//取对应定时器中最后一条，并加入
	TimerIt* temp = m_TimerList[timer->TimeMax + time];
	if (temp)
	{		
		while (temp->Next)
		{
			temp = temp->Next;
		}
		temp->Next = timer;
	}
	else
	{
		m_TimerList[timer->TimeMax + time] = timer;
	}
	return true;
}

void TimerManage::Run()
{	
	m_TimerIt = m_TimerList.begin();
	if (m_TimerIt != m_TimerList.end())
	{		
		if (timeGetTime() >= m_TimerIt->first)
		{
			TimerIt* out = m_TimerIt->second;
			m_TimerList.erase(m_TimerIt);
			CallBackTimer(out);
			if (ReMoveTimer(out))
			{
				AddTimer(out, m_TimerIt->first);
			}							
		}				
	}
}

void TimerManage::CallBackTimer(TimerIt* timer)
{
	TimerIt* temp = timer;
	//遍历执行回调函数
	while (temp)
	{
		if (temp->Callback)
		{
			if (temp->CountDown > 0)
			{
				temp->CountDown -= 1;
			}
			if (temp->UserData != NULL)
			{
				temp->Callback->virtualCallPtr(temp->UserData);
			}
			else
			{
				temp->Callback->virtualCall(temp->Type);
			}
		}
		temp = temp->Next;
	}
}

bool TimerManage::ReMoveTimer(TimerIt* timer)
{	
	TimerIt* temp = NULL;
	//遍历删除已结束的定时器
	while (timer)
	{
		if (timer->CountDown == 0)
		{
			temp = timer->Next;
            delete timer->Callback;
			delete timer;			
			timer = temp;
		}
		else
		{
			timer = timer->Next;
		}
	}
	return timer != NULL;
}