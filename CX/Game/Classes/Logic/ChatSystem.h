/********************************************************************
created:	2015/3/13
author:		LSH
desc:		聊天系统
*********************************************************************/

#ifndef __CHATSYSTEM_H__
#define __CHATSYSTEM_H__

#include "GameDef.h"
#include "ClientDef.h"

struct ChatAppendix
{
    unsigned int uniqueId;      // 唯一ID(数据库id+尾位类型id)
};

struct ChatMessage
{
    Chat_Channel channel;   // 世界/系统/附近玩具
    unsigned int senderId;
    std::string senderName;
    std::string msg;
    std::vector<int64> appendix;
};

class ChatSystem
{
public:
	ChatSystem();
	~ChatSystem();

    bool Init();
    void Shutdown();


    bool AddMessage(const ChatMessage* chatMessage);
    void RemovMessage(const ChatMessage* chatMessage);

    void SendChatMsg(const std::string& content, Chat_Channel channel, std::vector<int64>* appendixItems);

    const ChatMessage* GetMsgOfIndex(int type, unsigned int index);
    unsigned int GetMsgCount(int type);

    typedef std::vector<class RichItem*> RichItemSet;    
    void AppendixChannelAndUser(const ChatMessage* chatMessage, RichItemSet& outItems);
    void Parse(const ChatMessage* chatMessage, RichItemSet& outItems);
protected:
    std::vector<const ChatMessage*> _worldMsg;
    std::vector<const ChatMessage*> _teamMsg;
    std::vector<const ChatMessage*> _nearMsg;
    std::vector<const ChatMessage*> _guildMsg;
    std::vector<const ChatMessage*> _systemMsg;
};

extern ChatSystem g_ChatSystem;

#endif	//__CHATSYSTEM_H__