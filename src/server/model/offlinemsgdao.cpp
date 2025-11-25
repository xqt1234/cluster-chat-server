#include "offlinemsgdao.h"
#include "connectionPool.h"
#include "Logger.h"
std::vector<std::string> OffineMessageDAO::query(int userid)
{
    std::vector<std::string> vec;
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return vec;
    }
    try
    {
        std::string sql = "select id,message from offlinemsg where userid=? order by id;";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, userid);
        auto res = stmt->executeQuery();
        while (res->next())
        {
            vec.push_back(res->getString("message"));
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
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
    try
    {
        std::string sql = "delete from offlinemsg where userid = ?;";
        auto stmt = conn->prepare(sql);
        stmt->execute();
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
        
    }
    return false;
}

bool OffineMessageDAO::insert(int userid, std::string msg)
{
    auto conn = ConnectionPool::getInstance().getConnection();
    if (conn == nullptr)
    {
        return false;
    }
    try
    {
        std::string sql = "insert into offlinemsg(userid,message) values(?,?);";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1,userid);
        stmt->setString(2,msg);
        stmt->execute();
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}",e.what());
        
    }
    return false;
}
