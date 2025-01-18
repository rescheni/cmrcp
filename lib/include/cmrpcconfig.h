#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
// #include <cstdio>

// rpcServerip  rpcServerport zookeeperip zookeeperport
// 框架 读取文件类 

class CmrpcConfig
{
private:
    std::unordered_map<std::string,std::string> m_configMap;
public:
    // 负责加载 解析配置文件 
    void LoadConfigFile(const char * config_file);

    // 查询配置文件信息
    std::string Load(const std::string &key );

};

