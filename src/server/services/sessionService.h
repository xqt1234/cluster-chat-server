#pragma once
#include "baseservice.h"
class SessionService : BaseService
{
public:
    struct ConnectInfo
    {
        int m_userid = -1;
        bool m_isOnline = false;
        bool m_isLocal = false;
        TcpConnectionPtr m_conn = nullptr;
        long long m_version = -1;
    };
    
private:
    std::unordered_map<int, ConnectInfo> m_clientsMap;
    std::unordered_map<TcpConnectionPtr, int> m_clientsMapPtr;
    std::mutex m_clientsmapMtx;
public:
    SessionService(/* args */) = default;
    ~SessionService() = default;
    void removeConnection(const ConnectInfo& info);
    void addConnection(const ConnectInfo& info);
    //ConnectInfo getUser(int userid);
    void kickuser(std::string str);
    void checkAndKickLogin(const ConnectInfo& info);
    ConnectInfo checkHasLogin(int userid);
};
