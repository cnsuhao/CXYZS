#include "stdafx.h"
#include "GameServer.h"
//#include "Logic/GMap.h"
//#include "Logic/GPlayer.h"
#include "LogManage.h"
#include "StackStream.h"
#include <regex>
#include <iostream>
#include "FilteWord.h"
//static uint32 gRandomSeed = 3;
//
//int32 Rand()
//{
//    gRandomSeed = gRandomSeed * 214013 + 2531011;
//    return (gRandomSeed >> 16) & 32767; 
//}
//inline float Random() { return Rand() / 32768.0f; }
//inline float Random(float min, float max) { return Rand() * (max - min) / 32767.0f + min; }
//
//bool IsValidEmail(const char* email)
//{
//    const std::regex patten("\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*");
//
//    return std::regex_match(email, patten);
//}
//
//void TestEmail()
//{
//    for (;;)
//    {
//        std::string inpute;
//        std::cin  >> inpute;
//
//        if(IsValidEmail(inpute.c_str()))
//        {
//            std::cout << "有效邮箱" << std::endl;
//        }
//        else
//        {
//            std::cout << "无效邮箱" << std::endl;
//        }
//    }
//}
//
//void TestMap()
//{
//    gLog.SetName("GameServer");
//    gLog.Info("start.");
//    GMap map;
//    map.Init(3200, 2000);
//    Vec2 p;
//    for (int32 i = 0; i < 2000;++i)
//    {
//        GPlayer* plr = new GPlayer();
//        if (plr)
//        {
//            p.x = Random(0, map.GetWidth() - 1.0f);
//            p.y = Random(0, map.GetHeight() - 1.0f);
//            plr->SetPosition(p);
//            map.AddPlayer(plr);
//        }
//    }
//
//    //>据格子从上到下,从左到右更新格子
//    std::vector<GEntity*> playerSet;
//    std::vector<Block*>::iterator it = map.m_Blocks.begin();
//    for (std::vector<Block*>::iterator end = map.m_Blocks.end();
//        it != end; 
//        ++it)
//    {
//        Block* block = *it;
//        uint32 count = block->players.size();
//        for (uint32 i = 0; i < count; ++i)
//        {
//            static_cast<GPlayer*>(block->players[i])->GetBroadcastPlayerSet(playerSet);
//
//            //map.GetRangeSet<GPlayer>(100.0f, block->players[i], playerSet);
//        }
//    }
//
//    gLog.Info("end.");
//}
//
//
//void Echo(FilteWord& filter, std::string& content)
//{
//    if (filter.Ok(content.c_str()))
//    {
//        printf("%s没有被过滤\n", content.c_str());
//    }
//    else
//    {
//        printf("%s被过滤\n", content.c_str());
//    }
//}
//
//void TestRegEx()
//{
//    FilteWord filter1("Data/FliterWord-Name.txt");
//
//    for (;;)
//    {
//        std::string inpute;
//        std::cin  >> inpute;
//
//        Echo(filter1, inpute);
//        std::cout << "--------->" << filter1.Replace(inpute.c_str()) << std::endl;
//    }
//}

int _tmain(int argc, _TCHAR* argv[])
{
	//服务器ID
	uint16 ServerID = 0;
	if (argc > 1)
	{
        ServerID = _tstoi(argv[1]);
    }

	g_GameServer = new GameServer;
	if (!g_GameServer)
	{
		MessageBoxA(NULL,"内存分配失败(new GameServer)", "错误", MB_OK);
		return -1;
	}

    //TestEmail();

    //TestMap();

    //TestRegEx();

	g_GameServer->Start(ServerID);
    delete g_GameServer;
    g_GameServer = NULL;

	return 0;
}
