#include "Skill.h"
#include <cassert>
#include "CSVFileManager.h"
#include "Buff.h"
#include "Object.h"
#include "Monster.h"
#include "Player.h"
#include "MapManager.h"
#include "Map.h"
#include "BattleModule.h"
#include "BattleComponent.h"
#include "LogManage.h"

Skill::Skill()
{

}

Skill::~Skill()
{

}

void Skill::Init(Object* pAttacker, Object* pTarget)
{
    m_Attacker      = pAttacker;
    m_TargetGuid    = pTarget->m_GUID;
    m_TargetType    = pTarget->m_Type;
    m_Time          = 0.0f;
    m_IsEnd         = false;
}

void Skill::InnerBegin()
{
    assert(m_Attacker);
    assert(m_Info);

    OnBegin();
    gLog.Info("skill[%d] begin.", m_Info->id);
    if (!(m_Info->delay > 0.0f))
    {
        InnerUpdate(0.0f);
    }
}

void Skill::InnerUpdate(float dt)
{
    if (m_IsEnd)
        return;

    if (m_Time >= m_Info->delay)
    {
        Process();
        InnerEnd();
    }

    m_Time += dt;
}

void Skill::InnerEnd()
{
    m_IsEnd = true;
    OnEnd();
    gLog.Info("skill[%d] end.", m_Info->id);
}

void Skill::AddBuff(std::vector<Object*>& targets)
{
    auto it = targets.begin();
    for (auto end = targets.end(); it != end; ++it )
    {
        Object* pObj = *it;
        if (pObj->IsDead())
            continue;

        if (pObj->m_BattleComp == NULL)
            continue;

        for (int i = 0; i < 3; ++i)
        {
            if (m_Info->buffDatas[i].buffId != INVALID_ID)
            {
                const BuffInfo* pBuffInfo = g_CSVFileManager->GetBuffByID(m_Info->buffDatas[i].buffId);
                assert (pBuffInfo);
                if(pBuffInfo == NULL)
                    continue;
                
                Buff* pBuff = BuffFactory::AllocBuff(pBuffInfo, m_Info->buffDatas[i].time);
                if (pBuff == NULL)
                    continue;

                pBuff->SetData(m_Info->buffDatas[i].data);
                if(pBuff->m_BuffInfo->type == BuffType_YouYi)
                {
                    bool rs = pObj->m_BattleComp->AddBuff(pBuff);
                    if (!rs)
                        BuffFactory::FreeBuff(pBuff);
                }
                else
                {
                    if (HasState(pObj->m_BattleComp->m_WuDi))
                    {//>如果无敌.不接受新buff
                        BuffFactory::FreeBuff(pBuff);
                        continue;
                    }

                    //>如果能反弹指定类型的buff, 那么反弹给攻击者
                    bool reflect = false;
                    std::vector<const SkillInfo*> triggerSkills;
                    const SkillInfo* pInfo = NULL;
                    pObj->m_BattleComp->m_Interface->GetSkillsForInterestAddBuff(triggerSkills);
                    if (triggerSkills.empty()){
                        BuffFactory::FreeBuff(pBuff);
                        continue;
                    }

                    auto it = triggerSkills.begin();
                    for (auto end = triggerSkills.end(); it != end; ++it)
                    {
                        pInfo = *it;
                        if (pInfo->funcType == Skill_FanTanBuff)
                        {
                            int fanTanBuffType = (int)pInfo->triggerData[1];
                            if (pBuff->m_BuffInfo->type == fanTanBuffType)
                            {
                                if (pObj->m_BattleComp->IsTriggerPassiveSkill(pInfo))
                                {
                                    if (pObj->m_GUID == m_Attacker->m_GUID)
                                    {
                                        gLog.AddLog(LOG_LEVEL_ERROR, "Fan-Tan buff can't be self.[skill=%d]",pInfo->id);
                                    }
                                    else
                                    {//>经过被动触发的buff直接加给攻击者.不再触发其他.
                                        if(false == m_Attacker->m_BattleComp->AddBuff(pBuff))
                                        {
                                            BuffFactory::FreeBuff(pBuff);
                                        }
                                        reflect = true;
                                        break;
                                    }
                                }
                            }

                            if (fanTanBuffType == BuffType_YouYi)
                            {
                                gLog.AddLog(LOG_LEVEL_ERROR, "Fan-Tan buff type can't be YouYi buff.[skill=%d]",pInfo->id);
                            }
                        }
                    }
                            
                    if (!reflect)
                    {
                        bool add = true;
                        auto it = triggerSkills.begin();
                        for (auto end = triggerSkills.end(); it != end; ++it)
                        {//>计算免疫控制
                            pInfo = *it;
                            if (pInfo->funcType == Skill_MianYiKongZhi)
                            {
                                int fanTanBuffType = (int)pInfo->triggerData[1];
                                if (pBuff->m_BuffInfo->type == BuffType_KongZhi)
                                {
                                    if (pObj->m_BattleComp->IsTriggerPassiveSkill(pInfo))
                                    {
                                        add = false;
                                        break;
                                    }
                                }
                            }
                        }

                        if (add)
                        {
                            bool rs = pObj->m_BattleComp->AddBuff(pBuff);
                            if (!rs)
                                BuffFactory::FreeBuff(pBuff);
                        }
                        else
                        {
                            BuffFactory::FreeBuff(pBuff);
                        }
                    }
                }
            }
        }
    }
}

