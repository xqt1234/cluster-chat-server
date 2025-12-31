#pragma once
#include <string>
#include <functional>
#include <atomic>
#include "Buffer.h"
#include <memory>
class ClientNet
{
public:
    using DisconnectionCallBack = std::function<void()>;
    struct RpcMsgHeader
    {
        uint32_t magic;
        uint32_t datalength;
    };
    static const uint32_t kMagicNumber = 0x55AA55AA; 
    std::unique_ptr<Buffer> m_recvBuffer;
private:
    int m_fd{-1};
    DisconnectionCallBack m_disconnection;
    std::atomic<bool> m_isconnected{false};
public:
    ClientNet(/* args */);
    ~ClientNet();
    void connect();
    bool send(const std::string& str);
    std::string recvmsg();
    void setDisconnectionCallBack(const DisconnectionCallBack& cb);
private:
    void init();
};

