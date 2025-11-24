#pragma once
class User;
class UserDAO
{
private:
    /* data */
public:
    UserDAO(/* args */) = default;
    ~UserDAO() = default;
    bool insertUser(User& user);
    User queryUser(int id);
    bool updateUser(const User& user);
};

