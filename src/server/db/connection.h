#pragma once
#include <string>
#include <mysql/mysql.h>
#include <chrono>

using DbRes = std::unique_ptr<MYSQL_RES,void(*)(MYSQL_RES*)>;
class Connection
{
private:
    MYSQL* m_conn;
public:
    Connection(/* args */);
    ~Connection();
    
    bool connect(std::string hostaddr,std::string username,
    std::string password,std::string dbname,unsigned int port);
    DbRes query(std::string sql);
    int getLastId();
    bool update(std::string sql);
    int getIdleTime();
    void flushIdleTime();
    bool beginTransaction();
    bool commit();
    bool rollback();
private:
    std::chrono::steady_clock::time_point m_lastTime;
};

