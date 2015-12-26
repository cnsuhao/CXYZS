#include "UILogin.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "SceneManager.h"
#include "NetManager.h"
#include "CSVFileManager.h"

using namespace cocostudio;

const int cursorWidht = 6;

UILogin::UILogin():loginUI(NULL),registerUI(NULL)
{
    helpLable = NULL;
    lc_cursor = NULL;
    tf_userName = NULL;
    tf_password = NULL;
    tf_regUserName = NULL;
    tf_regPassword = NULL;
    tf_regRePassword = NULL;
    sp_canvas = NULL;

    panelLogin = NULL;
    panelAccountRecord = NULL;
    accountTemplate = NULL;
    accountSV = NULL;
}

UILogin::~UILogin()
{
    if (helpLable)
    {
        helpLable->release();
    }
    if (lc_cursor)
    {
        lc_cursor->release();
    }
    if (cursorAction)
    {
        cursorAction->release();
    }
}

void CalcPosition(TextField* textField, Label* helpLable, LayerColor* cursor)
{
    const string& content = textField->getString();
    int fontSize = textField->getFontSize();
    helpLable->setSystemFontSize(fontSize);
    if (content.length() == 0)
    {
        const string& holder =  textField->getPlaceHolder();
        helpLable->setString(holder);
        const Size& lableSize = helpLable->getContentSize();
        cursor->setPosition(-6, (textField->getContentSize().height - lableSize.height) / 2.0f);
    }else
    {
        helpLable->setString(content);
        const Size& lableSize = helpLable->getContentSize();
        cursor->changeHeight(lableSize.height);
        cursor->setPosition(lableSize.width, (textField->getContentSize().height - lableSize.height) / 2.0f);
    }
}