void Skill::GetTargetsByType(SkillTargetType type, std::vector<Object*>& targets)
{
    SkillTargetType targetType = type;  

    switch (targetType)
    {
    case SklTag_SingleTarget:

        if (m_TargetType == OBJECT_MONSTER)
        {
            Monster* pObj = g_MapManager->GetMonsterByID(m_TargetGuid);
            if (pObj)
                targets.push_back(pObj);
        }
        else if(m_TargetType == OBJECT_PLAYER)
        {
            Player* pObj = g_MapManager->GetPlayerByID(m_TargetGuid);
            if (pObj)
                targets.push_back(pObj);
        }
        break;
    case SklTag_OnlySelf:
        targets.push_back(m_Attacker);
        break;
    case SklTag_EnemyInSelfRang:
        {
            m_Attacker->m_Map->GetTargetRangeSet(10.0f, m_Attacker, m_Attacker, targets, Select_All);
        }
        break;
    case SklTag_TeamPlayerInSelfRang:
        {
            m_Attacker->m_Map->GetTargetRangeSet(10.0f, m_Attacker, m_Attacker, targets, Select_TeamMember);
        }
        break;
    case SklTag_EnemyInTargetRang:
        {
            Object* pObj = NULL;
            if (m_TargetType == OBJECT_MONSTER)
            {
                pObj = g_MapManager->GetMonsterByID(m_TargetGuid);
            }
            else if(m_TargetType == OBJECT_PLAYER)
            {
                pObj = g_MapManager->GetPlayerByID(m_TargetGuid);
            }
            if (pObj)
            {
                m_Attacker->m_Map->GetTargetRangeSet(10.0f, pObj, m_Attacker, targets, Select_All);
            }
        }break;
    default:
        break;
    }
}

