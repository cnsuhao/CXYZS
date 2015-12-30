#include "BattleComponent.h"
#include "Object.h"
#include "AnimComponent.h"
#include "CSVFileManager.h"
#include "ResManager.h"
#include "LogicManager.h"
#include "BaseFunction.h"
#include "Hero.h"

USING_NS_CC;
using namespace ui;


DirectionType GetDirection(const Vec2 &pos1, const Vec2 &pos2)
{
    if(pos1.x - pos2.x > 16 && abs(pos2.y - pos1.y) < 16)
    {
        return DIRECTION_LEFT;
    }
    else if(pos2.x - pos1.x > 16 && abs(pos2.y - pos1.y) < 16)
    {
        return DIRECTION_RIGHT;
    }
    else if(abs(pos2.x - pos1.x) < 16 && pos2.y - pos1.y > 16)
    {
        return DIRECTION_UP;
    }
    else if(abs(pos2.x - pos1.x) < 16 && pos1.y - pos2.y > 16)
    {
        return DIRECTION_DOWN;
    }
    else if(pos2.x - pos1.x > 16 && pos1.y - pos2.y > 16)
    {
        return DIRECTION_RIGHT_DOWN;
    }
    else if(pos1.x - pos2.x > 16 && pos2.y - pos1.y > 16)
    {
        return DIRECTION_LEFT_UP;
    }
    else if( pos2.x - pos1.x > 16 && pos2.y - pos1.y > 16)
    {
        return DIRECTION_RIGHT_UP;
    }
    else
    {
        return DIRECTION_LEFT_DOWN;
    }
}

BattleComponent::BattleComponent(MapObj* Owner, Interface* pInterface):m_Owner(Owner),m_Interface(pInterface)
{
    m_Time = 0.0f;
    m_SkillSoundDelay = 0.0f;
}

BattleComponent::~BattleComponent()
{

}

void BattleComponent::DoSkill(int skillId, MapObj* target)
{
    const SkillInfo* pSkillInfo = g_CSVFileManager->GetSkillByID(skillId);
    if (pSkillInfo)
    {
        if (m_Owner->m_AnimCompoenet)
        {
            if (target)
            {
                DirectionType direction = GetDirection(m_Owner->getPosition(), target->getPosition());
                m_Owner->m_AnimCompoenet->ChangeDirection(direction);
            }

            ActionType action = (ActionType)pSkillInfo->animId;
            CCASSERT(action < ACTION_MAX, "");
            m_Owner->m_AnimCompoenet->ChangeAction(action);

            if (pSkillInfo->soundId > 0)
            {
                bool playSound = true;
                m_SkillSoundGain = 1.0f;
                if (m_Owner->m_GUID != g_LogicManager->m_Hero->m_GUID )
                {
                    int tileDist = BaseFunction::TileDistance(m_Owner->GetTile(), g_LogicManager->m_Hero->GetTile());
                    if(tileDist > 6)
                    {//>如果大于6格,不能听见音效.
                        playSound = false;
                    }
                    else if(tileDist > 3)
                    {//>如果大于3格,听见音效音量50%.
                        m_SkillSoundGain = 0.5f;
                    }
                }
  
                if (playSound)
                {
                    if (pSkillInfo->soundDelay <= 0.0f)
                    {
                        g_ResManager->PlaySound(pSkillInfo->soundId, m_SkillSoundGain);
                    }
                    else
                    {
                        m_SkillSoundId = pSkillInfo->soundId;
                        m_SkillSoundDelay = pSkillInfo->soundDelay;
                    }
                }
            }

            m_Interface->OnSkillBegin(skillId, target);

            m_Time = pSkillInfo->time;
        }
    }
}

void BattleComponent::Update(float dt)
{
    if (m_Time > 0.0f)
    {//>更新技能时间
        m_Time -= dt;
        if (m_Time <= 0.0f)
        {
            m_Interface->OnSkillEnd();
        }
    }

    if (m_SkillSoundDelay > 0.0f)
    {//>更新延迟音效
        m_SkillSoundDelay -= dt;
        if (m_SkillSoundDelay <= 0.0f)
        {
            g_ResManager->PlaySound(m_SkillSoundId, m_SkillSoundGain);
        }
    }

    for (auto it = m_BuffDatas.begin(); it != m_BuffDatas.end(); )
    {//>更新buff
        it->lastTime -= dt;
        if (it->lastTime <= 0.0f)
        {
            BuffData data = *it;
            it = m_BuffDatas.erase(it);
            m_Interface->OnRemoveBuff(data);
            
        }
        else
        {
            ++it;
        }
    }
}

void BattleComponent::InitBuffList(std::vector<BuffData>& buffList)
{
    if (!buffList.empty())
    {
        m_BuffDatas.resize(buffList.size());

        int index = 0;
        auto it = buffList.begin();
        for (auto end = buffList.end(); it != end; ++it)
        {
            BuffData& buffData = *it;
            m_BuffDatas[index].buffId = buffData.buffId;
            m_BuffDatas[index].instanceId = buffData.instanceId;
            m_BuffDatas[index].lastTime = buffData.lastTime;
            m_BuffDatas[index].pUserData = NULL;
            const BuffInfo* pBuffInfo = g_CSVFileManager->GetBuffByID(buffData.buffId);
            if (pBuffInfo)
            {
                m_BuffDatas[index]. pBuffInfo = pBuffInfo;
                ++index;
            }
            else
            {
                CCASSERT(0, "");
            }
        }

        m_Interface->OnInitBuffList();
    }
}

void BattleComponent::AddBuff(int buffId, int instanceId, float lastTime)
{
    const BuffInfo* pBuffInfo = g_CSVFileManager->GetBuffByID(buffId);
    if (pBuffInfo)
    {
        size_t size = m_BuffDatas.size();
        m_BuffDatas.resize(size + 1);

        BuffData& buffData = m_BuffDatas[size];

        buffData.buffId = buffId;
        buffData.instanceId = instanceId;
        buffData.lastTime = lastTime;
        buffData.pBuffInfo = pBuffInfo;
        buffData.pUserData = NULL;
        m_Interface->OnAddBuff(buffData);
    }
}

void BattleComponent::RemoveBuff(int instanceId)
{
    auto it = m_BuffDatas.begin();
    for (auto end = m_BuffDatas.end(); it != end; ++it)
    {
        if (it->instanceId == instanceId)
        {
            
            BuffData data = *it;
            m_BuffDatas.erase(it);
            m_Interface->OnRemoveBuff(data);
            return;
        }
    }
}