bool UILogin::init()
{
    bool bRet = false;
    do 
    {

        CC_BREAK_IF(!UIBase::init());
        loginUI = CSLoader::createNode("UI/login.csb");	
        registerUI = CSLoader::createNode("UI/Registered.csb");
        if (loginUI == NULL || registerUI == NULL)
        {
            return false;
        }
        sp_canvas = Sprite::create("UI/Login/LoginCanvas.jpg");
        //sp_canvas->setPosition(g_ScreenHalfSize.x, g_ScreenHalfSize.y);
        sp_canvas->setAnchorPoint(Vec2::ZERO);
        addChild(sp_canvas);
        addChild(loginUI);
        addChild(registerUI);
        loginUI = loginUI->getChildByName("Node_1");
        registerUI = registerUI->getChildByName("Node_1");
        registerUI->setVisible(false);

        helpLable = Label::createWithSystemFont("","",10);
        helpLable->retain();
        lc_cursor = LayerColor::create(Color4B::WHITE ,cursorWidht,20);
        lc_cursor->retain();
        cursorAction = RepeatForever::create(Sequence::create(FadeOut::create(0.3f), FadeIn::create(0.3f), NULL));
        cursorAction->retain();
        // 输入框事件回调
        TextField::ccTextFieldCallback eventListener = [this](Ref* _sender, TextField::EventType type)
        {
            switch (type)
            {
            case cocos2d::ui::TextField::EventType::ATTACH_WITH_IME:
                {
                    TextField* textField = dynamic_cast<TextField*>(_sender);
                    CalcPosition(textField, helpLable, lc_cursor);
                    lc_cursor->removeFromParent();
                    lc_cursor->runAction(cursorAction);
                    textField->addChild(lc_cursor);
                }break;
            case cocos2d::ui::TextField::EventType::DETACH_WITH_IME:
                {
                    TextField* textField = dynamic_cast<TextField*>(_sender);
                    lc_cursor->removeFromParent();
                    if (textField->getName() == "TextFieldAccount" || textField->getName() == "TextFieldRegAccount")
                    {
                        if (BaseFunction::IsValidAccount(textField->getString().c_str()) <= 0)
                        {
                            textField->setTextColor(Color4B::RED);
                        }
                        else
                        {
                            textField->setTextColor(Color4B::WHITE);
                        }
                    }
                    else if (textField->getName() == "TextFieldPassword" || textField->getName() == "TextFieldRegPassword" || textField->getName() == "TextFieldRegRePassword")
                    {
                        if (BaseFunction::IsValidAccount(textField->getString().c_str()) <= 0)
                        {
                            textField->setTextColor(Color4B::RED);
                        }
                        else
                        {
                            textField->setTextColor(Color4B::WHITE);
                        }
                    }
                }break;
            case cocos2d::ui::TextField::EventType::INSERT_TEXT:
                {
                    TextField* textField = dynamic_cast<TextField*>(_sender);
                    CalcPosition(textField, helpLable, lc_cursor);
                }break;
            case cocos2d::ui::TextField::EventType::DELETE_BACKWARD:
                {
                    TextField* textField = dynamic_cast<TextField*>(_sender);
                    CalcPosition(textField, helpLable, lc_cursor);
                }break;
            default:
                break;
            }
        };

		//读取当前记住的密码
		string strUserName = UserDefault::getInstance()->getStringForKey("username");
		string strUserPW = UserDefault::getInstance()->getStringForKey("userpassword");
		strUserPW = BaseFunction::StrCode(strUserPW);

        Button* btn = nullptr;
        Layout* cliper = static_cast<Layout*>(loginUI->getChildByName("PanelCliper"));
        panelLogin = static_cast<Layout*>(cliper->getChildByName("PanelLogin"));
        panelAccountRecord = static_cast<Layout*>(cliper->getChildByName("PanelAccountRecord"));
        accountTemplate = static_cast<Layout*>(loginUI->getChildByName("AccountItem"));
        accountSV = static_cast<cc2d::ui::ScrollView*>(panelAccountRecord->getChildByName("SV_Account"));
        accountSV->setSwallowTouches(false);

        string registeredUsers = UserDefault::getInstance()->getStringForKey("registeredUsers");
        if (!registeredUsers.empty())
        {
            std::vector<string> arrayStr;
            BaseFunction::splitStr(registeredUsers,"|", &arrayStr);
            if (arrayStr.size() > 0 && (arrayStr.size() % 2) == 0)
            {
                for (size_t i = 0; i < arrayStr.size(); i+= 2)
                {
                    AddAcountItem(accountSV,accountTemplate->clone(), arrayStr[i].c_str());
                }
            }
        }

        accountSV->setLayoutType(Layout::Type::VERTICAL);
        accountSV->forceDoLayout();
        Size ContainerSize = accountSV->getInnerContainerSize();
        ContainerSize.height = accountSV->getChildrenCount() * accountTemplate->getContentSize().height;
        accountSV->setInnerContainerSize(ContainerSize);
        accountTemplate->setVisible(false);

        btn = static_cast<Button*>(panelLogin->getChildByName("ButtonRegister"));	
        if (btn)
        {
            // 注册账号
            btn->addClickEventListener([this](Ref* _sender)
            {
                ReplaceUI(loginUI, registerUI);
            });
        }

        btn = static_cast<Button*>(panelLogin->getChildByName("ButtonLogin"));	
        if (btn)
        {
            // 登陆游戏
            btn->addClickEventListener([this](Ref* _sender)
            {
                OnLogin();
            });
        }
        btn = static_cast<Button*>(loginUI->getChildByName("ButtonList"));
        if (btn)
        {
            btn->addClickEventListener([this](Ref* _sender)
            {
                PlaySwitchAnim();
            });
        }

        tf_userName = (TextField*)loginUI->getChildByName("TextFieldAccount");
        if (tf_userName)
        {
            // 账号
            tf_userName->setTextVerticalAlignment(TextVAlignment::CENTER);
            tf_userName->addEventListener(eventListener);
			//记住账号
			tf_userName->setString(strUserName);
        }
        tf_password = static_cast<TextField*>(panelLogin->getChildByName("TextFieldPassword"));
        if (tf_password)
        {
            // 密码
            tf_password->setTextVerticalAlignment(TextVAlignment::CENTER);
            tf_password->addEventListener(eventListener);
			//记住密码
			tf_password->setString(strUserPW);
        }

        btn = static_cast<Button*>(registerUI->getChildByName("ButtonReturn"));	
        if (btn)
        {
            // 返回登录
            btn->addClickEventListener([this](Ref* _sender)
            {
                ReplaceUI(registerUI, loginUI);
            });
        }

        btn = static_cast<Button*>(registerUI->getChildByName("ButtonSureRegister"));	
        if (btn)
        {
            // 确定注册
            btn->addClickEventListener([this](Ref* _sender)
            {
                OnConfirmRegister();
            });
        }

        tf_regUserName = static_cast<TextField*>(registerUI->getChildByName("TextFieldRegAccount"));	
        if (tf_regUserName)
        {
            // 注册账号
            tf_regUserName->setTextVerticalAlignment(TextVAlignment::CENTER);
            tf_regUserName->addEventListener(eventListener);
        }

        tf_regPassword = static_cast<TextField*>(registerUI->getChildByName("TextFieldRegPassword"));	
        if (tf_regPassword)
        {
            // 注册密码
            tf_regPassword->setTextVerticalAlignment(TextVAlignment::CENTER);
            tf_regPassword->addEventListener(eventListener);
        }

        tf_regRePassword = static_cast<TextField*>(registerUI->getChildByName("TextFieldRegRePassword"));
        if (tf_regRePassword)
        {
            // 确认密码
            tf_regRePassword->setTextVerticalAlignment(TextVAlignment::CENTER);
            tf_regRePassword->addEventListener(eventListener);
        }
        bRet = true;
    } while (0);

    return bRet;
}

