#pragma once
#include "baseservice.h"
class GroupService : public BaseService
{
private:
    /* data */
public:
    GroupService(/* args */) = default;
    ~GroupService() = default;
    void joinGroup(const TcpConnectionPtr &conn, json &js,int userid);
    void createGroup(const TcpConnectionPtr &conn, json &js,int userid);
    void queryGroup(int userid, json &js);
};
