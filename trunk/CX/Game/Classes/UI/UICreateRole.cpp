#include "UICreateRole.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "LogicManager.h"
#include "NetManager.h"
#include "ResManager.h"

using namespace cocostudio;

static char* IconTexturePath[2][UICreateRole::MaxHeadIndex] = { "Icon/nvzhu1.png", "Icon/nvzhu2.png", "Icon/nvzhu3.png", "Icon/nanzhu1.png", "Icon/nanzhu2.png", "Icon/nanzhu3.png" };

UICreateRole::UICreateRole()
{
    m_Root = NULL;

    m_bodyAImg = NULL;
    m_bodyBImg = NULL;

    m_heads[0] = m_heads[1] = m_heads[2] = 0;
    m_selectEffect = NULL;

    m_manCheckBox = NULL;
    m_wumanCheckBox = NULL;

    m_tfRoleName = NULL;

    m_roleSex = Sex_Woman;
    m_roleIconIndex = 0;
}

UICreateRole::~UICreateRole()
{
}

bool UICreateRole::init()
{
bool bRet = false;
    do 
    {
        CC_BREAK_IF(!UIBase::init());
        Node* root = CSLoader::createNode("UI/CreateRole.csb");	
        m_Root = root;

        static_cast<Button*>(root->getChildByName("EnterButton"));
        Button* btn = static_cast<Button*>(root->getChildByName("EnterButton"));
        btn->addClickEventListener([this](Ref* _sender)
        {
            if (GetRoleName().empty())
            {
                MessageBox("请为你的角色取一个名字.", "创建角色失败");
            }
            else
            {
                if (BaseFunction::Utf8Length(GetRoleName()) > 6 ||  GetRoleName().size() > 18)
                {
                    MessageBox("名字在18个字母或六个汉字以内.", "创建角色失败");
                    return;
                }

                MsgCreateRole msg(BaseFunction::u2a(GetRoleName()).c_str(), (char)m_roleSex, m_roleIconIndex);              
                g_NetManager->SendMsg(msg);
            }
        });

        m_tfRoleName = static_cast<TextField*>(root->getChildByName("RoleNameTextField"));
        if (m_tfRoleName)
        {
            m_tfRoleName->setTextVerticalAlignment(TextVAlignment::CENTER);
            m_tfRoleName->addEventListener([this](Ref* _sender, TextField::EventType type)
            {
                switch (type)
                {
                case cocos2d::ui::TextField::EventType::ATTACH_WITH_IME:
                    {
                    }break;
                case cocos2d::ui::TextField::EventType::DETACH_WITH_IME:
                    {
                    }break;
                case cocos2d::ui::TextField::EventType::INSERT_TEXT:
                    {
                    }break;
                case cocos2d::ui::TextField::EventType::DELETE_BACKWARD:
                    {
                    }break;
                default:
                    break;
                }
            });
        }

        m_heads[0] = static_cast<Sprite*>(root->getChildByName("Sprite_head1"));
        m_heads[1] = static_cast<Sprite*>(root->getChildByName("Sprite_head2"));
        m_heads[2] = static_cast<Sprite*>(root->getChildByName("Sprite_head3"));
        CCASSERT(m_heads[0] && m_heads[1] && m_heads[2], "");


        CheckBox::ccCheckBoxCallback selectIconCallback = ([this](Ref* sender,CheckBox::EventType type)
        {
            CheckBox* checkBox = static_cast<CheckBox*>(sender);
            m_roleIconIndex = checkBox->getTag();
            if (m_selectEffect)
                m_selectEffect->setPosition(checkBox->getPosition());
        });

        CheckBox* checkBox = static_cast<CheckBox*>(root->getChildByName("CheckBox_s1"));
        if (checkBox)
        {
            checkBox->setTag(0);
            checkBox->addEventListener(selectIconCallback);
        }
        checkBox = static_cast<CheckBox*>(root->getChildByName("CheckBox_s2"));
        if (checkBox)
        {
            checkBox->setTag(1);
            checkBox->addEventListener(selectIconCallback);
        }
        checkBox = static_cast<CheckBox*>(root->getChildByName("CheckBox_s3"));
        if (checkBox)
        {
            checkBox->setTag(2);
            checkBox->addEventListener(selectIconCallback);
        }

        m_manCheckBox = static_cast<CheckBox*>(root->getChildByName("ManCheckBox"));
        m_manCheckBox->addEventListener([this](Ref* sender,CheckBox::EventType type)
        {
            switch (type)
            {
            case CheckBox::EventType::SELECTED:
                {
					// 设置为男性角色
					SetSex(Sex_Man);
                }break;
            case CheckBox::EventType::UNSELECTED:
                {
					// 设置为女性角色
					SetSex(Sex_Woman);
                }break;
            default:
                break;
            }
        });
       
        m_wumanCheckBox = static_cast<CheckBox*>(root->getChildByName("WumanCheckBox"));
        m_wumanCheckBox->addEventListener([this](Ref* sender,CheckBox::EventType type)
        {
            switch (type)
            {
            case CheckBox::EventType::SELECTED:
                {
                    // 设置为女性角色
                    SetSex(Sex_Woman);
                }break;
            case CheckBox::EventType::UNSELECTED:
                {
					// 设置为男性角色
					SetSex(Sex_Man);
                }break;
            default:
                break;
            }
        });

        m_bodyAImg = static_cast<ImageView*>(root->getChildByName("BodyAImage"));
        m_bodyBImg = static_cast<ImageView*>(root->getChildByName("BodyBImage"));

        //>添加莲花上面的特效.
        CCParticleSystemQuad* system = CCParticleSystemQuad::create("Image/lianhua.plist");
        system->setTextureWithRect(CCTextureCache::sharedTextureCache()->addImage("Image/lianhua.png"),CCRectMake(0,0,32,32));
        Vec2 pos = m_bodyAImg->getPosition(); pos.y -= 100;
        system->setPosition(pos);
        m_Root->addChild(system);

        SetSex(Sex_Woman);
        SetDefaultHeadIndex();

        addChild(root);
        bRet = true;
    } while (0);

    return bRet;
}

