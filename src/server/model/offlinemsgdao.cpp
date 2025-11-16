#include "offlinemsgdao.h"
#include "connectionPool.h"
std::vector<std::string> OffineMessageDAO::query(int userid)
{
    std::vector<std::string> vec;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return vec;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf), "select id,message from offlinemessage where userid=%d order by id;", userid);
    DbRes res = conn->query(buf);
    if (res != nullptr)
    {
        MYSQL_ROW row = nullptr;
        while ((row = mysql_fetch_row(res.get())) != nullptr)
        {
            vec.push_back(row[1]);
        }
    }
    return vec;
}

bool OffineMessageDAO::remove(int userid)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf), "delete from offlinemssage where userid = %d;", userid);
    return conn->update(buf);
}

bool OffineMessageDAO::insert(int userid, std::string msg)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    char buf[1024];
    snprintf(buf,sizeof(buf),"insert into offlinemessage values(%d,'%s');",userid,msg.c_str());
    return conn->update(buf);
}
