#pragma once
#include <iostream>
#include <string>
class Token
{
private:

public:
    static Token& getInstance();

    std::string readToken(int userid);
    void clearToken(int userid);
    void saveToken(std::string& token,int userid);
private:
    std::string getTokenFileName(int userid);
    Token();
    ~Token() = default;
};

