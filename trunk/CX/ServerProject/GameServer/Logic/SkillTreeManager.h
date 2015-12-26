/********************************************************************
created:	2015/9/9
author:		lishihai
desc:		���ܹ���,����������߼�
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

    //>��ʼ��
	void Init();				
	//>�����
    bool AddSkill(Player* player, int32 skillType);
    //>��Ӽ���������
    bool AddSkillTreeExp(Player* player, int32 skillTreeType, uint32 addExp);
private:
    bool AddSkillTreeIdExp(Player* player, int32 skillTreeId, uint32 addExp);
    //>����id���Ҽ���
    SkillInfoIt* FindSkillTree(Player* player, int32 skillTreeId);
    //>���߼��������Ҽ���
    SkillInfoIt* FindSkillTreeByType(Player* player, int32 skillTreeType);
};
extern SkillManager* g_SkillManager;
#endif	//__SKILLMANAGER_H__
