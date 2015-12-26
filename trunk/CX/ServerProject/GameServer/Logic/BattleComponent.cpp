#include "BattleComponent.h"
#include "Object.h"
#include "Buff.h"
#include "Skill.h"
#include "CSVFileManager.h"
#include "LogManage.h"

BattleComponent::BattleComponent(Object* owner, Interface* pInterface/* = NULL */)
{
    m_Owner     = owner;
    m_Interface = pInterface;

    m_ChenMo    = 0;
    m_FengXue   = 0;
    m_XuanYun   = 0;
    m_HunShui   = 0;
    m_HunLuan   = 0;
    m_WuDi      = 0;

    ResetBuffInstanceIdCounter();
}

bool BattleComponent::UseSkill(int skillId, Object* target)
{
    if (m_Interface)
    {
        if (m_CurSkilllLeftTime > 0.0f)
        {
            gLog.Warning("current skill[%d] not end", m_CurSkillId);
            return false;
        }

        if (m_Owner->IsDead())
        {
            gLog.Warning("attacker[%llu] dead stats can't use skill.", m_Owner->m_GUID);
            return false;
        }

        if (target->IsDead())
        {
            gLog.Info("target is die.");
            return false;
        }

        bool isNormalSkill = m_Interface->IsNormalSkill(skillId);
        if (isNormalSkill == false && HasState(m_ChenMo))
        {//>沉默
            gLog.Warning("can't use skill, attacker[%llu] has ChenMo state.", m_Owner->m_GUID);
            return false;
        }

        if (HasState(m_XuanYun))
        {//>眩晕
            gLog.Warning("can't use skill, attacker[%llu] has XuanYun state.", m_Owner->m_GUID);
            return false;
        }

        if (HasState(m_HunShui))
        {//>昏睡
            gLog.Warning("can't use skill, attacker[%llu] has m_HunShui state.", m_Owner->m_GUID);
            return false;
        }

        if(!m_Interface->HasSkill(skillId))
        {
            //>没有此技能
            gLog.Warning("can't find skill[%d] when call UseSkill", skillId);
            return false;
        }

        const SkillInfo* pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
        if (pSkillInfo == NULL)
        {//>找不到技能配置
            gLog.Warning("skill[%d] can't find in table when call UseSkill", skillId);
            return false;
        }

        if (pSkillInfo->funcType != Skill_Cure && target->m_BattleComp && target->m_BattleComp->m_WuDi)
        {//>如果不是治疗技能,且对方处于无敌状态
            gLog.Info("cant use skill, target[%llu] state is WuDi.", target->m_GUID);
            return false;
        }

        auto it = m_SkillCDMap.find(SKILL_ID_TO_TYPE(skillId));
        if (it != m_SkillCDMap.end())
        {
            if(it->second > 0.0f)
            {
                //> cd 未冷却 
                gLog.Warning("skill[%d] cd not cooled when call UseSkill", skillId);
                return false;
            }
        }

        if (isNormalSkill && NotHasState(m_FengXue))
        {
            //>如果没有被封穴.有可能触发其他技能.
            bool isTriggerSkill = true;
            if (m_Interface->IsNormalSkill(skillId))
            {
                if (target->m_GUID == m_Owner->m_GUID)
                {
                    //> cd 未冷却 
                    gLog.Warning("normal skill not selected self when call UseSkill");
                    return false;
                }
                //>如果是普通攻击
                std::vector<const SkillInfo*> triggerSkills;
                m_Interface->GetSkillsForInterestNormalSkill(triggerSkills);
                if (!triggerSkills.empty())
                {
                    //>计算权重几率
                    float normalWeight = pSkillInfo->triggerData[0];
                    float mount = normalWeight;
                    auto it = triggerSkills.begin();
                    for (auto end = triggerSkills.end(); it != end; ++it)
                        mount += (*it)->triggerData[0];

                    const SkillInfo* triggerSkill = NULL;
                    float prob = BaseFunction::Random(0.0f, mount);
                    if (prob > normalWeight)
                    {
                        prob -= normalWeight;
                        it = triggerSkills.begin();
                        for (auto end = triggerSkills.end(); it != end; ++it)
                        {
                            if (prob <= (*it)->triggerData[0])
                            {
                                triggerSkill = *it;
                                break;
                            }
                        }

                        if (triggerSkill)
                        {
                            bool br = UseSkill(triggerSkill->id, target);
                            if (br){
                                return true;
                            }
                        }else
                        {
                            assert(0);
                        }
                    }
                    else
                    {
                        isTriggerSkill = false;
                    }
                }
            }

            //>检测功能类型
            if (isTriggerSkill)
            {
                gLog.Info("trigger passive skill.");
                if(!(pSkillInfo->funcType == Skill_Damage || pSkillInfo->funcType == Skill_QuChuYouYi || 
                    pSkillInfo->funcType == Skill_JianShe || pSkillInfo->funcType == Skill_XiXue ||
                    pSkillInfo->funcType == Skill_ChaoFeng || pSkillInfo->funcType == Skill_ZhenShiShangHai))
                {
                    gLog.Error("trigger skill func-type error call UseSkill.");
                    return false;
                }
            }
        }

        if(pSkillInfo->mpCost > target->m_Attribute[ATTRIBUTE_MP].GetValueI())
        {//>MP不足
            gLog.Warning("mp no enough when call UseSkill");
            return false;
        }

        Skill* pSkill = SkillFactory::AllocSkill(pSkillInfo);
        if (pSkill == NULL)
        {
            gLog.Error("AllocSkill failed.");
            return false;
        }

        pSkill->Init(m_Owner, target);
        m_SkillInstances.push_back(pSkill);
        pSkill->InnerBegin();

        if (it != m_SkillCDMap.end())
        {
            it->second = pSkillInfo->CD;
        }
        else
        {
            if(pSkillInfo->CD > 0.0f)
                m_SkillCDMap.insert(std::pair<int32, float>(SKILL_ID_TO_TYPE(skillId), pSkillInfo->CD));
        }

        //>设置技能时间
        m_CurSkilllLeftTime = pSkillInfo->time;
        m_CurSkillId = pSkillInfo->id;

        m_Interface->OnUseSkill(skillId, target);

        return true;
    }

    return false;
}

