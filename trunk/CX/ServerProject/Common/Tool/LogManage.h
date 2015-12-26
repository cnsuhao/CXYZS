/********************************************************************
created:	2015/07/18
author:		LL
desc:		日志管理类，用于记录服务器各种日志
*********************************************************************/
#ifndef _LogManage_
#define _LogManage_

#include <string>
#include <map>

class LogCatcher;

enum LoggerLevel
{
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_NITIFY,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_DEBUG1,
    LOG_LEVEL_DEBUG2,
};

// 非线程安全. 考虑打算线程安全使用另一个设计效率好的Logger
class LogManage
{
public:
	LogManage(void);
	~LogManage(void);	
    virtual void AddLog(LoggerLevel level, char *format, ...);

    void Error(char *format, ...);
    void Warning(char *format, ...);
    void Notify(char* format, ...);
    void Info(char* format, ...);

    void Debug(char* format, ...);
    void Debug1(char* format, ...);
    void Debug2(char* format, ...);

	void MsgIO(bool issend, int sysid, int msgid, int ilen);

    void SetName(const char* pName);
    void NewLine();

    inline unsigned int GetShowLevel() const { showLogLevel; }
    inline void SetShowLevel(int level) { showLogLevel = level; }

    void SetLogCatcher(int level, LogCatcher* catcher); // 根据不同管的服务器,选择安装的感兴趣的捕捉.
protected:
    void VError(char *format, va_list argptr);
    void VWarning(char *format, va_list argptr);
    void VNotify(char* format, va_list argptr);
    void VInfo(char* format, va_list argptr);

    void VDebug(char* format, va_list argptr);
    void VDebug1(char* format, va_list argptr);
    void VDebug2(char* format, va_list argptr);
protected:
	void SetColor(unsigned int color);
    void GetTime(char* buffer);
    void WritToFile(int level);
    unsigned int showLogLevel;
    void* hf;
    std::string name;

    std::map<int, LogCatcher*> catchers;
};

extern LogManage gLog;

#endif //_LogManage_