#pragma once
#include "baseservice.h"
class MessageService : public BaseService
{
private:
    /* data */
public:
    MessageService(/* args */);
    ~MessageService() = default;
    void ChatOne(const TcpConnectionPtr &conn, json &js,int userid);
    void ChatGroup(const TcpConnectionPtr &conn, json &js,int userid);
    void handleRedisPublis(std::string, std::string);
};
