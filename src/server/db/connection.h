#pragma once
#include <string>
#include <chrono>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
// using DbRes = std::unique_ptr<MYSQL_RES,void(*)(MYSQL_RES*)>;
class Connection
{
private:
    std::unique_ptr<sql::Connection> m_conn;
    bool m_inTransaction{false};
public:
    Connection(/* args */);
    ~Connection();
    
    bool connect(std::string hostaddr,std::string username,
    std::string password,std::string dbname,unsigned int port);
    std::unique_ptr<sql::PreparedStatement> prepare(std::string sql);

    //int getLastId();
    //bool update(std::string sql);
    //std::unique_ptr<sql::Connection>& getConn(){return m_conn;}
    int getIdleTime();
    void flushIdleTime();
    bool beginTransaction();
    bool commit();
    bool rollback();
private:
    std::chrono::steady_clock::time_point m_lastTime;
};

