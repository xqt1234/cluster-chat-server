#include "connectionPool.h"
#include <fstream>
#include "Logger.h"
#include <functional>
#include "config.h"
ConnectionPool::ConnectionPool()
{
    init();
    m_scanThread = std::thread(std::bind(&ConnectionPool::scanFunc, this));
}

ConnectionPool::~ConnectionPool()
{
    m_stop = true;
    m_cv.notify_all();
    if (m_scanThread.joinable())
    {
        m_scanThread.join();
    }
    while (!m_ConnectionQueue.empty())
    {
        Connection *conn = m_ConnectionQueue.front();
        m_ConnectionQueue.pop();
        delete conn;
    }
}

std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    if (m_stop)
    {
        return nullptr;
    }
    Connection *conn = nullptr;
    bool createNew = false;
    {
        std::unique_lock<std::mutex> lock(m_queueMtx);
        if (m_ConnectionQueue.empty() && m_connectionCnt < m_maxSize)
        {
            ++m_connectionCnt;
            createNew = true;
        }
        else
        {
            bool res = m_cv.wait_for(lock, std::chrono::milliseconds(m_connectTimeOut), [&]
                                     { return !m_ConnectionQueue.empty() || m_stop; });
            if (res)
            {
                if (m_stop)
                {
                    return nullptr;
                }
                conn = m_ConnectionQueue.front();
                m_ConnectionQueue.pop();
            }
        }
    }
    if (createNew)
    {
        Connection *tconn = new Connection();
        bool res = tconn->connect(m_ip, m_username, m_password, m_dbname, m_port);
        if (res)
        {
            conn = tconn;
        }
        else
        {
            --m_connectionCnt;
            delete tconn;
        }
    }
    if (conn == nullptr)
    {
        return nullptr;
    }
    std::shared_ptr<Connection> sp(conn, [this](Connection *p)
                                   {
        std::lock_guard<std::mutex> lock(m_queueMtx);
        m_ConnectionQueue.push(p); 
        m_cv.notify_one(); });
    sp->flushIdleTime();
    return sp;
}

ConnectionPool &ConnectionPool::getInstance()
{
    static ConnectionPool conn;
    return conn;
}

void ConnectionPool::init()
{
    loadConfig();

    for (int i = 0; i < m_initSize; ++i)
    {
        Connection *conn = new Connection();
        bool res = conn->connect(m_ip, m_username, m_password, m_dbname, m_port);
        if (res)
        {
            ++m_connectionCnt;
            m_ConnectionQueue.push(conn);
        }
        else
        {
            delete conn;
        }
    }
}

void ConnectionPool::scanFunc()
{
    while (!m_stop)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (m_stop)
        {
            break;
        }
        std::vector<Connection *> vec;
        {
            std::lock_guard<std::mutex> lock(m_queueMtx);
            while (!m_ConnectionQueue.empty() && m_ConnectionQueue.size() > m_initSize)
            {
                Connection *conn = m_ConnectionQueue.front();
                if (conn->getIdleTime() > m_maxIdleTime)
                {
                    m_ConnectionQueue.pop();
                    --m_connectionCnt;
                    vec.push_back(conn);
                }
                else
                {
                    break;
                }
            }
        }
        for (auto val : vec)
        {
            delete val;
        }
    }
}

void ConnectionPool::loadConfig()
{
    Config &config = Config::getInstance();
    m_ip = config.getValue("ip");
    m_port = atoi(config.getValue("port").c_str());
    m_username = config.getValue("username");
    m_dbname = config.getValue("dbname");
    m_password = config.getValue("password");
    m_initSize = atoi(config.getValue("initSize").c_str());
    m_maxSize = atoi(config.getValue("maxSize").c_str());
    m_maxIdleTime = atoi(config.getValue("maxIdleTime").c_str());
    m_connectTimeOut = atoi(config.getValue("connectionTimeOut").c_str());
}
