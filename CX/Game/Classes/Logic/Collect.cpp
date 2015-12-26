#include "Collect.h"
/*
#include "CSVFileManager.h"
#include "SceneManager.h"
#include "ResManager.h"
#include "BaseFunction.h"

Collect::Collect()
{
}

Collect::~Collect()
{
}

bool Collect::Init(int objid)
{
	m_ResInfo = g_CSVFileManager->GetResByID(objid);
	if (m_ResInfo)
	{
		m_GUID = objid;

		m_NameText = cc2dui::Text::create(BaseFunction::a2u(m_ResInfo->strName.c_str()), "", 16);
		if (m_NameText)
		{
			const NameColor *nameColor = g_CSVFileManager->GetNameColorByID(6);
			m_NameText->setColor(Color3B(nameColor->iR, nameColor->iG, nameColor->iB));
			addChild(m_NameText, 10);
			m_NameText->setPosition(Vec2(-20, 50));
		}

		if (m_ResInfo->iModelid != 0)
		{
			m_Image = g_ResManager->CreateSprite(m_ResInfo->iModelid);
			addChild(m_Image,0);
		}

		if(m_ResInfo->iEffectid != 0)
		{
			m_Effect = g_ResManager->CreateSprite(m_ResInfo->iEffectid);
			addChild(m_Effect,0);
		}

		return true;
	}
	return false;
}



void Collect::SetTilePos(const Vec2 &pos1)
{
    Vec2 position = g_SceneManager->m_MainLayer->Tile2Pos(pos1);
	setPosition(position);
    setLocalZOrder(10000 - (int)position.y);
}


bool Collect::IsSelection(const Vec2 &point)
{
    Vec2 pos = convertToNodeSpace(point);
	Rect rect = RectApplyAffineTransform(m_Image->getTextureRect(), m_Image->getNodeToParentAffineTransform());
    rect.origin.x = m_Image->getOffsetPosition().x - m_Image->getContentSize().width * m_Image->getAnchorPoint().x + m_Image->getPositionX();
    rect.origin.y = m_Image->getOffsetPosition().y - m_Image->getContentSize().height * m_Image->getAnchorPoint().y + m_Image->getPositionY();

	if (rect.containsPoint(pos))
	{
		return true;
	}
	return false;	
}
*/