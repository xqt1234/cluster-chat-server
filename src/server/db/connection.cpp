#include "connection.h"
#include <iostream>
#include <chrono>
#include "Logger.h"
#include <memory>
Connection::Connection()
{
    m_conn = mysql_init(nullptr);
    flushIdleTime();
}

Connection::~Connection()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
        m_conn = nullptr;
    }
}

bool Connection::connect(std::string hostaddr, std::string username,
                         std::string password, std::string dbname, unsigned int port)
{
    // std::cout << hostaddr << " " << username << " "
    // << password << " " << dbname << port << std::endl;
    m_conn = mysql_real_connect(m_conn, hostaddr.c_str(), username.c_str(),
                                password.c_str(), dbname.c_str(), port, nullptr, 0);
    if (m_conn == nullptr)
    {
        LOG_FATAL("数据库连接失败");
        return false;
    }
    return true;
}

DbRes Connection::query(std::string sql)
{
    LOG_DEBUG("sql:{}",sql);
    if (mysql_query(m_conn, sql.c_str()) != 0)
    {
        LOG_ERROR("数据库查询失败{} error:{}", sql, mysql_error(m_conn));
        return {nullptr, mysql_free_result};
    }
    // 使用 mysql_store_result 将结果缓存在客户端，避免如果调用方没有
    // 完全遍历或及时释放结果导致的 "Commands out of sync" 错误。
    MYSQL_RES *res = mysql_store_result(m_conn);
    //MYSQL_RES* res2 = mysql_use_result(m_conn);use只一遍一遍获取，store一次性获取。
    return {res, mysql_free_result};
}

int Connection::getLastId()
{
    return mysql_insert_id(m_conn);
}

bool Connection::update(std::string sql)
{
    if (mysql_query(m_conn, sql.c_str()) != 0)
    {
        LOG_ERROR("数据库更新失败{} error:{}", sql, mysql_error(m_conn));
        return false;
    }
    return true;
}

int Connection::getIdleTime()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - m_lastTime).count();
}

void Connection::flushIdleTime()
{
    m_lastTime = std::chrono::steady_clock::now();
}

bool Connection::beginTransaction()
{
    return mysql_autocommit(m_conn, false) == 0;
}

bool Connection::commit()
{
    return mysql_commit(m_conn) == 0;
}

bool Connection::rollback()
{
    bool res = (mysql_rollback(m_conn) == 0);
    mysql_autocommit(m_conn, true);
    return res;
}
