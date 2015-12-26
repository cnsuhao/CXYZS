#pragma once

#include "LogManage.h"

// 日志捕捉器.主要实现分文件记录
class LogCatcher
{
public:
    LogCatcher(const char* fileName);
    ~LogCatcher(void);


    void WriteToFile(int level, const char* type, const char* time, const char* messgae);
protected:
    std::string _fileName;
};

