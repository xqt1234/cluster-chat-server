#pragma once
#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
#include <sw/redis++/redis++.h>
#include <string>
#include <optional>
class RedisTool
{
private:
    std::function<void(std::string, std::string)> m_notifymessagehandler;
    std::unique_ptr<sw::redis::Redis> m_redispool;
    std::unique_ptr<sw::redis::Redis> m_pub_redis;
    std::unique_ptr<sw::redis::Subscriber> m_sub_redis;
    std::thread m_observeThread;
    bool m_running{false};
    std::function<void(std::string, std::string)> m_msgcallback;
public:
    RedisTool(/* args */);
    ~RedisTool();
    sw::redis::Redis& getRedis() { return *m_redispool; }
    std::optional<std::string> get(const std::string& key);
    bool connect();
    bool publish(int userid,const std::string& str);
    bool subscribe(int userid);
    bool unsubscribe(int userid);
    void observver_userid_message();
    void init_notify_handle(std::function<void(std::string, std::string)> fn);
    
};
