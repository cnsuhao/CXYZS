#include "NPC.h"
#include "CSVFileManager.h"
#include "ResManager.h"

USING_NS_CC;
using namespace cc2dui;


NPC::NPC()
{

}

NPC::~NPC()
{
    delete m_AnimCompoenet;
}

bool NPC::Init(int objid)
{
	m_Type = OBJECT_NPC;
    m_NPCInfo = g_CSVFileManager->GetNPCInfoByID(objid);
	if (m_NPCInfo)
	{
		m_HeadIcon = m_NPCInfo->strHeadPath;
        NpcAnimComp* pAmimComp = new NpcAnimComp(this, this);
        assert(pAmimComp);
        if (pAmimComp)
        {
            m_AnimCompoenet = pAmimComp;
            pAmimComp->LoadResouce(false);
            cc2d::Node* pBody = pAmimComp->GetBody();
            if (pBody)
            {
                pBody->setAnchorPoint(Vec2(m_NPCInfo->fAnchorX, m_NPCInfo->fAnchorY));
            }

            pAmimComp->ChangeDirection((m_NPCInfo->iNpcDirecion == 0) ? DIRECTION_LEFT_DOWN : DIRECTION_RIGHT_DOWN);
            Idle();

            const NameColor *nameColor = g_CSVFileManager->GetNameColorByID(5);
            assert(nameColor);
            SetName(m_NPCInfo->strName, (float)m_NPCInfo->iNameHeight, Color3B(nameColor->iR, nameColor->iG, nameColor->iB));

            m_AnimCompoenet->EnableShadow(true);

            schedule(CC_SCHEDULE_SELECTOR(NPC::Update), 1.0f);
            return true;     
        }
	}
    return false;
}

void NPC::ReleaseSelf(Node *sender, long data)
{
	removeFromParentAndCleanup(true);
}

void NPC::Update(float delta)
{
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->Update(delta);
    }
}

void NPC::Idle()
{
    MapObj::Idle();
    if (m_AnimCompoenet)
    {
        m_AnimCompoenet->ChangeAction(ACTION_IDLE);
    }
}


const char* NPC::GetModelName()
{
    return m_NPCInfo->strModleid.c_str();
}

void NPC::OnAttcakAnimEnd()
{

}
