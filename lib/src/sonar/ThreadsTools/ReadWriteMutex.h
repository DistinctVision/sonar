/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_READWRITEMUTEX_H
#define SONAR_READWRITEMUTEX_H

#include <mutex>
#include <condition_variable>

namespace sonar {

class ReadWriteMutex
{
public:
    ReadWriteMutex();

    int countReaders() const;

    void readLock();
    void readUnlock();

    void writeLock();
    void writeUnlock();

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_notifer;
    int m_countReaders;
};

class ReadLocker
{
public:
    ReadLocker(ReadWriteMutex & mutex);
    ~ReadLocker();

private:
    ReadWriteMutex & m_mutex;
};

class WriteLocker
{
public:
    WriteLocker(ReadWriteMutex & mutex);
    ~WriteLocker();

private:
    ReadWriteMutex & m_mutex;
};

class ReadWriteLocker
{
public:
    enum Type
    {
        Read,
        Write
    };

    ReadWriteLocker(ReadWriteMutex & mutex, Type type);
    ~ReadWriteLocker();

    Type type() const;

private:
    ReadWriteMutex & m_mutex;
    const Type m_type;
};

} // namespace sonar

#endif // SONAR_READWRITEMUTEX_H
