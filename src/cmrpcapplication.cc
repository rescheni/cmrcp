#include "cmrpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>


CmrpcConfig CMrpcApplication::m_config;




void ShowUsage()
{
    std::cout << "command -i < 3 \n" << "\r\n\t" << \
    "format: command -i <configfile>" << std::endl;
}


void CMrpcApplication::Init(int argc , char ** argv)
{
    if(argc < 2)
    {
        ShowUsage();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while( (c = getopt(argc,argv,"i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout << "invalid args!" << std::endl;
            ShowUsage();
            exit(EXIT_FAILURE);
        case ':':
            std::cout << "need configfile" << std::endl;
            ShowUsage();
            exit(EXIT_FAILURE);
        default:
            std::cout << "???" << std::endl;
            break;
        }
    }


    // 开始 加载配置文件
    // rpc_serverip  rpc_server_port zookeeper_ip  zookepper_port =


    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "rpcserverip"  <<":"<< m_config.Load("rpcserverip") <<"|"<<std::endl;
    // std::cout << "rpcserverport"  <<":"<< m_config.Load("rpcserverport")<<"|"<<  std::endl;
    // std::cout << "zookeeperip"  <<":"<< m_config.Load("zookeeperip") << "|"<<std::endl;
    // std::cout << "zookeeperport"  <<":"<< m_config.Load("zookeeperport")<<"|"<<  std::endl;

}

CMrpcApplication & CMrpcApplication::GetInstance()
{
    static CMrpcApplication app;
    return app;
}

CmrpcConfig & CMrpcApplication::GetConfig()
{
    return m_config;
}
