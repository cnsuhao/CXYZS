#pragma once

#include <map>

typedef struct WMsgMapItem_t{     int id;    const char* const msgStr; }WMsgMapItem;extern WMsgMapItem gWMsgData[];extern std::map<int, const char*> gWMsgMap;const char* TranslateWMsg(int id);