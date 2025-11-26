#include "chatservice.h"
#include "Logger.h"
#include "relationCache.h"
#include <chrono>
#include "group.h"
using namespace std::placeholders;
using json = nlohmann::json;
ChatService::ChatService()
{
    m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN), std::bind(&ChatService::Login, this, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_REGISTER), std::bind(&ChatService::Register, this, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_PRIVATE_CHAT), std::bind(&ChatService::ChatOne, this, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_ADD_FRIEND), std::bind(&ChatService::addFriend, this, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_CREATE_GROUP), std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_JOIN_GROUP), std::bind(&ChatService::joinGroup, this, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN_BY_TOKEN), std::bind(&ChatService::LoginByToken, this, _1, _2, _3)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_GROUP_CHAT), std::bind(&ChatService::ChatGroup, this, _1, _2, _3)});
    m_redis.connect();
    m_tokenManager = std::make_unique<TokenManager>(m_redis.getRedis());
}

ChatService::~ChatService()
{
}

void ChatService::Login(const TcpConnectionPtr &conn, json &js, int tmpid)
{
    std::string password = js.value("password", "");
    int userid = js.value("userid", -1);
    if (password == "" || userid == -1)
    {
        json jsres = buildErrorResponse({true, ErrType::USER_NOT_EXIST, "不存在该用户或者密码错误"});
        conn->send(jsres.dump());
        return;
    }
    User user = m_userdao.queryUser(userid);
    if (user.getId() != userid || user.getPassWord() != password)
    {
        json jsres = buildErrorResponse({true, ErrType::USER_NOT_EXIST, "不存在该用户或者密码错误"});
        conn->send(jsres.dump());
        return;
    }
    // 在redis注册登录状态
    m_redis.getRedis().sadd(m_online_users_key, std::to_string(userid));
    m_redis.subscribe(userid);
    buildLoginInfo(conn, js, user, false);
}

void ChatService::LoginByToken(const TcpConnectionPtr &conn, json &js, int userid)
{
    User user = m_userdao.queryUser(userid);
    buildLoginInfo(conn, js, user, true);
}

void ChatService::Register(const TcpConnectionPtr &conn, json &js, int userid)
{

    User user;
    user.setUserName(js.value("username", std::string()));
    user.setPassWord(js.value("password", std::string()));
    bool res = m_userdao.insertUser(user);
    if (res)
    {
        std::string devicename = js.value("device", "unknown");
        json resjs{{"userid", user.getId()},
                   {"username", user.getUserName()},
                   {"token", m_tokenManager->generateToken(user.getId(), devicename)}};
        json sendjson = buildResponse(resjs, MsgType::MSG_LOGIN_ACK);
        conn->send(sendjson.dump());
        return;
    }
    else
    {
        json resjs = buildErrorResponse({false, ErrType::DB_ERROR, "插入数据库失败"});
        conn->send(resjs.dump());
    }
}

void ChatService::ChatOne(const TcpConnectionPtr &conn, json &js, int userid)
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
            m_redis.publish(toid, offlinemsg);
        }
    }
    else
    {
        // 存储离线消息
        m_offlinemsgdao.insert(toid, offlinemsg);
    }
}

