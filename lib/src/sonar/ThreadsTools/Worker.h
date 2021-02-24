/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_WORKER_H
#define SONAR_WORKER_H

#include <list>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <thread>
#include <functional>

namespace sonar {

class WorkerPool;

class Worker
{
public:
    bool haveTask() const;

    void setTask(const std::function<void()> & task);
    void setTask(std::function<void()> && task);

private:
    friend class WorkerPool;

    Worker(WorkerPool * pool, int index);
    ~Worker();

    void _run();

    WorkerPool * m_pool;
    int m_index;

    mutable std::mutex m_mutex;
    bool m_stop;
    std::condition_variable m_notifer;
    std::shared_ptr<std::function<void()>> m_fTask;

    std::thread * m_thread;
};

} // namespace sonar

#endif // SONAR_WORKER_H
