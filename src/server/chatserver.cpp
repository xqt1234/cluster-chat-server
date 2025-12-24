#include "chatserver.h"
#include "json.hpp"
#include "Logger.h"
#include "chatservice.h"
using json = nlohmann::json;
ChatServer::ChatServer(EventLoop *loop, uint16_t port, std::string ipaddr)
    : m_loop(loop), m_server(new mymuduo::TcpServer(loop, port, ipaddr))
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
        // std::cout << "连接被关闭" << std::endl;
        m_service.removeConnection(conn);
        conn->shutdown();
    }
}

void ChatServer::start()
{
    //RelationCache::getInstance().initAllGroupUsers();
    m_server->start();
}

void ChatServer::setThreadNum(int num)
{
    m_server->setThreadNum(num);
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, mymuduo::Buffer *buf)
{
    std::string msg = buf->readAllAsString();
    json js;
    ChatService::ValidResult res = m_service.checkValid(msg, js);
    if (!res.success)
    {
        js = m_service.buildErrorResponse(std::move(res));
        conn->send(js.dump());
        return;
    }
    m_service.handMessage(conn,js);
}