const char* UILogin::GetUserName() const
{
    if (tf_userName)
    {
        return tf_userName->getString().c_str();
    }

    return NULL;
}

const char* UILogin::GetPassword() const
{
    if (tf_password)
    {
        return tf_password->getString().c_str();
    }

    return NULL;
}

const char* UILogin::GetRegisterUserName() const
{
    if (tf_regUserName)
    {
        return tf_regUserName->getString().c_str();
    }

    return NULL;
}

const char* UILogin::GetRegisterPassword() const
{
    if (tf_regPassword)
    {
        return tf_regPassword->getString().c_str();
    }

    return NULL;
}

const char* UILogin::GetRegsiterRePassword() const
{
    if (tf_regRePassword)
    {
        return tf_regRePassword->getString().c_str();
    }
    return NULL;
}

void UILogin::SetUserName(const char* name)
{
    if (name)
    {
        return tf_userName->setString(name);
    }
}

void UILogin::RecordNewRegisterUserName(const char* newUser, const char* pw)
{
    CCASSERT(newUser != NULL && pw != NULL, "newUser != NULL && pw != NULL");
    string User(newUser);
    string password(pw);
    password = BaseFunction::StrCode(password);

    string registeredUsers = UserDefault::getInstance()->getStringForKey("registeredUsers");
    if (!registeredUsers.empty())
    {
        std::vector<string> arrayStr;
        BaseFunction::splitStr(registeredUsers,"|", &arrayStr);
        CCASSERT(arrayStr.size() > 0 && (arrayStr.size() % 2) == 0, "");
        if (arrayStr.size() > 0 && (arrayStr.size() % 2) == 0)
        {
            for (size_t i = 0; i < arrayStr.size(); i+= 2)
            {
                if (arrayStr[i] == User)
                {
                    return;
                }
            }
        }
    }

    if (!registeredUsers.empty())
        registeredUsers.append("|");

    registeredUsers.append(User);
    registeredUsers.append("|");
    registeredUsers.append(password);
    UserDefault::getInstance()->setStringForKey("registeredUsers", registeredUsers);
}

