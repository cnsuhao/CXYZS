#include "UIPlayerInfo.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "Bag.h"
#include "CSVFileManager.h"
#include "ResManager.h"
#include "UITextAtlas.h"
#include "UILoadingBar.h"

using namespace cocostudio;
using namespace cocostudio::timeline;

static const char* EquipPartQuality[UIBag::EP_Number] = { "Image_Quality1", "Image_Quality2", "Image_Quality3", "Image_Quality4", "Image_Quality5", "Image_Quality6", "Image_Quality7", "Image_Quality8", "Image_Quality9", "Image_Quality10" };
static const char* EquipPartText[UIBag::EP_Number] = { "Text_wq", "Text_tk", "Text_xl", "Text_yf", "Text_jz", "Text_yd", "Text_sp", "Text_xz", "Text_cb", "Text_sz" };
static const char* EquipPartIcon[UIBag::EP_Number] = { "Sprite_Equipable1", "Sprite_Equipable2", "Sprite_Equipable3", "Sprite_Equipable4", "Sprite_Equipable5", "Sprite_Equipable6", "Sprite_Equipable7", "Sprite_Equipable8", "Sprite_Equipable9", "Sprite_Equipable10" };

static const EquipType EquipPartToTypes[UIBag::EP_Number] = {EQUIPTYPE_WEAPON, EQUIPTYPE_HEAD, EQUIPTYPE_NECKLACE, EQUIPTYPE_BODY, EQUIPTYPE_RING, EQUIPTYPE_BELT, EQUIPTYPE_ORNAMENTS, EQUIPTYPE_SHOES, EQUIPTYPE_WING, EQUIPTYPE_FASHION };

ShowPlayerInfo::ShowPlayerInfo()
{

}

void ShowPlayerInfo::SetByHero(Hero* pHero)
{
    guid            = pHero->m_GUID;
    level           = (int)pHero->m_Attribute[ATTRIBUTE_LEVEL];
    name.assign(pHero->m_Name);
    sex             = pHero->m_Sex;

    battlePower     = (int)pHero->m_BattlePower;
    if (pHero->m_isocietyID != 0){
        gangName = pHero->m_GangInfo.m_szName;
    }
    else{
        gangName = "------";   
    }

    hp              = (int)pHero->m_Attribute[ATTRIBUTE_HP];
    maxHp           = (int)pHero->m_Attribute[ATTRIBUTE_HPMAX];
    mp              = (int)pHero->m_Attribute[ATTRIBUTE_MP];
    maxMp           = (int)pHero->m_Attribute[ATTRIBUTE_MPMAX];
    attack          = (int)pHero->m_Attribute[ATTRIBUTE_ATTACK];
    defence         = (int)pHero->m_Attribute[ATTRIBUTE_DEFENSE];
    hit             = (int)pHero->m_Attribute[ATTRIBUTE_HIT];
    dodge           = (int)pHero->m_Attribute[ATTRIBUTE_DODGE];
    crit            = (int)pHero->m_Attribute[ATTRIBUTE_CRIT];
    tenaciy         = (int)pHero->m_Attribute[ATTRIBUTE_TENACITY];
    hitProb         = pHero->m_Attribute[ATTRIBUTE_HIT_PROB];
    critProb        = pHero->m_Attribute[ATTRIBUTE_CRIT_PROB];

    Bag& bag = pHero->m_Bag;
    CliItemInfo* pItem = NULL;
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        pItem = bag.GetEquipment((EquipType)i);
        if (pItem)
        {
            memcpy(&equipments[i], pItem, sizeof(CliItemInfo));
        }else
        {
            memset(&equipments[i], 0, sizeof(CliItemInfo));
        }
    }
}

void ShowPlayerInfo::SetByDetailMsg(MsgOtherRole_DetailInfo* pDetailMsg)
{
    guid            = pDetailMsg->guid;
    level           = pDetailMsg->level;
    name.assign(pDetailMsg->name);
    sex             = pDetailMsg->sex;

    battlePower     = pDetailMsg->battlePower;
    gangName.assign(pDetailMsg->gangName);

    hp              = pDetailMsg->hp;
    maxHp           = pDetailMsg->maxHp;
    mp              = pDetailMsg->mp;
    maxMp           = pDetailMsg->maxMp;
    attack          = pDetailMsg->attack;
    defence         = pDetailMsg->defence;
    hit             = pDetailMsg->hit;
    dodge           = pDetailMsg->dodge;
    crit            = pDetailMsg->crit;
    hitProb         = pDetailMsg->hitProb;
    critProb        = pDetailMsg->critProb;

    CliItemInfo* pItem;
    for (int i = 0; i < EQUIPTYPE_MAX; ++i)
    {
        pItem = &equipments[i];
        if (pDetailMsg->equipments[i].guid != INVALID_ID)
        {
            if (pItem != NULL)
            {
                //>取得物品信息
                memcpy(&pItem->Data , &pDetailMsg->equipments[i], sizeof(ItemInfoIt));
                pItem->pCfg = g_CSVFileManager->GetItemsByID(pItem->Data.id);
                assert(pItem->pCfg);
            }
        }else
        {
            memset(pItem, 0, sizeof(CliItemInfo));
        }
    }
}


