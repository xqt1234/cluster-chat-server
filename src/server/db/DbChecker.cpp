#include "DbChecker.h"
#include "Logger.h"
#include "config.h"
DbChecker::DbChecker()
{
    loadConfig();
}
bool DbChecker::connectAndCheckDb()
{
    std::string connection_string = "tcp://" + m_hostaddr + ":" + std::to_string(m_port);
    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
    m_conn.reset(driver->connect(connection_string, m_username, m_password));
    if (m_conn == nullptr)
    {
        LOG_FATAL("数据库连接失败");
        return false;
    }
    try
    {
        std::string sql = "SELECT COUNT(*) FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = ?";
        auto stmt = prepare(sql);
        stmt->setString(1, m_dbname);
        auto result = stmt->executeQuery();
        if (result->next() && result->getInt(1) > 0)
        {
            m_conn->setSchema(m_dbname);
            m_hasCheckedDb = true;
            return true;
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}", e.what());
    }
    LOG_ERROR("数据库{}不存在", m_dbname);
    return false;
}

bool DbChecker::checkTables(std::vector<std::string> &dbtables)
{
    if (!m_hasCheckedDb)
    {
        LOG_ERROR("请先检查数据库是否存在");
        return false;
    }
    bool res = true;
    for (auto &tbname : dbtables)
    {
        try
        {
            std::string singleSql = "SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = ? AND table_name = ?";
            auto stmt = prepare(singleSql);
            stmt->setString(1, m_dbname);
            stmt->setString(2, tbname);
            auto result = stmt->executeQuery();
            if (result->next() && result->getInt(1) > 0)
            {
                continue;
            }
            else
            {
                LOG_ERROR("表{}不存在", tbname);
                res = false;
            }
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("表{}不存在", e.what());
        }
    }
    return res;
}

bool DbChecker::createDb(std::string dbname)
{
    if (!m_conn)
    {
        return false;
    }
    try
    {
        if (dbname.empty() || dbname.length() > 64)
        {
            LOG_FATAL("数据库名称不合法{}", dbname);
        }

        std::string sql = "create database " + dbname + ";";
        auto stmt = prepare(sql);
        stmt->execute();
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("{}", e.what());
    }
    return false;
}

bool DbChecker::checkAll(std::vector<std::string> &dbtables)
{
    bool res = connectAndCheckDb();
    if (!res)
    {
        createDb(m_dbname);
        res = connectAndCheckDb();
        if (!res)
        {
            LOG_FATAL("数据库创建失败");
            return false;
        }
    }
    res = checkTables(dbtables);
    if (!res)
    {
        createDefaultTables("mysql.sql");
        res = checkTables(dbtables);
        if (!res)
        {
            LOG_FATAL("初始化表失败");
        }
    }
    return res;
}

std::unique_ptr<sql::PreparedStatement> DbChecker::prepare(std::string sql)
{
    return std::unique_ptr<sql::PreparedStatement>(m_conn->prepareStatement(sql));
}

void DbChecker::loadConfig()
{
    Config &config = Config::getInstance();
    config.loadConfig("mysql.ini");
    m_hostaddr = config.getValue("ip", "127.0.0.1");
    m_port = atoi(config.getValue("port", "3306").c_str());
    m_username = config.getValue("username", "root");
    m_dbname = config.getValue("dbname", "chatdb");
    m_password = config.getValue("password", "xqt123");
}

void DbChecker::createDefaultTables(std::string filename)
{
    std::ifstream ss(filename);
    if (!ss.is_open())
    {
        LOG_ERROR("找不到执行sql初始文件{}", filename);
        return;
    }
    std::string line;
    std::vector<std::string> sqlvec;
    while (getline(ss, line))
    {
        if (line.starts_with("create"))
        {
            sqlvec.push_back(line);
        }
    }
    for (auto &sql : sqlvec)
    {
        try
        {
            auto stmt = prepare(sql);
            stmt->execute();
        }
        catch (const std::exception &e)
        {
            LOG_FATAL("初始化表失败，可能数据库没建{}", e.what());
        }
    }
}
