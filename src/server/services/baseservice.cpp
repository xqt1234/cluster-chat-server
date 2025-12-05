#include "baseservice.h"

// 定义静态成员变量
UserDAO BaseService::m_userdao;
FriendDAO BaseService::m_frienddao;
GroupDAO BaseService::m_groupdao;
OffineMessageDAO BaseService::m_offlinemsgdao;
RedisTool BaseService::m_redis;
RelationCache BaseService::m_RelationCache;
BaseService::BaseService()
{
}
json BaseService::buildResponse(json &obj, MsgType type)
{
    json response = {
        {"msgid", static_cast<int>(type)},
        {"code", static_cast<int>(ErrType::SUCCESS)},
        {"timestamp", getCurrentTimeMillis()},
        {"message", "OK"},
        {"data", std::move(obj)}};
    return response;
}

int64_t BaseService::getCurrentTimeMillis()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               now.time_since_epoch())
        .count();
}

json BaseService::buildErrorResponse(ValidResult &&errmsg)
{
    json response = {
        {"msgid", static_cast<int>(MsgType::MSG_ERROR)},
        {"code", static_cast<int>(errmsg.errType)},
        {"timestamp", getCurrentTimeMillis()},
        {"message", std::move(errmsg.message)},
        {"data", json::object()}};
    return response;
}
