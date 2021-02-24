/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_SEMAPHORE_H
#define SONAR_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace sonar {

class Semaphore
{
public:
    Semaphore();
    Semaphore(int n);

    void acquire(int n = 1);

    int	available() const;

    void release(int n = 1);

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_notifer;
    int m_count;
};

} // namespace sonar

#endif // SONAR_SEMAPHORE_H
