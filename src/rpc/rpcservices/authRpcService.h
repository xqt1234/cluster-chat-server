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
using json = nlohmann::json;
class AuthRpcService : public miniRpc::RpcService
{
private:
    UserDAO m_userdao;
    FriendDAO m_frienddao;
    GroupDAO m_groupdao;
    OffineMessageDAO m_offlinemsgdao;
public:
    AuthRpcService(/* args */);
    ~AuthRpcService() = default;
    void login(const std::string&res,std::function<void(std::string &response)> callback);
    std::string buildLoginInfo(json &js, User &user, bool loginbytoken);
};
