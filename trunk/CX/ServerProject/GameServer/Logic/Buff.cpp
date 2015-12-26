#include "Buff.h"
#include "CSVFileManager.h"
#include "Object.h"
#include "Skill.h"
#include "BattleComponent.h"
#include "LogManage.h"

Buff::Buff()
{
    m_Owner = NULL;
    m_BuffInfo = NULL;
    m_Time = NULL;
    m_NeedUpdate = false;
}


Buff* BuffFactory::AllocBuff(const BuffInfo* pInfo, float time)
{
    Buff* pBuff = 0;
    BuffFunctionType funcId = (BuffFunctionType)pInfo->funcType;
    switch (funcId)
    {
    case Buff_ChenMo:
        pBuff = _ChenMoAllocer.Alloc();
        gLog.Info("<<<-------alloc ChenMo buff[%p]", pBuff);
        break;
    case Buff_FengXue:
        pBuff = _FengXueAllocer.Alloc();
        gLog.Info("<<<-------alloc FengXue buff[%p]", pBuff);
        break;
    case Buff_XuanYun:
        pBuff = _XuanYunAllocer.Alloc();
        gLog.Info("<<<-------alloc XuanYun buff[%p]", pBuff);
        break;
    case Buff_HunShui:
        pBuff = _HunShuiAllocer.Alloc();
        gLog.Info("<<<-------alloc HunShui buff[%p]", pBuff);
        break;
    case Buff_HunLuan:
        pBuff = _HunLuanAllocer.Alloc();
        gLog.Info("<<<-------alloc HunLuan buff[%p]", pBuff);
        break;
    case Buff_Wudi:
        pBuff = _WuDiAllocer.Alloc();
        gLog.Info("<<<-------alloc Wudi buff[%p]", pBuff);
        break;
    case Buff_DisableAddBuff:
        break;
    case Buff_HuDun:
        break;
    case Buff_HuDunPct:
        break;
    case Buff_HuDunMp2Hp:
        break;
    case Buff_DamagePctToHp:
        break;
    case Buff_FanShang:
        pBuff = _FanShangAllocer.Alloc();
        break;
    case Buff_HuoQiang:
        break;
    case Buff_ZhaoHuang:
        break;
    case Buff_DropHp:
        break;
    case Buff_DropCurHpPct:
        break;
    case Buff_DropHpMaxPct:
        break;
    case Buff_AddHpMax:
    case Buff_AddHpMaxPct:
    case Buff_AddMpMax:
    case Buff_AddMpMaxPct:
    case Buff_AddMoveSpeed:
    case Buff_AddAttackPower:
    case Buff_AddAttackPowerPct:
    case Buff_AddDefencePower:
    case Buff_AddDefencePowerPct:
    case Buff_AddCrit:
    case Buff_AddCritPct:
    case Buff_AddHit:
    case Buff_AddHitPct:
    case Buff_AddTenacity:
    case Buff_AddTenacityPct:
    case Buff_AddDodge:
    case Buff_AddDodgePct:
    case Buff_AddAtkFactor:
    case Buff_AddDefFactor:
    case Buff_AddCritFactor:
        pBuff = _ChangeObjAttrAllocer.Alloc();
        gLog.Info("<<<-------alloc _ChangeObjAttrAllocer buff[%p]", pBuff);
        break;
    default:
        break;
    }

    if (pBuff)
    {
        pBuff->m_BuffInfo = pInfo;
        pBuff->m_Time = time;
    }
    return pBuff;
}

