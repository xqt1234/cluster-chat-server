#include "sessionService.h"

void SessionService::removeConnection(const TcpConnectionPtr &conn)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    auto it = m_clientsMapPtr.find(conn);
    if (it != m_clientsMapPtr.end())
    {
        int userid = it->second;
        m_clientsMapPtr.erase(conn);
        m_clientsMap.erase(userid);
    }
}

void SessionService::removeConnection(int userid, const ConnInfo &info)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    auto it = m_clientsMap.find(userid);
    if (it != m_clientsMap.end())
    {
        // 如果版本号是-1，则直接删除，
        if (info.m_version != -1 && it->second.m_version != info.m_version)
        {
            return;
        }
        auto conn = it->second.m_conn;
        m_clientsMapPtr.erase(conn);
        m_clientsMap.erase(it);
    }
}

void SessionService::addConnection(const TcpConnectionPtr &conn, int userid, long long version)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    m_clientsMap[userid] = {conn, version};
    m_clientsMapPtr[conn] = userid;
}

SessionService::ConnInfo SessionService::getUser(int userid)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    auto it = m_clientsMap.find(userid);
    if (it != m_clientsMap.end())
    {
        return it->second;
    }
    else
    {
        return {nullptr, 0};
    }
}

TcpConnectionPtr SessionService::getConnection(int userid)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    auto it = m_clientsMap.find(userid);
    if (it != m_clientsMap.end())
    {
        return it->second.m_conn;
    }
    else
    {
        return nullptr;
    }
}

void SessionService::checkAndKickLogin(const TcpConnectionPtr &conn, json &js, int userid)
{
    // 检查是否已经登录，如果是，先在本地查，看是否是本服务器，本服务器，直接关闭连接。如果不是，向踢人频道发布消息。
    sw::redis::Redis &redis = m_redis.getRedis();
    std::string keyname = "userid" + std::to_string(userid);
    long long newVersion = redis.incr("newVersion");
    // 拼接，删除的时候，验证，如果是该版本，就删除
    std::string valuestr = m_kickchannelname + ":" + std::to_string(newVersion);
    // 本地有连接就删除
    removeConnection(userid, {nullptr, -1});

    auto resultvalue = redis.getset(keyname, valuestr);
    if (resultvalue.has_value())
    {
        std::string restr = resultvalue.value();
        int index = restr.find(":");
        std::string kickchannalname = restr.substr(0, index);
        m_redis.publish(kickchannalname, std::to_string(userid) + restr.substr(index));
    }
    std::string userchannal = "user:" + std::to_string(userid);
    m_redis.subscribe(userchannal);
    addConnection(conn, userid, newVersion);
}

void SessionService::kickuser(std::string str)
{
    int index = str.find(":");
    int userid = atoi(str.substr(0, index).c_str());
    long long versionid = atol(str.substr(index + 1).c_str());
    removeConnection(userid, {nullptr, versionid});
}

SessionService::ConnectInfo SessionService::checkHasLogin(int userid)
{
    {
        std::lock_guard<std::mutex> lock(m_clientsmapMtx);
        auto it = m_clientsMap.find(userid);
        if (it != m_clientsMap.end())
        {
            return {true,true,it->second.m_conn};
        }
    }
    sw::redis::Redis &redis = m_redis.getRedis();
    std::string keyname = "userid" + std::to_string(userid);
    auto resultvalue = redis.get(keyname);
    if(resultvalue.has_value())
    {
        return {true,false,nullptr};
    }
    return {false,false,nullptr};
}
