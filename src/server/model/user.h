#pragma once
#include <string>
class User
{
private:
    int m_id;
    std::string m_username;
    std::string m_password;
    std::string m_state{"offline"};
public:
    User(int id = -1) : m_id(id) {}
    User(const std::string &username, const std::string &password, int id = -1)
        : m_id(id), m_username(username), m_password(password) {}
    ~User() = default;
    void setId(int id) { m_id = id; }
    const int getId()const { return m_id; }
    void setUserName(std::string username) { m_username = username; }
    void setState(std::string state){m_state = state;}
    const std::string getState()const{return m_state;}
    const std::string getUserName()const { return m_username; }
    void setPassWord(std::string password) { m_password = password; }
    const std::string getPassWord() const { return m_password; }
};
