#include "frienddao.h"
#include "ConnectionPool.h"
std::vector<User> FriendDAO::query(int id)
{
    std::vector<User> vec;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return vec;
    }
    char buf[1024];
    
    snprintf(buf, sizeof(buf), "select a.id a.username from user a join groupuser b on a.id = b.userid where b.userid = %d;", id);
    MYSQL_RES *res = conn->query(buf);
    ;
    if (res != nullptr)
    {
        MYSQL_ROW row = nullptr;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setUserName(row[1]);
            vec.emplace_back(std::move(user));
        }
        mysql_free_result(res);
    }
    return vec;
}

bool FriendDAO::addFriend(int userid, int friendid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf), "insert into friend values(%d,%d)", userid,friendid);
    return conn->update(buf);
}

