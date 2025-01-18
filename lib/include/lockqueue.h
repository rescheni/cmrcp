#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


// 异步写 日志的队列

template<typename T>
class LockQueue
{
public:

    void Push(const T & data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
    }
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_queue.empty())
        {
            // 日志 队列为 空 xian'chn给进入 wait 状态 
            m_connvariable.wait(lock);
        }
        // 拿到锁
        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable  m_connvariable;
};
