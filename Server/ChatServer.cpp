#include "ChatServer.h"


ChatServer::ChatServer(EventLoop *loop, uint16_t port, std::string ipaddr)
    :m_loop(loop)
    , m_server(new TcpServer(loop,port,ipaddr))
{
    m_server->setMessageCallBack(std::bind(&ChatServer::onMessage,this,std::placeholders::_1,std::placeholders::_2));
    m_server->setConnectionCallBack(std::bind(&ChatServer::newConnection,this,std::placeholders::_1));
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
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf)
{
    std::string res = buf->readAllAsString();
    std::cout << res << std::endl;
}

void ChatServer::start()
{
    m_server->start();
}

void ChatServer::setThreadNum(int num)
{
    m_server->setThreadNum(num);
}