Skill* SkillFactory::AllocSkill(const SkillInfo* pSkillInfo)
{
    assert(pSkillInfo);

    Skill* pSkill = NULL;
    SkillFunction funcType = (SkillFunction)pSkillInfo->funcType;
    switch (funcType)
    {
    case Skill_Damage:
        pSkill = _DamageAndBuffAllocer.Alloc();
        gLog.Info("<-------alloc Skill_Damage[%p]", pSkill);
        break;
    case Skill_Cure:
        pSkill = _CureAllocer.Alloc();
        gLog.Info("<-------alloc Skill_Cure[%p]", pSkill);
        break;
    case Skill_BeHurtAddBuff:
        pSkill = NULL;
        break;
    case Skill_QuChuYouYi:
        pSkill = _QuChuYouYiAllocer.Alloc();
        gLog.Info("<-------alloc Skill_QuChuYouYi[%p]", pSkill);
        break;
    case Skill_JianShe:
        pSkill = _JianSheAllocer.Alloc();
        gLog.Info("<-------alloc Skill_JianShe[%p]", pSkill);
        break;
    case Skill_FanTanBuff :
        pSkill = NULL;
        break;
    case Skill_ChaoFeng:
        pSkill = _ChaoFengAllocer.Alloc();
        gLog.Info("<-------alloc Skill_ChaoFeng[%p]", pSkill);
        break;
    case Skill_XiXue:
        pSkill = _XiXueAllocer.Alloc();
        gLog.Info("<-------alloc Skill_XiXue[%p]", pSkill);
        break;
    case Skill_ZhenShiShangHai:
        pSkill = _ZhenShiShangHaiAllocer.Alloc();
        gLog.Info("<-------alloc Skill_ZhenShiShangHai[%p]", pSkill);
        break;
    case Skill_ChongSheng:
        pSkill = NULL;
        break;
    case Skill_MianYiKongZhi:
        pSkill = NULL;
        break;
    default:
        break;
    }

    if (pSkill)
        pSkill->m_Info = pSkillInfo;

    return pSkill;
}

void SkillFactory::FreeSkill(Skill* pSkill)
{
    SkillFunction funcType = (SkillFunction)pSkill->m_Info->funcType;
    switch (funcType)
    {
    case Skill_Damage:
        _DamageAndBuffAllocer.Free((DamageAndBuff*)pSkill);
        gLog.Info("------->free Skill_Damage[%p]", pSkill);
        break;
    case Skill_Cure:
        _CureAllocer.Free((Cure*)pSkill);
        gLog.Info("------->free Skill_Cure[%p]", pSkill);
        break;
    case Skill_BeHurtAddBuff:
        break;
    case Skill_QuChuYouYi:
        _QuChuYouYiAllocer.Free((QuChuYouYi*)pSkill);
        gLog.Info("------->free Skill_QuChuYouYi[%p]", pSkill);
        break;
    case Skill_JianShe:
        _JianSheAllocer.Free((JianShe*)pSkill);
        gLog.Info("------->free Skill_JianShe[%p]", pSkill);
        break;
    case Skill_FanTanBuff :
        break;
    case Skill_ChaoFeng:
        _ChaoFengAllocer.Free((ChaoFeng*)pSkill);
        gLog.Info("------->free Skill_ChaoFeng[%p]", pSkill);
        break;
    case Skill_XiXue:
        _XiXueAllocer.Free((XiXue*)pSkill);
        gLog.Info("------->free Skill_XiXue[%p]", pSkill);
        break;
    case Skill_ZhenShiShangHai:
        _ZhenShiShangHaiAllocer.Free((ZhenShiShangHai*)pSkill);
        gLog.Info("------->free Skill_ZhenShiShangHai[%p]", pSkill);
        break;
    case Skill_ChongSheng:
        break;
    case Skill_MianYiKongZhi:
        break;
    default:
        break;
    }
}

