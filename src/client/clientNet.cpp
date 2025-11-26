#include "clientNet.h"
#include <arpa/inet.h>
#include "Logger.h"
#include <sys/socket.h>
#include "config.h"
ClientNet::ClientNet()
{
    init();
}

ClientNet::~ClientNet()
{
    ::close(m_fd);
    std::cout << "网络层被析构" << std::endl;
}

void ClientNet::connect()
{
    Config &config = Config::getInstance();
    config.loadConfig("clientconf.ini");
    struct sockaddr_in addr{0};
    uint16_t port = atoi(config.getValue("chatserverport","9998").c_str());
    std::string ip = config.getValue("chatserverip","192.168.65.4");
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,ip.c_str(),&addr.sin_addr.s_addr);
    int ret = ::connect(m_fd,(sockaddr*)&addr,sizeof(addr));
    if(ret == -1)
    {
        LOG_FATAL("连接失败");
    }
}

bool ClientNet::send(const std::string& str)
{
    int n = ::send(m_fd,str.c_str(),str.length(),0);
    if(n < 0)
    {
        LOG_ERROR("发送失败");
        return false;
    }else
    {
        LOG_DEBUG("发送成功");
    }
    return true;
}

std::string ClientNet::recvmsg()
{
    char buf[1024]{};
    int n = ::recv(m_fd,buf,sizeof(buf),0);
    if(n < 0)
    {
        LOG_ERROR("收数据错误");
        return "";
    }else if (n == 0) {
        LOG_ERROR("服务器关闭了连接");
        if(m_disconnection)
        {
            m_disconnection();
        }
        return "";
    }
    return std::string(buf,n);
}

void ClientNet::setDisconnectionCallBack(const DisconnectionCallBack &cb)
{
    m_disconnection = cb;
}

void ClientNet::init()
{
    m_fd = socket(AF_INET,SOCK_STREAM,0);
    if(m_fd < 0)
    {
        LOG_FATAL("初始化错误");
    }
}
