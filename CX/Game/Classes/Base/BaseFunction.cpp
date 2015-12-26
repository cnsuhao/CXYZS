#include "BaseFunction.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#pragma comment(lib, "Netapi32.lib")
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "OCAPI.h"
#endif

#include <regex>

const int kMinSecond = 60;
const int kHourSecond = 60 * kMinSecond;
const int kDaySecond = 24 * kHourSecond;

std::string BaseFunction::StrCode(std::string &str)
{
	const char *temp = str.c_str();
	std::string outbuf;
	for (size_t i = 0; i < str.size(); i++)
	{
		outbuf.push_back(temp[i]^KEY);
	}
	return outbuf;
}

int BaseFunction::code_convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen) 
{   
	iconv_t cd;     
	const char *temp = inbuf;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
	const char **pin = &temp;
#else
	char **pin = (char **)&temp;
#endif     
	char **pout = &outbuf;    
	memset(outbuf,0,outlen);  


	cd = iconv_open(to_charset,from_charset);     
	if(cd==0) return -1;     
	if(iconv(cd,pin,&inlen,pout,&outlen)==-1) 
		return -1;    
	iconv_close(cd); 
	return 0; 
}

/*UTF8 to GB2312*/
std::string BaseFunction::u2a(const char *inbuf) 
{    
	std::string strRet;  
	size_t inlen = strlen(inbuf);    
	char * outbuf = new char[inlen * 2 + 2];   

	if(code_convert("utf-8", "gb18030", inbuf, inlen, outbuf, inlen * 2 + 2) == 0)    
	{        
		strRet = outbuf;    
	}    
	delete [] outbuf;  
	return strRet;
}

/*GB2312 to UTF8*/
std::string BaseFunction::a2u(const char *inbuf) 
{     
	std::string strRet;
	size_t inlen = strlen(inbuf);   
	char * outbuf = new char[inlen * 2 + 2];  
	if(code_convert("gb18030", "utf-8", inbuf, inlen, outbuf, inlen * 2 + 2) == 0)    
	{     
		strRet = outbuf; 
	}   
	delete [] outbuf;  
	return strRet; 
}

//转换成时:分:秒格式
std::string BaseFunction::GetHHMMSSStr(long long millisecond)
{
//  	TIME_ZONE_INFORMATION tzi;
// 	int zone = tzi.Bias/ -60; //时区，如果是中国标准时间则得到8
// 	CCLOG("FFFFF zone=%d",zone);
	millisecond = millisecond+8*3600*1000;
	long long sencond = millisecond / 1000;
	sencond = sencond % kDaySecond;

	const int kHour = sencond / kHourSecond;
	sencond -= kHour * kHourSecond;
	const int kMinute = sencond / kMinSecond;
	sencond -= kMinute * kMinSecond;
	char str[64] = {0};
	sprintf(str, "%02d:%02d:%02d", kHour, kMinute, sencond);
	return str;

// 	char str[64] = {0};
// 	time_t timep = sencond;
// 	tm* timeStuct = gmtime(&timep);
// 	sprintf(str, "%02d/%02d/%02d/%02d/%02d/%02d",timeStuct->tm_year + 1900,timeStuct->tm_mon + 1,timeStuct->tm_mday,timeStuct->tm_hour,timeStuct->tm_min,timeStuct->tm_sec);
// 	__String* tempb = __String::createWithFormat("%s",str);
// 	return tempb->getCString();

}

void BaseFunction::splitStr(const std::string &str, const std::string &sepStr, std::vector<std::string> *varVect)
{
	assert(!sepStr.empty());
	assert(varVect != NULL);
	varVect->clear();
	for (std::string::size_type curIndex = 0; curIndex < str.size(); ) {
		std::string::size_type index = str.find(sepStr, curIndex);
		std::string subStr;
		if (index == str.npos) {
			subStr = str.substr(curIndex);
			curIndex = str.npos;
		} else {
			subStr = str.substr(curIndex, index - curIndex);
			curIndex = index + sepStr.size();
		}
		varVect->push_back(subStr);
	}
}

void BaseFunction::splitStr(const std::string &str, const std::string &sepPrefix, const std::string &sepSuffix, std::vector<std::string> *varVect)
{
	assert(!sepPrefix.empty());
	assert(!sepSuffix.empty());
	assert(varVect != NULL);
	varVect->clear();
	for (std::string::size_type curIndex = 0; curIndex < str.size();) {
		std::string::size_type index1 = str.find(sepPrefix, curIndex);
		std::string::size_type index2 = str.find(sepSuffix, curIndex);
		std::string subStr;
		if (index1 == str.npos || index2 == str.npos) {
			return;
		} else {
			int dVal = index2 - index1 - sepPrefix.size();
			if (dVal > 0)
			{
				curIndex = index1 + sepPrefix.size();
				subStr = str.substr(curIndex, dVal);
				curIndex = index2 + sepSuffix.size();
			} else {
				CCLOG("error!!! check inputed sepPrefix and sepSuffix!");
				return;
			}
		}
		varVect->push_back(subStr);
	}
}