void DamageAndBuff::Process()
{
    gLog.Info("process DamageAndBuff.");
    std::vector<Object*> targets;
    GetTargetsByType((SkillTargetType)m_Info->targetType, targets);

    if (!targets.empty())
    {
        auto it = targets.begin();
        for (auto end = targets.end(); it != end; ++it )
        {
            Object* pObj = *it;
            if (pObj->IsDead())
                continue;

            if (pObj->m_BattleComp == NULL)
                continue;

            DropHpMode damage;
            if (HasState(pObj->m_BattleComp->m_WuDi))
            {//>如果无敌
                damage.hp = 0;
                damage.mode = DropHpMode::WuDiMianYiBuff;
                pObj->DropHP(damage);
            }
            else
            {
                //>计算伤害
                damage = BattleModule::CalcDamage(m_Attacker, this, pObj);
                if (damage.mode == DropHpMode::Dodge)
                {
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, damage.hp, damage.mode));
                }
                else
                {
                    //>计算吸血
                    int xiXue = m_Attacker->m_BattleComp->DamageToHpBuff(damage.hp);
                    if (xiXue > 0){
                        m_Attacker->AddHp(AddHpMode(xiXue, AddHpMode::XiXueBuff));
                        gLog.Info("attacker[%llu] XiXue [hp=%d]", m_Attacker->m_GUID, xiXue);
                    }

                    //>计算反伤
                    int fanShang = pObj->m_BattleComp->DamageInverseByFanShangBuff(damage.hp);
                    if (fanShang > 0){
                        m_Attacker->DropHP(DropHpMode(pObj->m_GUID, 0, fanShang, DropHpMode::DamageInverseBuff));
                        gLog.Info("target[%llu] FanShang to attacker[%llu] [hp=%d]", pObj->m_GUID, m_Attacker->m_GUID, fanShang);
                    }

                    //>计算护盾
                    int realDamage = pObj->m_BattleComp->DamageReduceByHuDunBuff(damage.hp);
                    if (realDamage != damage.hp){
                        pObj->DropHP(DropHpMode(m_Attacker->m_GUID, damage.hp - realDamage, 0, DropHpMode::HuDunXiShouBuff));
                    }

                    //>计算技能伤害
                    if (realDamage > 0)
                    {
                        pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, realDamage, damage.mode));
                        pObj->m_BattleComp->BuHurtProcess(); //>受击处理
                    }
                }
            }
        }
    }

    
    int lastTargetType = m_Info->targetType;
    //>添加技能buff
    for (int i = 0; i < 3; ++i)
    {
        if (lastTargetType != m_Info->buffDatas[i].targetType)
        {
            lastTargetType = m_Info->buffDatas[i].targetType;
            targets.clear();
            GetTargetsByType((SkillTargetType)lastTargetType, targets);
        }

        if (targets.empty())
            continue;
        
        //>添加buff
        AddBuff(targets);
    }
}

void Cure::Process()
{
    gLog.Info("process Cure.");
    std::vector<Object*> targets;
    GetTargetsByType((SkillTargetType)m_Info->targetType, targets);

    if (!targets.empty())
    {
        auto it = targets.begin();
        for (auto end = targets.end(); it != end; ++it )
        {
            Object* pObj = *it;
            AddHpMode cure = BattleModule::CalcTreatment(m_Attacker, this, pObj);

            pObj->AddHp(cure);
        }
    }

    int lastTargetType = m_Info->targetType;
    //>添加技能buff
    for (int i = 0; i < 3; ++i)
    {
        if (lastTargetType != m_Info->buffDatas[i].targetType)
        {
            lastTargetType = m_Info->buffDatas[i].targetType;
            targets.clear();
            GetTargetsByType((SkillTargetType)lastTargetType, targets);
        }

        if (targets.empty())
            continue;

        //>添加buff
        AddBuff(targets);
    }
}

