#include "UIHttpUpdate.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
using namespace cocostudio;

#include "UIWebView.h"
#include "BaseFunction.h"

UIHttpUpdate::UIHttpUpdate():m_Panel(NULL)
{
    m_CurVersionLabel       = NULL;
    m_LastVersionLabel      = NULL;
    m_NowAndTotalLabel      = NULL;
    m_DownloadPercentLabel  = NULL;
    m_ProgressBar = NULL;
}

UIHttpUpdate::~UIHttpUpdate()
{

}

bool UIHttpUpdate::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(!UIBase::init());
        m_Panel = CSLoader::createNode("UI/HttpUpdate.csb");
        if (m_Panel == NULL)
        {
            log("load UI/HttpUpdate.csb failed!");
            return false;
        }

        m_CurVersionLabel = static_cast<Text*>(m_Panel->getChildByName("currentVersionText"));
        m_LastVersionLabel = static_cast<Text*>(m_Panel->getChildByName("lastVersionText"));
        m_NowAndTotalLabel = static_cast<Text*>(m_Panel->getChildByName("downloadProgressBarText"));
        m_DownloadPercentLabel = static_cast<Text*>(m_Panel->getChildByName("downloadRateText"));
        m_ProgressBar = static_cast<LoadingBar*>(m_Panel->getChildByName("LoadingBar"));

        if (m_CurVersionLabel == NULL || m_LastVersionLabel == NULL ||
            m_NowAndTotalLabel == NULL || m_DownloadPercentLabel == NULL)
        {
            log("UIHttpUpdate get sub control failed.");
            return false;
        }
        addChild(m_Panel);

//#ifndef _WIN32
//        Size winSize = Director::getInstance()->getVisibleSize();
//        auto _webView = cocos2d::experimental::ui::WebView::create();
//        _webView->setPosition(winSize/2);
//        _webView->setContentSize(winSize * 0.5);
//        _webView->loadFile("gonggao.html");
//        _webView->setScalesPageToFit(true);
//
//        //_webView->setOnShouldStartLoading(CC_CALLBACK_2(WebViewTest::onWebViewShouldStartLoading, this));
//        //_webView->setOnDidFinishLoading(CC_CALLBACK_2(WebViewTest::onWebViewDidFinishLoading, this));
//        //_webView->setOnDidFailLoading(CC_CALLBACK_2(WebViewTest::onWebViewDidFailLoading, this));
//
//        addChild(_webView);
//#endif // !_WIN32

        bRet = true;
    } while (0);

    return bRet;
}

void UIHttpUpdate::SetCurrentVersion(const string& verstion)
{
    if (m_CurVersionLabel != NULL)
    {
        m_CurVersionLabel->setString(verstion);
    }
}

void UIHttpUpdate::SetLastVerstion(const string& verstion)
{
    if (m_LastVersionLabel != NULL)
    {
        m_LastVersionLabel->setString(verstion);
    }
}

void UIHttpUpdate::SetDownloadSize(uint32 nowSize, uint32 totalSize)
{
    if (m_NowAndTotalLabel != NULL)
    {
        m_NowAndTotalLabel->setString(StringUtils::format("%d/%d", nowSize, totalSize));
    }
}

void UIHttpUpdate::SetDownloadPercent(uint32 rate)
{
    if (m_DownloadPercentLabel != NULL)
    {
        m_DownloadPercentLabel->setString(StringUtils::format("%d%%", rate));
        m_ProgressBar->setPercent((float)rate);
    }
}
