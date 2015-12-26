#include "ChatSystem.h"
#include "CtrlRichText.h"
#include "UIChat.h"
#include "BaseFunction.h"
#include "CSVFileManager.h"
#include "StackStream.h"
#include "NetManager.h"
#include "LogicManager.h"
#include "UIManager.h"
#include "UIChat.h"
#include "UIMain.h"


ChatSystem g_ChatSystem;

ChatSystem::ChatSystem()
{
}

ChatSystem::~ChatSystem()
{

}

bool ChatSystem::Init()
{
    return true;
}



void ChatSystem::Shutdown()
{

}

bool ChatSystem::AddMessage(const ChatMessage* chatMessage)
{
    if (!(chatMessage && !chatMessage->msg.empty()))
    {
        CCASSERT(0, "");
        return false;
    }
    Chat_Channel channel = chatMessage->channel;
    switch (channel)
    {
    case Channel_World:
        _worldMsg.push_back(chatMessage);
        break;
    case Channel_SingleChat:
        break;
    case Channel_Near:
        _nearMsg.push_back(chatMessage);
        break;
    case Channel_Guild:
        _guildMsg.push_back(chatMessage);
        break;
    case Channel_Team:
        _teamMsg.push_back(chatMessage);
        break;
    case Channel_System:
        _systemMsg.push_back(chatMessage);
        break;
    default:
        break;
    }

    UIChat *pUIChat = (UIChat*)g_UIManager->GetUI(UI_CHAT);
    if (pUIChat)
    {
        pUIChat->OnAddMsg(chatMessage);
    }

    //>更新至界面
    UIMain *pUIMain = (UIMain*)g_UIManager->GetUI(UI_MAIN);
    if (pUIMain)
    {
        pUIMain->UpdateChat(chatMessage->channel,chatMessage->senderName,chatMessage->msg);
    }

    return true;
}

void ChatSystem::RemovMessage(const ChatMessage* chatMessage)
{
    std::vector<const ChatMessage*>* containerPtr = NULL;
    if (chatMessage)
    {
        Chat_Channel channel = chatMessage->channel;
        switch (channel)
        {
        case Channel_World:
            containerPtr = &_worldMsg;
            break;
        case Channel_SingleChat:
            break;
        case Channel_Near:
            containerPtr = &_nearMsg;
            break;
        case Channel_Guild:
            containerPtr = &_guildMsg;
            break;
        case Channel_Team:
            containerPtr = &_teamMsg;
            break;
        case Channel_System:
            containerPtr = &_systemMsg;
            break;
        default:
            break;
        }

        if (containerPtr)
        {
            std::vector<const ChatMessage*>& container = *containerPtr;
            int size = container.size();
            for (auto it = container.begin(); it != container.end(); ++it)
            {
                if(*it == chatMessage)
                {
                    container.erase(it);
                    break;
                }
            }
        }
    }
}

void ChatSystem::SendChatMsg(const std::string& content, Chat_Channel channel, std::vector<int64>* appendixItems)
{
    std::string sendContent = BaseFunction::u2a(content);

    StackStream<> charMsgStream;
    MsgChat chatMsg(ID_C2G, g_LogicManager->m_Hero->m_GUID, g_LogicManager->m_Hero->m_Name.c_str(), channel);
    chatMsg.msgSize = BaseFunction::u2a(content).size();
    chatMsg.itemCount = 0;
    charMsgStream.Wirte(&chatMsg, sizeof(chatMsg));
    charMsgStream.Wirte((void*)sendContent.c_str(), sendContent.size());
    MsgChat* pChatMsg = (MsgChat*)charMsgStream.Get();
    pChatMsg->Len = charMsgStream.GetSize();
    g_NetManager->SendMsg(*pChatMsg);
}

const ChatMessage* ChatSystem::GetMsgOfIndex(int type, unsigned int index)
{
    const ChatMessage* chatMessage = NULL;

    Chat_Channel channel = (Chat_Channel)type;
    switch (channel)
    {
    case Channel_World:
        {
            chatMessage = (_worldMsg.size() > index) ? _worldMsg[index] : NULL; 
        }break;
    case Channel_System:
        {
            chatMessage = (_systemMsg.size() > index) ? _systemMsg[index] : NULL; 
        }break;
    case Channel_SingleChat:
        {

        }break;
    case Channel_Team:
        {
            chatMessage = (_teamMsg.size() > index) ? _teamMsg[index] : NULL; 
        }break;
    case Channel_Near:
        {
            chatMessage = (_nearMsg.size() > index) ? _nearMsg[index] : NULL; 
        }break;
    case Channel_Guild:
        {
            chatMessage = (_guildMsg.size() > index) ? _guildMsg[index] : NULL; 
        }break;
    default:
        break;
    }
    return chatMessage;
}

