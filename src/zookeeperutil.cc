#include "zookeeperutil.h"
#include "cmrpcapplication.h"
#include "logger.h"
ZkClient::ZkClient()
    : m_zhandle(nullptr)
{

}
ZkClient::~ZkClient()
{
    if( m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle);
    }
}

// callback()
void global_watcher(zhandle_t *zh, int type, int state,
                     const char *path,void *watcherCtx)
{
    if(type == ZOO_SESSION_EVENT)   // 回调 的 消息 是和 会话相关的消息类型
    {
        if(state == ZOO_CONNECTED_STATE)    // zkclient 和 zkserver 连接成功
        {
            sem_t * sem = (sem_t *) zoo_get_context(zh);
            sem_post(sem);
        }
    }
}


// 连接 zkserver
void ZkClient::Start()
{
    std::string host = CMrpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = CMrpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;
    LOG_INFO("ip:port ===> %s",connstr.c_str());

    /*
        zookeeper_mt 多线程版本 提供了 三个线程
        zookeeper的 API 客户端程序提供了三个线程
      1  API 调用线程
      2  网络IO  线程  pthread_create poll
      3  watcher回调线程  pthread_create
    */
    m_zhandle = zookeeper_init(connstr.c_str(),global_watcher,30000,nullptr,nullptr,0);

    if(nullptr == m_zhandle)
    {
        LOG_ERR("zookeeper_init error");
        return;
    }
    LOG_INFO("zookeeper_init ===========> OK");

    sem_t sem;
    sem_init(&sem,0,0);

    zoo_set_context(m_zhandle,&sem);

    sem_wait(&sem);
    LOG_INFO("zookeeper_init success!");
}
void ZkClient::Create(const char * path,const char * data, int datalen,int state)
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag ;
    // 判断是否存在 
    flag = zoo_exists(m_zhandle,path,0,nullptr);
    // 不存在  
    if(ZNONODE == flag)
    {
        // 根据 state 创建永久性节点 还是 临时性 节点

        flag = zoo_create(m_zhandle,path,data,datalen,& ZOO_OPEN_ACL_UNSAFE,state,path_buffer,bufferlen);

        if(flag == ZOK)
        {
            LOG_INFO("znode create success ... path: %s" ,path);
        }
        else
        {
            LOG_ERR("flag %d" ,flag);
            LOG_ERR("znode create error ... path : %s",path);
            return;
        }
    }

}

std::string ZkClient::GetData(const char * path)
{
    char buffer[64];

    int bufferlen = sizeof buffer;
    int flag = zoo_get(m_zhandle,path,0,buffer,& bufferlen,nullptr);

    if(flag != ZOK)
    {
        LOG_ERR(" get node error... path : %s",path);
        return "";
    }
    else
    {
        return buffer;
    }

}