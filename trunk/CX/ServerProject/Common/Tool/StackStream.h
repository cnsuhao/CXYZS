#pragma once

/********************************************************************
created:	2015/7/16
author:		lishihai
desc:	    ջ������/�������
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

    inline unsigned int GetSize() const;        //>ȡ��ʵ�ʴ�С
    inline unsigned int GetCapacity() const;    //>ȡ���������
    inline void* Get();                         //>������ݵ�ַ
    inline void Wirte(void* src, size_t length);//>д����
    inline void Read(void* dest, size_t length);//>��ȡ��

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
//>ʹ�÷���
// 
// StackStream<> ss1;       //����һ��Ĭ��1024��С����
// StackStream<4096> ss2;   //����һ���ƶ�4096��С����
//
// MsgA msg;
// msg.id = 1122;
// msg.numberB = 10;
// ...
// ...
// ss1.Write(msg);  // д��ͷ
// for(int i = 0; i < msg.numberB; ++i)
// {
//      ss1.Write(&structB, sizeof(structB));   //д�뼯��.
// }
// MsgA* pMsg = (MsgA*)ss1.Get();   //ת����msg
// pMsg->Len = ss1.GetSize();          //��msg���ó���
//
// Net->Send(pMsg, pMsg->Len);      //����

// ��ȡ
// StackStream<> ss1(scr, srcLength);   //����һ��Ĭ��1024��С����
// MsgA msg;
// ss1.Read(msg);
// std::vector<structB> container;
// container.resize(msg.numberB);
// for(int i = 0; i < msg.numberB; ++i)
// {
//      ss1.(&container[i], sizeof(structB));   //д�뼯��.
// }
//