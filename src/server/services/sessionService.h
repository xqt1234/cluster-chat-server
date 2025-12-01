#pragma once
#include "baseservice.h"
#include <thread>
#include <atomic>
class SessionService : BaseService
{

private:
    std::unordered_map<int, ConnectInfo> m_clientsMap;
    std::unordered_map<TcpConnectionPtr, int> m_clientsMapPtr;
    std::mutex m_clientsmapMtx;
    std::atomic<bool> m_stop{false};
    std::thread m_aliveThread;
public:
    SessionService(/* args */);
    ~SessionService();
    bool removeConnection(const ConnectInfo& info,bool rstate = false);
    void addConnection(const ConnectInfo& info);
    void kickuser(std::string str);
    void checkAndKickLogin(const ConnectInfo& info);
    ConnectInfo checkHasLogin(int userid);
    void checkAlive();
    void updateAliveTime(int userid);
private:
    void removeAll(std::vector<int>& removeVec);
};
