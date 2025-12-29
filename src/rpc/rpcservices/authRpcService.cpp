#include "authRpcService.h"
#include "json.hpp"
#include "responseBuilder.h"
#include "Logger.h"
#include <functional>
AuthRpcService::AuthRpcService()
{
    addAsyncMethod("login",[this](const std::string& request,std::function<void(std::string& response)> callback){
        this->login(request,std::move(callback));
    });
}
//using RpcAsyncMethod = std::function<void(const std::string &request, std::function<void(std::string &response)> callback)>;
//void RpcService::addAsyncMethod(const std::string &name, RpcAsyncMethod method)
void AuthRpcService::login(const std::string &res, std::function<void(std::string &response)> callback)
{
    json js = json::parse(res);
    std::cout << "调用rpc的login方法" << res << std::endl;
    std::string password = js.value("password", "");
    int userid = js.value("userid", -1);
    if (password == "" || userid == -1)
    {
        json jsres = ResponseBuilder::buildErrorResponse({true, ErrType::USER_NOT_EXIST, "不存在该用户或者密码错误"});
        std::string response = jsres.dump();
        callback(response);
        return;
    }
    User user = m_userdao.queryUser(userid);
    if (user.getId() != userid || user.getPassWord() != password)
    {
        json jsres = ResponseBuilder::buildErrorResponse({true, ErrType::USER_NOT_EXIST, "不存在该用户或者密码错误"});
        std::string response = jsres.dump();
        callback(response);
        return;
    }
    std::string response = std::move(buildLoginInfo(js, user, false));
    callback(response);
}
std::string AuthRpcService::buildLoginInfo(json &js, User &user, bool loginbytoken)
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
        //m_RelationCache.initFriends(user.getId(), friendvec,ResponseBuilder::getCurrentTimeMillis());
        resjs["friends"] = friendsobj;
        resjs["friendsids"] = friendvec;
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
    return resjs.dump();
}

// 检查token，如果当前用户当前设备，有token记录，并且可用，返回。没有就生成。
    // if (!loginbytoken)
    // {
    //     std::string devicename = js.value("device", "unknown");
    //     resjs["token"] = m_tokenManager->generateToken(userid, devicename);
    // }
    // json sendjson = buildResponse(resjs, MsgType::MSG_LOGIN_ACK);
    // conn->send(sendjson.dump());