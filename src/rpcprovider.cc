#include "rpcprovider.h"
#include "cmrpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

// 框架提供给 外部使用可以发布rpc服务 的 方法 
void RpcProvider::NotifyService(google::protobuf::Service * service )
{
    ServiceInfo service_info;

    // 获取服务 对象的 描述信息
    const google::protobuf::ServiceDescriptor * pserviceDesc = service->GetDescriptor();


    std::string service_name = pserviceDesc->name();

    // 获取 服务 对象的 service 的 方法和 数量 
    int methodCnt = pserviceDesc->method_count();

    std::cout << "service_name: "<<service_name << std::endl;
    LOG_INFO("service_name : %s",service_name.c_str());

    for(int i =0 ;i < methodCnt ; i++ )
    {
        // 获取服务 对象指定下标的 服务方法 描述  (抽象)
        const google::protobuf::MethodDescriptor * pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name,pmethodDesc});
        // std::cout << "method_name: "<<method_name << std::endl;
        LOG_INFO("method_name : %s",method_name.c_str());

    }
    service_info.m_service = service;

    m_serviceMap.insert({service_name,service_info});
        
}


    // 启动rpc 服务节点 开始 提供rpc 服务 
void  RpcProvider::Run()
{   

    std::string ip = CMrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    std::uint16_t port = atoi(CMrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());


    std::cout << ip <<" : " << port << std::endl;

    muduo::net::InetAddress address(ip,port); 

    

    // 创建服务
    muduo::net::TcpServer server(& m_eventloop,address,"rpcProvider");
    // 绑定连接回调和消息读写方法回调  分离网络代码 和 业务代码 
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));


    // 设置 muduos 线程数量
    server.setThreadNum(4);

    // 将 rpc 发布的 服务 全部注册到 zookeeper 上面 让rpc client 可以从zookeeper 上发现服务 

    ZkClient zkClient;
    zkClient.Start();
    for(auto & sp : m_serviceMap )
    {
        std::string service_path = "/" + sp.first;
        zkClient.Create(service_path.c_str(),nullptr,0);

        for(auto & mp : sp.second.m_methodMap)
        {
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d",ip.c_str(),port);
            zkClient.Create(method_path.c_str(),method_path_data,strlen(method_path_data), ZOO_EPHEMERAL);
            // ZOO_EPHEMERAL 临时性 节点 
        }


    }
    

    



    std::cout << "RpcProvider start service at ip :" << ip << " port :" <<  port << std::endl;

    // 启动网络服务 
    std::cout << "启动网络服务 ==> OK"  << std::endl;
    server.start();
    m_eventloop.loop();


}


void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        // 连接断开
        std::cout << "连接 断开 ==> OK"  << std::endl;
        conn->shutdown(); 
    }

}
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp time)
{
    
    // 从网络接收的 远程 rpc 调用请求的 字符流  Login  args
    std::string  recv_buf = buffer->retrieveAllAsString();
    
    // 从字符流中读取 前四个字节的 内容 
    uint32_t header_size  = 0;
    recv_buf.copy((char*)&header_size,4);
    
    // 根据字符流中 读取的 header_size 读取 service_name method_name 
    std::string rpc_header_str = recv_buf.substr(4,header_size);
    cmrpc::RpcHeader rpcHeader ;


    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if(rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据反序列化成功

        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();

        std::cout <<"数据反序列化成功"<< std::endl;

    }else
    {
        // 数据反序列化失败 
        std::cout << "rpc_header_str" <<  rpc_header_str << " parse error ! " << std::endl;
        return ;
    }

    // 获取 rpc 方法参数的 字符流数据
    std::string args_str = recv_buf.substr(4 + header_size,args_size);

    std::cout << "==================Server==DEBUG=======================" << std::endl;
    
    std::cout << "header_size" <<  header_size << std::endl;
    std::cout << "rpc_header_str" <<  rpc_header_str << std::endl;
    std::cout << "args_str" <<  args_str << std::endl;

    std::cout << "service_name" <<  service_name << std::endl;
    std::cout << "method_name" <<  method_name << std::endl;
    std::cout << "args_size" <<  args_size << std::endl;
    std::cout << "=================================================" << std::endl;


    // 框架调用服务对象 调用服务方法
    
    // 获取 service  对象 和  method 对象 
    auto it= m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        std::cout << service_name <<" is not exist" << std::endl;
        return;
    }else{
        // google::protobuf::Service * service = it->second.m_service;
    }
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit ==  it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" <<  method_name <<"  is not exist" << std::endl;
        return;
    }

    google::protobuf::Service * service = it->second.m_service; // 获取 service 对象 new UserService
    const google::protobuf::MethodDescriptor* method = mit->second; // 获取method 对象

    // 生成 rpc 方法 调用的请求 request  和响应 response

    google::protobuf::Message * request = service->GetRequestPrototype(method).New();
    google::protobuf::Message * response = service->GetResponsePrototype(method).New();
    
    if(!request->ParseFromString(args_str))
    {
        std::cout << "request parse error , content" << args_str << std::endl;
        return;
    }

    // 给 下面的 method  的 方法的调用 绑定一个 回调函数
    google::protobuf::Closure * done = google::protobuf::NewCallback<RpcProvider,\
                                                    const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>
    (this,&RpcProvider::SendRpcResponse,conn,response);

    // 在框架上 从远端 rpc 请求 ， 调用当前 rpc 节点上的发布方法
    // new UserService().Login(controller,request,response,done);
    service->CallMethod(method,nullptr,request,response,done);

}


// Closure 的 回到操作 用于序列化rpc 的响应和 网络发送 
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))  // 对 response 进行 序列化 
    {
         // 序列化完成后 通过网络吧 rpc 方法 执行的 结果发送回 rpc 的  调用方
         conn->send(response_str);
    }else{
        std::cout <<  " Serialize response_str  error ! " << std::endl;
    }
    conn->shutdown();         // 由 rpcprovider 主动断开连接 
}

