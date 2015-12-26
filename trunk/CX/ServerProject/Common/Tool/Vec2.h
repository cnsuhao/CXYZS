#pragma once

/********************************************************************
created:	2015/7/16
author:		lishihai
desc:		2D������
*********************************************************************/

#include <cassert>
#include <cmath>
#include <limits>

#define EPSILON      1e-5f

inline bool IsFloatZero(float value)
{
    if (value > EPSILON || value < -EPSILON)
    {
        return false;
    }
    return true;
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

//-----------------------------------------------------------------------------------------
template <class T>
inline T MyAbs(T x)
{
    if (x < 0)
        return -x;

    return x;
}

//#ifdef max
//    #undef max
//#endif // max
//
//#ifdef min
//    #undef  min
//#endif // min

// ȡ��int�����ֵ
inline int GetMaxInt()
{// ����Ϊ�˱�����max���ͻ, �ֿ�д��������
    return ( std::numeric_limits<int>::max ) ();
}

// ȡ��int��С��ֵ
inline int GetMinInt()
{
    return ( std::numeric_limits<int>::min ) ();
}

// ȡ��unsigned int�����ֵ
inline unsigned int GetMaxUint()
{
    return ( std::numeric_limits<unsigned int>::max )();
}

// ȡ��float�����ֵ
inline float GetMaxFloat()
{
    return ( std::numeric_limits<float>::max ) (); 
}

// ȡ��float��С��ֵ
inline float GetMinFloat()
{
    return ( std::numeric_limits<float>::min ) ();
}

class BaseFunction
{
public:
    ////////////////////////////////////////////////////�����
    void SetRandomSeed(int seed)
    {
        gRandomSeed = seed;
    }

    /// Return a random integer between 0 and 32767 (RAND_MAX).
    int static Rand()
    {
        gRandomSeed = gRandomSeed * 214013 + 2531011;
        return (gRandomSeed >> 16) & 32767;
    }

    //�õ�һ�������
    inline static int Random(int range)
    {
        assert(range < 32767);
        return (Rand() * (range - 1) + 16384) / 32767;
    }

    inline static float Random(float range)
    {
        assert(range < 32767.0f);
        return Rand() * range / 32767.0f;
    }

    //�õ�һ����Χ�ڵ������
    inline static int Random(int min, int max)
    {
        assert(max < 32767);
        return (Rand() * (max - min - 1) + 16384) / 32767 + min;
    }

    inline static float Random(float min, float max)
    {
        assert(max < 32767.0f);
        return Rand() * (max - min) / 32767.0f + min;
    }
private:
    static int BaseFunction::gRandomSeed;
};

struct Vec2
{
    float x;
    float y;

    Vec2():x(0),y(0){}
    Vec2(float x, float y):x(x),y(y){}

    inline float distance(const Vec2& other) const
    {
        float dx = (x - other.x);
        float dy = (y - other.y);
        return sqrt(dx * dx + dy * dy);
    }

    inline float distancePow2(const Vec2& other) const
    {
        float dx = (x - other.x);
        float dy = (y - other.y);
        return dx * dx + dy * dy;
    }

    Vec2& Normalize()
    {
        float length = x * x + y * y;
        if (FloatEqual(length, 1.0f))
            return *this;

        length = sqrt(length);
#ifdef _DEBUG
        assert(!IsFloatZero(length));
#endif // _DEBUG

        if (length < EPSILON) length = EPSILON;
        const float invLen = 1.0f / length;
        x *= invLen;
        y *= invLen;
        return *this;
    }

    Vec2& operator = (const Vec2& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    Vec2 operator - (const Vec2& other)
    {
        Vec2 v;
        v.x = x - other.x;
        v.y = y - other.y;
        return v;
    }

    bool operator == (const Vec2& other)
    {
        if (x == other.x && y == other.y)
        {
            return true;
        }
        return false;
    }

    bool operator != (const Vec2& other)
    {
        if (x != other.x || y != other.y)
        {
            return true;
        }
        return false;
    }

    /** equals to Vec2(0,0) */
    static const Vec2 ZERO;
    /** equals to Vec2(1,1) */
    static const Vec2 ONE;
    /** equals to Vec2(1,0) */
    static const Vec2 UNIT_X;
    /** equals to Vec2(0,1) */
    static const Vec2 UNIT_Y;
    /** equals to Vec2(0.5, 0.5) */
    static const Vec2 ANCHOR_MIDDLE;
    /** equals to Vec2(0, 0) */
    static const Vec2 ANCHOR_BOTTOM_LEFT;
    /** equals to Vec2(0, 1) */
    static const Vec2 ANCHOR_TOP_LEFT;
    /** equals to Vec2(1, 0) */
    static const Vec2 ANCHOR_BOTTOM_RIGHT;
    /** equals to Vec2(1, 1) */
    static const Vec2 ANCHOR_TOP_RIGHT;
    /** equals to Vec2(1, 0.5) */
    static const Vec2 ANCHOR_MIDDLE_RIGHT;
    /** equals to Vec2(0, 0.5) */
    static const Vec2 ANCHOR_MIDDLE_LEFT;
    /** equals to Vec2(0.5, 1) */
    static const Vec2 ANCHOR_MIDDLE_TOP;
    /** equals to Vec2(0.5, 0) */
    static const Vec2 ANCHOR_MIDDLE_BOTTOM;
};


//����תΪid
inline Vec2 TileId2Tile(int tileId)
{
	//128�ǵ�ͼ����������	
	Vec2 tilePos(0,0);
	if (tileId != 0)
	{
		tilePos.x = float(tileId / 128);
		tilePos.y = float(tileId % 128);
	}

	return tilePos;
}
//idתΪ����
inline int Tile2TileId(const Vec2 &tilePos)
{
	//128�ǵ�ͼ����������	
	return (int)(tilePos.x) * 128 + (int)tilePos.y;
}

//ȡ�����Ӽ�ĸ���
inline static int TileDistance(const Vec2 &tilePos1, const Vec2 &tilePos2)
{
    float x = abs(tilePos1.x - tilePos2.x);
    float y = abs(tilePos1.y - tilePos2.y);
    return int(x>y?x:y);
}
