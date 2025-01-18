#include <iostream>
#include "cmrpcapplication.h"
#include "user.pb.h"
#include "cmrpcchannel.h"

int main(int argc, char **argv)
{
    // 框架的 初始化
    CMrpcApplication::Init(argc,argv);

    // 调用远程发布的 rpc方法 Login
    fixbug::UserServiceRpc_Stub stub(new CmRpcChannel());
    // 设置 rpc 方法的 请求
    fixbug::LoginRequest request;
    request.set_name("Zhangsan");
    request.set_pwd("password");
    // 设置 rpc 方法的响应 
    fixbug::LoginResponse response;

     // 调用 同步的 rpc 调用过程        MprpcChannel::callmethod 
    stub.Login(nullptr,&request,&response,nullptr);           // RpcChannel -> RpcChannel::callMethod 方法 集中来做所有Rpc方法调用的参数序列化和网络发送

    // 一次 rpc 调用完成  读 调用的结果

    if(response.result().errcode() == 0)
    {
        // 调用成功 
        std::cout << "rpc login response success " << response.sucess() << std::endl;
    }
    else
    {
        // 调用失败
        std::cout << "rpc login response error! errorCode :" << response.result().errcode() <<" : " << response.result().errmsg() << std::endl;
    }



    // 调用rpc 方法 Register

    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("cmrpc00");
    req.set_pwd("passwd_register");

    fixbug::RegisterResponse rsp;

    stub.Register(nullptr,&req,&rsp,nullptr);

    if(rsp.result().errcode() == 0)
    {
        std::cout << "rpc register response success : " << response.sucess() <<std::endl;
    }
    else
    {
        std::cout << "rpc register response error :" << response.result().errcode()<< " : " << response.result().errmsg() << std::endl;    
    }

    

    return 0;
}