bool BattleComponent::AddBuff(Buff* pBuff)
{
    assert(pBuff->m_BuffInfo);
    if (IsDisableAddBuff(pBuff->m_BuffInfo->type)) //>免疫指定类型的buff
    {
        //>免疫
        return false;
    }

    pBuff->m_Owner = m_Owner;
    pBuff->m_InstanceId = m_BuffInstanceIdCounter++;
    pBuff->OnAttach();
    m_BuffDatas.push_back(pBuff);

    if (m_Interface)
    {
        m_Interface->OnAddBuff(pBuff);
    }
    return true;
}

void BattleComponent::RemoveBuffByType(int type)
{
    Buff* pBuff = NULL;
    auto it = m_BuffDatas.begin();
    for (auto end = m_BuffDatas.end(); it != end; )
    {
        pBuff = *it;
        if (pBuff->m_BuffInfo->type == type)
        {
            pBuff->OnDettach();
            if (m_Interface)
            {
                m_Interface->OnRemoveBuff(pBuff);
            }
            m_BuffDatas.erase(it++);
            BuffFactory::FreeBuff(pBuff);
        }
        else
        {
            ++it;
        }
    }
}

bool BattleComponent::IsDisableAddBuff(int type)
{
    Buff* pBuff = NULL;
    auto it = m_BuffDatas.begin();
    for (auto end = m_BuffDatas.end(); it != end; ++it)
    {
        pBuff = *it;
        if(pBuff->m_BuffInfo->funcType == Buff_DisableAddBuff)
        {
            if (static_cast<MainYi*>(pBuff)->IsDisableAdd(type))
            {
                return true;
            }
        }
    }
    return false;
}

