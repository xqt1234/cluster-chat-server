#pragma once
#include "TcpServer.h"
#include "TcpConnection.h"
#include <iostream>
#include <memory>
#include "chatservice.h"
using EventLoop = mymuduo::EventLoop;
class ChatServer
{
private:
    EventLoop* m_loop;
    mymuduo::TcpServer* m_server;
    ChatService m_service;
public:
    ChatServer(mymuduo::EventLoop* loop,uint16_t port,std::string ipaddr);
    ~ChatServer();
    void onMessage(const TcpConnectionPtr& conn,mymuduo::Buffer* buf);
    void start();
    void setThreadNum(int num);
    void newConnection(const TcpConnectionPtr &conn);
private:
    
};

