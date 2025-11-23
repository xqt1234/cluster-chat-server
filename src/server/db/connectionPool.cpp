#include "connectionPool.h"
#include <fstream>
#include "Logger.h"
#include <functional>
ConnectionPool::ConnectionPool()
{
    init();
    m_scanThread = std::thread(std::bind(&ConnectionPool::scanFunc,this));
}

ConnectionPool::~ConnectionPool()
{
    m_stop = true;
    m_cv.notify_all();
    if(m_scanThread.joinable())
    {
        m_scanThread.join();
    }
    while(!m_ConnectionQueue.empty())
    {
        Connection* conn = m_ConnectionQueue.front();
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
        m_cv.notify_one(); 
    });
    sp->flushIdleTime();
    return sp;
}

ConnectionPool &ConnectionPool::getInstance()
{
    static ConnectionPool conn;
    return conn;
}

void ConnectionPool::createDefaultTables(std::string filename)
{
    std::ifstream ss(filename);
    if (!ss.is_open())
    {
        LOG_FATAL("找不到执行sql初始文件{}",filename);
        return;
    }
    std::string line;
    std::vector<std::string> sqlvec;
    while(getline(ss,line))
    {
        if(line.starts_with("create"))
        {
            sqlvec.push_back(line);
        }
    }
    auto conn = getConnection();
    if (conn != nullptr)
    {
        for (auto &sql : sqlvec)
        {
            try
            {
                auto stmt = conn->prepare(sql);
                stmt->execute();
            }
            catch(const std::exception& e)
            {
                LOG_ERROR("初始化表失败，可能数据库没建{}",e.what());
            }
        }
    }
}


void ConnectionPool::init()
{
    if (!loadConfig())
    {
        return;
    }
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
        if(m_stop)
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
                }else
                {
                    break;
                }
            }
        }
        for(auto val : vec)
        {
            delete val;
        }
    }
}

bool ConnectionPool::loadConfig()
{
    std::ifstream ss("mysql.ini");
    if (!ss.is_open())
    {
        LOG_FATAL("找不到数据库配置文件");
        return false;
    }
    std::string line;
    while (std::getline(ss, line))
    {
        Trim(line);
        if (line[0] == '#' || line.empty())
        {
            continue;
        }
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos)
        {
            LOG_ERROR("配置行不正确{}");
            continue;
        }
        std::string tmpkey = line.substr(0, eq_pos);
        std::string tmpval = line.substr(eq_pos + 1);
        Trim(tmpkey);
        Trim(tmpval);
        if (tmpkey == "ip")
        {
            m_ip = tmpval;
        }
        else if (tmpkey == "port")
        {
            m_port = atoi(tmpval.c_str());
        }
        else if (tmpkey == "username")
        {
            m_username = tmpval;
        }
        else if (tmpkey == "dbname")
        {
            m_dbname = tmpval;
        }
        else if (tmpkey == "password")
        {
            m_password = tmpval;
        }
        else if (tmpkey == "initSize")
        {
            m_initSize = atoi(tmpval.c_str());
        }
        else if (tmpkey == "maxSize")
        {
            m_maxSize = atoi(tmpval.c_str());
        }
        else if (tmpkey == "maxIdleTime")
        {
            m_maxIdleTime = atoi(tmpval.c_str());
        }
        else if (tmpkey == "connectionTimeOut")
        {
            m_connectTimeOut = atoi(tmpval.c_str());
        }
    }
    std::cout << m_ip << " " << m_port << " " << m_username << " "
              << m_dbname << " " << m_password << " " << m_initSize << " "
              << m_maxSize << " " << m_maxIdleTime << " " << m_connectTimeOut << "---" << std::endl;
    return true;
}

void ConnectionPool::Trim(std::string &str)
{
    size_t start_pos = str.find_first_not_of(" ");
    if (start_pos != std::string::npos)
    {
        str = str.substr(start_pos);
    }
    start_pos = str.find_last_not_of(" ");
    if (start_pos != std::string::npos)
    {
        str = str.substr(0, start_pos + 1);
    }
}