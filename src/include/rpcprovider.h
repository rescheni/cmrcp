#pragma once 

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

#include <string>
#include <functional>
#include <unordered_map>


#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>





// 框架提供 专门提供发布的  网络对象类 


class  RpcProvider
{

public:

    // 框架提供给 外部使用可以发布rpc服务 的 方法 
    void NotifyService(google::protobuf::Service * service );


    // 启动rpc 服务节点 开始 提供rpc 服务 
    void  Run();
    

private:

    //组合  EventLoop
    muduo::net::EventLoop m_eventloop;


    // service 服务 类型 信息 
    struct ServiceInfo
    {
        google::protobuf::Service * m_service; // 保存服务对象
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;// 保存服务 方法
    };

    std::unordered_map<std::string,ServiceInfo>  m_serviceMap; // 存储 注册成功的 服务对象和服务 方法的所有信息


    // 新 socket 连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr & );
    // 已建立连接用户读事件的 回调
    void OnMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
    // Closure 的 回调操作 用于 与劣化rpc 的 网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& ,google::protobuf::Message*);

};
