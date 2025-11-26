#include "config.h"
#include <fstream>
#include "Logger.h"
#include <iostream>
bool Config::loadConfig(std::string filename)
{
    std::ifstream ss(filename);
    if (!ss.is_open())
    {
        LOG_FATAL("找不到数据库配置文件");
        return false;
    }
    std::string line;
    while (std::getline(ss, line))
    {
        Trim(line);
        if (line[0] == '#' || line.empty())
        {
            continue;
        }
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos)
        {
            LOG_ERROR("配置行不正确{}");
            continue;
        }
        std::string tmpkey = line.substr(0, eq_pos);
        std::string tmpval = line.substr(eq_pos + 1);
        Trim(tmpkey);
        Trim(tmpval);
        m_configMap.insert({tmpkey,tmpval});
        
    }
    // for(auto& [key,val]: m_configMap)
    // {
    //     std::cout << "key" << key << "val:" << val << " ";
    // }
    return true;
}

void Config::Trim(std::string &str)
{
    size_t start_pos = str.find_first_not_of(" ");
    if (start_pos != std::string::npos)
    {
        str = str.substr(start_pos);
    }
    start_pos = str.find_last_not_of(" ");
    if (start_pos != std::string::npos)
    {
        str = str.substr(0, start_pos + 1);
    }
}

Config &Config::getInstance()
{
    static Config config;
    return config;
}

std::string Config::getValue(const std::string& key,const std::string& defalutval)
{
    auto it = m_configMap.find(key);
    if(it != m_configMap.end())
    {
        return it->second;
    }
    return defalutval;
}

Config::Config()
{
    
}
