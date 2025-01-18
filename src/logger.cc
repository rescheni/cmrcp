#include "logger.h"
#include <iostream>
#include <time.h>

// 获取 日志的 单例模式 
Logger & Logger::GetInstance()
{
    static Logger logger ;  // 定义 唯一的 logger 实例 
    return logger;
}

Logger::Logger()
{
    // 启动专门的 写日志线程
    std::thread writeLogTask([&](){

        for(;;)
        {
            // 获取 当前的日期  取 日志信息  写入相应的日志文件当中
            time_t now = time(nullptr);
            tm * nowtm = localtime(&now);
            char file_name[128];
            sprintf(file_name,"%d-%d-%d-log.txt",nowtm->tm_year + 1900,nowtm->tm_mon+1,nowtm->tm_mday);
            FILE * pf = fopen(file_name,"a+");
            std::cout << "write on " << file_name  << std::endl;
            if(pf == nullptr)
            {
                std::cout << "logger_file " <<file_name << " file open error" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lockque.Pop();

            char time_buffer[128] = {0};
            sprintf(time_buffer,"[%s] | %d:%d:%d => ", getLevelString(m_logLevel).c_str(),nowtm->tm_hour,nowtm->tm_min,nowtm->tm_sec);

            msg.insert(0,time_buffer);
            msg.append("\n");
            fputs(msg.c_str(),pf);
            fclose(pf);
        }
    });
    // 设置 分离线程
    writeLogTask.detach();
}
std::string Logger::getLevelString(int level)
{
    switch (level)
    {
    case INFO:  return "INFO";
    case ERROR: return "ERROR";
    }
    return "nomo";;
}

// 设置 日志
void Logger::SetLogLevel(LogLevel level)
{
    m_logLevel = level;
}
void Logger::Log(std::string msg)
{
    m_lockque.Push(msg);
}