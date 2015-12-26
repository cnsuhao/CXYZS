#include "BattleModule.h"
#include "Object.h"
#include "Skill.h"
#include "CSVFileManager.h"
#include "LogManage.h"

bool BattleModule::CalcDodoge(Object *pAttackObj, Object* pTargetObj)
{
    //max(����������ֵ/(����������ֵ+�ܻ�������ֵ��+ (������������ - �ܻ���������)��0��;

    float a = MyMax( pAttackObj->m_Attribute[ATTRIBUTE_HIT].GetValueF() / (pAttackObj->m_Attribute[ATTRIBUTE_HIT].GetValueF() + pTargetObj->m_Attribute[ATTRIBUTE_DODGE].GetValueF()), 0.0f );
    float b = pAttackObj->m_Attribute[ATTRIBUTE_HIT_PROB].GetBase() - pTargetObj->m_Attribute[ATTRIBUTE_DODGE_PROB].GetBase();

    float hitProbability = MyMax(a + b, 0.0f);
    gLog.Info("attacker[%llu] attack target[%llu], hitProbability[%f]", pAttackObj->m_GUID, pTargetObj->m_GUID, hitProbability);

    if (BaseFunction::Random(0.0, 1.0f) > hitProbability)
        return true;

    return false;
}

bool BattleModule::CalcCritical(Object *pAttackObj, Object* pTargetObj)
{
    //max(�����߱���ֵ/(�����߱���ֵ+�ܻ�������ֵ��+ �����߱����� - �ܻ��������ʣ�0��
    float a = MyMax( pAttackObj->m_Attribute[ATTRIBUTE_CRIT].GetValueF() / (pAttackObj->m_Attribute[ATTRIBUTE_CRIT].GetValueF() + pTargetObj->m_Attribute[ATTRIBUTE_TENACITY].GetValueF()), 0.0f );
    float b = pAttackObj->m_Attribute[ATTRIBUTE_CRIT_PROB].GetBase() - pTargetObj->m_Attribute[ATTRIBUTE_TENACITY_PROB].GetBase();

    float critProbability = MyMax(a + b, 0.0f);
    gLog.Info("attacker[%llu] attack target[%llu], critProbability[%f]", pAttackObj->m_GUID, pTargetObj->m_GUID, critProbability);

    if (BaseFunction::Random(0.0, 1.0f) > critProbability)
        return false;

    return true;
}

DropHpMode BattleModule::CalcDamage(Object *pAttackObj, Skill* pSkill, Object* pTargetObj)
{
    DropHpMode dm;

    if (CalcDodoge(pAttackObj, pTargetObj))
    {
        dm.hp = 0;
        dm.mode = DropHpMode::Dodge;
        gLog.Error("skill[%d] is dodge.", pSkill->GetInfo()->id);
        return dm;
    }

    //��max���������������� *����ϵ��  - MAX(������������� ,0)�� * ��1 + �������˺�״̬ϵ������1��+ ����random����Сֵ�����ֵ���� *��1+����������״̬ϵ����
    float a = pAttackObj->m_Attribute[ATTRIBUTE_ATTACK].GetValueF() * MyMax(pSkill->GetInfo()->factor, 0.0f) - pTargetObj->m_Attribute[ATTRIBUTE_DEFENSE].GetValueF();
    a = MyMax( a * (1.0f + pTargetObj->m_Attribute[ATTRIBUTE_HUR_FACTOR].GetBase()) , 1.0f);
   
    float b = BaseFunction::Random( pSkill->GetInfo()->minValue , pSkill->GetInfo()->maxValue) * (1.0f + pAttackObj->m_Attribute[ATTRIBUTE_ATK_FACTOR].GetValueF());

    dm.hp = int(a + b);
    if (CalcCritical(pAttackObj, pTargetObj))
    {
        dm.hp = int(dm.hp * pAttackObj->m_Attribute[ATTRIBUTE_CRIT_FACTOR].GetValueF());
        dm.mode = DropHpMode::Critical;
        return dm;
    }

    dm.mode = DropHpMode::Normal;
    return dm;
}

AddHpMode BattleModule::CalcTreatment(Object *pAttackObj, Skill* pSkill, Object* pTargetObj)
{
    //��ʩ���߹����� * ����ϵ����*��������ϵ�� + ����random����Сֵ�����ֵ��
    AddHpMode cure(0, AddHpMode::SkillCure);

    float a = pAttackObj->m_Attribute[ATTRIBUTE_ATTACK].GetValueF() * MyMax(pSkill->GetInfo()->factor, 0.0f) * pAttackObj->m_Attribute[ATTRIBUTE_CURE_FACTOR].GetValueF();
    int b = BaseFunction::Random( pSkill->GetInfo()->minValue, pSkill->GetInfo()->maxValue);

    cure.hp = int(a) + b;

    return cure;
}
