#include <iostream>
#include <string>
#include "friend.pb.h"
#include "cmrpcapplication.h" 
#include "rpcprovider.h"
#include <vector>

#include "logger.h"

class FriendService :public fixbug::FriendServerRpc
{
public:
    std::vector<std::string> GetFriendsList(uint32_t userid)
    {
        std::cout << "do GetFriendList service !  userid : "<< userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("Friend_1");
        vec.push_back("Friend_2");
        vec.push_back("Friend_3");
        vec.push_back("Friend_4");
        return vec;
    }



    // 重写 基类方法
    void GetFriendList(google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListRequest* request,
                       ::fixbug::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();
        // 调用本地方法
        std::vector<std::string> friendsList = GetFriendsList(userid);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        
        for(std::string name:friendsList)
        {
            std::string *p = response->add_friends();
            *p = name;
        }

        done->Run();
    }
};



int main(int argc, char  **argv)
{
    // Log test
    
    std::cout << "logger--test" << std::endl;
    LOG_INFO("first log message");
    // LOG_ERR("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);

    CMrpcApplication::Init(argc,argv);

    RpcProvider provider;

    provider.NotifyService(new FriendService());
    
    provider.Run();

    return 0;
}