void ChatService::ChatGroup(const TcpConnectionPtr &conn, json &js, int userid)
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
                m_redis.publish(toid, offlinemsg);
            }
        }
        else
        {
            // 存储离线消息
            m_offlinemsgdao.insert(toid, offlinemsg);
        }
    }
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, int userid)
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

    RelationCache::getInstance().addFriend(userid, friendid);
    User tfriend = m_userdao.queryUser(friendid);
    json resjson{
        {"msg", "添加好友成功"},
        {"friendid", friendid},
        {"friendname", tfriend.getUserName()}};
    json sendjs = buildResponse(resjson, MsgType::MSG_ADD_FRIEND_ACK);
    conn->send(sendjs.dump());
    auto it = m_clientsMap.find(friendid);
    if (it != m_clientsMap.end())
    {
        User user = m_userdao.queryUser(userid);
        json friendjs{
            {"friendid", userid},
            {"friendname", user.getUserName()},
            {"msg", std::string("被添加好友成功，对方是") + user.getUserName()}};
        json sendf = buildResponse(friendjs, MsgType::MSG_ADD_FRIEND_ACK);
        it->second->send(sendf.dump());
    }
}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, int userid)
{
    std::string groupname = js.value("groupname", "");
    std::string groupdesc = js.value("groupdesc", "");
    if (groupname == "" || groupdesc == "")
    {
        json jsres = buildErrorResponse({true, ErrType::INVALID_PARAMS, "群组参数错误"});
        conn->send(jsres.dump());
        return;
    }
    Group group(groupname, groupdesc);
    bool res = m_groupdao.createGroup(group, userid);
    if (!res)
    {
        json jsres = buildErrorResponse({true, ErrType::DB_ERROR, "创建群组失败"});
        conn->send(jsres.dump());
        return;
    }
    json jsres{
        {"msg", "创建群组成功"},
        {"groupid", group.getId()},
        {"groupname", group.getName()},
        {"groupdesc", group.getDesc()}};
    json sendf = buildResponse(jsres, MsgType::MSG_CREATE_GROUP_ACK);
    conn->send(sendf.dump());
}

void ChatService::joinGroup(const TcpConnectionPtr &conn, json &js, int userid)
{
    int groupid = js.value("groupid", -1);
    if (groupid == -1)
    {
        json jsres = buildErrorResponse({true, ErrType::GROUP_NOT_EXIST, "群组不存在"});
        conn->send(jsres.dump());
        return;
    }

    bool res = m_groupdao.addGroup(groupid, userid, "normal");
    if (!res)
    {
        json jsres = buildErrorResponse({true, ErrType::DB_ERROR, "加入群组失败"});
        conn->send(jsres.dump());
        return;
    }
    Group group = m_groupdao.queryGroupByGroupId(groupid);
    json jsres{
        {"msg", "加入群组成功"},
        {"groupid", group.getId()},
        {"groupname", group.getName()},
        {"groupdesc", group.getDesc()}};
    json sendf = buildResponse(jsres, MsgType::MSG_JOIN_GROUP_ACK);
    conn->send(sendf.dump());
}

MsgHandle ChatService::getHandler(int msgid)
{
    auto it = m_handlemap.find(msgid);
    if (it == m_handlemap.end())
    {
        auto func = [this](const TcpConnectionPtr &conn, json &js, int userid)
        {
            json jsres = buildErrorResponse({true, ErrType::PARAM_TYPE_ERROR, "参数类型错误"});
            conn->send(jsres.dump());
        };
        LOG_ERROR("参数类型错误{}", msgid);
        return func;
    }
    return it->second;
}

void ChatService::queryGroup(int userid, json &js)
{
    std::vector<Group> groups = m_groupdao.queryGroupsByUserId(userid);
    if (!groups.empty())
    {
        std::vector<json> groupstr;
        groupstr.reserve(groups.size());
        for (auto &tgroup : groups)
        {
            groupstr.emplace_back(json{
                {"groupid", tgroup.getId()},
                {"groupname", tgroup.getName()},
                {"groupdesc", tgroup.getDesc()}});
        }
        js["groups"] = groupstr;
    }
}

json ChatService::buildResponse(json &obj, MsgType type)
{
    json response = {
        {"msgid", static_cast<int>(type)},
        {"code", static_cast<int>(ErrType::SUCCESS)},
        {"timestamp", getCurrentTimeMillis()},
        {"message", "OK"},
        {"data", std::move(obj)}};
    return response;
}

