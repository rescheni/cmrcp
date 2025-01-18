#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

class CmRpcChannel : public google::protobuf::RpcChannel
{
public:


// 所有通过 stub 代理对象 调用的rpc 方法  统一做rpc方法调用数据的序列化和反序列化|网络发送
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                            google::protobuf::Message* response, google::protobuf::Closure* done);

};


