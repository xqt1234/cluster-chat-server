#pragma once
#include <string>
#include <mysql/mysql.h>
#include <chrono>
class Connection
{
private:
    MYSQL* m_conn;
public:
    Connection(/* args */);
    ~Connection();
    bool connect(std::string hostaddr,std::string username,
    std::string password,std::string dbname,unsigned int port);
    MYSQL_RES* query(std::string sql);
    bool update(std::string sql);
    int getIdleTime();
    void flushIdleTime();
private:
    std::chrono::steady_clock::time_point m_lastTime;
};

