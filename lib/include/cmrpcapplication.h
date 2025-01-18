#pragma once

// mprpc 框架的初始化 


#include "cmrpcconfig.h"
#include "cmrpcchannel.h"
#include "cmrpccontroller.h"



class CMrpcApplication
{
private:


// 单例模式  去除 拷贝构造  
    CMrpcApplication(){}
    CMrpcApplication(const CMrpcApplication & ) = delete;
    CMrpcApplication(CMrpcApplication &&) = delete;
    CMrpcApplication operator()(const CMrpcApplication &) = delete;



private:
    static CmrpcConfig m_config;

public:
    static void Init(int argc , char ** argv);
    static CMrpcApplication & GetInstance();
    static CmrpcConfig& GetConfig();
};
