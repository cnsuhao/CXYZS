/********************************************************************
created:	2015/10/31
author:		LL
desc:		�����ӽ���
*********************************************************************/

#ifndef __UICHESTITEM_H__
#define __UICHESTITEM_H__

#include "UIBase.h"
struct CliItemInfo;

class UIChestItem : public UIBase
{
public:
	UIChestItem();
	~UIChestItem();	
	virtual bool init();
	CREATE_FUNC(UIChestItem);

	void AddChestItem(CliItemInfo* pItem);
private:
	//�����
	cc2d::Node *m_wingPanel;
	int m_num;
};

#endif	//__UICHESTITEM_H__