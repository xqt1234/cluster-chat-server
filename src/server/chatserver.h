#pragma once
#include "TcpServer.h"
#include "TcpConnection.h"
#include <iostream>
#include <memory>
#include "chatservice.h"
class ChatServer
{
private:
    EventLoop* m_loop;
    TcpServer* m_server;
    ChatService m_service;
public:
    ChatServer(EventLoop* loop,uint16_t port,std::string ipaddr);
    ~ChatServer();
    void onMessage(const TcpConnectionPtr& conn,Buffer* buf);
    void start();
    void setThreadNum(int num);
    void newConnection(const TcpConnectionPtr &conn);
private:
    
};

