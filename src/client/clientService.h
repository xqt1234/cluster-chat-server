#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include "clientNet.h"
#include "user.h"
#include "json.hpp"
#include "public.h"
#include "callbacks.h"
using json = nlohmann::json;

class Group;
using Func = std::function<void(std::string)>;
class ClientService
{
public:
    struct ValidResult
    {
        bool success;
        ErrType errType;
        std::string message;
    };
private:
    User m_currentUser;
    std::unordered_map<int,User> m_friendVec;
    std::vector<Group> m_groupVec;
    std::vector<std::string> m_offline_friend;
    std::vector<std::string> m_offline_group;
    ClientNet m_clientNet;

    std::unordered_map<std::string, Func> m_commandHandleMap;
    //bool m_needflush{false};
public:
    static ClientService &getInstance();
    bool sendlogin(std::string &str);
    bool sendregister(std::string &str);
    bool handleService(std::string &str);
    void addCommand(std::string str, Func func);
    void getRecv();
    std::unordered_map<std::string, Func> &getHandleMap();
    bool setState(json& js);
    const User &getCurrentUser() const { return m_currentUser; }
    const std::unordered_map<int,User> &getFriend() const { return m_friendVec; }
    const std::vector<Group> &getGroup() const { return m_groupVec; }
    const std::vector<std::string> &getOfflineFriend() const { return m_offline_friend; }
    const std::vector<std::string> &getOfflineGroup() const { return m_offline_group; }
    std::vector<std::string> &getOfflineFriend() { return m_offline_friend; }
    std::vector<std::string> &getOfflineGroup() { return m_offline_group; }
    bool addFriend(std::string src);
    bool createGroup(std::string src);
    bool addGroup(std::string src);
    //bool getFlush();
    json buildResponse(json& obj,MsgType type);
    void setdisconnectionCallBack(const disconnectionCallBack& cb);
private:
    ClientService(/* args */);
    ~ClientService() = default;
    //Func getChatService(std::string &command);
    bool chatOne(std::string src);
    ValidResult checkValid(std::string& str,json& data);
    inline long long getCurrentTimeMillis();
    static const size_t MAX_JSON_LENGTH = 1024 * 1024; // 1MB

};