void QuChuYouYi::Process()
{
    gLog.Info("process QuChuYouYi.");
    std::vector<Object*> targets;
    GetTargetsByType((SkillTargetType)m_Info->targetType, targets);

    if (!targets.empty())
    {
        auto it = targets.begin();
        for (auto end = targets.end(); it != end; ++it )
        {
            Object* pObj = *it;
            if (pObj->IsDead())
                continue;

            if (pObj->m_BattleComp == NULL)
                continue;

            DropHpMode damage;
            if (HasState(pObj->m_BattleComp->m_WuDi))
            {//>如果无敌
                damage.hp = 0;
                damage.mode = DropHpMode::WuDiMianYiBuff;
                pObj->DropHP(damage);
            }
            else
            {
                //>计算伤害
                damage = BattleModule::CalcDamage(m_Attacker, this, pObj);
                if (damage.mode == DropHpMode::Dodge)
                {
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, damage.hp, damage.mode));
                }
                else
                {
                    //>计算吸血
                    int xiXue = m_Attacker->m_BattleComp->DamageToHpBuff(damage.hp);
                    if (xiXue > 0){
                        m_Attacker->AddHp(AddHpMode(xiXue, AddHpMode::XiXueBuff));
                        gLog.Info("attacker[%llu] XiXue [hp=%d]", m_Attacker->m_GUID, xiXue);
                    }

                    //>计算反伤
                    int fanShang = pObj->m_BattleComp->DamageInverseByFanShangBuff(damage.hp);
                    if (fanShang > 0){
                        m_Attacker->DropHP(DropHpMode(pObj->m_GUID, 0, fanShang, DropHpMode::DamageInverseBuff));
                        gLog.Info("target[%llu] FanShang to attacker[%llu] [hp=%d]", pObj->m_GUID, m_Attacker->m_GUID, fanShang);
                    }

                    //>计算护盾
                    int realDamage = pObj->m_BattleComp->DamageReduceByHuDunBuff(damage.hp);
                    if (realDamage != damage.hp){
                        pObj->DropHP(DropHpMode(m_Attacker->m_GUID, damage.hp - realDamage, 0, DropHpMode::HuDunXiShouBuff));
                    }

                    //>计算技能伤害
                    if (realDamage > 0)
                    {
                        pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, realDamage, damage.mode));
                        pObj->m_BattleComp->BuHurtProcess(); //>受击处理
                    }

                    //>驱除有益buff
                    pObj->m_BattleComp->RemoveBuffByType((int)Skill_QuChuYouYi);
                }
            }
        }
    }

    int lastTargetType = m_Info->targetType;
    //>添加技能buff
    for (int i = 0; i < 3; ++i)
    {
        if (lastTargetType != m_Info->buffDatas[i].targetType)
        {
            lastTargetType = m_Info->buffDatas[i].targetType;
            targets.clear();
            GetTargetsByType((SkillTargetType)lastTargetType, targets);
        }

        if (targets.empty())
            continue;

        //>添加buff
        AddBuff(targets);
    }
}

//void QuChuYouHai::Process()
//{
//    std::vector<Object*> targets;
//    GetSkillTargets(targets);
//
//    if (!targets.empty())
//    {
//        auto it = targets.begin();
//        for (auto end = targets.end(); it != end; ++it )
//        {
//
//            Object* pObj = *it;
//            if (pObj->m_BattleComp == NULL)
//                continue;
//
//            //>触发真实伤害
//            DamageResult dr = BattleModule::CalcDamage(m_Attacker, this, pObj);
//
//            //>计算吸血
//            int xiXue = m_Attacker->m_BattleComp->DamageToHpBuff(dr.damage);
//            if (xiXue > 0){
//                m_Attacker->AddHp(xiXue);
//            }
//
//            //>计算反伤
//            int fanShang = pObj->m_BattleComp->FanShangDamageBuff(dr.damage);
//            if (fanShang > 0){
//                m_Attacker->DropHP(fanShang);
//            }
//
//            //>计算护盾
//            int realDamage = pObj->m_BattleComp->DamageReduceHuDunBuff(dr.damage);
//            if (realDamage > 0){
//                pObj->DropHP(realDamage);
//            }
//
//            //>消除有害
//            pObj->m_BattleComp->RemoveBuffByType(1);
//        }
//
//        //>发送杀害数值
//        AddBuff(targets);
//    }
//}

