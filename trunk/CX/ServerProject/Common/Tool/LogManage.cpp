#include "LogManage.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <time.h>
#include "LogCatcher.h"
#include <windows.h>

LogManage gLog;

char staticBuffer[40960];
char staticMessage[40960];
char staticHead[256];
char staticTime[256];

void* stdout_handle;

#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TPURPLE FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY

#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE TNORMAL | FOREGROUND_INTENSITY

const char htmlHead[] = "<?xml version=\"1.0\" encoding=\"gb2321\" standalone=\"yes\"?>\n" 
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
    "<html><head>\n"
    "<title>Server Log File</title>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2321\"/>\n"
    "<style type=\"text/css\">\n"
    "body { background: #1a242a; color: #b4c8d2; margin-right: 20px; margin-left: 20px; font-size: 16px; font-family: 宋体, sans-serif, sans; }\n"
    "a { text-decoration: none; }\n"
    "a:link { color: #b4c8d2; }\n"
    "a:active { color: #ff9900; }\n"
    "a:visited { color: #b4c8d2; }\n"
    "a:hover { color: #ff9900; }\n"
    "h1 { text-align: center; }\n"
    "h2 { color: #ffffff; }\n"
    ".I, .N, .W, .E, D., D1, D2 { padding: 2px; overflow: auto; }\n"
    ".I { background-color: #080c10; color: #b4c8d2; }\n"
    ".N { background-color: #839ca7; color: #1a242a; }\n"
    ".W { background-color: #00ff00; color: #1a242a; }\n"
    ".E { background-color: #ff0000; color: #1a242a; }\n"
    ".D { background-color: #ffff00; color: #1a242a; }\n"
    ".D1 { background-color: #ff00ff; color: #1a242a; }\n"
    ".D2 { background-color: #00ffff; color: #1a242a; }\n"
    "</style></head><body>\n";
//    "<h1><a href=\"http://www.baidu.com/s?wd=从前有座山\">从前有座山</a> log file</h1>\n";

const char htmlTail[] = "</body></html>";

LogManage::LogManage(void)
{
	stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    showLogLevel = 0;
    hf = INVALID_HANDLE_VALUE;
}

LogManage::~LogManage(void)
{
    if (hf)
    {
        DWORD oWriteNumber;
        // 写入html尾
        WriteFile(hf, htmlTail, sizeof(htmlTail), &oWriteNumber, NULL);
        FlushFileBuffers(hf);
        CloseHandle(hf);
        hf = INVALID_HANDLE_VALUE;
    }
}

void LogManage::AddLog(LoggerLevel level, char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    switch (level)
    {
    case LOG_LEVEL_INFO: VInfo(format, argptr); break;
    case LOG_LEVEL_NITIFY: VNotify(format, argptr); break;
    case LOG_LEVEL_WARNING: VWarning(format, argptr); break;
    case LOG_LEVEL_ERROR: VError(format, argptr); break;
    case LOG_LEVEL_DEBUG: VDebug(format, argptr); break;
    case LOG_LEVEL_DEBUG1: VDebug1(format, argptr); break;
    case LOG_LEVEL_DEBUG2: VDebug2(format, argptr); break;
    default:
        {
            std::map<int, LogCatcher*>::iterator it = catchers.find(level);
            if (it != catchers.end() && it->second != NULL)
            {
                // 检查捕捉器是否感兴趣
                sprintf(staticHead, "E ");
                GetTime(staticTime);
                vsprintf(staticMessage, format, argptr);
                it->second->WriteToFile(level, staticHead, staticTime, staticMessage);
            }
        }
        break;
    }
    va_end(argptr);
}

void LogManage::Error(char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    VError(format, argptr);
    va_end(argptr);
}

void LogManage::Warning(char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    VWarning(format, argptr);
    va_end(argptr);
}

void LogManage::Notify(char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    VNotify(format, argptr);
    va_end(argptr);
}

