#pragma once
#include "baseservice.h"
#include <vector>
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
    void initGroupInRedis();
    std::vector<int> getGroupUsers(int groupid);
    void addToRedisGroup(int groupid,int userid);
};
