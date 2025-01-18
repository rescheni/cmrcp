#include "cmrpcchannel.h"
#include "rpcheader.pb.h"
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "cmrpcapplication.h"
#include "cmrpccontroller.h"
#include "zookeeperutil.h"


// 数据格式  
// header_size service_name method_name args_size + args
void CmRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                                google::protobuf::RpcController* controller, 
                                const google::protobuf::Message* request,
                                google::protobuf::Message* response, 
                                google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();      // service_name
    std::string method_name = method->name();   // method_name
    
    // 获取参数序列化 参数字符串长度 args_size

    uint32_t args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        // std::cout << "serialize request error!" << std::endl;
        controller->SetFailed("serialize request error!");
        return;
    }

    // 定义 rpc 的请求 
    cmrpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpcheader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }else
    {        
        // std::cout << "serialize header error!" << std::endl;
        controller->SetFailed("serialize request error!");
        return;
    }
    // 序列化完成

    // 组织 待发送的  字符串 
    // 数据格式  header_size service_name method_name args_size + args

    std::string  send_rpc_str;
    send_rpc_str.insert(0,std::string((char*)&header_size,4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    std::cout << "==================Channel==DEBUG=======================" << std::endl;
    std::cout << "header_size" <<  header_size << std::endl;
    std::cout << "rpc_header_str" <<  rpc_header_str << std::endl;
    std::cout << "args_str" <<  args_str << std::endl;

    std::cout << "service_name" <<  service_name << std::endl;
    std::cout << "method_name" <<  method_name << std::endl;
    std::cout << "args_size" <<  args_size << std::endl;
    std::cout << "=================================================" << std::endl;


    // 使用Tcp 完成 Rpc 方法的调用 
    int channelfd = socket(AF_INET,SOCK_STREAM,0);

    if(channelfd == -1)
    {
        std::cout << "Creat Socket error!  Errno :" << errno << std::endl;
        controller->SetFailed("Creat Socket error!  Errno :" + std::to_string(errno));
        // exit(EXIT_FAILURE);
        return;
    } 

    
    // rpc 调用方 service_name 和 method_name 服务 需要查询zk上 该服务 所在的 host 地址
    ZkClient ZkClient;
    ZkClient.Start();

    std::string method_path = "/" + service_name + "/" + method_name;
    // ip:port
    std::string host_data = ZkClient.GetData(method_path.c_str());
    if(host_data == "")
    {
        controller->SetFailed(method_path + " is not Exist");
        return;
    }
    int index = host_data.find(":");
    if(index == -1)
    {
        controller->SetFailed(method_path + " address is invalid");
        return;
    }
    std::string ip = host_data.substr(0,index);
    uint32_t port = atoi(host_data.substr(index + sizeof ':').c_str());

    // 设置 连接 
    // std::string ip = CMrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port  = atoi(CMrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());


    // 发起连接 rpc 服务 节点
    if(connect(channelfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        // std::cout << "connect errno! errno: " << errno << std::endl;
        controller->SetFailed("connect errno! errno: " + std::to_string(errno));


        close(channelfd);
        // exit(EXIT_FAILURE);
        return;
    }
    // 发送 rpc 请求 
    if(send(channelfd,send_rpc_str.c_str(),send_rpc_str.size(),0) == -1)
    {
        // std::cout << "send error! errno :" << errno << std::endl;
        controller->SetFailed("send error! errno :" + std::to_string(errno));

        close(channelfd);

        return;
    }

    // 接收 rpc 请求的 响应值
    char recv_buffer[1024] = {0};
    int recv_size = 0;
    if((recv_size = recv(channelfd,recv_buffer,1024,0) )== -1)
    {
        // std::cout << "recv error! errno :" << errno << std::endl;
        controller->SetFailed("recv error! errno :" + std::to_string(errno));

        close(channelfd);

        return;
    }

    // 反序列化 rpc 调用 的 响应数据
    std::string response_str(recv_buffer,0,recv_size);  // recv buf 中遇到问题 recvbuffer 中 遇到\0 数据就存下来了  导致反序列化失败
    if(!response->ParseFromArray(recv_buffer,recv_size))
    {
        // std::cout << "parse error  ! response_str:" << response_str << std::endl;
        controller->SetFailed("parse error  ! response_str:" + std::to_string(errno));
        close(channelfd);
        return;
    }
    

    // close(channelfd);
}
