# 项目名称：基于C/C++ 高性能RPC框架

## 项目简介

本项目实现了一个高性能的RPC框架，支持分布式服务调用。通过使用Protobuf进行序列化与反序列化，结合Muduo网络库处理通信，再借助Zookeeper实现服务注册与服务发现，确保框架的高效性与扩展性。

------

## 项目亮点

1. **高性能通信**：基于Muduo网络库，提供稳定高效的网络处理能力。
2. **灵活的序列化机制**：使用Protobuf作为数据协议，提供跨语言支持和高效的数据传输。
3. **分布式特性**：通过Zookeeper实现动态的服务注册与发现，支持分布式部署和扩展。
4. **模块化设计**：框架结构清晰，易于集成与扩展，支持用户自定义服务。

------

## 技术实现

- **序列化与反序列化**：基于Protobuf实现高效的数据编码与解码。
- **网络通信**：采用Muduo网络库处理网络的消息收发。
- **服务注册与发现**：通过Zookeeper API实现动态服务管理，确保服务的可用性和扩展性。

------

## 项目基本 结构

```bash
├── CMakeLists.txt      # cmake  文件
├── README.md           # 项目描述文件
├── autobuild.sh        # 自动构建脚本
├── bin                 # 生成二进制文件夹
│   ├── consumer        # 调用端
│   ├── provider        # 服务端
│   └── test.conf       # 项目配置文件
├── build               # 构建目录
├── example             # 使用例子
│   ├── callee          # 服务端示例
│   ├── caller          # 调用端示例
│   ├── friend.proto    # Protobuf 文件
│   ├── friend.pb.*     # Protobuf 生成代码
│   ├── user.proto      # Protobuf 文件
│   ├── user.pb.*       # Protobuf 生成代码
├── lib                 # 框架库文件
│   ├── include         # 头文件目录
│   ├── libcmrpc.a      # 静态库
├── src                 # 源代码
│   ├── logger.cc       # 日志实现
│   ├── rpcprovider.cc  # 服务端实现
│   ├── zookeeperutil.cc # Zookeeper封装
└── test
    └── protobuf        # Protobuf测试代码
```

------

## 项目使用方法

### 克隆代码

```bash
git clone https://github.com/rescheni/cmrcp.git
cd cmrpc
```

### 编译项目

```bash
sudo ./autobuild.sh
```

### 安装项目

将生成的静态库（libcmrpc.a）手动放入系统目录，或通过修改项目的CMake配置集成到其他项目中。

### 配置文件

创建并编辑配置文件，例如`test.conf`：

```conf
# RPC 节点的地址
rpcserverip=127.0.0.1
# RPC 节点端口
rpcserverport=8080
# Zookeeper 地址
zookeeperip=127.0.0.1
# Zookeeper 端口
zookeeperport=2181
```

### 启动测试

**启动服务端：**

```bash
cd bin/
./provider -i test.conf
```

**启动客户端：**

```bash
cd bin/
./consumer -i test.conf
```

### 使用示例

基于Protobuf  首先创建 protobuf 文件

```protobuf
syntax = "proto3";

package fixbug;

option cc_generic_services = true;


// rpc 好友模块 

message RequestCode
{
    int32 errcode = 1;
    bytes errmsg = 2;
}
message GetFriendListRequest
{
    uint32 userid = 1;
}
message GetFriendListResponse
{
    RequestCode result = 1;
    // repeated  列表
    repeated bytes friends = 2;
}
service FriendServerRpc
{
    rpc GetFriendList(GetFriendListRequest) returns(GetFriendListResponse);
}
```



#### 服务端示例

```cpp
#include "friend.pb.h"
#include "cmrpcapplication.h"
#include "rpcprovider.h"
#include <vector>
#include <iostream>

class FriendService : public fixbug::FriendServerRpc {
public:
    // 获取好友列表
    std::vector<std::string> GetFriendsList(uint32_t userid) {
        std::vector<std::string> friends = {"Alice", "Bob", "Charlie"};
        std::cout << "Serving GetFriendsList for userid: " << userid << std::endl;
        return friends;
    }

    // 重写 RPC 方法
    void GetFriendList(google::protobuf::RpcController* controller,
                       const fixbug::GetFriendListRequest* request,
                       fixbug::GetFriendListResponse* response,
                       google::protobuf::Closure* done) override {
        uint32_t userid = request->userid();
        auto friends = GetFriendsList(userid);
        for (const auto& friend_name : friends) {
            response->add_friends(friend_name);
        }
        response->mutable_result()->set_errcode(0);
        done->Run();
    }
};

int main(int argc, char** argv) {
    CMrpcApplication::Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new FriendService());
    provider.Run();
    return 0;
}
```

#### 调用端示例

```cpp
#include "cmrpcapplication.h"
#include "friend.pb.h"
#include <iostream>

int main(int argc, char** argv) {
    CMrpcApplication::Init(argc, argv);
    fixbug::FriendServerRpc_Stub stub(new CmRpcChannel());

    fixbug::GetFriendListRequest request;
    request.set_userid(1000);

    fixbug::GetFriendListResponse response;
    CmrpcController controller;

    stub.GetFriendList(&controller, &request, &response, nullptr);

    if (controller.Failed()) {
        std::cerr << controller.ErrorText() << std::endl;
    } else {
        for (const auto& friend_name : response.friends()) {
            std::cout << "Friend: " << friend_name << std::endl;
        }
    }
    return 0;
}
```

------

## 收获与反思

1. **性能优化**：通过合理的架构设计与高效的库选择，框架性能得以保证。
2. **技术积累**：学习了Protobuf、Muduo、Zookeeper等技术，掌握了RPC系统的核心原理。
3. **改进方向**：可以进一步支持异步调用、负载均衡等特性，提高项目的实用性与易用性。