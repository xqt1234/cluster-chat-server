#pragma once
#include <string>
#include <unordered_map>
class DbConfig
{
private:
    std::unordered_map<std::string,std::string> m_configMap;
public:
    void Trim(std::string &str);
    static DbConfig& getInstance();
    std::string getValue(const std::string& key,const std::string& defalutval);
private:
    DbConfig(/* args */);
    ~DbConfig() = default;
    bool loadConfig();
};
