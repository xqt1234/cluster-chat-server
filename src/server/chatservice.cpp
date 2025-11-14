#include "chatservice.h"
#include "Logger.h"
using namespace std::placeholders;
using json = nlohmann::json;
ChatService::ChatService()
{
    m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN), std::bind(&ChatService::Login, this, _1, _2)});
    // m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN),std::bind(&ChatService::Login,this,_1,_2)});
    // m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN),std::bind(&ChatService::Login,this,_1,_2)});
    // m_handlemap.insert({static_cast<int>(MsgType::MSG_LOGIN),std::bind(&ChatService::Login,this,_1,_2)});
}

ChatService::~ChatService()
{
}

void ChatService::Login(const TcpConnectionPtr &conn, json &js)
{
    int id = js["userid"];
    std::string password = js["password"];
    User user = m_userdao.queryUser(id);
    if (user.getId() != id || user.getPassWord() != password)
    {
        json jsres;
        jsres["msgid"] = static_cast<int>(MsgType::MSG_LOGIN_ACK);
        jsres["errono"] = 1;
        jsres["errmsg"] = "不存在该用户或者密码错误";
        jsres["id"] = user.getId();
        conn->send(js.dump());
        return;
    }

    json resjs;
    resjs["msgid"] = static_cast<int>(MsgType::MSG_LOGIN_ACK);
    resjs["errno"] = 0;
    resjs["userid"] = user.getId();
    resjs["state"] = user.getState(); // 等redis，使另外一个连接下线。
    resjs["username"] = user.getUserName();
    std::vector<User> friends = m_frienddao.query(id);
    if (!friends.empty())
    {
        std::vector<std::string> friendsstr;
        for (auto &user : friends)
        {
            json userjs;
            userjs["userid"] = user.getId();
            userjs["username"] = user.getId();
            userjs["state"] = user.getState();
            friendsstr.push_back(userjs.dump());
        }
        resjs["friends"] = friendsstr;
    }
    std::vector<Group> groups = m_groupdao.queryGroup(user.getId());
    if(!groups.empty())
    {
        std::vector<std::string> groupstr;
        for(auto& tgroup: groups)
        {
            json ttgroup;
            ttgroup["groupid"] = tgroup.getId();
            ttgroup["groupname"] = tgroup.getName();
            ttgroup["groupdesc"] = tgroup.getDesc();
            groupstr.push_back(ttgroup.dump());
        }
        resjs["groups"] = groupstr;
    }
    conn->send(js.dump());
}

MsgHandle ChatService::getHandler(int msgid)
{
    auto it = m_handlemap.find(msgid);
    if (it == m_handlemap.end())
    {
        auto func = [=](const TcpConnectionPtr &conn, json &js)
        {
            LOG_ERROR("消息类型有误{}", msgid);
        };
        return func;
    }
    return it->second;
}