void BuffFactory::FreeBuff(Buff* pBuff)
{
    BuffFunctionType funcId = (BuffFunctionType)pBuff->m_BuffInfo->funcType;
    switch (funcId)
    {
    case Buff_ChenMo:
        _ChenMoAllocer.Free((ChenMo*)pBuff);
        gLog.Info("------->>>free ChenMo buff[%p]", pBuff);
        break;
    case Buff_FengXue:
        _FengXueAllocer.Free((FengXue*)pBuff);
        gLog.Info("------->>>free FengXue buff[%p]", pBuff);
        break;
    case Buff_XuanYun:
        _XuanYunAllocer.Free((XuanYun*)pBuff);
        gLog.Info("------->>>free XuanYun buff[%p]", pBuff);
        break;
    case Buff_HunShui:
        _HunShuiAllocer.Free((HunShui*)pBuff);
        gLog.Info("------->>>free HunShui buff[%p]", pBuff);
        break;
    case Buff_HunLuan:
        _HunLuanAllocer.Free((HunLuan*)pBuff);
        gLog.Info("------->>>free HunLuan buff[%p]", pBuff);
        break;
    case Buff_Wudi:
        _WuDiAllocer.Free((WuDi*)pBuff);
        break;
    case Buff_DisableAddBuff:
        break;
    case Buff_HuDun:
        break;
    case Buff_HuDunPct:
        break;
    case Buff_HuDunMp2Hp:
        break;
    case Buff_DamagePctToHp:
        break;
    case Buff_FanShang:
        break;
    case Buff_HuoQiang:
        break;
    case Buff_ZhaoHuang:
        break;
    case Buff_DropHp:
        break;
    case Buff_DropCurHpPct:
        break;
    case Buff_DropHpMaxPct:
        break;
    case Buff_AddHpMax:
    case Buff_AddHpMaxPct:
    case Buff_AddMpMax:
    case Buff_AddMpMaxPct:
    case Buff_AddMoveSpeed:
    case Buff_AddAttackPower:
    case Buff_AddAttackPowerPct:
    case Buff_AddDefencePower:
    case Buff_AddDefencePowerPct:
    case Buff_AddCrit:
    case Buff_AddCritPct:
    case Buff_AddHit:
    case Buff_AddHitPct:
    case Buff_AddTenacity:
    case Buff_AddTenacityPct:
    case Buff_AddDodge:
    case Buff_AddDodgePct:
    case Buff_AddAtkFactor:
    case Buff_AddDefFactor:
    case Buff_AddCritFactor:
        _ChangeObjAttrAllocer.Free((ChangeObjAttribute*)pBuff);
        gLog.Info("------->>>free ChangeObjAttribute buff[%p]", pBuff);
        break;
    default:
        break;
    }
}

void ChenMo::OnAttach()
{
    m_Owner->m_BattleComp->m_ChenMo += 1;
    gLog.Info("add ChenMo Flag to Obj[%lld]", m_Owner->m_GUID);
}

void ChenMo::OnDettach()
{
    m_Owner->m_BattleComp->m_ChenMo -= 1;
    gLog.Info("remove ChenMo Flag to Obj[%lld]", m_Owner->m_GUID);
}

void FengXue::OnAttach()
{
    m_Owner->m_BattleComp->m_FengXue += 1;
    gLog.Info("add FengXue Flag to Obj[%lld]", m_Owner->m_GUID);
}

void FengXue::OnDettach()
{
    m_Owner->m_BattleComp->m_FengXue -= 1;
    gLog.Info("remove FengXue Flag to Obj[%lld]", m_Owner->m_GUID);
}

void XuanYun::OnAttach()
{
    m_Owner->m_BattleComp->m_XuanYun += 1;
    gLog.Info("add XuanYun Flag to Obj[%lld]", m_Owner->m_GUID);
}

void XuanYun::OnDettach()
{
    m_Owner->m_BattleComp->m_XuanYun -= 1;
    gLog.Info("remove XuanYun Flag to Obj[%lld]", m_Owner->m_GUID);
}

void HunShui::OnAttach()
{
    m_Owner->m_BattleComp->m_HunShui += 1;
    gLog.Info("add HunShui Flag to Obj[%lld]", m_Owner->m_GUID);
}

void HunShui::OnDettach()
{
    m_Owner->m_BattleComp->m_HunShui -= 1;
    gLog.Info("remove HunShui Flag to Obj[%lld]", m_Owner->m_GUID);
}

void HunLuan::OnAttach()
{
    m_Owner->m_BattleComp->m_HunLuan += 1;
    gLog.Info("add HunLuan Flag to Obj[%lld]", m_Owner->m_GUID);
}

