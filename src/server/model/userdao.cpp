#include "userdao.h"
#include "user.h"
#include "connectionPool.h"
#include <iostream>
bool UserDAO::inserUser(User &user)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    try
    {
        std::string sql = "insert into user(username,password) values(?,?);";
        auto stmt = conn->prepare(sql);
        stmt->setString(1, user.getUserName());
        stmt->setString(2, user.getPassWord());
        stmt->executeUpdate();
        auto stmt2 = conn->prepare("SELECT LAST_INSERT_ID()");
        std::unique_ptr<sql::ResultSet> res(stmt2->executeQuery());
        if (res->next())
        {
            int userid = res->getInt(1);
            user.setId(userid); // 回写到对象中
        }
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}

User UserDAO::queryUser(int id)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return User();
    }
    try
    {
        std::string sql = "select id,username,password from user where id=?";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, id);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        while (res->next())
        {
            User user;
            user.setId(res->getInt("id"));
            user.setUserName(res->getString("username"));
            user.setPassWord(res->getString("password"));
            // 释放结果集后再返回
            return user;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return User();
}

bool UserDAO::updateUser(const User &user)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    try
    {
        std::string sql = "update user set username=? password=? where userid =?;";
        auto stmt = conn->prepare(sql);
        stmt->setString(1, user.getUserName());
        stmt->setString(2, user.getPassWord());
        stmt->setInt(3, user.getId());
        stmt->executeUpdate();
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}
