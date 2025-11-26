#pragma once
#include <string>
#include <chrono>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
class DbChecker
{
private:
    std::unique_ptr<sql::Connection> m_conn;
    bool m_hasCheckedDb{false};
    std::string m_hostaddr;
    unsigned int m_port;
    std::string m_username;
    std::string m_dbname;
    std::string m_password;

public:
    DbChecker(/* args */);
    ~DbChecker() = default;
    bool connectAndCheckDb();
    bool checkTables(std::vector<std::string> &dbtables);
    bool createDb(std::string dbname);
    bool checkAll(std::vector<std::string> &dbtables);
    void createDefaultTables(std::string filename);

private:
    std::unique_ptr<sql::PreparedStatement> prepare(std::string sql);
    void loadConfig();
};