void ChatService::buildLoginInfo(const TcpConnectionPtr &conn, json &js, User &user, bool loginbytoken)
{
    // auto it = m_clientsMap.find(user.getId());
    // if (it != m_clientsMap.end())
    // {
    //     it->second->shutdown();
    //     m_clientsMap.erase(it);
    // }
    int userid = user.getId();
    json resjs;
    json userinfo{
        {"userid", user.getId()},
        {"username", user.getUserName()}};
    resjs["userinfo"] = userinfo;
    std::vector<User> friends = m_frienddao.query(userid);
    if (!friends.empty())
    {
        std::vector<int> friendvec;
        friendvec.reserve(friends.size());
        std::vector<json> friendsobj;
        friendsobj.reserve(friends.size());
        for (auto &fuser : friends)
        {
            friendvec.push_back(fuser.getId());
            friendsobj.emplace_back(json{
                {"userid", fuser.getId()},
                {"username", fuser.getUserName()},
                {"state", fuser.getState()}});
        }
        RelationCache::getInstance().initFriends(user.getId(), friendvec);
        resjs["friends"] = friendsobj;
    }
    std::vector<Group> groups = m_groupdao.queryGroupsByUserId(user.getId());
    if (!groups.empty())
    {
        std::vector<json> groupstr;
        groupstr.reserve(groups.size());
        for (auto &tgroup : groups)
        {
            groupstr.emplace_back(json({{"groupid", tgroup.getId()},
                                        {"groupname", tgroup.getName()},
                                        {"groupdesc", tgroup.getDesc()}}));
        }
        resjs["groups"] = groupstr;
    }
    std::vector<std::string> offlinemsgs = m_offlinemsgdao.query(userid);
    m_offlinemsgdao.remove(userid);
    json offline_array = json::array();
    for (auto &str : offlinemsgs)
    {
        try
        {
            json tmpmsg = json::parse(str);
            offline_array.push_back(tmpmsg);
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("{}", e.what());
        }
    }
    resjs["offlinemsg"] = offline_array;
    m_clientsMap.insert({user.getId(), conn});
    m_clientsMapPtr.insert({conn, user.getId()});
    // 检查token，如果当前用户当前设备，有token记录，并且可用，返回。没有就生成。
    if (!loginbytoken)
    {
        std::string devicename = js.value("device", "unknown");
        resjs["token"] = m_tokenManager->generateToken(userid, devicename);
    }
    json sendjson = buildResponse(resjs, MsgType::MSG_LOGIN_ACK);
    conn->send(sendjson.dump());
}

int ChatService::checkToken(std::string &str)
{
    auto res = m_tokenManager->getUserIdByToken(str);
    if (res)
    {
        return res.value();
    }
    else
    {
        return -1;
    }
}

void ChatService::removeConnection(const TcpConnectionPtr &conn)
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
        m_redis.unsubscribe(userid);
    }
}

ChatService::ValidResult ChatService::checkValid(std::string &src, json &data)
{
    if (src.empty())
    {
        return {false, ErrType::MESSAGE_EMPTY, "请求数据为空"};
    }
    if (src.length() > MAX_JSON_LENGTH)
    {
        return {false, ErrType::MESSAGE_TOO_LONG, "请求数据过长"};
    }
    try
    {
        data = json::parse(src);
    }
    catch (const std::exception &e)
    {
        return {false, ErrType::INVALID_REQUEST, "json解析失败"};
    }

    if (data.is_null() || !data.is_object() || data.value("msgid", -1) == -1)
    {
        return {false, ErrType::PARAM_TYPE_ERROR, "消息类型错误"};
    }
    if (data["msgid"] == MsgType::MSG_LOGIN)
    {
        return {true, ErrType::SUCCESS, "登录，通过验证，不检验载荷"};
    }
    if (data["msgid"] != MsgType::MSG_LOGIN &&
        data["msgid"] != MsgType::MSG_REGISTER &&
        data.value("token", "") == "")
    {
        return {false, ErrType::TOKEN_EXPIRED, "没有token消息"};
    }
    if (!data.contains("data") || !data["data"].is_object())
    {
        return {false, ErrType::MESSAGE_EMPTY, "没有有效数据"};
    }
    return {true, ErrType::SUCCESS, ""};
}

inline long long ChatService::getCurrentTimeMillis()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               now.time_since_epoch())
        .count();
}

json ChatService::buildErrorResponse(ValidResult &&errmsg)
{
    json response = {
        {"msgid", static_cast<int>(MsgType::MSG_ERROR)},
        {"code", static_cast<int>(errmsg.errType)},
        {"timestamp", getCurrentTimeMillis()},
        {"message", std::move(errmsg.message)},
        {"data", json::object()}};
    return response;
}