void JianShe::Process()
{
    gLog.Info("process JianShe.");
    std::vector<Object*> targets;
    GetTargetsByType((SkillTargetType)m_Info->targetType, targets);

    if (!targets.empty())
    {
        auto it = targets.begin();
        for (auto end = targets.end(); it != end; ++it )
        {
            Object* pObj = *it;
            if (pObj->IsDead())
                continue;

            if (pObj->m_BattleComp == NULL)
                continue;

            DropHpMode damage;
            if (HasState(pObj->m_BattleComp->m_WuDi))
            {//>如果无敌
                damage.hp = 0;
                damage.mode = DropHpMode::WuDiMianYiBuff;
                pObj->DropHP(damage);
            }
            else
            {

                //>计算伤害
                damage = BattleModule::CalcDamage(m_Attacker, this, pObj);
                if (damage.mode == DropHpMode::Dodge)
                {
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, damage.hp, damage.mode));
                }
                else
                {
                    //>计算吸血
                    int xiXue = m_Attacker->m_BattleComp->DamageToHpBuff(damage.hp);
                    if (xiXue > 0){
                        m_Attacker->AddHp(AddHpMode(xiXue, AddHpMode::XiXueBuff));
                        gLog.Info("attacker[%llu] XiXue [hp=%d]", m_Attacker->m_GUID, xiXue);
                    }

                    //>计算反伤
                    int fanShang = pObj->m_BattleComp->DamageInverseByFanShangBuff(damage.hp);
                    if (fanShang > 0){
                        m_Attacker->DropHP(DropHpMode(pObj->m_GUID, 0, fanShang, DropHpMode::DamageInverseBuff));
                        gLog.Info("target[%llu] FanShang to attacker[%llu] [hp=%d]", pObj->m_GUID, m_Attacker->m_GUID, fanShang);
                    }

                    //>计算护盾
                    int realDamage = pObj->m_BattleComp->DamageReduceByHuDunBuff(damage.hp);
                    if (realDamage != damage.hp){
                        pObj->DropHP(DropHpMode(m_Attacker->m_GUID, damage.hp - realDamage, 0, DropHpMode::HuDunXiShouBuff));
                    }

                    //>计算技能伤害
                    if (realDamage > 0)
                    {
                        pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, realDamage, damage.mode));
                        pObj->m_BattleComp->BuHurtProcess(); //>受击处理
                    }

                    //>计算溅射
                    std::vector<Object*> objects;
                    pObj->m_Map->GetTargetRangeSet(m_Info->funcData[0], pObj, m_Attacker, objects, SelectFilter::Select_All);
                    if (!objects.empty())
                    {
                        Object* pEnemy = NULL;
                        auto it = objects.begin();
                        for (auto end = objects.end(); it != end; ++it)
                        {
                            pEnemy = *it;
                            if (NULL == pEnemy->m_BattleComp)
                                continue;

                            int dropHp = int(damage.hp * m_Info->funcData[1]);
                            if(dropHp > 0){
                                pEnemy->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, dropHp, DropHpMode::JianShe));
                            }
                        }
                    }
                }
            }
        }
    }

    int lastTargetType = m_Info->targetType;
    //>添加技能buff
    for (int i = 0; i < 3; ++i)
    {
        if (lastTargetType != m_Info->buffDatas[i].targetType)
        {
            lastTargetType = m_Info->buffDatas[i].targetType;
            targets.clear();
            GetTargetsByType((SkillTargetType)lastTargetType, targets);
        }

        if (targets.empty())
            continue;

        //>添加buff
        AddBuff(targets);
    }
}

