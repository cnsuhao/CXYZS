/********************************************************************
created:	2014/12/6
author:		LSH
desc:		队伍面板
*********************************************************************/

#ifndef __UITEAM_H__
#define __UITEAM_H__

#include "UIBase.h"

class UITeam : public UIBase
{
public:
	UITeam();
	~UITeam();	
	virtual bool init();
	CREATE_FUNC(UITeam);
};

#endif	//__UITEAM_H__