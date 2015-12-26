/********************************************************************
created:	2015/8/3
author:		lishihai
desc:		http本本更新面板
*********************************************************************/

#ifndef __UIHTTPUPDATE_H__
#define __UIHTTPUPDATE_H__

#include "UIBase.h"
#include "GameDef.h"
#include "UILoadingBar.h"

class UIHttpUpdate : public UIBase
{
public:
	UIHttpUpdate();
	~UIHttpUpdate();	
    virtual bool init();
    CREATE_FUNC(UIHttpUpdate);

    void SetCurrentVersion(const string& verstion);       //>设置当日版本
    void SetLastVerstion(const string& verstion);         //>设置最新版本
    void SetDownloadSize(uint32 nowSize, uint32 totalSize); //>设置下载内容(当前下载量/总下载量)
    void SetDownloadPercent(uint32 rate);                  //>设置下载百分比

protected:

private:
    cc2d::Node*     m_Panel;
    cc2dui::Text*    m_CurVersionLabel;
    cc2dui::Text*    m_LastVersionLabel;
    cc2dui::Text*    m_NowAndTotalLabel;
    cc2dui::Text*    m_DownloadPercentLabel;
    cc2dui::LoadingBar* m_ProgressBar;
};

#endif	//__UIHTTPUPDATE_H__