void LogManage::Info(char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    VInfo(format, argptr);
    va_end(argptr);
}

void LogManage::Debug(char* format, ...)
{
#ifdef _DEBUG
    va_list argptr;
    va_start(argptr, format);
    VDebug(format, argptr);
    va_end(argptr);
#endif // _DEBUG
}

void LogManage::Debug1(char* format, ...)
{
#ifdef _DEBUG
    va_list argptr;
    va_start(argptr, format);
    VDebug1(format, argptr);
    va_end(argptr);
#endif // _DEBUG
}

void LogManage::Debug2(char* format, ...)
{
#ifdef _DEBUG
    va_list argptr;
    va_start(argptr, format);
    VDebug2(format, argptr);
    va_end(argptr);
#endif // _DEBUG
}

void LogManage::MsgIO(bool issend, int sysid, int msgid, int ilen)
{
	sprintf(staticHead, "MSG ");
	GetTime(staticTime);	
	std::string out;
	if (issend)
	{
		out += "send ";
	}
	else
	{
		out += "get ";
	}
	switch (sysid)
	{
	case 142: out += "ID_C2L"; break;
	case 143: out += "ID_L2C"; break;
	case 144: out += "ID_C2G"; break;
	case 145: out += "ID_G2C"; break;
	case 146: out += "ID_D2C"; break;
	case 147: out += "ID_G2L"; break;
	case 148: out += "ID_L2G"; break;
	case 149: out += "ID_G2D"; break;
	case 150: out += "ID_D2G"; break;
	case 151: out += "ID_GAME2GATE"; break;
	default:
		break;
	}
	sprintf(staticBuffer, "%s%s%s id=%d len=%d", staticHead, staticTime, out.c_str(), msgid, ilen);
	SetColor(TPURPLE);
	int len = strlen(staticBuffer);
	if(staticBuffer[len - 1] != '\n')
	{
		staticBuffer[len] = '\n';
		staticBuffer[len+1] = '\0';
	}
	fputs(staticBuffer, stdout);
	SetColor(TNORMAL);	
}


void LogManage::SetName(const char* pName)
{
    if (pName)
    {
        name.assign(pName);
    }
}

void LogManage::NewLine()
{
    fputs("\n", stdout);
}

void LogManage::SetLogCatcher(int level, LogCatcher* catcher)
{
    std::map<int, LogCatcher*>::iterator it = catchers.find(level);
    if (it != catchers.end())
    {
        it->second = catcher;
    }
    else
    {
        catchers[level] = catcher;
    }
}

void LogManage::VError(char *format, va_list argptr)
{
    sprintf(staticHead, "E ");
    GetTime(staticTime);
    vsprintf(staticMessage, format, argptr);
    sprintf(staticBuffer, "%s%s%s", staticHead, staticTime, staticMessage); 
    WritToFile(LOG_LEVEL_ERROR);       //write to file.
    if (showLogLevel <= LOG_LEVEL_ERROR)
    {// show to screen.
        SetColor(TRED);
        int len = strlen(staticBuffer);
        if(staticBuffer[len - 1] != '\n'){
            staticBuffer[len] = '\n';
            staticBuffer[len+1] = '\0';
        }
        fputs(staticBuffer, stdout);
        SetColor(TNORMAL);
    }
}

void LogManage::VWarning(char *format, va_list argptr)
{
    sprintf(staticHead, "W ");
    GetTime(staticTime);
    vsprintf(staticMessage, format, argptr);
    sprintf(staticBuffer, "%s%s%s", staticHead, staticTime, staticMessage); 
    WritToFile(LOG_LEVEL_WARNING);       //write to file.
    if (showLogLevel <= LOG_LEVEL_WARNING)
    {// show to screen.
        SetColor(TGREEN);
        int len = strlen(staticBuffer);
        if(staticBuffer[len - 1] != '\n'){
            staticBuffer[len] = '\n';
            staticBuffer[len+1] = '\0';
        }
        fputs(staticBuffer, stdout);
        SetColor(TNORMAL);
    }
}

