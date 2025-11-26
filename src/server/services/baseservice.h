#pragma once
#include "json.hpp"
#include "public.h"
#include "userdao.h"
#include "frienddao.h"
#include "groupdao.h"
#include "offlinemsgdao.h"
#include "redisTool.h"
#include "TcpConnection.h"
#include "relationCache.h"
#include "tokenManager.h"
using json = nlohmann::json;

class BaseService
{
public:
    struct ValidResult
    {
        bool success;
        ErrType errType;
        std::string message;
    };
protected:
    static UserDAO m_userdao;
    static FriendDAO m_frienddao;
    static GroupDAO m_groupdao;
    static OffineMessageDAO m_offlinemsgdao;
    static RedisTool m_redis;
    static std::string m_kickchannelname;
    static const std::string m_online_users_key;
    static std::unordered_map<int, TcpConnectionPtr> m_clientsMap;
    static std::unordered_map<TcpConnectionPtr, int> m_clientsMapPtr;
    static std::mutex m_clientsmapMtx;
    std::unique_ptr<TokenManager> m_tokenManager;
public:
    BaseService(/* args */) = default;
    virtual ~BaseService() = default;
    json buildResponse(json& obj,MsgType type);
    inline long long getCurrentTimeMillis();
    json buildErrorResponse(ValidResult&& errmsg);
};

