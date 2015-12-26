#include "FilteWord.h"
#include <regex>
#include <cassert>

FilteWord::FilteWord(const char* configFile)
{
    FILE* pf = fopen(configFile, "r+b");
    if (pf)
    {
        long positon = ftell(pf);
        fseek(pf, 0, SEEK_END);
        long length = ftell(pf);
        rewind(pf);
        char* pBuffer = new char [length + 1];
        long fileSize = fread(pBuffer, 1, length, pf);
        pBuffer[fileSize] = '\0';
        std::regex patten("\r\n");
        m_Content = std::regex_replace(pBuffer, patten, "|");
        delete[] pBuffer;
    }
    else
    {
        assert(0);
    }
}

FilteWord::~FilteWord(void)
{
}

bool FilteWord::Ok(const char* keyword)
{
    std::regex patten(m_Content);
    return !std::regex_search(keyword, patten);
}

//bool FilteWord::Seach(const char* expression)
//{
//    std::regex patten(expression);
//
//    return !std::regex_search(m_Content, patten);
//}

std::string FilteWord::Replace(const char* keyword, const std::string& repWord /*= "*"*/)
{
    std::regex patten(m_Content);
    return std::regex_replace(keyword, patten, repWord);
}