void LogManage::VNotify(char* format, va_list argptr)
{
    sprintf(staticHead, "N ");
    GetTime(staticTime);
    vsprintf(staticMessage, format, argptr);
    sprintf(staticBuffer, "%s%s%s", staticHead, staticTime, staticMessage); 
    WritToFile(LOG_LEVEL_NITIFY);       //write to file.
    if (showLogLevel <= LOG_LEVEL_NITIFY)
    {// show to screen.
        SetColor(TWHITE);
        int len = strlen(staticBuffer);
        if(staticBuffer[len - 1] != '\n'){
            staticBuffer[len] = '\n';
            staticBuffer[len+1] = '\0';
        }
        fputs(staticBuffer, stdout);
        SetColor(TNORMAL);
    }
}

void LogManage::VInfo(char* format, va_list argptr)
{
    sprintf(staticHead, "I ");
    GetTime(staticTime);
    vsprintf(staticMessage, format, argptr);
    sprintf(staticBuffer, "%s%s%s", staticHead, staticTime, staticMessage); 
    WritToFile(LOG_LEVEL_INFO);       //write to file.
    if (showLogLevel <= LOG_LEVEL_INFO)
    {// show to screen.
        int len = strlen(staticBuffer);
        if(staticBuffer[len - 1] != '\n'){
            staticBuffer[len] = '\n';
            staticBuffer[len+1] = '\0';
        }
        fputs(staticBuffer, stdout);
    }
}

void LogManage::VDebug(char* format, va_list argptr)
{
    sprintf(staticHead, "D ");
    GetTime(staticTime);
    vsprintf(staticMessage, format, argptr);
    sprintf(staticBuffer, "%s%s%s", staticHead, staticTime, staticMessage); 
    WritToFile(LOG_LEVEL_DEBUG);       //write to file.
    if (showLogLevel <= LOG_LEVEL_DEBUG)
    {// show to screen.
        SetColor(TYELLOW);
        int len = strlen(staticBuffer);
        if(staticBuffer[len - 1] != '\n'){
            staticBuffer[len] = '\n';
            staticBuffer[len+1] = '\0';
        }
        fputs(staticBuffer, stdout);
        SetColor(TNORMAL);
    }
}

void LogManage::VDebug1(char* format, va_list argptr)
{
    sprintf(staticHead, "D ");
    GetTime(staticTime);
    vsprintf(staticMessage, format, argptr);
    sprintf(staticBuffer, "%s%s%s", staticHead, staticTime, staticMessage); 
    WritToFile(LOG_LEVEL_DEBUG1);       //write to file.
    if (showLogLevel <= LOG_LEVEL_DEBUG1)
    {// show to screen.
        SetColor(TPURPLE);
        int len = strlen(staticBuffer);
        if(staticBuffer[len - 1] != '\n'){
            staticBuffer[len] = '\n';
            staticBuffer[len+1] = '\0';
        }
        fputs(staticBuffer, stdout);
        SetColor(TNORMAL);
    }
}

void LogManage::VDebug2(char* format, va_list argptr)
{
    sprintf(staticHead, "D ");
    GetTime(staticTime);
    vsprintf(staticMessage, format, argptr);
    sprintf(staticBuffer, "%s%s%s", staticHead, staticTime, staticMessage);
    WritToFile(LOG_LEVEL_DEBUG2);       //write to file.
    if (showLogLevel <= LOG_LEVEL_DEBUG2)
    {// show to screen.
        SetColor(TBLUE);
        int len = strlen(staticBuffer);
        if(staticBuffer[len - 1] != '\n'){
            staticBuffer[len] = '\n';
            staticBuffer[len+1] = '\0';
        }
        fputs(staticBuffer, stdout);
        SetColor(TNORMAL);
    }
}

void LogManage::SetColor(unsigned int color)
{
    SetConsoleTextAttribute(stdout_handle, (WORD)color);
}

