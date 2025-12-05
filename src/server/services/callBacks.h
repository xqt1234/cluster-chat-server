#pragma once
#include <functional>
#include "TcpConnection.h"
#include <unordered_map>
struct ConnectInfo
{
    int m_userid = -1;
    bool m_isOnline = false;
    bool m_isLocal = false;
    TcpConnectionPtr m_conn = nullptr;
    long long m_version = -1;
    int64_t m_lastheartTime = -1;
    std::string m_servername = "";
};
using GetConnCallBack = std::function<ConnectInfo(int)>;
using KickCallBack = std::function<void(std::string)>;
using CheckCallBack = std::function<void(const ConnectInfo&)>;
using RedisCallBack = std::function<void(std::unordered_map<std::string,std::string>&)>;

