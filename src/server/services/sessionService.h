#pragma once
#include "baseservice.h"
class SessionService : BaseService
{
public:
    struct ConnectInfo
    {
        bool m_isOnline;
        bool m_isLocal;
        TcpConnectionPtr m_conn;
    };
private:
    std::unordered_map<int, ConnInfo> m_clientsMap;
    std::unordered_map<TcpConnectionPtr, int> m_clientsMapPtr;
public:
    SessionService(/* args */) = default;
    ~SessionService() = default;
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnection(int userid,const ConnInfo& info);
    void addConnection(const TcpConnectionPtr &conn,int userid,long long version);
    ConnInfo getUser(int userid);
    TcpConnectionPtr getConnection(int userid);
    void kickuser(std::string str);
    void checkAndKickLogin(const TcpConnectionPtr &conn, json &js, int userid);
    ConnectInfo checkHasLogin(int userid);
};
