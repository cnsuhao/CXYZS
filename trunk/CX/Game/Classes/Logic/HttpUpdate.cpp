#pragma warning(disable:4996)

#include "HttpUpdate.h"
#include "HttpClient.h"
#include "BaseFunction.h"
#include <sstream>

USING_NS_CC;

using namespace std;

using namespace network;

UpdateMng::UpdateMng()
{
    files_rate = version_rate = 100;
    datas_rate = 100.0f;
    downloadbytes = 0;
}

UpdateMng::~UpdateMng()
{

}

UpdateMng& UpdateMng::GetInstance()
{
    static UpdateMng mng;
    return mng;
}


void UpdateMng::SetIPAndPort(const std::string& ip, const std::string& port)
{
    m_HttpAddress.clear();
    m_HttpAddress.append(ip);
    m_HttpAddress.append(":");
    m_HttpAddress.append(port);
}

void UpdateMng::RequestVersion()
{
    if (version_rate == 100 || version_rate == -1)
    {
        // 上次请求完成
        versionsList.clear();
        HttpRequest* requset = new HttpRequest();
        requset->setUrl( (m_HttpAddress + string("/version.txt")).c_str() );
        requset->setRequestType(HttpRequest::Type::GET);
        requset->setTag("RequestVersion");
        requset->setResponseCallback([this](HttpClient* client, HttpResponse* response)
        {
            long statusCode = response->getResponseCode();
            log("response code: %ld, tag=%s", statusCode, response->getHttpRequest()->getTag());

            if (!response->isSucceed()) {
                log("response failed");
                log("error buffer: %s",response->getErrorBuffer());
                version_rate = -1;
                return;
            }

            std::vector<char>* buffer = response->getResponseData();
            std::string s;
            s.assign(buffer->begin(), buffer->end());
            log("%s", BaseFunction::a2u("获得版本列表: ").c_str());
            log("%s", s.c_str());

            std::stringstream sstream(s);
            unsigned int t;
            while (!sstream.eof())
            {
                sstream >> t;
                versionsList.push_back(t);
            }

            version_rate = 100;
        });
        HttpClient::getInstance()->send(requset);
        requset->release();

        version_rate = 0;
    }
}

void UpdateMng::RequestFiles(unsigned int version)
{
    if (files_rate == 100 || files_rate == -1)
    {
        // 上次请求完成
        fileList.clear();
        HttpRequest* requset = new HttpRequest();
        std::stringstream sstream;
        sstream << m_HttpAddress; sstream << '/'; 
        sstream << version;
        sstream << "/updatelist.txt";
        std::string url;
        sstream >> url;
        requset->setUrl(url.c_str());
        requset->setRequestType(HttpRequest::Type::GET);
        requset->setTag("RequestFiles");
        requset->setUserData(reinterpret_cast<int*>(version));
        requset->setResponseCallback([this](HttpClient* client, HttpResponse* response)
        {
            long statusCode = response->getResponseCode();
            log("response code: %ld, tag=%s", statusCode, response->getHttpRequest()->getTag());

            if (!response->isSucceed()) {
                log("response failed");
                log("error buffer: %s",response->getErrorBuffer());
                files_rate = -1;
                return;
            }

			int* i = (int*)response->getHttpRequest()->getUserData();
			long long version = (long long)&i;
            std::vector<char>* buffer = response->getResponseData();
            std::string s;
            s.assign(buffer->begin(), buffer->end());
            log("%s%d%s", BaseFunction::a2u("获得版本[").c_str(), version, BaseFunction::a2u("]文件列表: ").c_str());
            log("%s", s.c_str());
            std::stringstream sstream(s);
            while (!sstream.eof())
            {
                int size = fileList.size();
                fileList.resize(size + 1);
                sstream >> fileList[size];
            }

            files_rate = 100;
        });
        HttpClient::getInstance()->send(requset);
        requset->release();

        files_rate = 0;
    }
}

const std::vector<std::string>& UpdateMng::GetFilesList()
{
    // 删除名字为空的文件.
    std::vector<std::string>::iterator it = fileList.begin();
    for (;it != fileList.end(); )
    {
        if ((*it).size() <= 0)
        {
            it = fileList.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return fileList;
}

void UpdateMng::RequestData(unsigned int version, const std::vector<std::string>& fileList)
{
    if (datas_rate == 100.0f || datas_rate == -1.0f)
    {
        dataList.clear();
        data_step = 100.0f / (1.0f * fileList.size());
        for (size_t i = 0; i < fileList.size(); ++i)
        {
            HttpRequest* requset = new HttpRequest();
            std::stringstream sstream;
            sstream << m_HttpAddress; sstream << '/'; 
            sstream << version; sstream << '/';
            sstream << fileList[i];
            std::string url;
            sstream >> url;
            requset->setUrl(url.c_str());
            requset->setRequestType(HttpRequest::Type::GET);
            requset->setTag(fileList[i].c_str());
            requset->setResponseCallback([this](HttpClient* client, HttpResponse* response)
            {
                long statusCode = response->getResponseCode();
                log("response code: %ld, url=%s", statusCode, response->getHttpRequest()->getUrl());

                if (!response->isSucceed()) {
                    log("response failed");
                    log("error buffer: %s",response->getErrorBuffer());
                    datas_rate = -1.0f;
                    return;
                }
                if (datas_rate == -1.0f)
                {
                    return;
                }

                std::vector<char>* buffer = response->getResponseData();
                dataList[string(response->getHttpRequest()->getTag())].resize(buffer->size());
                downloadbytes += buffer->size();
                std::copy(buffer->begin(), buffer->end(),dataList[string(response->getHttpRequest()->getTag())].begin());

                datas_rate = MIN(100.0f, datas_rate + data_step);
            });
            HttpClient::getInstance()->send(requset);
            requset->release();
        }
        
        datas_rate = 0;
    }
}
