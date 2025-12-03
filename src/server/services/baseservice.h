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
    struct ConnectInfo
    {
        int m_userid = -1;
        bool m_isOnline = false;
        bool m_isLocal = false;
        TcpConnectionPtr m_conn = nullptr;
        long long m_version = -1;
        int64_t m_lastheartTime = -1;
    };
protected:
    static UserDAO m_userdao;
    static FriendDAO m_frienddao;
    static GroupDAO m_groupdao;
    static OffineMessageDAO m_offlinemsgdao;
    static RedisTool m_redis;
public:
    BaseService(/* args */) = default;
    virtual ~BaseService() = default;
    json buildResponse(json& obj,MsgType type);
    int64_t getCurrentTimeMillis();
    json buildErrorResponse(ValidResult&& errmsg);
};

