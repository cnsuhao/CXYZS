/********************************************************************
created:	2015/9/9
author:		lishihai
desc:		技能管理,处理功法相关逻辑
*********************************************************************/

#ifndef __SKILLMANAGER_H__
#define __SKILLMANAGER_H__

#include "GameDef.h"

class Player;
struct SkillInfoIt;

class SkillManager
{
public:
	SkillManager();
	~SkillManager();

    //>初始化
	void Init();				
	//>激活技能
    bool AddSkill(Player* player, int32 skillType);
    //>添加技能树经验
    bool AddSkillTreeExp(Player* player, int32 skillTreeType, uint32 addExp);
private:
    bool AddSkillTreeIdExp(Player* player, int32 skillTreeId, uint32 addExp);
    //>更具id查找技能
    SkillInfoIt* FindSkillTree(Player* player, int32 skillTreeId);
    //>更具技能树查找技能
    SkillInfoIt* FindSkillTreeByType(Player* player, int32 skillTreeType);
};
extern SkillManager* g_SkillManager;
#endif	//__SKILLMANAGER_H__