UIPlayerInfo::UIPlayerInfo()//:Page(SeletedPage::Info)
{

}

UIPlayerInfo::~UIPlayerInfo()
{
}

bool UIPlayerInfo::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!UIBase::init());

        Size winSize = Director::getInstance()->getVisibleSize();
		//加载角色属性面板
		rolePanel = CSLoader::createNode("UI/PlayerInfo.csb");
		roleAttPanel = static_cast<Node*>(rolePanel->getChildByName("attribute_Panel"));
		if (rolePanel == NULL)
			return false;
		addChild(rolePanel);

        Node* attouchNode = static_cast<Node*>(rolePanel->getChildByName("Node_4"));
        CCASSERT(attouchNode, "");

        Node* pBagPanel = static_cast<Node*>(CSLoader::createNode("UI/Bag.csb"));
        if (pBagPanel)
        {
            Layer* pHeroPanel = static_cast<Layer*>(pBagPanel->getChildByName("Panel_Hero"));
            if (pHeroPanel)
            {
                pHeroPanel->removeFromParentAndCleanup(false);
                pHeroPanel->setPosition(Vec2::ZERO);
                attouchNode->addChild(pHeroPanel);

                Widget::ccWidgetClickCallback clickcallback = ([this](Ref* _sender)
                {
                    ImageView* img = (ImageView*)(_sender);
                    OnClickEquipment(UIBag::EquipPart(img->getTag()));
                });

                for (int i = 0; i < UIBag::EP_Number; ++i)
                {
                    _equipPartNames[i] = static_cast<Text*>(pHeroPanel->getChildByName(EquipPartText[i]));
                    _equipPartQuality[i] = static_cast<ImageView*>(pHeroPanel->getChildByName(EquipPartQuality[i]));
                    _equipPartIcon[i] = static_cast<Sprite*>(pHeroPanel->getChildByName(EquipPartIcon[i]));

                    CCASSERT(_equipPartNames[i] && _equipPartQuality[i] && _equipPartIcon[i], "");

                    _equipPartIcon[i]->setVisible(false);
                    _equipPartQuality[i]->loadTexture(EmptyItemTexture);
                    _equipPartQuality[i]->setTouchEnabled(true);
                    _equipPartQuality[i]->setTag((UIBag::EquipPart)i);
                    _equipPartQuality[i]->addClickEventListener(clickcallback);
                }

                _textFaction = static_cast<Text*>(pHeroPanel->getChildByName("Text_Group"));
                _textLevel = static_cast<Text*>(pHeroPanel->getChildByName("Text_Level"));
                _playerFace = static_cast<Node*>(pHeroPanel->getChildByName("Charater_Attach_Node"));
            }
        }



		Button* btn = nullptr;
		//关闭按钮
		btn = static_cast<Button*>(rolePanel->getChildByName("bt_Close"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref* _sender)
			{
				g_UIManager->HideUI(UI_PLAYERINFO);
			});
		}

        if (roleAttPanel)
        {
            Node* node = roleAttPanel->getChildByName("Image_55");
            //战斗力
            TextAtlas* textAtlas;
            textAtlas = TextAtlas::create("3586582", "Public/font_levelnum.png", 14, 20, "0");
            textAtlas->setPositionX(node->getPositionX() + node->getContentSize().width * 0.5);
            textAtlas->setPositionY(node->getPositionY());
            textAtlas->setName("atlas_zhandouli");
            roleAttPanel->addChild(textAtlas);
        }

        bRet = true;
	} while (0);

	return bRet;
}

void UIPlayerInfo::OnShow()
{
    UIBase::OnShow();
    ReflushEquipment();
    LoadPlayerFace();
    SetInfo();
}

