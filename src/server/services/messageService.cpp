#include "messageService.h"

MessageService::MessageService()
{
    m_redis.init_notify_handle(std::bind(&MessageService::handleRedisPublis,this,_1,_2));
}

void MessageService::ChatOne(const TcpConnectionPtr &conn, json &js, int userid)
{
    int toid = js.value("toid", -1);
    if (userid == -1 || toid == -1)
    {
        json jsres = buildErrorResponse({true, ErrType::INVALID_PARAMS, "用户参数错误"});
        conn->send(jsres.dump());
        return;
    }
    bool isfriend = RelationCache::getInstance().isFriend(userid, toid);
    if (!isfriend)
    {
        json jsres = buildErrorResponse({true, ErrType::FRIEND_NOT_EXIST, "对方还是不是你的好友"});
        conn->send(jsres.dump());
        return;
    }
    json resjs{
        {"msg", js["msg"]},
        {"msgid", static_cast<int>(MsgType::MSG_PRIVATE_CHAT_ACK)},
        {"fromid", userid}};
    std::string offlinemsg = resjs.dump();
    json sendjson = buildResponse(resjs, MsgType::MSG_PRIVATE_CHAT_ACK);
    // 先在redis查询是否在线,本地是否有连接
    bool isonline = m_redis.getRedis().sismember(m_online_users_key, std::to_string(userid));
    if (isonline)
    {
        TcpConnectionPtr targetConn;
        {
            std::lock_guard<std::mutex> lock(m_clientsmapMtx);
            auto it = m_clientsMap.find(toid);
            if (it != m_clientsMap.end())
            {
                targetConn = it->second;
            }
        }
        if (targetConn)
        {
            targetConn->send(sendjson.dump());
        }
        else
        {
            m_redis.publish(std::to_string(toid), offlinemsg);
        }
    }
    else
    {
        // 存储离线消息
        m_offlinemsgdao.insert(toid, offlinemsg);
    }
}

void MessageService::ChatGroup(const TcpConnectionPtr &conn, json &js, int userid)
{
    int groupid = js.value("groupid", -1);
    std::string str = js.value("msg", "");
    if (groupid == -1 || str == "")
    {
        json jsres = buildErrorResponse({true, ErrType::INVALID_PARAMS, "用户参数错误"});
        conn->send(jsres.dump());
        return;
    }
    json resjs{
        {"msg", js["msg"]},
        {"msgid", static_cast<int>(MsgType::MSG_GROUP_CHAT_ACK)},
        {"groupid", groupid},
        {"userid", userid}};
    std::string offlinemsg = resjs.dump();
    json sendjson = buildResponse(resjs, MsgType::MSG_GROUP_CHAT_ACK);
    std::unordered_set<int> userset = RelationCache::getInstance().getAllUserFromGroup(groupid);
    // 后期需要替换存储方式。小群的时候，采用写扩散方案
    for (int toid : userset)
    {
        if (toid == userid)
        {
            continue;
        }
        // 先检查本地，如果有，直接发送，如果没有，检查是否在redis中在线，在线就发送redis的频带，不在线就存储离线消息
        bool isonline = m_redis.getRedis().sismember(m_online_users_key, std::to_string(userid));
        if (isonline)
        {
            TcpConnectionPtr targetConn;
            {
                std::lock_guard<std::mutex> lock(m_clientsmapMtx);
                auto it = m_clientsMap.find(toid);
                if (it != m_clientsMap.end())
                {
                    targetConn = it->second;
                }
            }
            if (targetConn)
            {
                targetConn->send(sendjson.dump());
            }
            else
            {
                m_redis.publish(std::to_string(toid), offlinemsg);
            }
        }
        else
        {
            // 存储离线消息
            m_offlinemsgdao.insert(toid, offlinemsg);
        }
    }
}

void MessageService::handleRedisPublis(std::string key, std::string value)
{
    if(key == m_kickchannelname)
    {
        int kickid = atoi(value.c_str());
        auto it = m_clientsMap.find(kickid);
        if(it != m_clientsMap.end())
        {
            auto conn = it->second;
            m_clientsMapPtr.erase(conn);
            m_clientsMap.erase(it);
            conn->connectDestroyed();
            // 移除订阅
            std::string userchannal = "user:" + std::to_string(kickid);
            m_redis.unsubscribe(userchannal);
        }
    }else if(key == "userid")
    {

    }
}
