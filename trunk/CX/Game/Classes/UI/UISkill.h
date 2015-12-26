/********************************************************************
created:	2015/12/4
author:		lishihai
desc:		功法面板
*********************************************************************/

#ifndef __UISKILL_H__
#define __UISKILL_H__

#include "UIBase.h"

class UISkill : public UIBase
{
public:
	UISkill();
	~UISkill();	
	virtual bool init();
	CREATE_FUNC(UISkill);

    virtual bool DisabledJoystickWhenSelfShow() { return true; }

    enum
    {
        Skill_Page = 2,

        WuJi_TypeNumer = 3,
        WuJi_SkillNumber = 4,

        JueXue_PageNumber = 3,
        JueXue_NumberSkillPerPage = 15,


        JueXue_PortablePageNumber = 3,
        JueXue_PortableNumber = 7,
    };

    enum WuJiType
    {
        WuJiType_Dao,
        WuJiType_Jian,
        WuJiType_FoChen,
    };


    void SetWuJiType(WuJiType type);


    void SetJueXuePageIndex(int index);
    void SetJueXuePortableIndex(int index);
   
protected:
    void SetWuJiSkillInfo(int index, int skillId, bool isSeleced);
    void SetWuJiSkillDesc(int skillId);
    void SetJueXueSkillDesc(int skillId);

    void SwitchPage(int id);

    void LoadJueXuePageIndex(int index);
    void LoadJueXuePortableIndex(int index);

private:
	//主面板
	cc2d::Node *m_RootPanel;
    cc2d::Node *m_WujiPanel;
    cc2d::Node *m_JueXuePanel;


    cc2dui::CheckBox* m_SkillPage[Skill_Page];
    cc2dui::CheckBox* m_WuJiType[WuJi_TypeNumer];
    cc2d::Node*       m_WuJiSkill[WuJi_SkillNumber];
    cc2dui::ImageView* m_HeightLight[WuJi_SkillNumber];


    cc2dui::Button*  m_JueXueNextPage;
    cc2dui::Button*  m_JueXuePreviousPage;
    cc2dui::Button*  m_JueXuePortableSwitch;

    cc2dui::ImageView* m_JueXuePageIndex[JueXue_PageNumber];
    cc2dui::ImageView* m_JueXuePortableIndex[JueXue_PortablePageNumber];

    cc2d::Node*     m_JueXueOfPage[JueXue_NumberSkillPerPage];
    cc2d::Node*     m_JueXuePortable[JueXue_PortableNumber];

    int32  m_SelectWuJiId;
    int32  m_CurJueXuePage;
    int32  m_CurPortableJueXuePage;
};

#endif	//__UISKILL_H__