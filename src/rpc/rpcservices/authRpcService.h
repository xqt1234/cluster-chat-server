#pragma once
#include <string>
#include "rpcService.h"
#include "frienddao.h"
#include "user.h"
#include "userdao.h"
#include "group.h"
#include "groupdao.h"
#include "offlinemsgdao.h"
#include "json_fwd.hpp"
#include "tokenManager.h"
using json = nlohmann::json;
class AuthRpcService : public miniRpc::RpcService
{
private:
    UserDAO m_userdao;
    FriendDAO m_frienddao;
    GroupDAO m_groupdao;
    OffineMessageDAO m_offlinemsgdao;
    std::unique_ptr<TokenManager> m_tokenManager;
    RedisTool m_redis;
public:
    AuthRpcService(/* args */);
    ~AuthRpcService() = default;
    void login(const std::string&res,std::function<void(std::string &response)> callback);
    void tokenLogin(const std::string&res,std::function<void(std::string &response)> callback);
    void buildLoginInfo(json &js,json& resjs, User &user, bool isbytoken);
private:
    int verifyToken(std::string &str);
};
