#include "LogCatcher.h"


LogCatcher::LogCatcher(const char* fileName)
{
    _fileName.assign(fileName);
}


LogCatcher::~LogCatcher(void)
{
}

void LogCatcher::WriteToFile(int level, const char* type, const char* time, const char* messgae)
{

}