std::string BaseFunction::m_imeiStr = "";
std::string BaseFunction::getImei()
{  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		//JniMethodInfo mInfo;//定义Jni函数信息结构体
		//imeiStr = __String::createWithFormat("%d",Random(10000, 9999999))->getCString();
	}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		m_imeiStr = [[[OCAPI shareInstance] getDeviceId] UTF8String];
	}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		//GUID uuid; 
		//CoCreateGuid(&uuid); 
		//// Spit the address out 
		//char mac_addr[18]; 
		//sprintf(mac_addr,"%02X:%02X:%02X:%02X:%02X:%02X", 
		//	uuid.Data4[2],uuid.Data4[3],uuid.Data4[4], 
		//	uuid.Data4[5],uuid.Data4[6],uuid.Data4[7]); 
		//m_imeiStr = mac_addr;

		char macAddr[200];   
		NCB ncb;     
		typedef struct _ASTAT_     
		{
			ADAPTER_STATUS adapt;  
			NAME_BUFFER NameBuff[30];     
		}ASTAT, *PASTAT;     
		ASTAT Adapter;     

		typedef struct _LANA_ENUM     
		{  
			UCHAR length;  
			UCHAR lana[MAX_LANA];     
		}LANA_ENUM;     
		LANA_ENUM lana_enum;     

		UCHAR uRetCode;     
		memset(&ncb, 0, sizeof(ncb));     
		memset(&lana_enum, 0, sizeof(lana_enum));     

		ncb.ncb_command = NCBENUM;     
		ncb.ncb_buffer = (unsigned char*)&lana_enum;     
		ncb.ncb_length = sizeof(LANA_ENUM);     
		uRetCode = Netbios(&ncb);     

		for(int lana=0; lana<lana_enum.length; lana++)     
		{
			ncb.ncb_command = NCBRESET; 
			ncb.ncb_lana_num = lana_enum.lana[lana]; 
			uRetCode = Netbios(&ncb);   
			if(uRetCode == NRC_GOODRET)  
				break; 
		} 

		memset(&ncb, 0, sizeof(ncb));  
		ncb.ncb_command = NCBASTAT;  
		ncb.ncb_lana_num = lana_enum.lana[0];
		strcpy((char*)ncb.ncb_callname, "*"); 
		ncb.ncb_buffer = (unsigned char *)&Adapter;
		ncb.ncb_length = sizeof(Adapter);
		uRetCode = Netbios(&ncb);  

		sprintf(macAddr,"%02X%02X%02X%02X%02X%02X",     
			Adapter.adapt.adapter_address[0],     
			Adapter.adapt.adapter_address[1],     
			Adapter.adapt.adapter_address[2],     
			Adapter.adapt.adapter_address[3],     
			Adapter.adapt.adapter_address[4],     
			Adapter.adapt.adapter_address[5]  
		);

		m_imeiStr = macAddr;
	}
#endif

	return m_imeiStr;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
	void Java_org_cocos2dx_cpp_AppActivity_getDeviceId(JNIEnv* env, jobject thiz, jstring idStr)
	{
		/*const char* deviceId;
		deviceId = env->GetStringUTFChars(idStr, false);
		CCLog("IIIIIIIIIIIIIIIIId id=%s", deviceId);
		std::string tempId(deviceId);
		BaseFunction::m_imeiStr = tempId;*/
	}
}
#endif

bool BaseFunction::DistanceToBeIgnore(const Vec2& a, const Vec2& b)
{
    if (abs(a.x - b.x) > 10.0f || abs(a.y - b.y) > 10.0f)
    {
        return false;
    }
    return true;
}

int BaseFunction::Utf8Length(const char* utf8code)
{
    const unsigned char *s = (const unsigned char*)utf8code;
    int length = 0;
    while(*s) {
        int code = *s++;
        if(code > 0xf0) {
            if(*s) s++;
            if(*s) s++;
            if(*s) s++;
        } else if(code > 0xe0) {
            if(*s) s++;
            if(*s) s++;
        } else if(code > 0xc0) {
            if(*s) s++;
        }
        length++;
    }
    return length;
}

int BaseFunction::IsValidAccount(const char* account)
{
    int length = strlen(account);
    if(length != BaseFunction::Utf8Length(account))
    {
        return 0;
    }

    // 非法字符
    const std::regex patten("[^0-9a-zA-z]");
    bool rs = std::regex_search(account, patten);
    if (rs)
    {
        return 0;
    }

    if(length < 6 || length > 16)
    {
        if (length > 16)
        {
            return -3;
        }
        return -2;
    }

    return 1;
}

int BaseFunction::IsValidPassword(const char* password)
{
    int length = strlen(password);
    if(length != BaseFunction::Utf8Length(password))
    {
        return 0;
    }

    if(length < 6 || length > 12)
    {
        if (length > 12)
        {
            return -3;
        }
        return -2;
    }

    return 1;
}

