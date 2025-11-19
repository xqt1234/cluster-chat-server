#pragma once
#include <string>
#include <functional>
class ClientNet
{
public:
    using DisconnectionCallBack = std::function<void()>;
private:
    int m_fd{-1};
    DisconnectionCallBack m_disconnection;
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

