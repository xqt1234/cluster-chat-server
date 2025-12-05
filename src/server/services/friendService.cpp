#include "friendService.h"
#include "sessionService.h"
#include <functional>
#include <iostream>
using namespace sw::redis;
using Attrs = std::unordered_map<std::string, std::string>;
using Item = std::pair<std::string, Optional<Attrs>>;
using ItemStream = std::vector<Item>;
FriendService::FriendService()
{
    m_redis.addCommand("addfriend",std::bind(&FriendService::addFriendToLocal,this,std::placeholders::_1));
}
void FriendService::addFriend(const TcpConnectionPtr &conn, json &js, int userid)
{
    int friendid = js.value("friendid", -1);
    if (userid == -1 || friendid == -1)
    {
        json jsres = buildErrorResponse({true, ErrType::INVALID_PARAMS, "用户参数错误"});
        conn->send(jsres.dump());
        return;
    }
    bool res = (m_frienddao.isFriend(userid, friendid));
    if (res)
    {
        json jsres = buildErrorResponse({true, ErrType::FRIEND_ALREADY_EXISTS, "添加好友失败，对方已经是你的好友"});
        conn->send(jsres.dump());
        return;
    }
    else
    {
        res = m_frienddao.addFriend(userid, friendid);
        if (!res)
        {
            json jsres = buildErrorResponse({true, ErrType::DB_ERROR, "数据库插入错误，添加好友失败"});
            conn->send(jsres.dump());
            return;
        }
    }
    m_RelationCache.addFriend(userid, friendid);
    User tfriend = m_userdao.queryUser(friendid);
    json resjson{
        {"msg", "添加好友成功"},
        {"friendid", friendid},
        {"friendname", tfriend.getUserName()}};
    json sendjs = buildResponse(resjson, MsgType::MSG_ADD_FRIEND_ACK);
    conn->send(sendjs.dump());

    User user = m_userdao.queryUser(userid);
    json friendjs{
        {"friendid", userid},
        {"friendname", user.getUserName()},
        {"msg", std::string("被添加好友成功，对方是") + user.getUserName()}};
    json sendf = buildResponse(friendjs, MsgType::MSG_ADD_FRIEND_ACK);
    if (m_getConn)
    {
        ConnectInfo info = m_getConn(friendid);
        if (info.m_isLocal)
        {
            m_RelationCache.addFriend(friendid,userid);
            info.m_conn->send(sendf.dump());
        }
        else if (info.m_isOnline)
        {
            std::string userchannal = "to:" + std::to_string(friendid);
            m_redis.publish(userchannal, sendf.dump());
            Attrs attrs = {{"userid", std::to_string(userid)},
                           {"cmd", "addfriend"},
                           {"friendid",std::to_string(friendid)},
                           {"servername", info.m_servername}};
            m_redis.addToStream(attrs);
        }
        else
        {
            // 存储离线消息
            m_offlinemsgdao.insert(friendid, sendf.dump());
        }
    }
}

void FriendService::addFriendToLocal(std::unordered_map<std::string,std::string>& paramMap)
{
    
    int userid = atoi(paramMap["friendid"].c_str());
    int friendid = atoi(paramMap["userid"].c_str());
    // std::cout << "通过redis stream添加好友" << "userid:" << userid << " friendid" << friendid << std::endl;
    m_RelationCache.addFriend(userid,friendid);
}
