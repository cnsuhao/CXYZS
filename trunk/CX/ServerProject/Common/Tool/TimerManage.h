/********************************************************************
created:	2015/07/09
author:		LL
desc:		��ʱ�������࣬���ڶ�ʱ������������
�÷�������AddTimer������ָ���ص�����void FunType(int); ������ѭ����ִ��run������
ע�⣺ʹ�ô˶�ʱ������������40�죬����ÿ��40������һ�ε��ԡ�
*********************************************************************/
#ifndef _TimerManage_
#define _TimerManage_
#include <map>
#include "..\GateServer\LinkManager.h"

class TimerManage;
typedef void (TimerManage::*FunType)(int);
typedef void (*FunType1)(Link_Info);
//typedef void (*FunType)(int);	//����ص�����ָ��

class CallbackBase
{
public:
    virtual void virtualCall(int argv) {}
	virtual void virtualCallPtr(void* argv) {}
};

template <class T>
class Callback : public CallbackBase
{
public:
    typedef void (T::*func)(int argv);
	typedef void (T::*func_void)(void* argv);
    Callback(T* obj, func f):m_Obj(obj),m_Func(f) {}
	Callback(T* obj, func_void f):m_Obj(obj),m_Func_void(f) {}
    virtual void virtualCall(int argv) { if(m_Obj && m_Func) { (m_Obj->*m_Func)(argv); } }
	virtual void virtualCallPtr(void* argv) { if(m_Obj && m_Func_void) { (m_Obj->*m_Func_void)(argv); } }
private:
    T*      m_Obj;
    func    m_Func;
	func_void m_Func_void;
};


struct TimerIt
{
	int Type;		        //��ʱ����
	unsigned int TimeMax;	//��ʱʱ��
	int CountDown;	        //ʣ�����
	void* UserData;			//�Զ�������ָ��
	CallbackBase* Callback;	//�ص�����ָ��
	TimerIt *Next;
};

class TimerManage
{
public:
	TimerManage(void);
	~TimerManage(void);	

	bool AddTimer(int type, unsigned int TimeMax, CallbackBase* callback, int CountDown = 1, void* pUserData = NULL);	//����һ����ʱ����m_TimerList����
	void Run();					//����ִ�ж�ʱ��,ʱ�䵽��ִ��	

protected:
	bool AddTimer(TimerIt* timer, unsigned int time);	//����һ����ʱ����m_TimerList����,��ָ����ʼʱ�䣬һ������2�μ�ʱ
	void CallBackTimer(TimerIt* timer);	//ִ�ж�ʱ���ص�
	bool ReMoveTimer(TimerIt* timer);	//�Ƴ����ö�ʱ��,��������Ч��ʱ���򷵻�true

	 FunType    _touchEventSelector;

	std::map<unsigned int, TimerIt*> m_TimerList;	//��ʱ���б�
	std::map<unsigned int, TimerIt*>::iterator m_TimerIt;
};
extern TimerManage* g_TimerManage;
#endif //_TimerManage_