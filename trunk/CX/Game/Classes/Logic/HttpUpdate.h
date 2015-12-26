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

    //>设置服务器地址和端口
    void SetIPAndPort(const std::string& ip, const std::string& port);
    
    // 请求版本清单
    void RequestVersion();
    // 获取进度
    int GetVersionRate() { return version_rate; }// 返回(0 - 100)更新的进度, -1表示请求失败.
    // 获得版本清单
    const std::vector<unsigned int>& GetVersionList() { return versionsList; }

    // 请求更新列表文件清单
    void RequestFiles(unsigned int version);
    // 获取进度
    int GetFilesRate() const { return files_rate; } // 返回(0 - 100)更新的进度, -1表示请求失败.
    // 获得文件清单
    const std::vector<std::string>& GetFilesList();
    
    // 请求更新文件数据
    void RequestData(unsigned int version, const std::vector<std::string>& fileList);
    // 获取进度
    int GetDataRate() const { return (int)ceil(datas_rate); }
    // 获得下载字节
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