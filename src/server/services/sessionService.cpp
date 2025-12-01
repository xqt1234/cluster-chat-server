#include "sessionService.h"
#include "Logger.h"
SessionService::SessionService()
{
    m_aliveThread = std::thread(std::bind(&SessionService::checkAlive, this));
}
SessionService::~SessionService()
{
    m_stop = true;
    if (m_aliveThread.joinable())
    {
        m_aliveThread.join();
    }
}

bool SessionService::removeConnection(const ConnectInfo &info, bool rstate)
{
    bool res = false;
    int userid = -1;
    if (info.m_conn != nullptr)
    {
        std::lock_guard<std::mutex> lock(m_clientsmapMtx);
        auto it = m_clientsMapPtr.find(info.m_conn);
        if (it != m_clientsMapPtr.end())
        {
            userid = it->second;
            m_clientsMapPtr.erase(info.m_conn);
            m_clientsMap.erase(userid);
            info.m_conn->shutdown();
            res = true;
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
                return false;
            }
            userid = info.m_userid;
            auto conn = it->second.m_conn;
            m_clientsMapPtr.erase(conn);
            m_clientsMap.erase(it);
            conn->shutdown();
            res = true;
        }
    }
    if (res)
    {
        std::string keyname = "userid:" + std::to_string(userid);
        std::string userchannal = "to:" + std::to_string(userid);
        m_redis.unsubscribe(userchannal);
        m_redis.getRedis().del(keyname);
    }
    return res;
}

void SessionService::addConnection(const ConnectInfo &info)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    m_clientsMap[info.m_userid] = info;
    m_clientsMap[info.m_userid].m_lastheartTime = getCurrentTimeMillis();
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
        LOG_DEBUG("发布踢人消息 key:{} value:{}", kickchannalname, std::to_string(userid) + restr.substr(index));
        m_redis.publish(kickchannalname, std::to_string(userid) + restr.substr(index));
    }
    else
    {
        LOG_DEBUG("不踢人，之前没有该用户");
    }
    // std::string userchannal = "user:" + std::to_string(userid);
    // m_redis.subscribe(userchannal);
    addConnection({userid, false, false, info.m_conn, newVersion});
}

void SessionService::kickuser(std::string str)
{
    int index = str.find(":");
    int userid = atoi(str.substr(0, index).c_str());
    long long versionid = atol(str.substr(index + 1).c_str());
    bool res = removeConnection({userid, false, false, nullptr, versionid});
    std::string userchannal = "to:" + std::to_string(userid);
    LOG_DEBUG("踢掉{},取消订阅{}", userid, userchannal);
    if (res)
    {
        // 踢人不用set在线状态，因为被别人已经设置了。
        m_redis.unsubscribe(userchannal);
    }
}

BaseService::ConnectInfo SessionService::checkHasLogin(int userid)
{
    {
        std::lock_guard<std::mutex> lock(m_clientsmapMtx);
        auto it = m_clientsMap.find(userid);
        if (it != m_clientsMap.end())
        {
            return {userid, true, true, it->second.m_conn};
        }
    }
    sw::redis::Redis &redis = m_redis.getRedis();
    std::string keyname = "userid:" + std::to_string(userid);
    auto resultvalue = redis.get(keyname);
    if (resultvalue.has_value())
    {
        return {userid, true, false, nullptr};
    }
    return {userid, false, false, nullptr};
}

void SessionService::checkAlive()
{
    std::vector<int> kickVec;
    while (!m_stop.load())
    {
        int64_t currentTime = getCurrentTimeMillis();

        for (auto it = m_clientsMap.begin(); it != m_clientsMap.end(); ++it)
        {
            if (currentTime - it->second.m_lastheartTime > 20 * 1000)
            {
                kickVec.push_back(it->first);
            }
        }
        if (!kickVec.empty())
        {
            removeAll(kickVec);
            for (int &userid : kickVec)
            {
                std::string userchannal = "to:" + std::to_string(userid);
                std::string keyname = "userid:" + std::to_string(userid);
                LOG_DEBUG("心跳超时，踢掉{},取消订阅{}", userid, userchannal);
                m_redis.unsubscribe(userchannal);
                m_redis.getRedis().del(keyname);
            }
            kickVec.clear();
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void SessionService::updateAliveTime(int userid)
{
    m_clientsMap[userid].m_lastheartTime = getCurrentTimeMillis();
    ;
}

void SessionService::removeAll(std::vector<int> &removeVec)
{
    std::lock_guard<std::mutex> lock(m_clientsmapMtx);
    for (int &userid : removeVec)
    {
        auto it = m_clientsMap.find(userid);
        if (it != m_clientsMap.end())
        {
            auto conn = it->second.m_conn;
            m_clientsMapPtr.erase(conn);
            m_clientsMap.erase(it);
            conn->shutdown();
        }
    }
}