bool BaseFunction::IsValidEmail(const char* email)
{
    const std::regex patten("\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*");

    return std::regex_match(email, patten);
}

#define EPSILON      1e-5f

inline bool IsFloatZero(float value)
{
    if (value > EPSILON || value < -EPSILON)
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------------------
template <class T>
inline T MyMin( T a, T b)
{
    return a < b ? a : b;
}

//-----------------------------------------------------------------------------------------
template <class T>
inline T MyMax( T a,  T b)
{
    return a > b ? a : b;
}

inline bool FloatEqual(float a, float b, float deviation = EPSILON)
{
    float dif = a >= b ? a - b : b - a;
    if(dif < deviation)
    {
        return true;
    }
    return false;
}


bool BaseFunction::GetTilesFromLine(const Vec2& start, const Vec2& end, float tileSize, std::vector<Vec2>& tiles)
{
    float dx = end.x - start.x;
    float dy = end.y - start.y;

    if (abs(dx) > abs(dy))
    {
        float dydx = dy / dx;
        if (dx > 0)
        {
            float ceilX = ceil((start.x + 0.001f) / tileSize); //> +0.001对于等于0的情况做舍取
            float startY = start.y + (ceilX - (start.x + 0.001f) / tileSize) * tileSize * dydx + 0.001f;    //>根据x步进到下一格子算取y
            float startX = ceilX * tileSize;    //>x步进到下一格子

            Vec2 tile;
            tile.y = floor(startY / tileSize);
            tile.x = floor(startX / tileSize);
            tiles.push_back(tile); //>下一个保存到数组

            int number = (int)floor(dx / tileSize);
            float lastTileY = tile.y;
            for (int i = 1; i < number; ++i)
            {
                tile.y = floor((startY + i * tileSize * dydx) / tileSize); //>y步进通过计算斜率得到
                tile.x = (startX / tileSize) + i;   //>x直接步进一格
                if (lastTileY != tile.y && !FloatEqual(dydx, 1.0)){ //>关键要考虑的情况.这里和画直线函数有关键的区别
                    tiles.push_back(Vec2(tile.x - 1.0f, tile.y));
                    lastTileY = tile.x;
                }
                tiles.push_back(tile);
            }
        }
        else
        {
            float floorX = floor(start.x / tileSize);
            float startY = MyMax(0.0f, (start.y - (start.x / tileSize - floorX) * tileSize * dydx - 0.001f));
            float startX = floorX * tileSize - 0.001f;

            Vec2 tile;
            tile.y = floor(startY / tileSize);
            tile.x = floor(startX / tileSize);
            tiles.push_back(tile);

            int number = (int)floor(-dx / tileSize);
            float lastTileY = tile.y;
            for (int i = 1; i < number; ++i)
            {
                tile.y = floor((startY - i * tileSize * dydx) / tileSize);
                tile.x = floor(startX / tileSize) - i;
                if (lastTileY != tile.y && !FloatEqual(dydx, 1.0)){
                    tiles.push_back(Vec2(tile.x + 1.0f, tile.y));
                    lastTileY = tile.x;
                }
                tiles.push_back(tile);
            }
        }
    }
    else
    {
        float dxdy = dx / dy;
        if (dy > 0)
        {
            float ceilY = ceil((start.y + 0.001f) / tileSize);
            float startY = ceilY * tileSize;
            float startX = start.x + (ceilY - (start.y + 0.001f) / tileSize) * tileSize * dxdy + 0.001f;

            Vec2 tile;
            tile.y = floor(startY / tileSize);
            tile.x = floor(startX / tileSize);
            tiles.push_back(tile);

            int number = (int)floor(dy / tileSize);
            float lastTileX = tile.x;
            for (int i = 1; i < number; ++i)
            {
                tile.y = (startY / tileSize) + i;
                tile.x = floor((startX + i * tileSize * dxdy) / tileSize);
                if (lastTileX != tile.x && !FloatEqual(dxdy, 1.0)){
                    tiles.push_back(Vec2(tile.x, tile.y - 1.0f));
                    lastTileX = tile.x;
                }
                tiles.push_back(tile);
            }
        }
        else
        {
            float dxdy = dx / dy;
            float floorY = floor(start.y / tileSize);
            float startY = floorY * tileSize - 0.001f;
            float startX = MyMax(0.0f, start.x - (start.y / tileSize - floorY) * tileSize * dxdy - 0.001f);

            Vec2 tile;
            tile.y = floor(startY / tileSize);
            tile.x = floor(startX / tileSize);
            tiles.push_back(tile);

            int number = (int)floor(-dy / tileSize);
            float lastTileX = tile.x;
            for (int i = 1; i < number; ++i)
            {
                tile.y = floor(startY / tileSize) - i;
                tile.x = floor((startX - i * tileSize * dxdy) / tileSize);
                if (lastTileX != tile.x && !FloatEqual(dxdy, 1.0)){
                    tiles.push_back(Vec2(tile.x, tile.y + 1));
                    lastTileX = tile.x;
                }
                tiles.push_back(tile);
            }
        }
    }
    return true;
}
