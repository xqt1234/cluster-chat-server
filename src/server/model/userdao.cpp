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
    char buf[1024];
    snprintf(buf, sizeof(buf), "insert into user(username,password) values('%s','%s');",
             const_cast<User &>(user).getUserName().c_str(),
             const_cast<User &>(user).getPassWord().c_str());
    std::cout << std::string(buf) << std::endl;
    if(conn->update(buf))
    {
        int id = mysql_insert_id(conn->getconnection());
        user.setId(id);
        return true;
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
    char buf[1024];
    snprintf(buf,sizeof(buf),"select id,username,password from user where id=%d",id);
    DbRes res = conn->query(buf);
    if(res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res.get());
        if(row != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setUserName(row[1]);
            user.setPassWord(row[2]);
            // 释放结果集后再返回
            return user;
        }
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
    char buf[1024];
    snprintf(buf, sizeof(buf), "update user set username='%s' password='%s' where userid =%d;",
             const_cast<User &>(user).getUserName().c_str(),
             const_cast<User &>(user).getPassWord().c_str(),
             const_cast<User &>(user).getId());
    return conn->update(buf);
}
