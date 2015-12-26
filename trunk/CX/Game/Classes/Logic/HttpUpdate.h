#pragma once

/********************************************************************
created:	2015/5/6
author:		LSH
desc:		http resource update 
*********************************************************************/

#include "ClientDef.h"

class UpdateMng
{
    UpdateMng();
public:
    typedef std::vector<char> responseData;


    ~UpdateMng();
    
    static UpdateMng& GetInstance();

    //>���÷�������ַ�Ͷ˿�
    void SetIPAndPort(const std::string& ip, const std::string& port);
    
    // ����汾�嵥
    void RequestVersion();
    // ��ȡ����
    int GetVersionRate() { return version_rate; }// ����(0 - 100)���µĽ���, -1��ʾ����ʧ��.
    // ��ð汾�嵥
    const std::vector<unsigned int>& GetVersionList() { return versionsList; }

    // ��������б��ļ��嵥
    void RequestFiles(unsigned int version);
    // ��ȡ����
    int GetFilesRate() const { return files_rate; } // ����(0 - 100)���µĽ���, -1��ʾ����ʧ��.
    // ����ļ��嵥
    const std::vector<std::string>& GetFilesList();
    
    // ��������ļ�����
    void RequestData(unsigned int version, const std::vector<std::string>& fileList);
    // ��ȡ����
    int GetDataRate() const { return (int)ceil(datas_rate); }
    // ��������ֽ�
    int GetDownloadBytes() const { return downloadbytes; }
    
    const std::map<std::string, responseData>& GetFilesData() { return dataList; }
    
private:
    std::string m_HttpAddress;  // ip and port

    int version_rate;
    int files_rate;
    float datas_rate;
    float data_step;
    int downloadbytes;

    std::vector<unsigned int> versionsList;
    std::vector<std::string> fileList;
    std::map<std::string, responseData> dataList;

};