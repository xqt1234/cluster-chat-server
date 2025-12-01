#include "redisTool.h"
#include <iostream>
#include <chrono>
#include "Logger.h"
#include "config.h"
using namespace std;
using namespace sw::redis;
RedisTool::RedisTool()
{
}

RedisTool::~RedisTool()
{
    m_running = false;
    m_pub_redis->publish("redis:close", "close");
    if (m_observeThread.joinable())
    {
        m_observeThread.join();
    }
}

std::optional<std::string> RedisTool::get(const std::string &key)
{
    return m_redispool->get(key);
}

bool RedisTool::connect()
{

    Config &config = Config::getInstance();
    config.loadConfig("redis.ini");
    try
    {
        LOG_DEBUG("正在连接redis.....");
        // 配置连接选项
        ConnectionOptions connection_opts;
        connection_opts.host = config.getValue("redisip", "127.0.0.1");            // Redis服务器地址
        connection_opts.port = atoi(config.getValue("redisport", "6379").c_str()); // Redis服务器端口
        connection_opts.socket_timeout = std::chrono::milliseconds(100);
        connection_opts.password = config.getValue("redispassword", "xqt123").c_str(); // 如果Redis服务器设置了密码，取消注释并填写
        m_pub_redis = std::make_unique<Redis>(connection_opts);
        // 配置连接池选项 [citation:9]
        ConnectionPoolOptions pool_opts;
        pool_opts.size = 10; // 连接池大小（最大连接数）[citation:9]
        // pool_opts.wait_timeout = std::chrono::milliseconds(100); // 获取连接的最大等待时间（根据并发调整）
        // pool_opts.connection_lifetime = std::chrono::minutes(10); // 连接的最大生命周期（可选）

        // 创建带连接池的Redis对象
        m_redispool = std::make_unique<sw::redis::Redis>(connection_opts, pool_opts);
        m_redispool->ping();
        m_sub_connection = std::make_unique<sw::redis::Redis>(connection_opts);
        m_sub_redis = std::make_unique<sw::redis::Subscriber>(m_sub_connection->subscriber());
        m_running = true;
        m_sub_redis->on_message(std::bind(&RedisTool::recvMsg, this, std::placeholders::_1, std::placeholders::_2));
        m_sub_redis->subscribe("redis:close");
        m_observeThread = std::thread(std::bind(&RedisTool::observver_userid_message, this));
        LOG_DEBUG("redis连接成功!");
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_FATAL("redis创建失败{}", e.what());
    }
    return false;
}

void RedisTool::observver_userid_message()
{
    std::vector<std::string> subs;
    std::vector<std::string> usubs;
    while (m_running)
    {
        try
        {
            {
                std::lock_guard<std::mutex> lock(m_subMtx);
                if(!m_subs.empty())
                {
                    m_subs.swap(subs);
                }
                if(!m_unsubs.empty())
                {
                    m_unsubs.swap(usubs);
                }
            }
            if(!subs.empty())
            {
                for(auto& channel : subs)
                {
                    m_sub_redis->subscribe(channel);
                }
                subs.clear();
            }
            if(!usubs.empty())
            {
                for(auto& channel : usubs)
                {
                    m_sub_redis->unsubscribe(channel);
                }
                usubs.clear();
            }
            m_sub_redis->consume();
        }
        catch (const sw::redis::TimeoutError &e)
        {
            continue;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("redis错误{}", e.what());
        }
    }
    LOG_DEBUG("订阅线程退出");
}

void RedisTool::recvMsg(std::string key, std::string value)
{
    if (m_msgcallback)
    {
        m_msgcallback(key, value);
    }
}

bool RedisTool::publish(const std::string &key, const std::string &str)
{
    try
    {
        m_pub_redis->publish(key, str);
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("发布失败{}", e.what());
        return false;
    }
}

bool RedisTool::subscribe(const std::string &channel)
{
    try
    {
        std::lock_guard<std::mutex> lock(m_subMtx);
        m_subs.push_back(channel);
        // m_sub_redis->subscribe(channel);
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("订阅失败{}", e.what());
        return false;
    }
}

bool RedisTool::unsubscribe(const std::string &channel)
{
    try
    {
        std::lock_guard<std::mutex> lock(m_subMtx);
        m_unsubs.push_back(channel);
        // m_sub_redis->unsubscribe(channel);
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("取消订阅失败{}", e.what());
        return false;
    }
}

void RedisTool::init_notify_handle(std::function<void(std::string, std::string)> fn)
{
    m_msgcallback = fn;
}
