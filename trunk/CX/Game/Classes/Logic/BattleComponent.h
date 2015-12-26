/********************************************************************
created:	2015/12/2
author:		lishihai
desc:		战斗类
*********************************************************************/

#ifndef __BATLLE_COMPONENT_H__
#define __BATLLE_COMPONENT_H__

#include "ClientDef.h"
#include "math/Vec2.h"

namespace cc2d
{
    class Sprite;
    namespace ui
    {
        class LoadingBar;
    }
}

class MapObj;
class BattleComponent
{
public:
    struct BuffData;
    class Interface
    {
    public:
        virtual void OnSkillBegin(int skillId, MapObj* target) = 0;
        virtual void OnSkillEnd() = 0;

        virtual void OnInitBuffList() = 0;
        virtual void OnAddBuff(const BuffData& buffData) = 0;
        virtual void OnRemoveBuff(const BuffData& buffData) = 0;
    };

    struct BuffData
    {
        BuffData() {}

        int buffId;
        int instanceId;
        float lastTime;
        const struct BuffInfo* pBuffInfo;
        mutable void* pUserData;

        BuffData& operator =(BuffData& other)
        {
            buffId = other.buffId;
            instanceId = other.instanceId;
            lastTime = other.lastTime;
            pBuffInfo = other.pBuffInfo;
            pUserData = other.pUserData;
            return *this;
        }
    };


public:
	BattleComponent(MapObj* Owner, Interface* pInterface);
	virtual ~BattleComponent();

    void DoSkill(int skillId, MapObj* target);
    void Update(float dt);

    void InitBuffList(std::vector<BuffData>& buffList);
    void AddBuff(int buffId, int instanceId, float lastTime);
    void RemoveBuff(int instanceId);

    inline const std::vector<BuffData>& GetBuffDatas();
protected:
    MapObj*      m_Owner;
    Interface*   m_Interface;
    float        m_Time;
    float        m_SkillSoundDelay;     //>技能音效延迟
    int          m_SkillSoundId;        //>技能音效id
    float        m_SkillSoundGain;      //>技能音效音量

    std::vector<BuffData> m_BuffDatas;    //>buff数据
};

inline const std::vector<BattleComponent::BuffData>& BattleComponent::GetBuffDatas()
{
    return m_BuffDatas;
}

#endif	//__BATLLE_COMPONENT_H__
