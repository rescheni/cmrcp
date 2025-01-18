#pragma once 

#include <google/protobuf/service.h>
#include <string>


class CmrpcController : public::google::protobuf::RpcController
{
public:
    CmrpcController(/* args */);
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);
    // ~CmrpcController();

    // no realize
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure * callback);
private:
    /* data */
    bool m_failed;          // rpc 方法的执行过程
    std::string m_errText;   // rpc 方法执行过程中的错误信息
};
