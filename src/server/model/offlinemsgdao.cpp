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
    try
    {
        std::string sql = "select id,message from offlinemessage where userid=? order by id;";
        auto stmt = conn->prepare(sql);
        stmt->setInt(1, userid);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        while (res->next())
        {
            vec.push_back(res->getString("message"));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
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
        std::string sql = "delete from offlinemssage where userid = ?;";
        auto stmt = conn->prepare(sql);
        stmt->execute();
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        
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
        std::string sql = "insert into offlinemessage values(?,?);";
        auto stmt = conn->prepare(sql);
        stmt->execute();
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        
    }
    return false;
}
