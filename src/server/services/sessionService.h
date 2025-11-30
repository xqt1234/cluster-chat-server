#pragma once
#include "baseservice.h"
class SessionService : BaseService
{

private:
    std::unordered_map<int, ConnectInfo> m_clientsMap;
    std::unordered_map<TcpConnectionPtr, int> m_clientsMapPtr;
    std::mutex m_clientsmapMtx;
public:
    SessionService(/* args */) = default;
    ~SessionService() = default;
    void removeConnection(const ConnectInfo& info);
    void addConnection(const ConnectInfo& info);
    void kickuser(std::string str);
    void checkAndKickLogin(const ConnectInfo& info);
    ConnectInfo checkHasLogin(int userid);
};
