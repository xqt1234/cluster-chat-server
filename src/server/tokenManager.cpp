#include "tokenManager.h"
#include <iostream>
using namespace sw::redis;


std::string TokenManager::generateToken(int userid,std::string& devicename)
{
    unsigned char buffer[32];
    RAND_bytes(buffer, sizeof(buffer));

    static const char *hex = "0123456789abcdef";
    std::string token;
    token.reserve(64);
    for (int i = 0; i < 32; ++i)
    {
        token.push_back(hex[(buffer[i] >> 4) & 0xF]);
        token.push_back(hex[buffer[i] & 0xF]);
    }
    //std::string key = "user_tokens:" + std::to_string(userid);

    // 删除旧token
    // auto oldtoken = m_redis.get(key);
    // if(oldtoken.has_value())
    // {
    //     m_redis.del(oldtoken.value());
    // }
    //m_redis.setex("token:" + token, 3600, std::to_string(userid));
    m_redis.setex("token:" + token, 3600, std::to_string(userid));
    // 存储用户的所有 token（用于批量管理）
    //m_redis.setex("user_tokens:" + userid, 3600, std::to_string(userid));
    // 同用户同设备，自动覆盖,记录token，下次生成的时候删除旧token
    //m_redis.set("user_tokens:" + std::to_string(userid) + ":" + devicename, token);
    // redis.expire("user_tokens:" + std::to_string(userid), 3600);
    return token;
}

// void TokenManager::removeToken(int userid, std::string &devicename)
// {
//     m_redis.del("user_tokens:" + std::to_string(userid) + ":" + devicename);
// }

// bool TokenManager::logout(std::string token)
// {
//     auto userid = getUserIdByToken(token);
//     if (!userid)
//         return false;
//     // 删除 token
//     m_redis.del("token:" + token);
//     return true;
// }

std::optional<int> TokenManager::getUserIdByToken(std::string &token)
{
    std::optional<std::string> result = m_redis.get("token:" + token);
    if (result.has_value())
    {
        m_redis.expire("token:" + token, 3600);
        return std::atoi(result.value().c_str());
    }
    return std::nullopt;
}
