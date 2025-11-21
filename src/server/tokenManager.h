#pragma once
#include <openssl/rand.h>
#include <string>
#include "redisTool.h"
class TokenManager
{
private:
    sw::redis::Redis& m_redis;
public:
    TokenManager(sw::redis::Redis& redis):m_redis(redis){}
    ~TokenManager() = default;
    std::string generateToken(int userid);
    bool logout(std::string token);
    std::optional<int> getUserIdByToken(std::string &str);
};
