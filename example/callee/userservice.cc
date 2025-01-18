#include <iostream>
#include <string>
#include "user.pb.h"
#include "cmrpcapplication.h" 
#include "rpcprovider.h"






// 本地服务  UserService

class UserService: public fixbug::UserServiceRpc   // RPC服务的 提供者
{
public:

    bool Login(std::string name,std::string pwd)
    {
        std::cout << "Doing Local service : Login" << std::endl;
        std::cout << name <<":" << pwd <<  std::endl;
        return false;
    }



    bool Register(uint32_t id,std::string name, std::string pwd)
    {
        std::cout << "do  local service Register !" << std::endl;
        std::cout << "id :" << id<<"\t name:" << name <<std::endl;
        return true;
    }

    // UserServiceRpc 框架 直接调用 
    // 重写 UserServiceRpc 虚函数   下面是方法 直接调用的  

    /**
     *  // 明确业务
     * caller(呼叫者) ===> Login（LoginRequest）  => muduo =>  callee(被调用方)
     * callee    =====>  Login(LoginRequest)  ===> 👇  重写的这个 LoginRequest 方法上
     * 
     */

    void Login(google::protobuf::RpcController* controller,
                const ::fixbug::LoginRequest* request,
                ::fixbug::LoginResponse* response,
                ::google::protobuf::Closure* done)
    {
        // 框架 给业务上报了请求参数  LoginRequest  应用 业务获取响应数据做本地业务

        std::string name = request->name();
        std::string  pwd = request->pwd();

        bool login_request = Login(name,pwd);  // 本地业务

        // 将响应 写入  错错误码 错误消息  返回值  
        // response->set_sucess(login_request);

        // 无异常
        fixbug::ResultCode * code = response->mutable_result();
        // code->set_errcode(1);
        code->set_errcode(0);
        // code->set_errmsg("Login do no error but error ");
        code->set_errmsg("");
        response->set_sucess(login_request);    


        // 执行回调操作 
        done->Run();
    }
    void Register(google::protobuf::RpcController* controller,
            const ::fixbug::RegisterRequest* request,
            ::fixbug::RegisterResponse* response,
            ::google::protobuf::Closure* done)
    {
        uint32_t id=  request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Register(id,name,pwd);
        
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);
        
        done->Run();
    }
};


int main(int argc, char  ** argv)
{

    /// 滴哦用框架的 初始化操作  xxxx -i config.conf
    CMrpcApplication::Init(argc, argv); 

    // provider （供应商）  是 一个  远程rpc网络服务对象  吧 user Service对象发布到rpc 节点上 
    RpcProvider provider;
    provider.NotifyService(new UserService());  // (通知服务)

    // 启动一个 Rpc发布节点    run 以后进入阻塞状态 等待远程的  rpc 调用请求  
    std::cout << "启动一个发布节点 ==> OK"  << std::endl;
    provider.Run();

    return 0;
}
