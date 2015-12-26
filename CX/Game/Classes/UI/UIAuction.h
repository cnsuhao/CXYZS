/********************************************************************
created:	2014/12/6
author:		LSH
desc:		寄售面板
*********************************************************************/

#ifndef __UIAUCTION_H__
#define __UIAUCTION_H__

#include "UIBase.h"

class UIAuction : public UIBase
{
public:
	UIAuction();
	~UIAuction();	
	virtual bool init();
	CREATE_FUNC(UIAuction);
};

#endif	//__UIAUCTION_H__