void ChaoFeng::Process()
{
    gLog.Info("process ChaoFeng.");
    std::vector<Object*> targets;
    GetTargetsByType((SkillTargetType)m_Info->targetType, targets);

    if (!targets.empty())
    {
        auto it = targets.begin();
        for (auto end = targets.end(); it != end; ++it )
        {
            Object* pObj = *it;
            if (pObj->IsDead())
                continue;

            if (pObj->m_BattleComp == NULL)
                continue;

            DropHpMode damage;
            if (HasState(pObj->m_BattleComp->m_WuDi))
            {//>如果无敌
                damage.hp = 0;
                damage.mode = DropHpMode::WuDiMianYiBuff;
                pObj->DropHP(damage);
            }
            else
            {
                //>计算吸血
                int xiXue = m_Attacker->m_BattleComp->DamageToHpBuff(damage.hp);
                if (xiXue > 0){
                    m_Attacker->AddHp(AddHpMode(xiXue, AddHpMode::XiXueBuff));
                    gLog.Info("attacker[%llu] XiXue [hp=%d]", m_Attacker->m_GUID, xiXue);
                }

                //>计算反伤
                int fanShang = pObj->m_BattleComp->DamageInverseByFanShangBuff(damage.hp);
                if (fanShang > 0){
                    m_Attacker->DropHP(DropHpMode(pObj->m_GUID, 0, fanShang, DropHpMode::DamageInverseBuff));
                    gLog.Info("target[%llu] FanShang to attacker[%llu] [hp=%d]", pObj->m_GUID, m_Attacker->m_GUID, fanShang);
                }

                //>计算护盾
                int realDamage = pObj->m_BattleComp->DamageReduceByHuDunBuff(damage.hp);
                if (realDamage != damage.hp){
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, damage.hp - realDamage, 0, DropHpMode::HuDunXiShouBuff));
                }

                if (realDamage > 0)
                {
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, realDamage, damage.mode));
                    pObj->m_BattleComp->BuHurtProcess(); //>受击处理

                    if (pObj->m_Type == OBJECT_MONSTER)
                    {
                        //>添加仇恨

                    }
                }
            }
        }
    }

    int lastTargetType = m_Info->targetType;
    //>添加技能buff
    for (int i = 0; i < 3; ++i)
    {
        if (lastTargetType != m_Info->buffDatas[i].targetType)
        {
            lastTargetType = m_Info->buffDatas[i].targetType;
            targets.clear();
            GetTargetsByType((SkillTargetType)lastTargetType, targets);
        }

        if (targets.empty())
            continue;

        //>添加buff
        AddBuff(targets);
    }
}

void XiXue::Process()
{
    gLog.Info("process XiXue.");
    std::vector<Object*> targets;
    GetTargetsByType((SkillTargetType)m_Info->targetType, targets);

    if (!targets.empty())
    {
        auto it = targets.begin();
        for (auto end = targets.end(); it != end; ++it )
        {
            Object* pObj = *it;
            if (pObj->IsDead())
                continue;

            if (pObj->m_BattleComp == NULL)
                continue;

            DropHpMode damage;
            if (HasState(pObj->m_BattleComp->m_WuDi))
            {//>如果无敌
                damage.hp = 0;
                damage.mode = DropHpMode::WuDiMianYiBuff;
                pObj->DropHP(damage);
            }
            else
            {
                //>计算伤害
                damage = BattleModule::CalcDamage(m_Attacker, this, pObj);

                //>计算吸血
                int xiXue = m_Attacker->m_BattleComp->DamageToHpBuff(damage.hp);
                if (xiXue > 0){
                    m_Attacker->AddHp(AddHpMode(xiXue, AddHpMode::XiXueBuff));
                }

                //>计算反伤
                int fanShang = pObj->m_BattleComp->DamageInverseByFanShangBuff(damage.hp);
                if (fanShang > 0){
                    m_Attacker->DropHP(DropHpMode(pObj->m_GUID, 0, fanShang, DropHpMode::DamageInverseBuff));
                }

                //>计算护盾
                int realDamage = pObj->m_BattleComp->DamageReduceByHuDunBuff(damage.hp);
                if (realDamage != damage.hp){
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, damage.hp - realDamage, 0, DropHpMode::HuDunXiShouBuff));
                }

                //>计算技能伤害
                if (realDamage > 0)
                {
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, realDamage, damage.mode));
                    pObj->m_BattleComp->BuHurtProcess(); //>受击处理

                    int skillXiXue = int(damage.hp * m_Info->funcData[0]);
                    m_Attacker->AddHp(AddHpMode(skillXiXue, AddHpMode::XiXue));
                }
            }
        }
    }

    int lastTargetType = m_Info->targetType;
    //>添加技能buff
    for (int i = 0; i < 3; ++i)
    {
        if (lastTargetType != m_Info->buffDatas[i].targetType)
        {
            lastTargetType = m_Info->buffDatas[i].targetType;
            targets.clear();
            GetTargetsByType((SkillTargetType)lastTargetType, targets);
        }

        if (targets.empty())
            continue;

        //>添加buff
        AddBuff(targets);
    }
}

