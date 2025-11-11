#include "Connection.h"
#include <iostream>
#include <chrono>
#include "Logger.h"
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
    // m_conn = mysql_real_connect(m_conn, hostaddr.c_str(), username.c_str(),
    //                    password.c_str(), dbname.c_str(), port, nullptr, 0);
    if(m_conn == nullptr)
    {
        LOG_FATAL("数据库连接失败");
        return false;
    }
    return true;
}

MYSQL_RES *Connection::query(std::string sql)
{
    if(mysql_query(m_conn,sql.c_str()) != 0)
    {
        LOG_ERROR("数据库查询失败",sql);
        return nullptr;
    }
    return mysql_use_result(m_conn);
}

bool Connection::update(std::string sql)
{
    if(mysql_query(m_conn,sql.c_str()) != 0)
    {
        LOG_ERROR("数据库更新失败{}",sql);
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