void HunLuan::OnDettach()
{
    m_Owner->m_BattleComp->m_HunLuan -= 1;
    gLog.Info("remove HunLuan Flag to Obj[%lld]", m_Owner->m_GUID);
}

void WuDi::OnAttach()
{
    m_Owner->m_BattleComp->m_WuDi += 1;
    gLog.Info("add WuDi Flag to Obj[%lld]", m_Owner->m_GUID);
}

void WuDi::OnDettach()
{
    m_Owner->m_BattleComp->m_WuDi -= 1;
    gLog.Info("remove WuDi Flag to Obj[%lld]", m_Owner->m_GUID);
}

void MainYi::SetData(float data[3])
{
    m_DisableBuffType = (int)data[1];
}

int HuDun::DamageReduce(int damage)
{
    if (m_Time <= 0.0f)
        return damage;

    if (m_Hp > damage)
    {
        m_Hp -= damage;
        return 0;
    }
    else if(m_Hp == damage)
    {
        m_Time = -1.0;
        return 0;
    }
    else
    {
		m_Time = -1.0;
        return damage - m_Hp;
    }
}

int DamagePctToHp::GetHp(int damage)
{
    return int(damage * m_HpFactor);
}

void FanShang::SetData(const float data[3])
{
    m_FanShangFactor = data[0];
}

int FanShang::FanShangDamage(int damage)
{
    return int(m_FanShangFactor * damage);
}

void ChiXuDiaoXue::SetData(float data[3])
{
    m_IntervalCd = data[0];
    m_DropHp = data[1];

    m_Interval = m_IntervalCd;
}

void ChiXuDiaoXue::Update(float dt)
{
    m_Interval -= dt;
    if (m_Interval < 0.0f)
    {
        if (m_BuffInfo->funcType == BuffFunctionType::Buff_DropHp)
        {
            if (m_Owner->m_BattleComp && m_Owner->m_BattleComp->m_WuDi)
            {
                m_Owner->DropHP(DropHpMode(0, 0, 0, DropHpMode::WuDiMianYiBuff));
            }
            else
            {
                int dropHp = (int)m_DropHp;
                if (dropHp > 0)
                {
                    m_Owner->DropHP(DropHpMode(0, 0, dropHp, DropHpMode::DropHpBuff));
                }
            }
        }
        else if(m_BuffInfo->funcType == BuffFunctionType::Buff_DropCurHpPct)
        {
            if (m_Owner->m_BattleComp && m_Owner->m_BattleComp->m_WuDi)
            {
                m_Owner->DropHP(DropHpMode(0, 0, 0, DropHpMode::WuDiMianYiBuff));
            }
            else
            {
                int dropHp = int(m_Owner->m_Attribute[ATTRIBUTE_HP].GetValueF() * m_DropHp);
                if (dropHp > 0)
                {
                    m_Owner->DropHP(DropHpMode(0, 0, dropHp, DropHpMode::DropHpBuff));
                }
            }
        }
        else if(m_BuffInfo->funcType == BuffFunctionType::Buff_DropHpMaxPct)
        {
            if (m_Owner->m_BattleComp && m_Owner->m_BattleComp->m_WuDi)
            {
                m_Owner->DropHP(DropHpMode(0, 0, 0, DropHpMode::WuDiMianYiBuff));
            }
            else
            {
                int dropHp = int(m_Owner->m_Attribute[ATTRIBUTE_HPMAX].GetValueF() * m_DropHp);
                if (dropHp > 0)
                {
                    m_Owner->DropHP(DropHpMode(0, 0, dropHp, DropHpMode::DropHpBuff));
                }
            }
        }
        m_Interval += m_IntervalCd;
    }
}

void ChangeObjAttribute::SetData(const float data[3])
{
    m_AddValue = data[0]; 
}

