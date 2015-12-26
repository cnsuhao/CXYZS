/********************************************************************
created:	2015/5/6
author:		LSH
desc:		版本控制器 
*********************************************************************/

#pragma once

class VersionController
{
    VersionController(void);
public:


    class Lister
    {
    public:
        virtual void OnChechFinished(bool result) {};
        virtual void OnUpdateFinished(bool result) {};
    };

    ~VersionController(void);
    
    static VersionController& GetInstance();

    void SetLister(Lister* lister) { _lister = lister; }

    // 检测版本
    void CheckVersion();
    // 获得本地版本
    unsigned int GetLocalVersion();
    // 设置本地版本
    void SetLocalVersion(unsigned int version);
    // 检测制定版本的下一个版本
    int GetNextVerion(unsigned int version);
    // 得到最新版本
    int GetLastVerion();
    // 更新到制定版本
    void UpdateToVersion(unsigned int version);
    // 获得当前下载数量
    int GetDownloadInfo();
    // 获得最大下载量
    int GetMaxDownloadInfo();

    void Update(float dt);
private:
    void WriteFiles();
private:
    Lister* _lister;
    int state;
    unsigned int requestVersion;
};
