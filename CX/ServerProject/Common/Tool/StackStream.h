#pragma once

/********************************************************************
created:	2015/7/16
author:		lishihai
desc:	    栈上输入/输出流类
*********************************************************************/

#include <cassert>

template <int capacity = 1024> class StackStream;

template <int capacity>
class StackStream
{
public:
    StackStream(void):writePos(0),readPos(0),size(0),refBuff(0){
    }

    StackStream(void* refBuff, unsigned int size):writePos(0),readPos(0),size(size),refBuff(refBuff){
    }

    ~StackStream(void) {}

    inline unsigned int GetSize() const;        //>取得实际大小
    inline unsigned int GetCapacity() const;    //>取得最大容量
    inline void* Get();                         //>获得数据地址
    inline void Wirte(void* src, size_t length);//>写入流
    inline void Read(void* dest, size_t length);//>读取流

private:
    unsigned int writePos;
    unsigned int readPos;
    unsigned int size;
    char datas[capacity];
    void* refBuff;
};

template <int capacity>
unsigned int StackStream<capacity>::GetSize() const
{
    return size;
}

template <int capacity>
unsigned int StackStream<capacity>::GetCapacity() const
{
    return capacity;
}

template <int capacity>
void* StackStream<capacity>::Get()
{
    if (refBuff != NULL)
    {
        return refBuff;
    }
    return (void*)datas;
}

template <int capacity>
void StackStream<capacity>::Wirte(void* src, size_t length)
{
    assert(length <= capacity - writePos);
    memcpy(datas+writePos, src, length);
    writePos += length;
    size += length;
}

template <int capacity>
void StackStream<capacity>::Read(void* dest, size_t length)
{
    assert(length <= size - readPos);
    void* pBuff = refBuff != NULL ? (char*)refBuff+readPos : datas+readPos;
    if (dest != NULL){
        memcpy(dest, pBuff, length);
    }
    readPos += length;
}

//-----------------------------------------------------------------------------------------
//>使用方法
// 
// StackStream<> ss1;       //申请一个默认1024大小的流
// StackStream<4096> ss2;   //申请一个制定4096大小的流
//
// MsgA msg;
// msg.id = 1122;
// msg.numberB = 10;
// ...
// ...
// ss1.Write(msg);  // 写入头
// for(int i = 0; i < msg.numberB; ++i)
// {
//      ss1.Write(&structB, sizeof(structB));   //写入集合.
// }
// MsgA* pMsg = (MsgA*)ss1.Get();   //转换成msg
// pMsg->Len = ss1.GetSize();          //给msg设置长度
//
// Net->Send(pMsg, pMsg->Len);      //发送

// 读取
// StackStream<> ss1(scr, srcLength);   //申请一个默认1024大小的流
// MsgA msg;
// ss1.Read(msg);
// std::vector<structB> container;
// container.resize(msg.numberB);
// for(int i = 0; i < msg.numberB; ++i)
// {
//      ss1.(&container[i], sizeof(structB));   //写入集合.
// }
//