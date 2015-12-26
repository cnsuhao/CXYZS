/**************************************************************************
created:	2014/12/08
author:		LL
desc:		���������ࣺGB2312��utf8���໥ת���ӿ�
***************************************************************************/
#pragma once
#ifndef __UITOOLS_H__
#define __UITOOLS_H__

#include "ClientDef.h"
#include "CCPlatformDefine.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "..\cocos2d\iconv\include\iconv.h"
#include "..\cocos2d\cocos\platform\android\jni\JniHelper.h"
#else
#include "iconv.h"
#endif
#pragma comment(lib,"libiconv.lib")

#define PI 3.1415926
#define KEY 1314		//�ӽ��ܵ���Կ
//GB2312��utf8��ת���ӿ�
class BaseFunction
{

public:
	////////////////////////////////////////////////////�����
	
	//�õ�һ�������
	inline static int Random(int seed)
	{
		if (seed == 0) 
			return 0;
		if (seed > RAND_MAX)
		{
			return (RAND_MAX*rand() + rand()) % seed;
		}
		return rand() % seed;
	}

	//�õ�һ����Χ�ڵ������
	inline static int Random(int start, int end)
	{
		int temp = end - start + 1;
		return Random(temp) + start;
	}
	//ȡ2��֮��ĽǶ�
	static float PointToAngle(const cc2d::Vec2 &pos1, const cc2d::Vec2 &pos2)
	{
		float o = pos2.x - pos1.x;
		float a = pos2.y - pos1.y;
		float at = (float) CC_RADIANS_TO_DEGREES( atanf( o/a) );

		if( a < 0 ) 
		{
			if(  o < 0 )
				at = 180 + fabs(at);
			else
				at = 180 - fabs(at);    
		}
		return at;
	}

    inline static float Lerp(float a, float b, float t)
    {
        return (1.0f - t)*a + t*b;
    }

	//ȡ�����Ӽ�ĸ���
	inline static int TileDistance(const cc2d::Vec2 &pos1, const cc2d::Vec2 &pos2)
	{
		int x = abs(pos1.x - pos2.x);
		int y = abs(pos1.y - pos2.y);
		return x>y?x:y;
	}
	//���ܽ���
	static std::string StrCode(std::string &str);
	
	//////////////////////////////////////////////////////////////////////////����ת���ӿ�
	static int code_convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen); 
	static std::string u2a(const char *inbuf);	//utf8תgb
	static std::string a2u(const char *inbuf);	//gbתutf8
    static inline std::string u2a(const std::string& instr) { return u2a(instr.c_str()); }
    static inline std::string a2u(const std::string& instr) { return a2u(instr.c_str()); }
	static std::string getImei(); //��ȡ�豸ʶ����
	static std::string GetHHMMSSStr (long long millisecond);  ///ת����ʱ:��:���ʽ

	//��ȡ�ñ�ʾ���ָ����ַ���
	static void splitStr(const std::string &str, const std::string &sepStr, std::vector<std::string> *varVect);
	//��ȡ�ñ�ʾ���ָ����ַ�����ȡ������ʾ��֮����ַ�����
	static void splitStr(const std::string &str, const std::string &sepPrefix, const std::string &sepSuffix, std::vector<std::string> *varVect);

	static std::string m_imeiStr;

    static bool DistanceToBeIgnore(const cc2d::Vec2& a, const cc2d::Vec2& b);

    static int Utf8Length(const char* utf8code);

    static int Utf8Length(const std::string& utf8code) { return Utf8Length(utf8code.c_str()); }

    static int IsValidAccount(const char* account); //return value (1Ϊ�Ϸ�,0�Ƿ�����,-1�������ַ�,-2����̫��,-3����̫��)

    static int IsValidPassword(const char* password); // return value(1Ϊ�Ϸ�,0�Ƿ�����,-1�������ַ�,-2����̫��,-3����̫��)

    static bool IsValidEmail(const char* email);   //����Ƿ�Ϊ��Ч������.

    static bool GetTilesFromLine(const cc2d::Vec2& start, const cc2d::Vec2& end, float tileSize, std::vector<cc2d::Vec2>& tiles);
};
#endif  // __TOOLS_H__