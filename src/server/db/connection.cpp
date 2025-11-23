#include "connection.h"
#include <iostream>
#include <chrono>
#include "Logger.h"
#include <memory>

Connection::Connection()
{
    flushIdleTime();
}

Connection::~Connection()
{
    if (m_inTransaction && m_conn)
    {
        try
        {
            m_conn->rollback();
            m_conn->setAutoCommit(true);
        }
        catch (...)
        {
            // 忽略析构函数中的异常
        }
    }
}

bool Connection::connect(std::string hostaddr, std::string username,
                         std::string password, std::string dbname, unsigned int port)
{
    std::string connection_string = "tcp://" + hostaddr + ":" + std::to_string(port);
    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
    m_conn.reset(driver->connect(connection_string, username, password));
    if (m_conn == nullptr)
    {
        LOG_FATAL("数据库连接失败");
        return false;
    }
    m_conn->setSchema(dbname);
    return true;
}

std::unique_ptr<sql::PreparedStatement> Connection::prepare(std::string sql)
{
    return std::unique_ptr<sql::PreparedStatement>(m_conn->prepareStatement(sql));
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
    try
    {
        m_conn->setAutoCommit(false);
        m_inTransaction = true;
        return true;
    }
    catch (...)
    {
        m_inTransaction = false;
        return false;
    }
}
bool Connection::commit()
{
    try
    {
        m_conn->commit();
        m_conn->setAutoCommit(true);
        m_inTransaction = false;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool Connection::rollback()
{
    try
    {
        m_conn->rollback();
        m_conn->setAutoCommit(true);
        m_inTransaction = false;
        return true;
    }
    catch (...)
    {
        return false;
    }
}
