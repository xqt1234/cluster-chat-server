#pragma once
#include "baseservice.h"
class FriendService : public BaseService
{
private:
    /* data */
public:
    FriendService(/* args */) = default;
    ~FriendService() = default;
    void addFriend(const TcpConnectionPtr &conn, json &js,int userid);
};

