#include "Worker.h"
#include "WorkerPool.h"

#include <cassert>
#include <iostream>
#include <chrono>

using namespace std;

namespace sonar {

Worker::Worker(WorkerPool * pool, int index)
{
    m_index = index;
    m_pool = pool;
    m_stop = false;
    m_thread = new thread(&Worker::_run, this);
}

Worker::~Worker()
{
    {
        lock_guard<mutex> locker(m_mutex); (void)locker;
        m_stop = true;
    }
    m_notifer.notify_one();
    m_thread->join();
    delete m_thread;
}

bool Worker::haveTask() const
{
    return (m_fTask.get() != nullptr);
}

void Worker::setTask(const function<void()> & task)
{
    {
        unique_lock<mutex> locker(m_mutex); (void)locker;
        m_fTask = make_shared<function<void()>>(task);
    }
    m_notifer.notify_one();
}

void Worker::setTask(std::function<void ()> && task)
{
    {
        unique_lock<mutex> locker(m_mutex); (void)locker;
        m_fTask = make_shared<function<void()>>(move(task));
    }
    m_notifer.notify_one();
}

void Worker::_run()
{
    cout << "Worker[" << m_index <<"] - start" << endl;
    unique_lock<mutex> locker(m_mutex);
    for (;;)
    {
        //cout << "Worker[" << m_index <<"] - check task:" << ((m_fTask.get() != nullptr) ? "true" : "false") << endl;
        if (m_fTask.get() != nullptr)
        {
            {
                try
                {
                    (*m_fTask)();
                }
                catch (const exception & e)
                {
                    string e_message = string("exception on worker:")  + e.what();
                    cerr << e_message.c_str() << endl;
                }
                catch (...)
                {
                    cerr << "exception on worker" << endl;
                }
            }
            {
                try
                {
                    m_fTask.reset();
                }
                catch (const exception & e)
                {
                    string e_message = string("exception on removing of task:")  + e.what();
                    cerr << e_message.c_str() << endl;
                }
                catch (...)
                {
                    cerr << "exception on removing of taskr" << endl;
                }
            }
            {
                lock_guard<mutex> lockerPool(m_pool->m_workerMutex); (void)lockerPool;
                m_pool->m_aviableWorkerIndices.push_back(m_index);
                m_pool->m_notifer.notify_one();
            }
        }
        //cout << "Worker[" << m_index <<"] - check stop:" << (m_stop ? "true" : "false") << endl;
        if (m_stop)
            break;
        m_notifer.wait(locker); //m_notifer.wait_for(locker, chrono::seconds(5));
    }
    cout << "Worker[" << m_index <<"] - stop" << endl;
}

} // namespace sonar