void UIPlayerInfo::SetInfo()
{
    if (roleAttPanel)
    {
        TextAtlas* pAtlasZdl = static_cast<TextAtlas*>(roleAttPanel->getChildByName("atlas_zhandouli"));
        if (pAtlasZdl)
        {
            pAtlasZdl->setString(StringUtils::format("%d", _detailInfo.battlePower));
        }

        Text* pGuildName = static_cast<Text*>(roleAttPanel->getChildByName("Text_guild_name"));
        if (pGuildName)
        {
            pGuildName->setString(_detailInfo.gangName);
        }

        Text* pSMZ = static_cast<Text*>(roleAttPanel->getChildByName("Text_shengmingzhi"));
        if (pSMZ)
        {
            pSMZ->setString(StringUtils::format("%d/%d", _detailInfo.hp, _detailInfo.maxHp));
        }

        LoadingBar* pLRed = static_cast<LoadingBar*>(roleAttPanel->getChildByName("LoadingBar_red"));
        if (pLRed)
        {
            pLRed->setPercent(int(100.0f * _detailInfo.hp/_detailInfo.maxHp));
        }

        Text* pNLZ = static_cast<Text*>(roleAttPanel->getChildByName("Text_neilizhi"));
        if (pNLZ)
        {
            pNLZ->setString(StringUtils::format("%d/%d", _detailInfo.mp, _detailInfo.maxMp));
        }

        LoadingBar* pLblue = static_cast<LoadingBar*>(roleAttPanel->getChildByName("LoadingBar_blue"));
        if (pLblue)
        {
            pLblue->setPercent(int(100.0f * _detailInfo.mp/_detailInfo.maxMp));
        }

        Text* pGJQU = static_cast<Text*>(roleAttPanel->getChildByName("Text_gongjiqiangdu"));
        if (pGJQU)
        {
            pGJQU->setString(StringUtils::format("%d", _detailInfo.attack));
        }

        Text* pFSQU = static_cast<Text*>(roleAttPanel->getChildByName("Text_fasuqiangdu"));
        if (pFSQU)
        {
            pFSQU->setString(StringUtils::format("%d", 0));
        }

        Text* pWLFY = static_cast<Text*>(roleAttPanel->getChildByName("Text_wulifangyu"));
        if (pWLFY)
        {
            pWLFY->setString(StringUtils::format("%d", _detailInfo.defence));
        }

        Text* pFSFY = static_cast<Text*>(roleAttPanel->getChildByName("Text_fasufangyu"));
        if (pFSFY)
        {
            pFSFY->setString(StringUtils::format("%d", 0));
        }

        //////////////////////////////////////////////////////////////////////////
        Text* pMZZ = static_cast<Text*>(roleAttPanel->getChildByName("Text_shengming"));
        if (pMZZ)
        {
            pMZZ->setString(StringUtils::format("%d", _detailInfo.hit));
        }

        Text* pSBZ = static_cast<Text*>(roleAttPanel->getChildByName("Text_shanbi"));
        if (pSBZ)
        {
            pSBZ->setString(StringUtils::format("%d", _detailInfo.dodge));
        }

        Text* pPJZ = static_cast<Text*>(roleAttPanel->getChildByName("Text_pojia"));
        if (pPJZ)
        {
            pPJZ->setString(StringUtils::format("%0.2f", _detailInfo.hitProb));
        }

        Text* pCTZ = static_cast<Text*>(roleAttPanel->getChildByName("Text_chuantou"));
        if (pCTZ)
        {
            pCTZ->setString(StringUtils::format("%0.2f", _detailInfo.critProb));
        }

        Text* pBJZ = static_cast<Text*>(roleAttPanel->getChildByName("Text_baoji"));
        if (pBJZ)
        {
            pBJZ->setString(StringUtils::format("%d", _detailInfo.crit));
        }
    }
}

void UIPlayerInfo::SetInfo(MsgOtherRole_DetailInfo* pMsg)
{
    _detailInfo.SetByDetailMsg(pMsg);
}

void UIPlayerInfo::SetInfo(Hero* pHero)
{
    _detailInfo.SetByHero(pHero);
}

void UIPlayerInfo::OnClickEquipment(UIBag::EquipPart part)
{
    CliItemInfo* pItem = &_detailInfo.equipments[EquipPartToTypes[part]];
    if (pItem != NULL)
    {
        g_UIManager->OpenUI(UI_ITEM_INFO);
        UIItemInfo* pUIItemInfo = static_cast<UIItemInfo*>(g_UIManager->GetUI(UI_ITEM_INFO));
        if (pUIItemInfo)
        {
            vector<UIItemInfo::ButtonOperation> operations;
            if (g_LogicManager->m_Hero && _detailInfo.guid == g_LogicManager->m_Hero->m_GUID)
            {
                MakeOperation(pItem, operations, true);
                pUIItemInfo->SetupItem(pItem, operations, NULL);
            }
            else
            {
                pUIItemInfo->SetupItem(pItem, operations, _detailInfo.equipments);
            }
        }
    }
}

