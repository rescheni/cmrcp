#pragma once

#include "lockqueue.h"

enum LogLevel
{
    INFO,   // 普通信息
    ERROR,  // 错误信息
};

class Logger
{
public:
    // 获取 日志的 单例模式 
    static  Logger & GetInstance();

    // 设置 日志
    void SetLogLevel(LogLevel level);
    void Log(std::string msg);
private:
    int m_logLevel;     // 日志 级别
    LockQueue<std::string> m_lockque;
    
    // 下为 单例模式
    Logger();
    Logger(const Logger & ) = delete;
    Logger(Logger &&) = delete;
    const Logger operator()(const Logger &) = delete;
    const Logger operator()(Logger &) = delete;
    std::string getLevelString(int  level);

};

// 定义 日志宏
#define LOG_INFO(logmsgformat,...)          \
    do  \
    {   \
        Logger & logger = Logger::GetInstance();    \
        logger.SetLogLevel(INFO);    \
        char c[1024] = {0};            \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);  \
        logger.Log(c);   \
    } while (0);




#define LOG_ERR(logmsgformat,...)           \
    do  \
    {   \
        Logger & logger = Logger::GetInstance();    \
        logger.SetLogLevel(ERROR);    \
        char c[1024] = {0};            \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);  \
        logger.Log(c);   \
    } while (0);