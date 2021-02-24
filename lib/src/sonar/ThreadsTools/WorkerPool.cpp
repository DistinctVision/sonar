#include "WorkerPool.h"

#include <cassert>
#include <iostream>

#include "sonar/General/cast.h"

using namespace std;

namespace sonar {

WorkerPool::WorkerPool(int size)
{
    setSize(size);
}

WorkerPool::~WorkerPool()
{
    lock_guard<mutex> locker(m_mutex); (void)locker;
    _release();
}

int WorkerPool::numberCoresOfProcessor()
{
    return static_cast<int>(thread::hardware_concurrency());
}

int WorkerPool::size() const
{
    lock_guard<mutex> locker(m_mutex); (void)locker;
    return static_cast<int>(m_workers.size());
}

void WorkerPool::setSize(int size)
{
    assert(size >= 0);
    lock_guard<mutex> locker(m_mutex); (void)locker;
    _release();
    {
        lock_guard<mutex> workerLocker(m_workerMutex); (void)workerLocker;
        m_workers.resize(static_cast<size_t>(size));
        for (int i = 0; i < size; ++i)
        {
            m_workers[cast<size_t>(i)] = new Worker(this, i);
            m_aviableWorkerIndices.push_back(i);
        }
    }
}

void WorkerPool::doTask(const std::function<void()> & task)
{
    lock_guard<mutex> locker(m_mutex); (void)locker;
    unique_lock<mutex> workerLocker(m_workerMutex); (void)workerLocker;
    while (m_aviableWorkerIndices.empty())
        m_notifer.wait(workerLocker);
    m_workers[cast<size_t>(m_aviableWorkerIndices.front())]->setTask(task);
    m_aviableWorkerIndices.pop_front();
}

void WorkerPool::doTask(std::function<void ()> && task)
{
    lock_guard<mutex> locker(m_mutex); (void)locker;
    unique_lock<mutex> workerLocker(m_workerMutex); (void)workerLocker;
    while (m_aviableWorkerIndices.empty())
        m_notifer.wait(workerLocker);
    m_workers[cast<size_t>(m_aviableWorkerIndices.front())]->setTask(task);
    m_aviableWorkerIndices.pop_front();
}

void WorkerPool::waitTasksFinish() const
{
    lock_guard<mutex> locker(m_mutex); (void)locker;
    unique_lock<mutex> workerLocker(m_workerMutex); (void)workerLocker;
    while (m_aviableWorkerIndices.size() != m_workers.size())
        m_notifer.wait(workerLocker);
}

void WorkerPool::_release()
{
    cout << "start releasing worker pool[" << m_workers.size() << "]" << endl;
    {
        unique_lock<mutex> workerLocker(m_workerMutex); (void)workerLocker;

        string debugAviableStr = "aviable list[" + to_string(m_aviableWorkerIndices.size()) + "] = ";
        for (int index: m_aviableWorkerIndices)
        {
            debugAviableStr += to_string(index) + " ";
        }
        cout << debugAviableStr << endl;

        cout << "wait for finish tasks" << endl;

        while (m_aviableWorkerIndices.size() != m_workers.size())
            m_notifer.wait(workerLocker);

        debugAviableStr = "aviable list[" + to_string(m_aviableWorkerIndices.size()) + "] = ";
        for (int index: m_aviableWorkerIndices)
        {
            debugAviableStr += to_string(index) + " ";
        }

        cout << debugAviableStr << endl;
    }
    for (size_t i = 0; i < m_workers.size(); ++i)
    {
        delete m_workers[i];
        cout << "worker[" << i << "] - deleted" << endl;
    }
    m_workers.clear();
    m_aviableWorkerIndices.clear();
}

} // namespace sonar
