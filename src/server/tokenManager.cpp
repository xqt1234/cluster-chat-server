#include "tokenManager.h"
using namespace sw::redis;


std::string TokenManager::generateToken(int userid)
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
    m_redis.setex("token:" + token, 3600, std::to_string(userid));
    // 存储用户的所有 token（用于批量管理）
    // redis.sadd("user_tokens:" + std::to_string(userid), token);
    // redis.expire("user_tokens:" + std::to_string(userid), 3600);
    return token;
}

bool TokenManager::logout(std::string token)
{
    auto userid = getUserIdByToken(token);
    if (!userid)
        return false;
    // 删除 token
    m_redis.del("token:" + token);
    return true;
}

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
