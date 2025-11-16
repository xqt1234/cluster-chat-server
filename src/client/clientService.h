#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include "clientNet.h"
#include "user.h"
class Group;
using Func = std::function<void(std::string)>;
class ClientService
{
public:
private:
    User m_currentUser;
    std::vector<User> m_friendVec;
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
    bool setState(std::string &str);
    const User &getCurrentUser() const { return m_currentUser; }
    const std::vector<User> &getFriend() const { return m_friendVec; }
    const std::vector<Group> &getGroup() const { return m_groupVec; }
    const std::vector<std::string> &getOfflineFriend() const { return m_offline_friend; }
    const std::vector<std::string> &getOfflineGroup() const { return m_offline_group; }
    std::vector<std::string> &getOfflineFriend() { return m_offline_friend; }
    std::vector<std::string> &getOfflineGroup() { return m_offline_group; }
    bool addFriend(std::string src);
    //bool getFlush();
private:
    ClientService(/* args */);
    ~ClientService() = default;
    Func getChatService(std::string &command);
    void chatOne(std::string src);

};
