#include "sessionService.h"

void SessionService::removeConnection(const TcpConnectionPtr &conn)
{
    auto it = m_clientsMapPtr.find(conn);
    if (it != m_clientsMapPtr.end())
    {
        int userid = it->second;
        auto userit = m_clientsMap.find(userid);
        m_clientsMapPtr.erase(it);
        if (userit != m_clientsMap.end())
        {
            m_clientsMap.erase(userit);
        }
        // 从redis中删除，就不在线了
        m_redis.getRedis().srem(m_online_users_key, std::to_string(userid));
        m_redis.unsubscribe(std::to_string(userid));
    }
}