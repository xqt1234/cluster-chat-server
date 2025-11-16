#include "chatservice.h"
#include "Logger.h"
#include "relationCache.h"
using namespace std::placeholders;
using json = nlohmann::json;
ChatService::ChatService()
{
    m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN), std::bind(&ChatService::Login, this, _1, _2)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_REG), std::bind(&ChatService::Register, this, _1, _2)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_CHAT_ONE), std::bind(&ChatService::ChatOne, this, _1, _2)});
    m_handlemap.insert({static_cast<int>(MsgType::MSG_ADD_FRIEND), std::bind(&ChatService::addFriend, this, _1, _2)});

    // m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN),std::bind(&ChatService::Login,this,_1,_2)});
    // m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN),std::bind(&ChatService::Login,this,_1,_2)});
}

ChatService::~ChatService()
{
}

void ChatService::Login(const TcpConnectionPtr &conn, json &js)
{
    int id = js["userid"];
    std::string password = js["password"];
    User user = m_userdao.queryUser(id);
    std::cout << password << " " << user.getPassWord() << " " << id << " " << user.getId() << std::endl;
    if (user.getId() != id || user.getPassWord() != password)
    {
        json jsres;
        jsres["msgid"] = static_cast<int>(MsgType::MSG_LOGIN_ACK);
        jsres["errno"] = 1;
        jsres["errmsg"] = "不存在该用户或者密码错误";
        jsres["id"] = user.getId();
        conn->send(jsres.dump());
        return;
    }

    json resjs;
    resjs["msgid"] = static_cast<int>(MsgType::MSG_LOGIN_ACK);
    resjs["errno"] = 0;
    resjs["userid"] = user.getId();
    resjs["state"] = user.getState(); // 等redis，使另外一个连接下线。
    resjs["username"] = user.getUserName();
    std::vector<User> friends = m_frienddao.query(id);
    if (!friends.empty())
    {
        std::vector<int> friendvec;
        std::vector<std::string> friendsstr;
        for (auto &fuser : friends)
        {
            json userjs;
            userjs["userid"] = fuser.getId();
            friendvec.push_back(fuser.getId());
            userjs["username"] = fuser.getUserName();
            userjs["state"] = fuser.getState();
            friendsstr.push_back(userjs.dump());
        }
        RelationCache::getInstance().initFriends(user.getId(), friendvec);
        resjs["friends"] = friendsstr;
    }
    std::vector<Group> groups = m_groupdao.queryGroup(user.getId());
    if (!groups.empty())
    {
        std::vector<std::string> groupstr;
        for (auto &tgroup : groups)
        {
            json ttgroup;
            ttgroup["groupid"] = tgroup.getId();
            ttgroup["groupname"] = tgroup.getName();
            ttgroup["groupdesc"] = tgroup.getDesc();
            groupstr.push_back(ttgroup.dump());
        }
        resjs["groups"] = groupstr;
    }
    m_clientsMap.insert({user.getId(), conn});
    conn->send(resjs.dump());
}

void ChatService::Register(const TcpConnectionPtr &conn, json &js)
{

    std::cout << "注册账号" << std::endl;
    User user;
    user.setUserName(js["username"]);
    user.setPassWord(js["password"]);
    bool res = m_userdao.inserUser(user);
    if (res)
    {
        json resjson;
        resjson["userid"] = user.getId();
        resjson["errno"] = 0;
        resjson["username"] = user.getUserName();
        resjson["msgid"] = static_cast<int>(MsgType::MSG_LOGIN_ACK);
        conn->send(resjson.dump());
    }
    else
    {
        json resjson;
        resjson["userid"] = user.getId();
        resjson["errno"] = 1;
        resjson["msgid"] = static_cast<int>(MsgType::MSG_LOGIN_ACK);
        conn->send(resjson.dump());
    }
}

void ChatService::ChatOne(const TcpConnectionPtr &conn, json &js)
{
    int userid = js["userid"];
    int toid = js["toid"];
    bool isfriend = RelationCache::getInstance().isFriend(userid, toid);
    if (!isfriend)
    {
        json resjson;
        resjson["userid"] = userid;
        resjson["errno"] = 1;
        resjson["errmsg"] = "对方还是不是你的好友";
        resjson["msgid"] = static_cast<int>(MsgType::MSG_CHAT_ONE);
        conn->send(resjson.dump());
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_clientsmapMtx);
        auto it = m_clientsMap.find(toid);
        if (it != m_clientsMap.end())
        {
            it->second->send(js.dump());
            return;
        }
    }
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js)
{
    int userid = js["userid"];
    int friendid = js["friendid"];
    bool res = (m_frienddao.isFriend(userid, friendid));
    json resjson;
    if (res)
    {
        json resjson;
        resjson["userid"] = userid;
        resjson["errno"] = 1;
        resjson["errmsg"] = "添加好友失败，对方已经是你的好友";
        resjson["msgid"] = static_cast<int>(MsgType::MSG_ADD_FRIEND);
        conn->send(resjson.dump());
        return;
    }
    else
    {
        res = m_frienddao.addFriend(userid, friendid);
        if (!res)
        {
            json resjson;
            resjson["userid"] = userid;
            resjson["errno"] = 1;
            resjson["errmsg"] = "添加好友失败";
            resjson["msgid"] = static_cast<int>(MsgType::MSG_ADD_FRIEND);
            conn->send(resjson.dump());
            return;
        }
    }

    RelationCache::getInstance().addFriend(userid, friendid);
    User tfriend = m_userdao.queryUser(friendid);
    resjson["userid"] = userid;
    resjson["msgid"] = static_cast<int>(MsgType::MSG_ADD_FRIEND);
    resjson["errno"] = 0;
    resjson["msg"] = "添加好友成功";
    resjson["friendid"] = friendid;
    resjson["friendname"] = tfriend.getUserName();
    conn->send(resjson.dump());
    auto it = m_clientsMap.find(friendid);
    if (it != m_clientsMap.end())
    {
        json friendjs;
        friendjs["userid"] = friendid;
        friendjs["msgid"] = static_cast<int>(MsgType::MSG_ADD_FRIEND);
        friendjs["errno"] = 0;
        std::string username = js["username"];
        friendjs["msg"] = std::string("被添加好友成功，对方是") + username;
        friendjs["friendid"] = userid;
        friendjs["friendname"] = js["username"];
        it->second->send(friendjs.dump());
    }
}

MsgHandle ChatService::getHandler(int msgid)
{
    auto it = m_handlemap.find(msgid);
    if (it == m_handlemap.end())
    {
        auto func = [=](const TcpConnectionPtr &conn, json &js)
        {
            LOG_ERROR("消息类型有误{}", msgid);
        };
        return func;
    }
    return it->second;
}
