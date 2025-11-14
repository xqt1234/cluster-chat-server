#pragma once
#include <netinet/in.h>
#include <string>
#include <memory>
#include "CallBacks.h"
#include "Acceptor.h"
#include <unordered_map>
#include "EventLoopThreadPoll.h"
using std::string;

/**
 * @brief 库入口类。主要思路就是启动绑定，设置线程数量，回调函数接口，然后在新连接到来的时候，
 * 传递回调给给新连接tcpconnection。提供开始函数完成创建线程。
 * 作为核心路径分发新连接到eventloop，对应提供接口操作删除连接。该接口作为回调传入新连接中
 */
class TcpServer
{
public:
    
private:
    std::unique_ptr<Acceptor> m_acceptor;
    ConnectionCallBack m_ConnectionCallBack;
    MessageCallBack m_MessageCallBack;
    uint64_t m_nextConnId{0};
    std::unordered_map<uint64_t,TcpConnectionPtr> m_clients;
    EventLoop* m_loop;
    EventLoopThreadPoll* m_pool;

public:
    TcpServer(EventLoop* loop, uint16_t port,string addr = "0.0.0.0");
    ~TcpServer();
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void start();
    void setThreadNum(int num);
    void setConnectionCallBack(const ConnectionCallBack& cb)
    {
        m_ConnectionCallBack = cb;
    }
    void setMessageCallBack(const MessageCallBack& cb)
    {
        m_MessageCallBack = cb;
    }
private:
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
};