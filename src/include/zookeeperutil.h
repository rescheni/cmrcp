#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();

     // zkClent  启动连接 zkserver
    void Start();
    // 在 zkserver中 创建 znode 节点
    void Create(const char * path,const char * data, int datalen,int state = 0);

    // 根据 参数指定 的  znode 节点路径 或者 znode 的 值 
    std::string GetData(const char * path);
private :
    // znode  标识
    zhandle_t * m_zhandle;
};