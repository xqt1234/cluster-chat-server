#include "authService.h"
#include "chatservice.h"
#include "userdao.h"
#include "Logger.h"
#include "config.h"
#include "responseBuilder.h"
AuthService::AuthService()
{
    // m_tokenManager = std::make_unique<TokenManager>(m_redis.getRedis());
}
void AuthService::setRpcChannel(miniRpc::RpcChannel* channel)
{
    m_channel = channel;
}
void AuthService::login(const TcpConnectionPtr &conn,const json &js, int tmpid)
{
    m_channel->callMethodAsync("UserService","Login",js.dump(),[this,conn](std::string res){
        json response = json::parse(res);
        if(conn && conn->isConnected())
        {
            if(response["errcode"] != static_cast<int>(ErrType::SUCCESS))
            {
                conn->send(res);
                return;
            }else
            {
                json resdata = response["data"];
                int userid = resdata["userinfo"].value("userid",-1);
                m_CheckCallBack({userid,false,false,conn});
                m_redis.subscribe("to:" + std::to_string(userid));
                LOG_DEBUG("订阅频道：{}","to:" + std::to_string(userid));
                json sendjson = buildResponse(resdata, MsgType::MSG_LOGIN_ACK);
                std::vector<int> friendvec = response["data"]["friendsids"];
                m_RelationCache.initFriends(userid, friendvec,ResponseBuilder::getCurrentTimeMillis());
                conn->send(sendjson.dump());
            }
            
        }
    });
}

void AuthService::LoginByToken(const TcpConnectionPtr &conn,const json &js, int userid)
{
    m_channel->callMethodAsync("UserService","LoginByToken",js.dump(),[this,conn](std::string res){
        json response = json::parse(res);
        if(conn && conn->isConnected())
        {
            if(response["errcode"] != static_cast<int>(ErrType::SUCCESS))
            {
                conn->send(res);
                return;
            }else
            {
                json resdata = response["data"];
                int userid = resdata["userinfo"].value("userid",-1);
                m_CheckCallBack({userid,false,false,conn});
                m_redis.subscribe("to:" + std::to_string(userid));
                LOG_DEBUG("订阅频道：{}","to:" + std::to_string(userid));
                json sendjson = buildResponse(resdata, MsgType::MSG_LOGIN_BY_TOKEN_ACK);
                std::vector<int> friendvec = response["data"]["friendsids"];
                m_RelationCache.initFriends(userid, friendvec,ResponseBuilder::getCurrentTimeMillis());
                conn->send(sendjson.dump());
            }
            
        }
    });
}

void AuthService::registUser(const TcpConnectionPtr &conn, const json &js, int tmpid)
{
    m_channel->callMethodAsync("UserService","RegistUser",js.dump(),[this,conn](std::string res){
        json response = json::parse(res);
        if(conn && conn->isConnected())
        {
            if(response["errcode"] != static_cast<int>(ErrType::SUCCESS))
            {
                conn->send(res);
                return;
            }else
            {
                json resdata = response["data"];
                json sendjson = buildResponse(resdata, MsgType::MSG_LOGIN_BY_TOKEN_ACK);
                conn->send(sendjson.dump());
            }
        }
    });
}
