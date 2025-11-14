#pragma once
#include "InetAddress.h"

/**
 * @brief 主要思路是封装监听的文件描述符，然后对其进行端口复用之类的设置。
 * 提供监听，绑定接口
 * 其他情况下，对新连接的文件描述符包装一下，在析构的时候，能够自行关闭该文件描述符。
 */
class Socket
{
private:
    int m_sockfd;

public:
    Socket();
    Socket(int fd);
    ~Socket();
    int accept(InetAddress *peerAddr);
    void bindAddress(InetAddress& listenAddr);
    void listen();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
    void shutdownWrite();
    int fd(){return m_sockfd;}
};