void ZhenShiShangHai::Process()
{
    gLog.Info("process ZhenShiShangHai.");
    std::vector<Object*> targets;
    GetTargetsByType((SkillTargetType)m_Info->targetType, targets);

    if (!targets.empty())
    {
        auto it = targets.begin();
        for (auto end = targets.end(); it != end; ++it )
        {
            Object* pObj = *it;
            if (pObj->IsDead())
                continue;

            if (pObj->m_BattleComp == NULL)
                continue;

            DropHpMode damage;
            if (HasState(pObj->m_BattleComp->m_WuDi))
            {//>如果无敌
                damage.hp = 0;
                damage.mode = DropHpMode::WuDiMianYiBuff;
                pObj->DropHP(damage);
            }
            else
            {
                //>触发真实伤害
                float defence = pObj->m_Attribute[ATTRIBUTE_DEFENSE].GetBase();
                float dodge = pObj->m_Attribute[ATTRIBUTE_DODGE].GetBase();
                pObj->m_Attribute[ATTRIBUTE_DEFENSE].SetBase(0.0f);
                pObj->m_Attribute[ATTRIBUTE_DODGE].SetBase(0.0f);
                damage = BattleModule::CalcDamage(m_Attacker, this, pObj);
                pObj->m_Attribute[ATTRIBUTE_DEFENSE].SetBase(defence);
                pObj->m_Attribute[ATTRIBUTE_DODGE].SetBase(dodge);

                //>计算吸血
                int xiXue = m_Attacker->m_BattleComp->DamageToHpBuff(damage.hp);
                if (xiXue > 0){
                    m_Attacker->AddHp(AddHpMode(xiXue, AddHpMode::XiXueBuff));
                }

                //>计算反伤
                int fanShang = pObj->m_BattleComp->DamageInverseByFanShangBuff(damage.hp);
                if (fanShang > 0){
                    m_Attacker->DropHP(DropHpMode(pObj->m_GUID, 0, fanShang, DropHpMode::DamageInverseBuff));
                }

                //>计算护盾
                int realDamage = pObj->m_BattleComp->DamageReduceByHuDunBuff(damage.hp);
                if (realDamage != damage.hp){
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, damage.hp - realDamage, 0, DropHpMode::HuDunXiShouBuff));
                }

                //>计算技能伤害
                if (realDamage > 0)
                {
                    pObj->DropHP(DropHpMode(m_Attacker->m_GUID, m_Info->beHurtEffectId, realDamage, damage.mode));
                    pObj->m_BattleComp->BuHurtProcess(); //>受击处理
                }
            }
        }
    }

    int lastTargetType = m_Info->targetType;
    //>添加技能buff
    for (int i = 0; i < 3; ++i)
    {
        if (lastTargetType != m_Info->buffDatas[i].targetType)
        {
            lastTargetType = m_Info->buffDatas[i].targetType;
            targets.clear();
            GetTargetsByType((SkillTargetType)lastTargetType, targets);
        }

        if (targets.empty())
            continue;

        //>添加buff
        AddBuff(targets);
    }
}

SkillAllocer<DamageAndBuff>  SkillFactory::_DamageAndBuffAllocer;
SkillAllocer<Cure>           SkillFactory::_CureAllocer;
SkillAllocer<QuChuYouYi>     SkillFactory::_QuChuYouYiAllocer;
SkillAllocer<JianShe>        SkillFactory::_JianSheAllocer;
SkillAllocer<ChaoFeng>       SkillFactory::_ChaoFengAllocer;
SkillAllocer<XiXue>          SkillFactory::_XiXueAllocer;
SkillAllocer<ZhenShiShangHai> SkillFactory::_ZhenShiShangHaiAllocer;

