#include "clientNet.h"
#include <arpa/inet.h>
#include "Logger.h"
#include <sys/socket.h>
#include "config.h"
#include <vector>
#include <endian.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
ClientNet::ClientNet()
    :m_recvBuffer(std::make_unique<Buffer>())
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
    struct sockaddr_in addr{0};
    uint16_t port = atoi(config.getValue("chatserverport").c_str());
    std::string ip = config.getValue("chatserverip");
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
    int totallen = sizeof(RpcMsgHeader) + str.length();
    std::vector<char> sendvec(totallen);
    RpcMsgHeader* header = reinterpret_cast<RpcMsgHeader*>(sendvec.data());
    header->datalength = htonl(str.length());
    header->magic = htonl(kMagicNumber);
    memcpy(sendvec.data() + sizeof(RpcMsgHeader),str.data(),str.length());
    std::string sendstr = std::string(sendvec.data(),sendvec.size());
    int n = ::send(m_fd,sendstr.c_str(),sendstr.length(),0);
    if(n < 0)
    {
        LOG_ERROR("发送失败");
        return false;
    }else
    {
        //LOG_DEBUG("发送成功");
    }
    return true;
}

std::string ClientNet::recvmsg()
{
    // 1. 从 socket 读取数据到 Buffer
    int tErr;
    int n = m_recvBuffer->readFd(m_fd,&tErr);  // muduo 提供的方法，直接封装 recv
    if (n <= 0) {
        if (n == 0) {
            LOG_ERROR("服务器关闭连接");
            if (m_disconnection) m_disconnection();
        } else {
            LOG_ERROR("收数据错误");
        }
        return "";
    }
    const char *data = m_recvBuffer->peek();
    int len = m_recvBuffer->readableBytes();
    while (len > sizeof(RpcMsgHeader))
    {
        RpcMsgHeader rpchead;
        memcpy(&rpchead,m_recvBuffer->peek(),sizeof(RpcMsgHeader));
        // const RpcMsgHeader* rpchead = reinterpret_cast<const RpcMsgHeader*>(buffer->peek());
        uint32_t magic = ntohl(rpchead.magic);
        if (kMagicNumber != magic)
        {
            m_recvBuffer->retrieve(1);
            len = m_recvBuffer->readableBytes();
            data = m_recvBuffer->peek();
            continue;
        }
        uint32_t datalen = ntohl(rpchead.datalength);
        if (len < sizeof(RpcMsgHeader) + datalen)
        {
            break;
        }
        m_recvBuffer->retrieve(sizeof(RpcMsgHeader));
        std::string msg = m_recvBuffer->readAsString(datalen);
        data = m_recvBuffer->peek();
        len = m_recvBuffer->readableBytes();
        return msg;
    }
    return "";
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
