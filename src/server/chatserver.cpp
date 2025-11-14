#include "chatserver.h"
#include "json.hpp"
#include "Logger.h"
#include "chatservice.h"
using json = nlohmann::json;
ChatServer::ChatServer(EventLoop *loop, uint16_t port, std::string ipaddr)
    : m_loop(loop), m_server(new TcpServer(loop, port, ipaddr))
{
    m_server->setMessageCallBack(std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    m_server->setConnectionCallBack(std::bind(&ChatServer::newConnection, this, std::placeholders::_1));
}

ChatServer::~ChatServer()
{
}

void ChatServer::newConnection(const TcpConnectionPtr &conn)
{
    if (conn->isConnected())
    {
        // std::cout << "启用外面的回调" << std::endl;
    }
    else
    {
        // std::cout << "断开连接" << std::endl;
    }
}
void ChatServer::start()
{
    m_server->start();
}

void ChatServer::setThreadNum(int num)
{
    m_server->setThreadNum(num);
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf)
{
    std::string msg = buf->readAllAsString();
    std::cout << msg << std::endl;
    if(msg == "")
    {
        return;
    }
    json js;
    try
    {
        js = json::parse(msg);
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("解析错误");
        return;
    }
    int msgid = js["msgid"];
    MsgHandle handle = m_service.getHandler(msgid);
    handle(conn,js);
}
