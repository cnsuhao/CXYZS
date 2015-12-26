/********************************************************************
created:	2015/3/20
author:		LL
desc:		跨平台socket
*********************************************************************/
#ifndef __LLSOCKET_H__
#define __LLSOCKET_H__
 
#include "cocos2d.h"

#ifdef WIN32  
#include <windows.h>  
#include <WinSock.h>  
#else  
#include <sys/socket.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
 
#define SOCKET int  
#define SOCKET_ERROR -1  
#define INVALID_SOCKET -1  
  
#endif  
  
#ifndef CHECKF  
#define CHECKF(x) \
    do \
{ \
    if (!(x)) { \
    CCLOG("CHECKF", #x, __FILE__, __LINE__); \
    return 0; \
    } \
} while (0)  
#endif  
  
#define _MAX_MSGSIZE 16 * 1024      // 暂定一个消息最大为16k  
#define BLOCKSECONDS    10          // INIT函数阻塞时间  
#define INBUFSIZE   (64*1024)       //? 具体尺寸根据剖面报告调整  接收数据的缓存  
#define OUTBUFSIZE  (8*1024)        //? 具体尺寸根据剖面报告调整。 发送数据的缓存，当不超过8K时，FLUSH只需要SEND一次  
  
class LLSocket 
{  
public:  
    LLSocket(void);  
    bool    Create(const char* pszServerIP, int nServerPort, int nBlockSec = BLOCKSECONDS, bool bKeepAlive = false);  
    bool    SendMsg(void* pBuf, int nSize);		//发送消息到缓存，也就是拼包，然后由Flush函数发送。
    bool    ReceiveMsg(void* pBuf, int& nSize);  //接收消息
    bool    Flush(void);			//真实发送消息
    bool    Check(void);			//检查连接状态
    void    Destroy(void);			//关闭连接
    SOCKET  GetSocket(void) const { return m_sockClient; }  
private:  
    bool    recvFromSock(void);     // 从网络中读取尽可能多的数据  
    bool    hasError();				// 是否发生错误，注意，异步模式未完成非错误  
    void    closeSocket();			// 关闭Socket
  
    SOCKET  m_sockClient;  
  
    // 发送数据缓冲  
    char    m_bufOutput[OUTBUFSIZE];    //? 可优化为指针数组  
    int     m_nOutbufLen;  
  
    // 环形缓冲区  
    char    m_bufInput[INBUFSIZE];  
    int     m_nInbufLen;  
    int     m_nInbufStart;              // INBUF使用循环式队列，该变量为队列起点，0 - (SIZE-1)  
};  

#endif	//__LLSOCKET_H__