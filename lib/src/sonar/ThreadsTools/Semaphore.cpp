#include "Semaphore.h"

#include <cassert>

using namespace std;

namespace sonar {

Semaphore::Semaphore():
    m_count(0)
{
}

Semaphore::Semaphore(int n):
    m_count(n)
{
}

void Semaphore::acquire(int n)
{
    assert(n >= 0);
    unique_lock<mutex> locker(m_mutex); (void)locker;
    while (m_count < n)
        m_notifer.wait(locker);
    m_count -= n;
    m_notifer.notify_one();
}

int Semaphore::available() const
{
    lock_guard<mutex> locker(m_mutex); (void)locker;
    return m_count;
}

void Semaphore::release(int n)
{
    assert(n > 0);
    lock_guard<mutex> locker(m_mutex); (void)locker;
    m_count += n;
    m_notifer.notify_one();
}

} // namespace sonar
