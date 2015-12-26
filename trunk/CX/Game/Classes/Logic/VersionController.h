/********************************************************************
created:	2015/5/6
author:		LSH
desc:		�汾������ 
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

    // ���汾
    void CheckVersion();
    // ��ñ��ذ汾
    unsigned int GetLocalVersion();
    // ���ñ��ذ汾
    void SetLocalVersion(unsigned int version);
    // ����ƶ��汾����һ���汾
    int GetNextVerion(unsigned int version);
    // �õ����°汾
    int GetLastVerion();
    // ���µ��ƶ��汾
    void UpdateToVersion(unsigned int version);
    // ��õ�ǰ��������
    int GetDownloadInfo();
    // ������������
    int GetMaxDownloadInfo();

    void Update(float dt);
private:
    void WriteFiles();
private:
    Lister* _lister;
    int state;
    unsigned int requestVersion;
};
