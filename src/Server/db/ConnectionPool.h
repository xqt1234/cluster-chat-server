#pragma once
#include "Connection.h"
#include <memory>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <thread>
class ConnectionPool
{
private:
    std::string m_ip;
    unsigned short m_port{0};
    std::string m_username;
    std::string m_password;
    std::string m_dbname;
    int m_initSize{0};
    int m_maxSize{0};
    int m_maxIdleTime{0};
    int m_connectTimeOut{0};
    std::queue<Connection*> m_ConnectionQueue;
    std::mutex m_queueMtx;
    std::atomic<int> m_connectionCnt{0};
    std::condition_variable m_cv;
    std::atomic<bool> m_stop{false};
    std::thread m_scanThread;
public:
    
    std::shared_ptr<Connection> getConnection();
    static ConnectionPool& getInstance();
    void createDefaultTables(std::string filename);
private:
    ConnectionPool(/* args */);
    ~ConnectionPool();
    ConnectionPool(const ConnectionPool& src) = delete;
    ConnectionPool& operator=(const ConnectionPool& src) = delete;
    bool loadConfig();
    void Trim(std::string &str);
    void init();
    void scanFunc();
    

};

