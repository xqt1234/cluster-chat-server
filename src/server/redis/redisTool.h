#pragma once
#include <thread>
#include <functional>
#include <sw/redis++/redis++.h>
#include <string>
#include <optional>
#include <mutex>
#include <vector>
class RedisTool
{
private:
    std::function<void(std::string, std::string)> m_notifymessagehandler;
    std::unique_ptr<sw::redis::Redis> m_redispool;
    std::unique_ptr<sw::redis::Redis> m_pub_redis;
    std::unique_ptr<sw::redis::Redis> m_sub_connection;
    std::unique_ptr<sw::redis::Subscriber> m_sub_redis;
    std::mutex m_subMtx;
    std::vector<std::string> m_subs;
    std::vector<std::string> m_unsubs;
    std::thread m_observeThread;
    bool m_running{false};
    std::function<void(std::string, std::string)> m_msgcallback;
public:
    RedisTool(/* args */);
    ~RedisTool();
    sw::redis::Redis& getRedis() { return *m_redispool; }
    std::optional<std::string> get(const std::string& key);
    bool connect();
    bool publish(const std::string& key,const std::string& str);
    bool subscribe(const std::string& channel);
    bool unsubscribe(const std::string& channel);
    void observver_userid_message();
    void recvMsg(std::string key,std::string value);
    void init_notify_handle(std::function<void(std::string, std::string)> fn);
};
