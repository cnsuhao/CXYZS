#include "VersionController.h"
#include "NetManager.h"
#include "HttpUpdate.h"
#include "CCUserDefault.h"
#include "CCFileUtils.h"
#include <sstream>

USING_NS_CC;

const char clientKey[] = "client_version";
 
VersionController::VersionController(void):_lister(nullptr),state(0)
{

}


VersionController::~VersionController(void)
{
}

VersionController& VersionController::GetInstance()
{
    static VersionController controller;
    return controller;
}

unsigned int VersionController::GetLocalVersion()
{
    unsigned int version = 10100;
    std::string versionStr = UserDefault::getInstance()->getStringForKey(clientKey);
    if (!versionStr.empty())
    {
        std::stringstream sstream(versionStr);
        sstream >> version;
    }
  
    return version;
}


void VersionController::SetLocalVersion(unsigned int version)
{
    std::stringstream sstream;
    std::string verStr;
    sstream << version;
    sstream >> verStr;
    UserDefault::getInstance()->setStringForKey(clientKey, verStr);
}

int VersionController::GetNextVerion(unsigned int version)
{
    const std::vector<unsigned int>& list = UpdateMng::GetInstance().GetVersionList();
    CCASSERT(UpdateMng::GetInstance().GetVersionRate() == 100, "");
    CCASSERT(!list.empty(), "");
    
    unsigned int min = 9999;
    int idx = -1;
    for (size_t i = 0; i < list.size(); ++i)
    {
        if (version < list[i])
        {
            if (list[i] - version < min)
            {
                min = list[i] - version;
                idx = i;
            }
        }
    }

    if (idx != -1)
    {
        return list[idx];
    }

    return version;
}

int VersionController::GetLastVerion()
{
    const std::vector<unsigned int>& list = UpdateMng::GetInstance().GetVersionList();
    if (!list.empty())
    {
        return list[list.size() - 1];
    }
    return GetLocalVersion();
}

void VersionController::CheckVersion()
{
    UpdateMng::GetInstance().SetIPAndPort(g_NetManager->GetResIP(), g_NetManager->GetResPort());
    UpdateMng::GetInstance().RequestVersion();
    state = 1;
}

void VersionController::UpdateToVersion(unsigned int version)
{
    UpdateMng::GetInstance().RequestFiles(version);
    state = 2;
    requestVersion = version;
}

int VersionController::GetDownloadInfo()
{
    if(state == 3)
        return UpdateMng::GetInstance().GetDownloadBytes(); 
    
    return 0;
}

int VersionController::GetMaxDownloadInfo()
{
    return 100;
}

void VersionController::Update(float dt)
{
    if (state == 1)
    {
        int rate = UpdateMng::GetInstance().GetVersionRate();
        if (rate == 100 || rate == -1)
        {
            if (_lister)
            {
                state = 0;
                _lister->OnChechFinished(rate == 100);
            }
        }
    }
    else if (state == 2)
    {
        int rate = UpdateMng::GetInstance().GetFilesRate();
        if (rate == 100)
        {
            state = 3;
            UpdateMng::GetInstance().RequestData(requestVersion,UpdateMng::GetInstance().GetFilesList());
        }
        else if(rate == -1)
        {
            if (_lister)
            {
                state = 0;
                _lister->OnUpdateFinished(false);
            }
        }
    }
    else if (state == 3)
    {
        int rate = UpdateMng::GetInstance().GetDataRate();
        if (rate == 100 || rate == -1)
        {
            if (_lister)
            {
                state = 0;
                WriteFiles();
                SetLocalVersion(requestVersion);
                _lister->OnUpdateFinished(true);
            }
        }
    }
    
}

void VersionController::WriteFiles()
{
    std::string writablePath = FileUtils::getInstance()->getWritablePath();

    const std::map<std::string, UpdateMng::responseData>& datas = UpdateMng::GetInstance().GetFilesData();

    for (std::map<std::string, UpdateMng::responseData>::const_iterator it = datas.begin();
        it != datas.end(); ++it)
    {
        std::string key = it->first;
        std::string fileName = writablePath + it->first;

        int offset = 0;
        size_t p = key.find('/');
        while (p != std::string::npos)
        { 
            FileUtils::getInstance()->createDirectory(writablePath + key.substr(0, p));
            offset = p + 1;
            p = key.find('/', offset);
        }
        

        FILE* fp = fopen(fileName.c_str(), "wb");
        if (fp)
        {
            size_t ret = fwrite(it->second.data(), 1, it->second.size(), fp);
            fclose(fp);
            if (ret != 0)
                log("Writing file to writable path succeed.");
        }
    }
}