const string& UICreateRole::GetHeadIconPath(RoleSex sex, int iconIndex)
{
    static string emptyStr;
    emptyStr.clear();
    if (sex == Sex_Man)
    {
        if (iconIndex < MaxHeadIndex)
        {
            return emptyStr.assign(IconTexturePath[Sex_Man][iconIndex]);
        }
    }
    else
    {
        if (iconIndex < MaxHeadIndex)
        {
            return emptyStr.assign(IconTexturePath[Sex_Woman][iconIndex]);
        }
    }
    return emptyStr;
}

void UICreateRole::SetSex(RoleSex sex)
{
    

    if (sex == Sex_Man)
    {
        // 男性角色
        m_bodyAImg->setVisible(true);
        m_bodyBImg->setVisible(false);
        if (!m_manCheckBox->isSelected())
        {
            m_manCheckBox->setSelected(true);
        }
        if (m_wumanCheckBox->isSelected())
        {
            m_wumanCheckBox->setSelected(false);
        }

        for (int i = 0; i < MaxHeadIndex; ++i)
            m_heads[i]->setTexture(IconTexturePath[Sex_Man][i]);
    }
    else if (sex ==Sex_Woman)
    {
        // 女性角色
        m_bodyAImg->setVisible(false);
        m_bodyBImg->setVisible(true);
        if (!m_wumanCheckBox->isSelected())
        {
            m_wumanCheckBox->setSelected(true);
        }
        if (m_manCheckBox->isSelected())
        {
            m_manCheckBox->setSelected(false);
        }

        for (int i = 0; i < MaxHeadIndex; ++i)
            m_heads[i]->setTexture(IconTexturePath[Sex_Woman][i]);
    }
	m_roleSex = sex;
}

const string& UICreateRole::GetRoleName() const
{
    return m_tfRoleName->getString();
}

unsigned char UICreateRole::GetHeadIconIndex()
{
    return m_roleIconIndex;
}


void UICreateRole::SetDefaultHeadIndex()
{
    if (m_selectEffect == NULL)
    {
        m_selectEffect = g_ResManager->CreateSprite(8);
        m_roleIconIndex = 1;
        CheckBox* checkBox = static_cast<CheckBox*>(m_Root->getChildByName("CheckBox_s2"));
        if (checkBox && m_selectEffect)
        {
            checkBox->getParent()->addChild(m_selectEffect);
            m_selectEffect->setPosition(checkBox->getPosition());
        }
    }
}