int BattleComponent::DamageReduceByHuDunBuff(int damage)
{
    Buff* pBuff = NULL;
    auto it = m_BuffDatas.begin();
    for (auto end = m_BuffDatas.end(); it != end; ++it)
    {
        pBuff = *it;
        if(pBuff->m_BuffInfo->funcType == Buff_HuDun ||
            pBuff->m_BuffInfo->funcType == Buff_HuDunPct ||
            pBuff->m_BuffInfo->funcType == Buff_HuDunMp2Hp)
        {
            damage = static_cast<HuDun*>(pBuff)->DamageReduce(damage);
            if (damage == 0)
                return damage;
        }
    }
    return damage;
}

int BattleComponent::DamageInverseByFanShangBuff(int damage)
{
    Buff* pBuff = NULL;
    auto it = m_BuffDatas.begin();
    int fanShang = 0;
    for (auto end = m_BuffDatas.end(); it != end; ++it)
    {
        pBuff = *it;
        if(pBuff->m_BuffInfo->funcType == Buff_FanShang)
        {
            fanShang += static_cast<FanShang*>(pBuff)->FanShangDamage(damage);
        }
    }
    return fanShang;
}

int BattleComponent::DamageToHpBuff(int damage)
{
    Buff* pBuff = NULL;
    auto it = m_BuffDatas.begin();
    int addHp = 0;
    for (auto end = m_BuffDatas.end(); it != end; ++it)
    {
        pBuff = *it;
        if(pBuff->m_BuffInfo->funcType == Buff_DamagePctToHp)
        {
            addHp += static_cast<DamagePctToHp*>(pBuff)->GetHp(damage);
        }
    }
    return addHp;
}

bool BattleComponent::HasWudiBuff()
{
    Buff* pBuff = NULL;
    auto it = m_BuffDatas.begin();
    for (auto end = m_BuffDatas.end(); it != end; ++it)
    {
        pBuff = *it;
        if(pBuff->m_BuffInfo->funcType == Buff_Wudi)
        {
            return true;
        }
    }
    return false;
}

bool BattleComponent::IsTriggerPassiveSkill(const SkillInfo* pSkillInfo)
{
    if (!(pSkillInfo->funcType == Skill_BeHurtAddBuff ||
          pSkillInfo->funcType == Skill_FanTanBuff || 
          pSkillInfo->funcType == Skill_ChongSheng ||
          pSkillInfo->funcType == Skill_MianYiKongZhi))
    {
        return false;
    }


    float p = BaseFunction::Random(0.0f, 1.0f);
    if (p <= pSkillInfo->triggerData[0])
        return true;
   
    return false;
}

void BattleComponent::BuHurtProcess()
{
    BeHurtTriggerPassiveSkill();            //>受击触发被动技能
    
    BeHurtRemoveXuanYunBuff();              //>受击消除昏睡buff
}

void BattleComponent::BeHurtTriggerPassiveSkill()
{
    std::vector<const SkillInfo*> triggerSkills;
    const SkillInfo* pInfo = NULL;
    m_Interface->GetSkillsForInterestBeHurt(triggerSkills); //>获得受击所可能触发的技能
    if (!triggerSkills.empty())
    {
        auto it = triggerSkills.begin();
        for (auto end = triggerSkills.end(); it != end; ++it)
        {
            pInfo = *it;
            if (pInfo->funcType == Skill_BeHurtAddBuff)
            {
                if(!IsTriggerPassiveSkill(pInfo))   //>检测被动技能是否触发
                    continue;
                
                gLog.Info("obj[%llu] be hurt trigger passive skill add buff.", m_Owner->m_GUID);
                for (int i = 0; i < 3; ++i)
                {
                    if (pInfo->buffDatas[i].buffId == INVALID_ID)
                        continue;

                    const BuffInfo* pBuffInfo = g_CSVFileManager->GetBuffByID(pInfo->buffDatas[i].buffId);
                    assert (pBuffInfo);
                    if(pBuffInfo)
                    {
                        Buff* pBuff = BuffFactory::AllocBuff(pBuffInfo, pInfo->buffDatas[i].time);
                        if (pBuff)
                        {
                            pBuff->SetData(pInfo->buffDatas[i].data);
                            if(false == AddBuff(pBuff))
                            {
                                BuffFactory::FreeBuff(pBuff);
                            }
                        }
                    }
                }
            }
        }
    }
}

