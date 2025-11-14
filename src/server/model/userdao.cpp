#include "userdao.h"
#include "user.h"
#include "connectionPool.h"
bool UserDAO::inserUser(const User &user)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf), "insert into user values('%s','%s');",
             const_cast<User &>(user).getUserName().c_str(),
             const_cast<User &>(user).getPassWord().c_str());
    return conn->update(buf);
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
    MYSQL_RES* res = conn->query(buf);
    if(res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if(row != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setUserName(row[1]);
            user.setPassWord(row[2]);
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
