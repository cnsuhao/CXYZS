#pragma once

#include "LogManage.h"

// ��־��׽��.��Ҫʵ�ַ��ļ���¼
class LogCatcher
{
public:
    LogCatcher(const char* fileName);
    ~LogCatcher(void);


    void WriteToFile(int level, const char* type, const char* time, const char* messgae);
protected:
    std::string _fileName;
};