void ChangeObjAttribute::OnAttach()
{
    BuffFunctionType type = (BuffFunctionType)m_BuffInfo->funcType;
    switch (type)
    {
    case Buff_ChenMo:
    case Buff_FengXue:
    case Buff_XuanYun:
    case Buff_HunShui:
    case Buff_HunLuan:
    case Buff_Wudi:
    case Buff_DisableAddBuff:
    case Buff_HuDun:
    case Buff_HuDunPct:
    case Buff_HuDunMp2Hp:
    case Buff_DamagePctToHp:
    case Buff_FanShang:
    case Buff_HuoQiang:
    case Buff_ZhaoHuang:
    case Buff_DropHp:
    case Buff_DropCurHpPct:
    case Buff_DropHpMaxPct:
        gLog.Warning("error buff type.");
        break;
    case Buff_AddHpMax:
        m_Owner->m_Attribute[ATTRIBUTE_HPMAX].AddBuffValue(m_AddValue);
        break;
    case Buff_AddHpMaxPct:
        m_Owner->m_Attribute[ATTRIBUTE_HPMAX].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddMpMax:
        m_Owner->m_Attribute[ATTRIBUTE_MPMAX].AddBuffValue(m_AddValue);
        break;
    case Buff_AddMpMaxPct:
        m_Owner->m_Attribute[ATTRIBUTE_MPMAX].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddMoveSpeed:
        m_Owner->m_Attribute[ATTRIBUTE_MOVESPEED_FACTOR].AddBuffValue(m_AddValue);
        break;
    case Buff_AddAttackPower:
        m_Owner->m_Attribute[ATTRIBUTE_ATTACK].AddBuffValue(m_AddValue);
        break;
    case Buff_AddAttackPowerPct:
        m_Owner->m_Attribute[ATTRIBUTE_ATTACK].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddDefencePower:
        m_Owner->m_Attribute[ATTRIBUTE_DEFENSE].AddBuffValue(m_AddValue);
        break;
    case Buff_AddDefencePowerPct:
        m_Owner->m_Attribute[ATTRIBUTE_DEFENSE].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddCrit:
        m_Owner->m_Attribute[ATTRIBUTE_CRIT].AddBuffValue(m_AddValue);
        break;
    case Buff_AddCritPct:
        m_Owner->m_Attribute[ATTRIBUTE_CRIT].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddHit:
        m_Owner->m_Attribute[ATTRIBUTE_HIT].AddBuffValue(m_AddValue);
        break;
    case Buff_AddHitPct:
        m_Owner->m_Attribute[ATTRIBUTE_HIT].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddTenacity:
        m_Owner->m_Attribute[ATTRIBUTE_TENACITY].AddBuffValue(m_AddValue);
        break;
    case Buff_AddTenacityPct:
        m_Owner->m_Attribute[ATTRIBUTE_TENACITY].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddDodge:
        m_Owner->m_Attribute[ATTRIBUTE_DODGE].AddBuffValue(m_AddValue);
        break;
    case Buff_AddDodgePct:
        m_Owner->m_Attribute[ATTRIBUTE_DODGE].AddBuffPercent(m_AddValue);
        break;
    case Buff_AddAtkFactor:
        m_Owner->m_Attribute[ATTRIBUTE_ATK_FACTOR].AddBuffValue(m_AddValue);
        break;
    case Buff_AddDefFactor:
        m_Owner->m_Attribute[ATTRIBUTE_HUR_FACTOR].AddBuffValue(m_AddValue);
        break;
    case Buff_AddCritFactor:
        m_Owner->m_Attribute[ATTRIBUTE_CRIT_PROB].AddBuffValue(m_AddValue);
        break;
    default:
        break;
    }
}

void ChangeObjAttribute::OnDettach()
{

}

BuffAllocer<ChenMo>  BuffFactory::_ChenMoAllocer;
BuffAllocer<FengXue> BuffFactory::_FengXueAllocer;
BuffAllocer<XuanYun> BuffFactory::_XuanYunAllocer;
BuffAllocer<HunShui> BuffFactory::_HunShuiAllocer;
BuffAllocer<HunLuan> BuffFactory::_HunLuanAllocer;
BuffAllocer<WuDi>    BuffFactory::_WuDiAllocer;
BuffAllocer<FanShang> BuffFactory::_FanShangAllocer;
BuffAllocer<ChangeObjAttribute> BuffFactory::_ChangeObjAttrAllocer;