void UILogin::DeleteRegisterUserName(const std::string& user)
{
    string registeredUsers = UserDefault::getInstance()->getStringForKey("registeredUsers");
    std::vector<string> arrayStr;
    BaseFunction::splitStr(registeredUsers,"|", &arrayStr);
    for (size_t i = 0; i < arrayStr.size(); i+= 2)
    {
        if (arrayStr[i] == user)
        {
            arrayStr.erase(arrayStr.begin() + i+1);
            arrayStr.erase(arrayStr.begin() + i);
            break;
        }
    }
    string newRegisteredUsers;
    for (size_t j = 0; j < arrayStr.size(); j+=2)
    {
        if(!newRegisteredUsers.empty())
            newRegisteredUsers.append("|");
        newRegisteredUsers.append(arrayStr[j]);
        newRegisteredUsers.append("|");
        newRegisteredUsers.append(arrayStr[j+1]);
    }

    UserDefault::getInstance()->setStringForKey("registeredUsers", newRegisteredUsers);
}

void UILogin::OnLogin()
{
    const char* userName = GetUserName();
    const char* password = GetPassword();

	if(userName[0] == 0 || password[0] == 0)
		return;

	MsgRegiser_Login msg((char*)userName, (char*)password, 0, Msg_Account_Login);
	g_NetManager->SendMsg(msg);
}

