#include "sessionService.h"

void SessionService::removeConnection(const ConnectInfo &info)
{
    if (info.m_conn != nullptr)
    {
        std::lock_guard<std::mutex> lock(m_clientsmapMtx);
        auto it = m_clientsMapPtr.find(info.m_conn);
        if (it != m_clientsMapPtr.end())
        {
            int userid = it->second;
            m_clientsMapPtr.erase(info.m_conn);
            m_clientsMap.erase(userid);
            info.m_conn->shutdown();
        }
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_clientsmapMtx);
        auto it = m_clientsMap.find(info.m_userid);
        if (it != m_clientsMap.end())
        {
            // 有版本号，且版本号相同才删除
            if (info.m_version != -1 && it->second.m_version != info.m_version)
            {
                return;
            }
            auto conn = it->second.m_conn;
            m_clientsMapPtr.erase(conn);
            m_clientsMap.erase(it);
            conn->shutdown();
        }
    }
}

void SessionService::addConnection(const ConnectInfo &info)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    m_clientsMap[info.m_userid] = info;
    m_clientsMapPtr[info.m_conn] = info.m_userid;
}

void SessionService::checkAndKickLogin(const ConnectInfo &info)
{
    int userid = info.m_userid;
    // 检查是否已经登录，如果是，先在本地查，看是否是本服务器，本服务器，直接关闭连接。如果不是，向踢人频道发布消息。
    sw::redis::Redis &redis = m_redis.getRedis();
    std::string keyname = "userid:" + std::to_string(userid);
    long long newVersion = redis.incr("newVersion");
    // 拼接，删除的时候，验证，如果是该版本，就删除
    std::string valuestr = m_kickchannelname + ":" + std::to_string(newVersion);
    // 本地有连接就删除
    removeConnection({userid, false, false, nullptr, -1});

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
    addConnection({userid,false,false,info.m_conn, newVersion});
}

void SessionService::kickuser(std::string str)
{
    int index = str.find(":");
    int userid = atoi(str.substr(0, index).c_str());
    long long versionid = atol(str.substr(index + 1).c_str());
    removeConnection({userid,false, false, nullptr, versionid});
}

BaseService::ConnectInfo SessionService::checkHasLogin(int userid)
{
    {
        std::lock_guard<std::mutex> lock(m_clientsmapMtx);
        auto it = m_clientsMap.find(userid);
        if (it != m_clientsMap.end())
        {
            return {userid,true, true, it->second.m_conn};
        }
    }
    sw::redis::Redis &redis = m_redis.getRedis();
    std::string keyname = "userid:" + std::to_string(userid);
    auto resultvalue = redis.get(keyname);
    if (resultvalue.has_value())
    {
        return {userid,true, false, nullptr};
    }
    return {userid,false, false, nullptr};
}
