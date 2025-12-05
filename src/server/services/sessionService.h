#pragma once
#include "baseservice.h"
#include <thread>
#include <atomic>
#include "callBacks.h"
// #include "redisTool.h"
class SessionService : public BaseService
{
public:
    using AddFriendCallBack = std::function<void(int,int)>;
private:
    std::unordered_map<int, struct ConnectInfo> m_clientsMap;
    std::unordered_map<TcpConnectionPtr, int> m_clientsMapPtr;
    std::mutex m_clientsmapMtx;
    std::atomic<bool> m_stop{false};
    std::thread m_aliveThread;
    
    AddFriendCallBack m_AddFriendCallBack;
public:
    SessionService(/* args */);
    ~SessionService();
    bool removeConnection(const ConnectInfo& info,bool rstate = false);
    void addConnection(const ConnectInfo& info);
    void kickuser(std::unordered_map<std::string,std::string>& paramMap);
    void checkAndKickLogin(const ConnectInfo& info);
    ConnectInfo checkHasLogin(int userid);
    void checkAlive();
    void updateAliveTime(int userid);
    void setAddFriendCallBack(const AddFriendCallBack& cb)
    {
        m_AddFriendCallBack = cb;
    }
private:
    void removeAll(std::vector<int>& removeVec);
    std::string m_servername;
    //RedisTool m_redis;
};
