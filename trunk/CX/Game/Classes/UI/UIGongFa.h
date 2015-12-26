/********************************************************************
created:	2014/12/6
author:		LSH
desc:		功法面板
*********************************************************************/

#ifndef __UIGONGFA_H__
#define __UIGONGFA_H__

#include "UIBase.h"

struct SkillTreeInfo;
struct SkillInfo;

class UIGongFa : public UIBase
{
public:
	UIGongFa();
	~UIGongFa();	
	virtual bool init();
	CREATE_FUNC(UIGongFa);

    static const int ActivteSkillNum = 4;
    static const int CanUseSkillNum = 7;
    virtual void OnShow();      //>显示回调,子类重载记得调用父类方法
    virtual bool DisabledJoystickWhenSelfShow() { return true; }

    void    SetCurSkillTreeType(uint32 treeType);               //>设置当前选择技能树类型.
    uint32  GetCurSkillTreeType()  { return m_CurTreeType; }    //>获得当前选择技能树类型.
    void    RelaodSkillTree();                                  //>获得新技能树重新加载

    //>设置技能后回调.
    void    OnSetCommonSkill(uint32 skillId);                   //>设置普通技能
    void    OnSetSkill1(uint32 skillId);                        //>设置技能1
    void    OnSetSkill2(uint32 skillId);                        //>设置技能2
    void    OnSetSkill3(uint32 skillId);                        //>设置技能3
    void    OnSetSkill4(uint32 skillId);                        //>设置技能4
    void    OnSetSkill5(uint32 skillId);                        //>设置技能5
    void    OnSetSkill6(uint32 skillId);                        //>设置技能6
    void    OnActivateSkillType(int32 skillTreeType);           //>激活主修技能树
private:
    void    SetupSkillsByTreeId(int32 skillTreeId);				//>根据功法ID设置信息
    void    SetSkillTreeInfo(int32 treeType);                   //>设置右边面板    
    void    SetSkillPanelInfo(uint32 skillId);                  //>设置左边面板
    void    ReloadChoseSkillIcon();                             //>重新设置下面面板


	//主面板
	cc2d::Node *m_gongfaPanel;

	cc2dui::Layout *m_Template;								//checkbox模板
	cc2dui::ScrollView *m_magicSV;							//功法技能列表
	map<int32, cc2dui::CheckBox*> m_mainChecks;					    //上面功法checkboxs
	void AddMagicItem(cc2dui::ScrollView* sv,cc2dui::Widget* layout,const SkillTreeInfo *tagSkillTree);
    void UpdateMagicItems();
	void LoadCheckBox();

	cc2dui::Layout *m_skillTemplate;						//技能模板
	cc2dui::ScrollView *m_skillSV;							//技能滚动
	cc2dui::CheckBox*         m_skillChecks[ActivteSkillNum];	    //技能的checkboxs
    cc2dui::ImageView*        m_CanUseSkillSolt[CanUseSkillNum];    //能使用技能的槽位
    bool              m_HasSkill[ActivteSkillNum];          //拥有技能.
	cc2dui::Button*           m_startBtn;                           //开始修炼按钮
    cc2dui::ImageView*        m_startImg;                           //正在修炼图标
    
    int m_CurTreeType;
    class Hero* m_Hero;
};

#endif	//__UIGONGFA_H__