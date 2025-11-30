#include "authService.h"
#include "chatservice.h"
#include "userdao.h"
#include "Logger.h"
#include "config.h"
AuthService::AuthService()
{
    m_redis.connect();
    m_kickchannelname = "kick:" + Config::getInstance().getValue("servername","server01");
    m_redis.subscribe(m_kickchannelname);
    m_tokenManager = std::make_unique<TokenManager>(m_redis.getRedis());
}
void AuthService::login(const TcpConnectionPtr &conn, json &js, int tmpid)
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
    if(m_CheckCallBack)
    {
        m_CheckCallBack({userid,false,false,conn});
    }
    buildLoginInfo(conn, js, user, false);
}

void AuthService::LoginByToken(const TcpConnectionPtr &conn, json &js, int userid)
{
    User user = m_userdao.queryUser(userid);
    if(m_CheckCallBack)
    {
        m_CheckCallBack({userid,false,false,conn});
    }
    buildLoginInfo(conn, js, user, true);
}

void AuthService::registerUser(const TcpConnectionPtr &conn, json &js, int tmpid)
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

int AuthService::verifyToken(std::string &str)
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


void AuthService::buildLoginInfo(const TcpConnectionPtr &conn, json &js, User &user, bool loginbytoken)
{
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

    // 检查token，如果当前用户当前设备，有token记录，并且可用，返回。没有就生成。
    if (!loginbytoken)
    {
        std::string devicename = js.value("device", "unknown");
        resjs["token"] = m_tokenManager->generateToken(userid, devicename);
    }
    json sendjson = buildResponse(resjs, MsgType::MSG_LOGIN_ACK);
    conn->send(sendjson.dump());
}


