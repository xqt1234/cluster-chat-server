#include "messageService.h"
#include "json.hpp"
using json = nlohmann::json;
MessageService::MessageService()
{
    m_redis.init_notify_handle(std::bind(&MessageService::handleRedisPublis, this, _1, _2));
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

    if (m_getConn)
    {
        ConnectInfo info = m_getConn(toid);
        if (info.m_isLocal)
        {
            info.m_conn->send(sendjson.dump());
        }
        else if (info.m_isOnline)
        {
            m_redis.publish(std::to_string(toid), offlinemsg);
        }
        else
        {
            // 存储离线消息
            m_offlinemsgdao.insert(toid, offlinemsg);
        }
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

        if (m_getConn)
        {
            ConnectInfo info = m_getConn(toid);
            if (info.m_isLocal)
            {
                info.m_conn->send(sendjson.dump());
            }
            else if (info.m_isOnline)
            {
                m_redis.publish(std::to_string(toid), offlinemsg);
            }
            else
            {
                // 存储离线消息
                m_offlinemsgdao.insert(toid, offlinemsg);
            }
        }
    }
}

void MessageService::handleRedisPublis(std::string key, std::string value)
{
    if (key == m_kickchannelname)
    {
        m_kickcallBack(value);
        // int kickid = atoi(value.c_str());
        // auto it = m_clientsMap.find(kickid);
        // if(it != m_clientsMap.end())
        // {
        //     auto conn = it->second;
        //     m_clientsMapPtr.erase(conn);
        //     m_clientsMap.erase(it);
        //     conn->connectDestroyed();
        //     // 移除订阅
        //     std::string userchannal = "user:" + std::to_string(kickid);
        //     m_redis.unsubscribe(userchannal);
        // }
    }
    else if (key == "userid")
    {
    }
}
