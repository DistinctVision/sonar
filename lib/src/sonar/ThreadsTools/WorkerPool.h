/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_WORKERPOOL_H
#define SONAR_WORKERPOOL_H

#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>

#include "Worker.h"

namespace sonar {

class WorkerPool
{
public:
    WorkerPool(int size = numberCoresOfProcessor());
    ~WorkerPool();

    static int numberCoresOfProcessor();

    int size() const;
    void setSize(int size);

    void doTask(const std::function<void()> & task);
    void doTask(std::function<void()> && task);

    void waitTasksFinish() const;

private:
    friend class Worker;

    WorkerPool(const WorkerPool & ) = delete;
    void operator = (const WorkerPool & ) = delete;

    mutable std::mutex m_mutex;
    std::vector<Worker*> m_workers;

    mutable std::mutex m_workerMutex;
    std::list<int> m_aviableWorkerIndices;
    mutable std::condition_variable m_notifer;

    void _release();
};

} // namespce sonar

#endif // SONAR_WORKERPOOL_H
