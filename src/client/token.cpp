#include "token.h"
#include <fstream>
#include <filesystem>
Token::Token()
{
}

Token &Token::getInstance()
{
    static Token token;
    return token;
}

std::string Token::readToken(int userid)
{
    std::ifstream iss(getTokenFileName(userid));
    std::string res;
    if (iss.is_open())
    {
        getline(iss, res);
    }
    else
    {
        res = "";
    }
    return res;
}

void Token::clearToken(int userid)
{
    std::ofstream ofs(getTokenFileName(userid), std::ios::trunc);
    ofs.close();
}

void Token::saveToken(std::string &token,int userid)
{
    std::ofstream oss(getTokenFileName(userid));
    if (oss.is_open())
    {
        oss << token;
    }
}

std::string Token::getTokenFileName(int userid)
{
    std::filesystem::path tokendir = "token";
    if (!std::filesystem::exists(tokendir))
    {
        std::filesystem::create_directories(tokendir);
    }
    std::string filename = std::to_string(userid) + "_token.key";
    // return (logdir / filename).string();
    return (tokendir / filename).string();
}
