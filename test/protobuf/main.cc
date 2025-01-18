#include <iostream>
#include "test.pb.h"
#include <string>

using namespace fixbug;




int main(int argc, char const *argv[])
{

    // LoginRequest req;
    // req.set_name("SET name");
    // req.set_pwd("admin");
    

    // std::string send_str;

    // // 对象数据 序列化     
    // if(req.SerializeToString(& send_str))
    // {
    //     std::cout << send_str << std::endl;
    // }


    // // 从数据 反序列化 

    // std::cout << "=======反序列化=======" << std::endl;
 
    // LoginRequest rereq;

    // if(rereq.ParseFromString(send_str))
    // {
    //     std::cout << rereq.name() << std::endl;
    //     std::cout << rereq.pwd() << std::endl;
    // }


    // LoginResponse rsp;

    // ResultCode * rc = rsp.mutable_result();

    // rc->set_errcode(1);
    // rc->set_errmsg("登录处理失败了");
    

    GetFriendListResponse rsp;
    ResultCode * rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("登录成功");



    //  添加一个 user
    User* user1 = rsp.add_friend_list();
    user1->set_name("user1");
    user1->set_age(11);
    user1->set_sex(User::MAN);
    
    std::cout << rsp.friend_list_size() << std::endl;


    std::cout << rsp.friend_list(0).name() << std::endl;


    return 0;
}
