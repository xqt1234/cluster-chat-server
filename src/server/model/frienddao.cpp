#include "frienddao.h"
#include "connectionPool.h"
#include "user.h"
#include <iostream>
#include "Logger.h"
std::vector<User> FriendDAO::query(int id)
{
    std::vector<User> vec;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return vec;
    }
    try
    {
        std::string sql = "select a.id,a.username from user a join friend b on a.id = b.friendid where b.userid = ?;";
        auto stmt = conn->prepare(sql);
        if (stmt)
        {
            stmt->setInt(1, id);
            auto res = stmt->executeQuery();
            while (res->next())
            {
                User user;
                user.setId(res->getInt("id"));
                user.setUserName(res->getString("username"));
                vec.emplace_back(std::move(user));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
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
    try
    {
        if (!conn->beginTransaction()) {
            return false;
        }
        char buf[1024];
        std::string sql1 = "insert into friend values(?,?);";
        auto res1 = conn->prepare(sql1);
        res1->setInt(1, userid);
        res1->setInt(2, friendid);
        res1->executeUpdate();
        auto res2 = conn->prepare(sql1);
        res2->setInt(1, friendid);
        res2->setInt(2, userid);

        res2->executeUpdate();
        conn->commit();
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
        conn->rollback();
    }
    return false;
}

bool FriendDAO::isFriend(int userid, int friendid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    try
    {
        std::string sql = "select 1 from friend where userid=? and friendid=? limit 1;";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, userid);
        stmt->setInt(2, friendid);
        auto res = stmt->executeQuery();
        if (res->next())
        {
            return true;
        }
        return false;
        /* code */
    }
    catch (const std::exception &e)
    {
        // std::cerr << e.what() << '\n';
    }
    return false;
}
