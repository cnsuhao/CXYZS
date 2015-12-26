#pragma once

/********************************************************************
created:	2014/7/28
author:		lishihai
desc:		�ؼ��ֹ�����
*********************************************************************/

#include <string>

class FilteWord
{
public:
    FilteWord(const char* configFile);
    ~FilteWord(void);

    bool Ok(const char* keyword);   //>���˼��. �������trun˵��û�б����˷�֮˵����������.

    //bool Seach(const char* expression); //>���� ͨ��������ʽ(ֻ����չ�����ӿ�ռʱ����). 

    std::string Replace(const char* keyword, const std::string& repWord = "***");   //>�滻���˴�. ����Ok�ɹ������ʹ��.����������.(����ͻ���ʹ��,�����ڷ�����)
private:
    std::string m_Content;
};

//-----------------------------------------------------------------------------------------
// ʹ�÷���
// //>Ok����
// FilteWord gNamefilter("Data/FliterWord-Name.txt");   //>Name.txt�к��йؼ���"��Сƽ"
// bool result = gNamefilter.Ok("���ǵ�Сƽ");          //>resultΪfalse��ʾ��ͨ�� 
// 
// //>Replace����
// FilteWord gChatfilter("Data/FliterWord-Chat.txt");                       //>Chat.txt�к��йؼ���"ë��","��Сƽ" 
// string result = gNamefilter.Replace("ë�󶫺͵�Сƽ�ǹ�����ϯ");         //>resultΪ"***��***�ǹ�����ϯ"
// string result = gNamefilter.Replace("ë�󶫺͵�Сƽ�ǹ�����ϯ", "xyz");  //>resultΪ"xyz��xyz�ǹ�����ϯ"