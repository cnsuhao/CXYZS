#include "AIComponent.h"
#include "Object.h"
#include "Map.h"
#include "LogManage.h"

AIComponent::AIComponent(Object* woner, Interface* interfece):m_Woner(woner),m_Interfece(interfece)
{

}

void AIComponent::ClearGrudge()
{
    m_GrudgeTable.clear();
}
