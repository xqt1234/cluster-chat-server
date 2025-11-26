#pragma once
#include <string>
#include <unordered_map>
class Config
{
private:
    std::unordered_map<std::string,std::string> m_configMap;
public:
    void Trim(std::string &str);
    static Config& getInstance();
    std::string getValue(const std::string& key,const std::string& defalutval);
    bool loadConfig(std::string filename);
private:
    Config(/* args */);
    ~Config() = default;
    
};
