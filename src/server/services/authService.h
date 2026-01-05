#pragma once
#include "TcpConnection.h"
#include "baseservice.h"
#include <functional>
#include "callBacks.h"
#include "rpcApplication.h"
class UserDAO;
//用户认证 + Token + 在线状态
class AuthService : public BaseService
{
public:
    
private:
    // std::unique_ptr<TokenManager> m_tokenManager;
    CheckCallBack m_CheckCallBack;
    miniRpc::RpcChannel* m_channel;
public:
    AuthService();
    ~AuthService()= default;
    void setRpcChannel(miniRpc::RpcChannel* channel);
    void login(const TcpConnectionPtr &conn,const json &js, int tmpid);
    void LoginByToken(const TcpConnectionPtr &conn,const json &js,int userid);
    void registUser(const TcpConnectionPtr &conn,const json &js, int tmpid);
    // int verifyToken(std::string &str);
    // void buildLoginInfo(const TcpConnectionPtr &conn,const json &js,User& user,bool loginbytoken);
    void setCheckCallBack(const CheckCallBack& cb)
    {
        m_CheckCallBack = cb;
    }
};
