/********************************************************************
created:	2015/07/09
author:		LL
desc:		定时器管理类，用于定时触发函数调用
用法：调用AddTimer函数并指定回调函数void FunType(int); 并在主循环处执行run函数。
注意：使用此定时器最多持续工作40天，至少每隔40天重启一次电脑。
*********************************************************************/
#ifndef _TimerManage_
#define _TimerManage_
#include <map>
#include "..\GateServer\LinkManager.h"

class TimerManage;
typedef void (TimerManage::*FunType)(int);
typedef void (*FunType1)(Link_Info);
//typedef void (*FunType)(int);	//定义回调函数指针

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
	int Type;		        //计时类型
	unsigned int TimeMax;	//计时时间
	int CountDown;	        //剩余次数
	void* UserData;			//自定义数据指针
	CallbackBase* Callback;	//回调函数指针
	TimerIt *Next;
};

class TimerManage
{
public:
	TimerManage(void);
	~TimerManage(void);	

	bool AddTimer(int type, unsigned int TimeMax, CallbackBase* callback, int CountDown = 1, void* pUserData = NULL);	//增加一个定时器到m_TimerList队列
	void Run();					//持续执行定时器,时间到则执行	

protected:
	bool AddTimer(TimerIt* timer, unsigned int time);	//增加一个定时器到m_TimerList队列,并指定开始时间，一般用于2次计时
	void CallBackTimer(TimerIt* timer);	//执行定时器回调
	bool ReMoveTimer(TimerIt* timer);	//移除无用定时器,若还有有效定时器则返回true

	 FunType    _touchEventSelector;

	std::map<unsigned int, TimerIt*> m_TimerList;	//定时器列表
	std::map<unsigned int, TimerIt*>::iterator m_TimerIt;
};
extern TimerManage* g_TimerManage;
#endif //_TimerManage_