void UILogin::OnConfirmRegister()
{
    const char* regName = GetRegisterUserName();
    const char* regPassword = GetRegisterPassword();
    const char* regRePassword = GetRegsiterRePassword();
    const TextInfo* regFaildText = g_CSVFileManager->GetTextByID(4);
    const TextInfo* regNameEmptyText = g_CSVFileManager->GetTextByID(21);
    const TextInfo* regNameInvailedText = g_CSVFileManager->GetTextByID(22);
    const TextInfo* regNameInvailedText1 = g_CSVFileManager->GetTextByID(23);
    const TextInfo* regNameInvailedText2 = g_CSVFileManager->GetTextByID(24);
    const TextInfo* regNameInvailedText3 = g_CSVFileManager->GetTextByID(24);
    const TextInfo* regPasswordEmptyText = g_CSVFileManager->GetTextByID(26);
    const TextInfo* regPasswordInvailedText = g_CSVFileManager->GetTextByID(27);
    const TextInfo* regPasswordInvailedText1 = g_CSVFileManager->GetTextByID(28);
    const TextInfo* regPasswordInvailedText2 = g_CSVFileManager->GetTextByID(29);
    const TextInfo* regPasswordInvailedText3 = g_CSVFileManager->GetTextByID(29);
    const TextInfo* regRePasswordEmptyText = g_CSVFileManager->GetTextByID(31);
    const TextInfo* regPasswordDifferentText = g_CSVFileManager->GetTextByID(32);

    // TODO: 修改中文字符串通过配置来读取
	if(regName[0] == 0)
    {
        MessageBox(regNameEmptyText ? regNameEmptyText->strContent.c_str() : "用户名未填写.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    }

    int code = BaseFunction::IsValidAccount(regName);
    switch (code)
    {
    case 0:
        MessageBox(regNameInvailedText ? regNameInvailedText->strContent.c_str() : "用户名只能为英文字母或数字组成.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    case -1:
        MessageBox(regNameInvailedText1 ? regNameInvailedText1->strContent.c_str() : "用户名含有非法字符.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    case -2:
        MessageBox(regNameInvailedText2 ? regNameInvailedText2->strContent.c_str() : "用户名长度不能小于6个字符.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    case -3:
        MessageBox(regNameInvailedText3 ? regNameInvailedText3->strContent.c_str() : "用户名长度不能大于16个字符.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    default:
        break;
    }

    if (regPassword[0] == 0)
    {
        MessageBox(regPasswordEmptyText ? regPasswordEmptyText->strContent.c_str() : "密码未填写", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    }

    code = BaseFunction::IsValidPassword(regPassword);
    switch (code)
    {
    case 0:
        MessageBox(regPasswordInvailedText ? regPasswordInvailedText->strContent.c_str() : "密码只能为英文字母或数字组成.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    case -1:
        MessageBox(regPasswordInvailedText1 ? regPasswordInvailedText1->strContent.c_str() : "密码含有非法字符.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    case -2:
        MessageBox(regPasswordInvailedText2 ? regPasswordInvailedText2->strContent.c_str() : "密码长度不能小于6个字符.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    case -3:
        MessageBox(regPasswordInvailedText3 ? regPasswordInvailedText3->strContent.c_str() : "密码长度不能大于12个字符.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    default:
        break;
    }

    if (regRePassword[0] == 0)
    {
        MessageBox(regRePasswordEmptyText ? regRePasswordEmptyText->strContent.c_str() : "确认密码未填写.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    }

    const char* str1 = regPassword;
    const char* str2 = regRePassword;
    while (*str1)
    {
        if(*str1++ != *str2++)
        {
            MessageBox(regPasswordDifferentText ? regPasswordDifferentText->strContent.c_str() : "两次此输入密码不一致.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
            return;
        }
    }
    if (*str1 != *str2)
    {
        MessageBox(regPasswordDifferentText ? regPasswordDifferentText->strContent.c_str() : "两次此输入密码不一致.", regFaildText ? regFaildText->strContent.c_str() : "注册失败");
        return;
    }

	
	MsgRegiser_Login msg((char*)regName, (char*)regPassword, 0, Msg_Regiser_Account);
	g_NetManager->SendMsg(msg);
}


void UILogin::ReplaceUI(cc2d::Node* oldUI, cc2d::Node* newUI)
{
    if (oldUI)
    {
        oldUI->setVisible(false);
    }
    if (newUI)
    {
        newUI->setVisible(true);
        newUI->setScale(0.7f, 0.7f);
        newUI->runAction(EaseElasticOut::create(ScaleTo::create(1.0f, 1.0f, 1.0f)));
    }
}

void UILogin::AddAcountItem(cc2d::ui::ScrollView* sv, cc2d::ui::Widget* widget, const char* itemName)
{
    cc2d::ui::Layout* newLayout = static_cast<cc2d::ui::Layout*>(widget);
    newLayout->setVisible(true);
    newLayout->setTouchEnabled(true);
    Text* text =  static_cast<cc2d::ui::Text*>(newLayout->getChildByName("Account"));
    if (text)
    {
        text->setString(itemName);
    }
    newLayout->addClickEventListener([this](Ref* _sender)
    {
        Layout* layout = static_cast<Layout*>(_sender);
        Text* text =  static_cast<cc2d::ui::Text*>(layout->getChildByName("Account"));
        if (text)
        {
            SetUserName(text->getString().c_str());
        }
        
        PlaySwitchAnim();
    });
    

    Button* button =  static_cast<cc2dui::Button*>(newLayout->getChildByName("ButtonClose"));
    if (button)
    {
        button->setUserData(text);
        button->addClickEventListener([this](Ref* _sender)
        {
            Button* button = static_cast<Button*>(_sender);
            Text* text =  static_cast<cc2d::ui::Text*>(button->getUserData());
            DeleteRegisterUserName(text->getString());
            accountSV->removeChild(button->getParent());
            accountSV->forceDoLayout();
            Size ContainerSize = accountSV->getInnerContainerSize();
            ContainerSize.height = accountSV->getChildrenCount() * accountTemplate->getContentSize().height;
            accountSV->setInnerContainerSize(ContainerSize);
        });
    }

    sv->addChild(newLayout);
}

void UILogin::PlaySwitchAnim()
{
    if (panelLogin->getPosition().x == 0)
    {
        panelLogin->runAction(EaseSineOut::create(MoveTo::create(0.3f,Vec2(-panelLogin->getContentSize().width, 0))));
        panelAccountRecord->runAction(EaseSineOut::create(MoveTo::create(0.3f,Vec2(0, 0))));
    }
    else
    {
        panelLogin->runAction(EaseSineOut::create(MoveTo::create(0.3f,Vec2(0, 0))));
        panelAccountRecord->runAction(EaseSineOut::create(MoveTo::create(0.3f,Vec2(panelLogin->getContentSize().width, 0))));
    }
}

//切换注册窗口与登录窗口
void UILogin::switchWindows(bool bReg)
{
	if(bReg)
	{
        ReplaceUI(loginUI, registerUI);
	}
	else
	{
        ReplaceUI(registerUI, loginUI);
	}
}