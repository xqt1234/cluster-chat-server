#include "authRpcService.h"
#include "json.hpp"
#include "responseBuilder.h"
#include "Logger.h"
#include <functional>
AuthRpcService::AuthRpcService()
{
    m_tokenManager = std::make_unique<TokenManager>(m_redis.getRedis());
    addAsyncMethod("Login", [this](const std::string &request, std::function<void(std::string & response)> callback)
                   { this->login(request, std::move(callback)); });
    addAsyncMethod("LoginByToken", [this](const std::string &request, std::function<void(std::string & response)> callback)
                   { this->tokenLogin(request, std::move(callback)); });
    addAsyncMethod("RegistUser", [this](const std::string &request, std::function<void(std::string & response)> callback)
                   { this->registUser(request, std::move(callback)); });
}
// using RpcAsyncMethod = std::function<void(const std::string &request, std::function<void(std::string &response)> callback)>;
// void RpcService::addAsyncMethod(const std::string &name, RpcAsyncMethod method)
void AuthRpcService::login(const std::string &res, std::function<void(std::string &response)> callback)
{
    json js = json::parse(res);
    std::cout << "调用rpc的login方法" << res << std::endl;
    std::string password = js.value("password", "");
    int userid = js.value("userid", -1);
    if (password == "" || userid == -1)
    {
        json jsres{
            {"errcode", static_cast<int>(ErrType::USER_NOT_EXIST)},
            {"errmsg", "不存在该用户或者密码错误"}};
        std::string response = jsres.dump();
        callback(response);
        return;
    }
    User user = m_userdao.queryUser(userid);
    if (user.getId() != userid || user.getPassWord() != password)
    {
        json jsres{
            {"errcode", static_cast<int>(ErrType::USER_NOT_EXIST)},
            {"errmsg", "不存在该用户或者密码错误"}};
        std::string response = jsres.dump();
        callback(response);
        return;
    }
    json jsres;
    json jsdata;
    jsres["errcode"] = static_cast<int>(ErrType::SUCCESS);
    buildLoginInfo(js, jsdata, user, false);
    jsres["data"] = jsdata;
    std::string response = jsres.dump();
    callback(response);
}
void AuthRpcService::tokenLogin(const std::string &res, std::function<void(std::string &response)> callback)
{
    std::cout << "调用rpc的tokenLogin方法" << res << std::endl;
    json js = json::parse(res);
    std::string token = js.value("token", "");
    if (token == "")
    {
        json jsres{
            {"errcode", static_cast<int>(ErrType::MISSING_PARAM)},
            {"errmsg", "不存在该用户或者密码错误"}};
        std::string response = jsres.dump();
        callback(response);
        return;
    }
    int userid = verifyToken(token);
    if (userid == -1)
    {
        json jsres{
            {"errcode", static_cast<int>(ErrType::TOKEN_EXPIRED)},
            {"errmsg", "token过期"}};
        std::string response = jsres.dump();
        callback(response);
        return;
    }
    json jsres;
    json jsdata;
    User user = m_userdao.queryUser(userid);
    jsres["errcode"] = static_cast<int>(ErrType::SUCCESS);
    buildLoginInfo(js, jsdata, user, true);
    jsres["data"] = jsdata;
    std::string response = jsres.dump();
    callback(response);
}
void AuthRpcService::registUser(const std::string &request, std::function<void(std::string &response)> callback)
{
    std::cout << "调用rpc的registUser方法" << request << std::endl;
    json js = json::parse(request);
    User user;
    user.setUserName(js.value("username", std::string()));
    user.setPassWord(js.value("password", std::string()));
    bool res = m_userdao.insertUser(user);
    json jsres;
    if (res)
    {
        jsres["errcode"] = static_cast<int>(ErrType::SUCCESS);
        std::string devicename = js.value("device", "unknown");
        json jsdata{{"userid", user.getId()},
                    {"username", user.getUserName()},
                    {"token", m_tokenManager->generateToken(user.getId(), devicename)}};
        jsres["data"] = jsdata;
    }
    else
    {
        json jsdata{
            {"errcode", static_cast<int>(ErrType::DB_ERROR)},
            {"errmsg", "数据库错误"}};
        jsres["data"] = jsdata;
    }
    std::string response = jsres.dump();
    callback(response);
}
void AuthRpcService::buildLoginInfo(json &js, json &resjs, User &user, bool isbytoken)
{
    int userid = user.getId();
    json userinfo{
        {"userid", user.getId()},
        {"username", user.getUserName()}};
    resjs["userinfo"] = userinfo;
    std::vector<User> friends = m_frienddao.query(userid);
    std::vector<json> friendsobj;
    std::vector<int> friendvec;
    friendvec.reserve(friends.size());
    friendsobj.reserve(friends.size());
    for (auto &fuser : friends)
    {
        friendvec.push_back(fuser.getId());
        friendsobj.emplace_back(json{
            {"userid", fuser.getId()},
            {"username", fuser.getUserName()},
            {"state", fuser.getState()}});
    }
    resjs["friends"] = friendsobj;
    resjs["friendsids"] = friendvec;
    std::vector<Group> groups = m_groupdao.queryGroupsByUserId(user.getId());
    std::vector<json> groupstr;
    if (!groups.empty())
    {
        groupstr.reserve(groups.size());
        for (auto &tgroup : groups)
        {
            groupstr.emplace_back(json({{"groupid", tgroup.getId()},
                                        {"groupname", tgroup.getName()},
                                        {"groupdesc", tgroup.getDesc()}}));
        }
    }
    resjs["groups"] = groupstr;
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
    if (!isbytoken)
    {
        std::string devicename = js.value("device", "unknown");
        resjs["token"] = m_tokenManager->generateToken(userid, devicename);
    }
    resjs["offlinemsg"] = offline_array;
}

int AuthRpcService::verifyToken(std::string &str)
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

// 检查token，如果当前用户当前设备，有token记录，并且可用，返回。没有就生成。
// if (!loginbytoken)
// {
//     std::string devicename = js.value("device", "unknown");
//     resjs["token"] = m_tokenManager->generateToken(userid, devicename);
// }
// json sendjson = buildResponse(resjs, MsgType::MSG_LOGIN_ACK);
// conn->send(sendjson.dump());