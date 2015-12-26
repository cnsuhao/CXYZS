/********************************************************************
created:	2015/4/29
author:		LSH
desc:		创建角色
*********************************************************************/

#ifndef __UICREATEROLE_H__
#define __UICREATEROLE_H__

#include "UIBase.h"

class UICreateRole : public UIBase
{
public:
	UICreateRole();
	~UICreateRole();	
	virtual bool init();
	CREATE_FUNC(UICreateRole);

    static const int MaxHeadIndex = 3;

    static const string& GetHeadIconPath(RoleSex sex, int iconIndex);

    void SetSex(RoleSex sex);   // 设置性别
    inline RoleSex GetSex() const { return m_roleSex; }

    const string& GetRoleName() const;  // 获得角色名字

    unsigned char GetHeadIconIndex();   // 获得头像索引
protected:
    void SetDefaultHeadIndex();
    cc2d::Node* m_Root;

    cc2dui::ImageView* m_bodyAImg;
    cc2dui::ImageView* m_bodyBImg;

    cc2d::Sprite* m_heads[3];
    cc2d::Sprite* m_selectEffect;

    cc2dui::CheckBox* m_manCheckBox;
    cc2dui::CheckBox* m_wumanCheckBox;

    cc2dui::TextField* m_tfRoleName;

    RoleSex m_roleSex; // 角色性别
    unsigned char m_roleIconIndex;
};

#endif	//__UICREATEROLE_H__