#include <iostream>
#include "cmrpcapplication.h"
#include "friend.pb.h"


int main(int argc, char **argv)
{
    // 框架的 初始化
    CMrpcApplication::Init(argc,argv);

    // 调用远程发布的 rpc方法 Login
    fixbug::FriendServerRpc_Stub stub(new CmRpcChannel());
    // 设置 rpc 方法的 请求
    fixbug::GetFriendListRequest request;
    
    request.set_userid(1000);
    
    // 设置 rpc 方法的响应 
    fixbug::GetFriendListResponse response;

    // 
    CmrpcController controller;

     // 调用 同步的 rpc 调用过程        MprpcChannel::callmethod 
    stub.GetFriendList(& controller,&request,&response,nullptr);           // RpcChannel -> RpcChannel::callMethod 方法 集中来做所有Rpc方法调用的参数序列化和网络发送
    
    // 一次 rpc 调用完成  读 调用的结果
    
    if(controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }    
    else
    {
        if(response.result().errcode() == 0)
        {
            // 调用成功 
            std::cout << "rpc GetFriendList response success " << std::endl;
            int size = response.friends_size();
            for(int i = 0;i < size;i ++)
            {
                std::cout << "index : " << i + 1 << " name: "<< response.friends(i) << std::endl;
            }
            return 0;
        }
        else
        {
            // 调用失败
            std::cout << "rpc GetFriendList response error! errorCode :" << response.result().errcode() <<" : " << response.result().errmsg() << std::endl;
        }        
    }
    
    
    
    
    



    return 0;
}