void UIPlayerInfo::MakeOperation(CliItemInfo* pItem, vector<UIItemInfo::ButtonOperation>& operations, bool wearing)
{
    if (pItem->pCfg->iBigClass == ITEMTYPE_EQUIP)
    {
        //>添加装备按钮
        UIItemInfo::ButtonOperation operation;
        //operation.operation = UIItemInfo::Equip;
        //if (wearing)
        //    operation.textId = 607;
        //else
        //    operation.textId = 606;
        //operations.push_back(operation);

        //>添加合成按钮
        operation.operation = UIItemInfo::Compose;
        operation.textId = 603;
        operations.push_back(operation);

        //>添加升星按钮
        if (pItem->CanUpStar() && !pItem->IsMaxUpStarLevel())
        {
            operation.operation = UIItemInfo::UpStar;
            operation.textId = 604;
            operations.push_back(operation);
        }

        //>最后添加出售按钮
        //operation.operation = UIItemInfo::Sell;
        //operation.textId = 613;
        //operations.push_back(operation);
    }
    else
    {
        //>添加使用按钮
        UIItemInfo::ButtonOperation operation;
        operation.operation = UIItemInfo::Use;
        operation.textId = 616;
        operations.push_back(operation);


        //>最后添加出售按钮
        operation.operation = UIItemInfo::Sell;
        operation.textId = 613;
        operations.push_back(operation);
    }
}

void UIPlayerInfo::LoadPlayerFace()
{
    if (_playerFace)
    {
        if (g_LogicManager->m_Hero && _detailInfo.guid == g_LogicManager->m_Hero->m_GUID)
        {
            _playerFace->stopAllActions();
            _playerFace->removeAllChildrenWithCleanup(true);
            Node* node = g_LogicManager->m_Hero->CreateFaceCopyNode();
            if (node)
            {
                node->setScale(1.5f);
                _playerFace->addChild(node);
            }
        }
        else
        {
            _playerFace->stopAllActions();
            _playerFace->removeAllChildrenWithCleanup(true);

            int faceId[EQUIPTYPE_MAX];
            for (int i = 0; i < EQUIPTYPE_MAX; ++i)
            {
                if (_detailInfo.equipments[i].Data.guid != INVALID_ID)
                {
                    faceId[i] = _detailInfo.equipments[i].pCfg->iFaceID;
                }else
                {
                    faceId[i] = 0;
                }
            }

            Node* node = Player::CreateFaceCopyNode(faceId, _detailInfo.sex);
            if (node)
            {
                node->setScale(1.5f);
                _playerFace->addChild(node);
            }
        }
    }
}

void UIPlayerInfo::ReflushEquipment()
{
    _textFaction->setString(BaseFunction::a2u(_detailInfo.name));
    const TextInfo *pText = g_CSVFileManager->GetTextByID(666);
    _textLevel->setString(BaseFunction::a2u(StringUtils::format(pText->strContent.c_str(), _detailInfo.level))); //%d级

    for (int i = 0; i < UIBag::EP_Number; ++i)
    {
        CliItemInfo* pItem = &_detailInfo.equipments[EquipPartToTypes[i]];
        if (pItem->Data.guid == INVALID_ID)
        {   
            _equipPartNames[i]->setVisible(true);
            _equipPartIcon[i]->setVisible(false);
            _equipPartQuality[i]->loadTexture(EmptyItemTexture);

            _equipPartQuality[i]->removeAllChildren();
        }
        else
        {
            _equipPartNames[i]->setVisible(false);
            _equipPartIcon[i]->setTexture(pItem->pCfg->strIcon);
            _equipPartIcon[i]->setVisible(true);
            _equipPartQuality[i]->loadTexture(ItemQualityTextures[pItem->pCfg->iQuality]);

            //>套装特效代码
            if (pItem->pCfg->iSuitID != 0)
            {
                const SuitInfo* pInfo = g_CSVFileManager->GetSuitByID(pItem->pCfg->iSuitID);
                if (pInfo)
                {
                    Sprite* sprite = g_ResManager->CreateSprite(pInfo->iSuitEffceid, _equipPartQuality[i]->getAnchorPointInPoints());
                    _equipPartQuality[i]->removeAllChildren();
                    _equipPartQuality[i]->addChild(sprite);
                }
            }
            else
            {
                _equipPartQuality[i]->removeAllChildren();
            }
        }
    }
}