void BattleComponent::BeHurtRemoveXuanYunBuff()
{
    //>删除眩晕buff
    Buff* pBuff = NULL;
    auto it = m_BuffDatas.begin();
    for (auto end = m_BuffDatas.end(); it != end; )
    {
        pBuff = *it;
        if (pBuff->m_BuffInfo->funcType == Buff_XuanYun)
        {
            pBuff->OnDettach();
            if (m_Interface)
            {
                m_Interface->OnRemoveBuff(pBuff);
            }
            m_BuffDatas.erase(it++);
            BuffFactory::FreeBuff(pBuff);

            gLog.Info("obj[%llu] be hurt remove XuanYun buff.", m_Owner->m_GUID);
        }
        else
        {
            ++it;
        }
    }
}

void BattleComponent::Update(float dt)
{
    m_CurSkilllLeftTime -= dt;

    UpdateCDs(dt);

    UpdateBuffs(dt);

    UpdateSkills(dt);
}

void BattleComponent::UpdateSkills(float dt)
{
    if (!m_SkillInstances.empty())
    {
        //>删除结束的技能
        Skill* pSkill = NULL;
        auto it = m_SkillInstances.begin();
        for (auto end = m_SkillInstances.end(); it != end; )
        {
            pSkill = *it;
            if (pSkill->IsEnd())
            {
                if (m_CurSkillId == pSkill->GetInfo()->id)
                    m_CurSkillId = 0;

                m_SkillInstances.erase(it++);
                SkillFactory::FreeSkill(pSkill);
            }
            else
            {
                ++it;
            }
        }

        //>更新技能
        it = m_SkillInstances.begin();        
        for (auto end = m_SkillInstances.end(); it != end; ++it)
        {
            pSkill = *it;
            pSkill->InnerUpdate(dt);
        }
    }
}

void BattleComponent::UpdateBuffs(float dt)
{
    if (!m_BuffDatas.empty())
    {
        //>删除结束的buff
        Buff* pBuff = NULL;
        auto it = m_BuffDatas.begin();
        for (auto end = m_BuffDatas.end(); it != end; )
        {
            pBuff = *it;
            if (pBuff->GetTime() <= 0.0f)
            {
                pBuff->OnDettach();
                if (m_Interface)
                {
                    m_Interface->OnBuffDead(pBuff);
                }
                m_BuffDatas.erase(it++);
                BuffFactory::FreeBuff(pBuff);
            }
            else
            {
                ++it;
            }
        }

        //>更新buff
        it = m_BuffDatas.begin();        
        for (auto end = m_BuffDatas.end(); it != end; ++it)
        {
            pBuff = *it;
            pBuff->m_Time -= dt;
            if (pBuff->m_NeedUpdate)
            {
                pBuff->Update(dt);
            }
        }
    }
    else
    {//>重置实例id计数器
        ResetBuffInstanceIdCounter();
    }
}

void BattleComponent::UpdateCDs(float dt)
{
    if (!m_SkillCDMap.empty())
    {
        auto it = m_SkillCDMap.begin();
        for (auto end = m_SkillCDMap.end(); it != end; ++it)
        {
            if (it->second > 0.0f)
            {
                it->second -= dt;
            }
        }
    }
}
