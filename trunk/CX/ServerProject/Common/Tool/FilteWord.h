#pragma once

/********************************************************************
created:	2014/7/28
author:		lishihai
desc:		关键字过滤器
*********************************************************************/

#include <string>

class FilteWord
{
public:
    FilteWord(const char* configFile);
    ~FilteWord(void);

    bool Ok(const char* keyword);   //>过滤检测. 如果返回trun说明没有被过滤反之说明被过滤了.

    //bool Seach(const char* expression); //>查找 通过正则表达式(只是扩展方法接口占时不用). 

    std::string Replace(const char* keyword, const std::string& repWord = "***");   //>替换过滤词. 必须Ok成功情况下使用.否则无意义.(建议客户端使用,不放在服务器)
private:
    std::string m_Content;
};

//-----------------------------------------------------------------------------------------
// 使用方法
// //>Ok方法
// FilteWord gNamefilter("Data/FliterWord-Name.txt");   //>Name.txt中含有关键字"邓小平"
// bool result = gNamefilter.Ok("我是邓小平");          //>result为false表示不通过 
// 
// //>Replace方法
// FilteWord gChatfilter("Data/FliterWord-Chat.txt");                       //>Chat.txt中含有关键字"毛泽东","邓小平" 
// string result = gNamefilter.Replace("毛泽东和邓小平是国家主席");         //>result为"***和***是国家主席"
// string result = gNamefilter.Replace("毛泽东和邓小平是国家主席", "xyz");  //>result为"xyz和xyz是国家主席"