void LogManage::GetTime(char* buffer)
{
    time_t currTime = time(NULL);
    struct tm _tm;
    localtime_s(&_tm, &currTime);
    //fprintf(stderr,"%04d-%02d-%02d %02d:%02d:%02d ",_tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d ",_tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
}

void LogManage::WritToFile(int level)
{
    //>如果写文件效率慢,考虑直接走网络socket发送到logger服务器. logger.效率不是问题,只要不宕机日志迟早会写完的.
    static char tempMessage[40960];
    DWORD oWriteNumber;

    if(hf == INVALID_HANDLE_VALUE)
    {
        time_t currTime = time(NULL);
        struct tm _tm;
        localtime_s(&_tm, &currTime);
        char fileName[64];
        if (name.empty())
        {
            sprintf(fileName, "%04d-%02d-%02d(%02d.%02d).html",_tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min);
        }else
        {
            sprintf(fileName, "%s%04d-%02d-%02d(%02d.%02d).html",name.c_str(), _tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min);
        }
        
        hf = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        {
            // 写入html头
            WriteFile(hf, htmlHead, sizeof(htmlHead), &oWriteNumber, NULL);
            // title
            char title[256];
            if (!name.empty())
                sprintf(title, "<h1><a href=\"http://www.baidu.com/s?wd=从前有座山\">从前有座山</a> %s log</h1>\n",name.c_str());
            else
                sprintf(title, "<h1><a href=\"http://www.baidu.com/s?wd=从前有座山\">从前有座山</a> log file</h1>\n");

            WriteFile(hf, title, strlen(title), &oWriteNumber, NULL);
            FlushFileBuffers(hf);

            SetFilePointer(hf, 0, 0, FILE_END);
        }
    }

    bool hasType = true;
    if (hf != INVALID_HANDLE_VALUE)
    {
        switch (level)
        {
        case LOG_LEVEL_INFO:
            {
                sprintf(tempMessage, "<div class=\"I\">%s%s\"%s\"</div>\n",staticHead, staticTime, staticMessage);
            }break;
        case LOG_LEVEL_NITIFY:
            {
                sprintf(tempMessage, "<div class=\"N\">%s%s\"%s\"</div>\n",staticHead, staticTime, staticMessage);
            }break;
        case LOG_LEVEL_WARNING:
            {
                sprintf(tempMessage, "<div class=\"W\">%s%s\"%s\"</div>\n",staticHead, staticTime, staticMessage);
            }break;
        case LOG_LEVEL_ERROR:
            {
                sprintf(tempMessage, "<div class=\"E\">%s%s\"%s\"</div>\n",staticHead, staticTime, staticMessage);
            }break;
        case LOG_LEVEL_DEBUG:
            {
                sprintf(tempMessage, "<div class=\"D\">%s%s\"%s\"</div>\n",staticHead, staticTime, staticMessage);
            }break;
        case LOG_LEVEL_DEBUG1:
            {
                sprintf(tempMessage, "<div class=\"D1\">%s%s\"%s\"</div>\n",staticHead, staticTime, staticMessage);
            }break;
        case LOG_LEVEL_DEBUG2:
            {
                sprintf(tempMessage, "<div class=\"D2\">%s%s\"%s\"</div>\n",staticHead, staticTime, staticMessage);
            }break;
        default:{
                hasType = false;
                break;
            }
        }
    }

    if (hasType)
    {
        WriteFile(hf, tempMessage, strlen(tempMessage), &oWriteNumber, NULL);
        static unsigned int count = 0;
        ++count;
        if (count > 10)
        {
            FlushFileBuffers(hf);
            count = 0;
        }
    }

    std::map<int, LogCatcher*>::iterator it = catchers.find(level);
    if (it != catchers.end() && it->second != NULL)
    {
        it->second->WriteToFile(level, staticHead, staticTime, staticMessage);
    }
}
