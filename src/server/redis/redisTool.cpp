#include "redisTool.h"
#include <iostream>
#include <chrono>
#include "Logger.h"
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
    try
    {
        // 配置连接选项
        ConnectionOptions connection_opts;
        connection_opts.host = "127.0.0.1"; // Redis服务器地址
        connection_opts.port = 6379;        // Redis服务器端口
        //connection_opts.socket_timeout = std::chrono::seconds(5);
        // connection_opts.password = "your_password"; // 如果Redis服务器设置了密码，取消注释并填写
        m_pub_redis = std::make_unique<sw::redis::Redis>(connection_opts);
        // 配置连接池选项 [citation:9]
        ConnectionPoolOptions pool_opts;
        pool_opts.size = 10; // 连接池大小（最大连接数）[citation:9]
        // pool_opts.wait_timeout = std::chrono::milliseconds(100); // 获取连接的最大等待时间（根据并发调整）
        // pool_opts.connection_lifetime = std::chrono::minutes(10); // 连接的最大生命周期（可选）

        // 创建带连接池的Redis对象
        m_redispool = std::make_unique<sw::redis::Redis>(connection_opts, pool_opts);
        m_redispool->ping();
        m_sub_redis = std::make_unique<sw::redis::Subscriber>(m_pub_redis->subscriber());
        m_running = true;
        m_sub_redis->on_message(m_msgcallback);
        m_sub_redis->subscribe("redis:close");
        m_observeThread = std::thread(std::bind(&RedisTool::observver_userid_message, this));
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_FATAL("redis创建失败");
        std::cerr << e.what() << '\n';
    }
    return false;
}

void RedisTool::observver_userid_message()
{
    while (m_running)
    {
        try
        {
            m_sub_redis->consume();
        }
        catch (const sw::redis::TimeoutError &e)
        {
            continue;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    LOG_DEBUG("订阅线程退出");
}

bool RedisTool::publish(int userid, const std::string &str)
{
    try
    {
        m_pub_redis->publish(std::to_string(userid), str);
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("发布失败{}",e.what());
        return false;
    }
}

bool RedisTool::subscribe(int userid)
{
    try
    {
        m_sub_redis->subscribe(std::to_string(userid));
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("订阅失败{}",e.what());
        return false;
    }
}

bool RedisTool::unsubscribe(int userid)
{
    try
    {
        m_sub_redis->unsubscribe(to_string(userid));
        return true;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("取消订阅失败{}",e.what());
        return false;
    }
}

void RedisTool::init_notify_handle(std::function<void(std::string, std::string)> fn)
{
    m_msgcallback = fn;
}
