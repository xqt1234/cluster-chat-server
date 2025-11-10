#include "ClientNet.h"
#include <arpa/inet.h>
#include "Logger.h"
#include <sys/socket.h>

ClientNet::ClientNet()
{
    init();
}

ClientNet::~ClientNet()
{
}

void ClientNet::connect()
{
    struct sockaddr_in addr{0};
    uint16_t port = 9999;
    std::string ip = "192.168.65.4";
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,ip.c_str(),&addr.sin_addr.s_addr);
    int ret = ::connect(m_fd,(sockaddr*)&addr,sizeof(addr));
    if(ret == -1)
    {
        LOG_FATAL("连接失败");
    }
}

void ClientNet::send(std::string str)
{
    int n = ::send(m_fd,str.c_str(),str.length(),0);
    if(n < 0)
    {
        LOG_ERROR("发送失败");
    }else
    {
        LOG_INFO("发送成功");
    }

}

void ClientNet::init()
{
    m_fd = socket(AF_INET,SOCK_STREAM,0);
    if(m_fd < 0)
    {
        LOG_FATAL("初始化错误");
    }
}
