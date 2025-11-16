#include "frienddao.h"
#include "connectionPool.h"
#include "user.h"
#include <iostream>
std::vector<User> FriendDAO::query(int id)
{
    std::vector<User> vec;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return vec;
    }
    char buf[1024];

    snprintf(buf, sizeof(buf), "select a.id,a.username from user a join friend b on a.id = b.userid where b.userid = %d;", id);
    DbRes res = conn->query(buf);
    if (res != nullptr)
    {
        MYSQL_ROW row = nullptr;
        while ((row = mysql_fetch_row(res.get())) != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setUserName(row[1]);
            vec.emplace_back(std::move(user));
        }
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
    conn->beginTransaction();
    char buf[1024];
    snprintf(buf, sizeof(buf), "insert into friend values(%d,%d)", userid, friendid);
    char buf2[1024];
    snprintf(buf2, sizeof(buf2), "insert into friend values(%d,%d)", friendid, userid);
    conn->update(buf);
    if (!conn->update(buf))
    {
        conn->rollback();
        return false;
    }
    if (!conn->update(buf2))
    {
        conn->rollback();
        return false;
    }
    conn->commit();
    return true;
}

bool FriendDAO::isFriend(int userid, int friendid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf), "select 1 from friend where userid=%d and friendid=%d limit 1;", userid, friendid);
    std::cout << buf << std::endl;
    DbRes res = conn->query(buf);
    if (res == nullptr)
    {
        return false;
    }
    MYSQL_ROW row = nullptr;
    bool exist = false;
    while((row = mysql_fetch_row(res.get())) != nullptr)
    {
        exist = true;
    }
    return exist;
}
