#pragma once
class User;
class UserDAO
{
private:
    /* data */
public:
    UserDAO(/* args */) = default;
    ~UserDAO() = default;
    bool inserUser(const User& user);
    User queryUser(int id);
    bool updateUser(const User& user);
};

