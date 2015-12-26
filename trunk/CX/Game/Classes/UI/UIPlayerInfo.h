/********************************************************************
created:	2014/12/6
author:		LSH
desc:		角色面板
*********************************************************************/

#ifndef __UIPLAYERINFO_H__
#define __UIPLAYERINFO_H__

#include "UIBase.h"
#include "UIBag.h"


struct ShowPlayerInfo
{
public:
    int64 guid;
    int level;
    std::string  name;
    int sex;

    int battlePower;
    std::string gangName;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int attack;
    int defence;
    int hit;
    int dodge;    
    int crit;
    int tenaciy;
    float hitProb;
    float critProb;
    CliItemInfo equipments[EQUIPTYPE_MAX];
public:
    ShowPlayerInfo();

    void SetByHero(class Hero* pHero);

    void SetByDetailMsg(struct MsgOtherRole_DetailInfo* pMsg);
};



class UIPlayerInfo : public UIBase
{
public:
	UIPlayerInfo();
	~UIPlayerInfo();	
	virtual bool init();
	CREATE_FUNC(UIPlayerInfo);

    virtual bool DisabledJoystickWhenSelfShow() { return true; }
    virtual void OnShow();

    void SetInfo(Hero* pHero);
    void SetInfo(MsgOtherRole_DetailInfo* pMsg);
private:
    void SetInfo();
	//主面板
	cc2d::Node* rolePanel;
	//属性面板
	cc2d::Node* roleAttPanel;


    cc2dui::Text* _equipPartNames[UIBag::EP_Number];
    cc2d::Sprite* _equipPartIcon[UIBag::EP_Number];
    cc2dui::ImageView* _equipPartQuality[UIBag::EP_Number];
    cc2dui::Text* _textFaction;
    cc2dui::Text* _textLevel;
    cc2d::Node* _playerFace;

    ShowPlayerInfo _detailInfo;
public:
    // 点击装备
    void OnClickEquipment(UIBag::EquipPart part);
    void MakeOperation(CliItemInfo* pItem, vector<UIItemInfo::ButtonOperation>& operations, bool wearing);
    void LoadPlayerFace();        //>加载角色外观
    void ReflushEquipment();    //>刷新装备拦

};

#endif	//__UIPLAYERINFO_H__