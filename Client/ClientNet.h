#pragma once
#include <string>
class ClientNet
{
private:
    int m_fd{-1};
public:
    ClientNet(/* args */);
    ~ClientNet();
    void connect();
    void send(std::string str);
private:
    void init();
};