unsigned int ChatSystem::GetMsgCount(int type)
{
    switch (type)
    {
    case Channel_World:
        {
            return _worldMsg.size();
        }break;
    case Channel_System:
        {
            return _systemMsg.size(); 
        }break;
    case Channel_SingleChat:
        {

        }break;
    case Channel_Team:
        {
            return _teamMsg.size(); 
        }break;
    case Channel_Near:
        {
            return _nearMsg.size(); 
        }break;
    case Channel_Guild:
        {
            return _guildMsg.size(); 
        }break;
    default:
        break;
    }
    return 0;
}

void ChatSystem::AppendixChannelAndUser(const ChatMessage* chatMessage, RichItemSet& outItems)
{
    Chat_Channel channel = chatMessage->channel;
    const TextInfo* info = NULL;
    switch (channel)
    {
    case Channel_World:
        info = g_CSVFileManager->GetTextByID(15); //>世界
        if (info){
            outItems.push_back(RichItemText::create(-1, Color3B::YELLOW, BaseFunction::a2u(info->strContent).c_str())); 
        }
        break;
    case Channel_SingleChat:
        info = g_CSVFileManager->GetTextByID(17); //>私聊
        if (info){
            outItems.push_back(RichItemText::create(-1, Color3B::MAGENTA, BaseFunction::a2u(info->strContent).c_str())); 
        }
        break;
    case Channel_Near:
        info = g_CSVFileManager->GetTextByID(20); //>当前
        if (info){
            outItems.push_back(RichItemText::create(-1, Color3B::WHITE, BaseFunction::a2u(info->strContent).c_str())); 
        }
        break;
    case Channel_Guild:
        info = g_CSVFileManager->GetTextByID(18); //>帮派
        if (info){
            outItems.push_back(RichItemText::create(-1, Color3B::ORANGE, BaseFunction::a2u(info->strContent).c_str())); 
        }
        break;
    case Channel_Team:
        info = g_CSVFileManager->GetTextByID(19); //>队伍
        if (info){
            outItems.push_back(RichItemText::create(-1, Color3B::BLUE, BaseFunction::a2u(info->strContent).c_str())); 
        }
        break;
    case Channel_System:
        info = g_CSVFileManager->GetTextByID(16); // 系统
        if(info){
            outItems.push_back(RichItemText::create(-1, Color3B::RED, BaseFunction::a2u(info->strContent).c_str())); 
        }
        break;
    default:
        break;
    }

    outItems.push_back(RichItemText::create(-1, Color3B::GREEN,BaseFunction::a2u(chatMessage->senderName).c_str(),Color4B::GREEN));
}

void ChatSystem::Parse(const ChatMessage* chatMessage, RichItemSet& outItems)
{
    if (!chatMessage->appendix.empty())
    {
        RichItem* newItem = nullptr;
        static string token("|");
        unsigned int index = 0;
        unsigned int last_index = -1;
        const string& msg = chatMessage->msg;
        size_t pos = msg.find(token, 0);
        bool first = true;
        while (pos != string::npos)
        {
            ++last_index;
            string text = msg.substr(last_index, pos - last_index);
            if (!text.empty() && token != text)
            {
                outItems.push_back(RichItemText::create(-1, Color3B::WHITE, BaseFunction::a2u(text.c_str()).c_str()));
            }

            string AppendixName("[");
            AppendixName.append("LINK ITEM NAME");
            AppendixName.append("]");
            outItems.push_back(RichItemText::create(-1, Color3B::MAGENTA, BaseFunction::a2u(AppendixName.c_str()).c_str(), Color4B::MAGENTA));
            
            last_index = pos;
            index = pos + 1;

            pos = msg.find(token, index);
        }
        if (index < msg.length())
        {
            string text = msg.substr(index);
            if (!text.empty())
            {
                outItems.push_back(RichItemText::create(-1, Color3B::WHITE, BaseFunction::a2u(text.c_str()).c_str()));
            }
        }
    }
    else
    {
        outItems.push_back(RichItemText::create(-1, Color3B::WHITE, BaseFunction::a2u(chatMessage->msg.c_str()).c_str()));
    }
}

