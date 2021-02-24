#include "ReadWriteMutex.h"

#include <cassert>

using namespace std;

namespace sonar {

ReadWriteMutex::ReadWriteMutex():
    m_countReaders(0)
{}

int ReadWriteMutex::countReaders() const
{
    lock_guard<mutex> locker(m_mutex); (void)(locker);
    return m_countReaders;
}

void ReadWriteMutex::readLock()
{
    {
        lock_guard<mutex> locker(m_mutex); (void)(locker);
        ++m_countReaders;
    }
    //m_notifer.notify_all();
}

void ReadWriteMutex::readUnlock()
{
    {
        lock_guard<mutex> locker(m_mutex); (void)(locker);
        --m_countReaders;
        assert(m_countReaders >= 0);
    }
    m_notifer.notify_all();
}

void ReadWriteMutex::writeLock()
{
    unique_lock<mutex> locker(m_mutex);
    while (m_countReaders > 0)
        m_notifer.wait(locker);
    locker.release();
}

void ReadWriteMutex::writeUnlock()
{
    m_mutex.unlock();
}

ReadLocker::ReadLocker(ReadWriteMutex & mutex):
    m_mutex(mutex)
{
    m_mutex.readLock();
}

ReadLocker::~ReadLocker()
{
    m_mutex.readUnlock();
}

WriteLocker::WriteLocker(ReadWriteMutex & mutex):
    m_mutex(mutex)
{
    m_mutex.writeLock();
}

WriteLocker::~WriteLocker()
{
    m_mutex.writeUnlock();
}

ReadWriteLocker::ReadWriteLocker(ReadWriteMutex & mutex,
                                 ReadWriteLocker::Type type):
    m_mutex(mutex),
    m_type(type)
{
    switch (m_type) {
    case Read:
        m_mutex.readLock();
        break;
    case Write:
        m_mutex.writeLock();
        break;
    }
}

ReadWriteLocker::~ReadWriteLocker()
{
    switch (m_type) {
    case Read:
        m_mutex.readUnlock();
        break;
    case Write:
        m_mutex.writeUnlock();
        break;
    }
}

ReadWriteLocker::Type ReadWriteLocker::type() const
{
    return m_type;
}

} // namespace sonar
