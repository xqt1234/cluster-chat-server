#pragma once
#include "TcpConnection.h"
#include "baseservice.h"
#include <functional>
#include "callBacks.h"
class UserDAO;
//用户认证 + Token + 在线状态
class AuthService : public BaseService
{
public:
    
private:
    std::unique_ptr<TokenManager> m_tokenManager;
    CheckCallBack m_CheckCallBack;
    
public:
    AuthService();
    ~AuthService()= default;
    void login(const TcpConnectionPtr &conn, json &js, int tmpid);
    void LoginByToken(const TcpConnectionPtr &conn, json &js,int userid);
    void registerUser(const TcpConnectionPtr &conn, json &js, int tmpid);
    int verifyToken(std::string &str);
    void buildLoginInfo(const TcpConnectionPtr &conn, json &js,User& user,bool loginbytoken);
    void setCheckCallBack(const CheckCallBack& cb)
    {
        m_CheckCallBack = cb;
    }
};
