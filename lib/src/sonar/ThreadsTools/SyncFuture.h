/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_FUTURE_H
#define SONAR_FUTURE_H

#include <functional>
#include <mutex>
#include <memory>
#include <utility>
#include <exception>

namespace sonar {

namespace _internal_future_ {

template <typename value_type>
struct _SyncInternalData
{
    std::mutex mutex;
    std::shared_ptr<std::function<void()>> syncFunc;
    std::shared_ptr<value_type> value;
    bool syncFlag;

    _SyncInternalData(const std::function<void()> & syncFunc):
        syncFunc(std::make_shared<std::function<void()>>(syncFunc)),
        syncFlag(false)
    {}

    _SyncInternalData(std::function<void()> && syncFunc):
        syncFunc(std::make_shared<std::function<void()>>(std::move(syncFunc))),
        syncFlag(false)
    {}

    ~_SyncInternalData()
    {
        this->wait();
    }

    bool wait()
    {
        std::unique_lock<std::mutex> locker(this->mutex);
        if (!this->syncFlag)
        {
            this->syncFlag = true;
            locker.unlock();
            (*this->syncFunc)();
            this->syncFunc.reset();
            return true;
        }
        return false;
    }

    value_type get()
    {
        std::unique_lock<std::mutex> locker(this->mutex);
        if (!this->syncFlag)
        {
            this->syncFlag = true;
            locker.unlock();
            (*this->syncFunc)();
            this->syncFunc.reset();
        }
        return *this->value;
    }
};

template <>
struct _SyncInternalData<void>
{
    std::mutex mutex;
    std::shared_ptr<std::function<void()>> syncFunc;
    bool valueFlag;
    bool syncFlag;

    _SyncInternalData(const std::function<void()> & syncFunc):
        syncFunc(std::make_shared<std::function<void()>>(syncFunc)),
        valueFlag(false),
        syncFlag(false)
    {}

    _SyncInternalData(std::function<void()> && syncFunc):
        syncFunc(std::make_shared<std::function<void()>>(std::move(syncFunc))),
        valueFlag(false),
        syncFlag(false)
    {}

    ~_SyncInternalData()
    {
        this->wait();
    }

    bool wait()
    {
        std::unique_lock<std::mutex> locker(this->mutex);
        if (!this->syncFlag)
        {
            this->syncFlag = true;
            locker.unlock();
            (*this->syncFunc)();
            this->syncFunc.reset();
            return true;
        }
        return false;
    }

    void get()
    {
        std::unique_lock<std::mutex> locker(this->mutex);
        if (!this->syncFlag)
        {
            this->syncFlag = true;
            locker.unlock();
            (*this->syncFunc)();
            this->syncFunc.reset();
        }
    }
};

} // namespace _internal_future_

template <typename value_type>
class SyncPromise;

template <typename value_type>
class SyncFuture
{
public:
    SyncFuture() = default;

    bool wait()
    {
        return m_internal->wait();
    }

    value_type get()
    {
        return m_internal->get();
    }

private:
    friend class SyncPromise<value_type>;

    std::shared_ptr<_internal_future_::_SyncInternalData<value_type>> m_internal;

    SyncFuture(const std::shared_ptr<_internal_future_::_SyncInternalData<value_type>> & internal):
        m_internal(internal)
    {}
};

template <typename value_type>
class SyncPromise
{
public:
    SyncPromise(const std::function<void()> & syncFunc)
    {
        m_internal = std::make_shared<_internal_future_::_SyncInternalData<value_type>>(syncFunc);
    }

    SyncPromise(std::function<void()> && syncFunc)
    {
        m_internal = std::make_shared<_internal_future_::_SyncInternalData<value_type>>(std::move(syncFunc));
    }

    void setValue(const value_type & value)
    {
        std::lock_guard<std::mutex> locker(m_internal->mutex); static_cast<void>(locker);
        if (m_internal->value)
        {
            throw std::runtime_error("Twice setting of value");
        }
        m_internal->value = std::make_shared<value_type>(value);
    }

    SyncFuture<value_type> getFuture()
    {
        return SyncFuture<value_type>(m_internal);
    }

private:
    friend class SyncFuture<value_type>;

    std::shared_ptr<_internal_future_::_SyncInternalData<value_type>> m_internal;
};

template <>
class SyncPromise<void>;

template <>
class SyncFuture<void>
{
public:
    SyncFuture() = default;

    bool wait()
    {
        return m_internal->wait();
    }

    void get()
    {
        return m_internal->get();
    }

private:
    friend class SyncPromise<void>;

    std::shared_ptr<_internal_future_::_SyncInternalData<void>> m_internal;

    SyncFuture(const std::shared_ptr<_internal_future_::_SyncInternalData<void>> & internal):
        m_internal(internal)
    {}
};

template <>
class SyncPromise<void>
{
public:
    SyncPromise(const std::function<void()> & syncFunc)
    {
        m_internal = std::make_shared<_internal_future_::_SyncInternalData<void>>(syncFunc);
    }

    SyncPromise(std::function<void()> && syncFunc)
    {
        m_internal = std::make_shared<_internal_future_::_SyncInternalData<void>>(std::move(syncFunc));
    }

    void setValue()
    {
        std::lock_guard<std::mutex> locker(m_internal->mutex); static_cast<void>(locker);
        if (m_internal->valueFlag)
        {
            throw std::runtime_error("Twice setting of value");
        }
        m_internal->valueFlag = true;
    }

    SyncFuture<void> getFuture()
    {
        return SyncFuture<void>(m_internal);
    }

private:
    friend class SyncFuture<void>;

    std::shared_ptr<_internal_future_::_SyncInternalData<void>> m_internal;
};

} // namespace sonar

#endif // SONAR_FUTURE_H
