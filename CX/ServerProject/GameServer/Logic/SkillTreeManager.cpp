#include "SkillTreeManager.h"
#include "LogManage.h"
#include "MsgInfo.h"
#include "Player.h"
#include "CSVFileManager.h"
#include "GameServer.h"


SkillManager *g_SkillManager = nullptr;

SkillManager::SkillManager()
{
}

SkillManager::~SkillManager()
{
}

void SkillManager::Init()
{
}


bool SkillManager::AddSkill(Player* player, int32 skillType)
{
    /*
    assert(player);

    int skillTreeId = skillTreeType * 1000 + 1;

    const SkillTreeInfo* pSkillTreeTpl = g_CSVFileManager->GetSkillTreeByID(skillTreeId);
    if (pSkillTreeTpl == NULL)
    {
        gLog.Warning("can't find skillTree[%d] when call AddSkillTree.", skillTreeId);
        return false;
    }

    if (FindSkillTreeByType(player, skillTreeType) != NULL)
    {
        MsgNotify notify(ID_G2C, Notify_S2C_ItemUse_RepeatedAddSkillTree);
        notify.OBJID = player->m_GUID;
        g_GameServer->SendToClient(&notify);
        gLog.Info("repeated add skillTree[%d] when call AddSkillTree.", skillTreeId);
        return false;
    }

    std::vector<SkillTreeInfoIt>& treeList = player->m_SkillTreeList;
    int index = treeList.size();
    treeList.resize(index + 1);
    
    treeList[index].id = skillTreeId;
    treeList[index].level = 1;
    treeList[index].exp = 0;

    if (index == 0)
        player->ActivateSkillTree(SKILL_ID_TO_TYPE(skillTreeId));

    MsgSkillTree_Add addMsg;
    addMsg.SysID = ID_G2D;
    addMsg.OBJID = player->m_GUID;
    addMsg.addSkillTree.id = treeList[index].id;
    addMsg.addSkillTree.level = treeList[index].level;
    addMsg.addSkillTree.exp = treeList[index].exp;

    //>发送到数据库服务器
    g_GameServer->SendToDB(&addMsg);

    //>发送到客户端
    addMsg.SysID = ID_G2C;
    g_GameServer->SendToClient(&addMsg);
    */
    return true;
}

bool SkillManager::AddSkillTreeExp(Player* player, int32 skillTreeType, uint32 addExp)
{
    /*
    SkillInfoIt* pSkillTree = FindSkillTreeByType(player, skillTreeType);
    if (pSkillTree == NULL)
    {
        gLog.Warning("can't find skillTree[type=%d] when call AddSkillTreeExp.", skillTreeType);
        return false;
    }

    return AddSkillTreeIdExp(player, pSkillTree->id, addExp);
    */
    return false;
}

bool SkillManager::AddSkillTreeIdExp(Player* player, int32 skillTreeId, uint32 addExp)
{
    /*
    assert(player);

    SkillTreeInfoIt* pSkillTree = FindSkillTree(player, skillTreeId);
    if (pSkillTree)
    {
        const SkillTreeInfo* pSkillTreeTpl = g_CSVFileManager->GetSkillTreeByID(skillTreeId);
        const SkillTreeInfo* pSkillTreePulsTpl = g_CSVFileManager->GetSkillTreeByID(skillTreeId + 1);
        assert(pSkillTreeTpl);
        bool upLevel = false;
        if (pSkillTree->exp + (int32)addExp > pSkillTreeTpl->iLevelExp) //升级
        {
            if (pSkillTreePulsTpl == NULL)
            {
                if (pSkillTree->exp == pSkillTreeTpl->iLevelExp)
                {
                    MsgNotify notify(ID_G2C, Notify_S2C_ItemUse_SkillTreeExpFull);
                    notify.OBJID = player->m_GUID;
                    g_GameServer->SendToClient(&notify);
                    gLog.Info("repeated add skillTree[%d] when call AddSkillTreeIdExp.", skillTreeId);
                    return false;
                }else
                {
                    pSkillTree->exp = pSkillTreeTpl->iLevelExp;
                }
            }
            else
            {
                pSkillTree->exp = (pSkillTree->exp + addExp) - pSkillTreeTpl->iLevelExp;
                upLevel = true;
            }
        }else
        {
            pSkillTree->exp += addExp;
        }

        MsgSkillTree_Update updateMsg;
        updateMsg.SysID = ID_G2D;
        updateMsg.OBJID = player->m_GUID;
        updateMsg.oldId = pSkillTree->id;
        //>如果升级
        if (upLevel)
            pSkillTree->id += 1;
        updateMsg.updateSkillTree.id = pSkillTree->id;
        updateMsg.updateSkillTree.level = (pSkillTree->id) % 1000;
        updateMsg.updateSkillTree.exp = pSkillTree->exp;

        //>发送到数据库服务器
        g_GameServer->SendToDB(&updateMsg);

        //>发送到客户端
        updateMsg.SysID = ID_G2C;
        g_GameServer->SendToClient(&updateMsg);

        if (upLevel)
        {
            //>如果升级了.可能触发新技能激活.激活工作
            // do something.
        }


        if (upLevel && pSkillTreePulsTpl && pSkillTree->exp > pSkillTreePulsTpl->iLevelExp)
        {
            return AddSkillTreeIdExp(player, skillTreeId + 1, 0);
        }

        return true;
    }
    */
    return false;
}

SkillInfoIt* SkillManager::FindSkillTree(Player* player, int32 skillTreeId)
{
    /*
    assert(player);

    std::vector<SkillTreeInfoIt>& treeList = player->m_SkillTreeList;
    std::vector<SkillTreeInfoIt>::iterator it = treeList.begin();
    for (std::vector<SkillTreeInfoIt>::iterator end = treeList.end(); 
        it != end; ++it)
    {

        if (it->id == skillTreeId)
        {
            return &(*it);
        }
    }
    */
    return NULL;
}

SkillInfoIt* SkillManager::FindSkillTreeByType(Player* player, int32 skillTreeType)
{
    /*
    assert(player);

    std::vector<SkillTreeInfoIt>& treeList = player->m_SkillTreeList;
    std::vector<SkillTreeInfoIt>::iterator it = treeList.begin();
    for (std::vector<SkillTreeInfoIt>::iterator end = treeList.end(); 
        it != end; ++it)
    {

        if (SKILL_ID_TO_TYPE(it->id) == skillTreeType)
        {
            return &(*it);
        }
    }
    */
    return NULL;
}
