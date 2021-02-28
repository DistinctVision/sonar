/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_LOGGER_H
#define SONAR_LOGGER_H

#include <iostream>
#include <memory>
#include <typeinfo>
#include <thread>
#include <string>
#include <fstream>

#if defined(QT_CORE_LIB)
#include <QString>
#include <QDebug>
#endif

#include "macros.h"
#include "Point2.h"

#if defined(EIGEN3_LIB)
#include <Eigen/Eigen>
#endif

#if defined(OPENCV_LIB)
#include <opencv2/core.hpp>
#endif

#define SONAR_LOG_ENABLED
//#define SONAR_LOG_FILE_ENABLED
#define SONAR_LOG_CONSOLE_ENABLED

#if defined(SONAR_LOG_FILE_ENABLED)
    #define SONAR_LOG_PATH "log.txt"
#else
    #define SONAR_LOG_PATH ""
#endif

namespace sonar {

enum class LogLevel: int
{
    Info = 0,
    Warning,
    Error
};

class BaseLogger
{
public:
    template < typename Type >
    static std::string toString(const Type & arg)
    {
        SONAR_UNUSED(arg);
        return std::string("( Type is not defined in BaseLogger::toString('") +
                           typeid(Type).name() + "') )";
    }
};

template <>
std::string BaseLogger::toString<const char *>(const char * const & arg);

template <>
std::string BaseLogger::toString<std::string>(const std::string & arg);

template <>
std::string BaseLogger::toString<bool>(const bool & arg);

template <>
std::string BaseLogger::toString<char>(const char & arg);

template <>
std::string BaseLogger::toString<unsigned char>(const unsigned char & arg);

template <>
std::string BaseLogger::toString<short>(const short & arg);

template <>
std::string BaseLogger::toString<unsigned short>(const unsigned short & arg);

template <>
std::string BaseLogger::toString<int>(const int & arg);

template <>
std::string BaseLogger::toString<unsigned int>(const unsigned int & arg);

template <>
std::string BaseLogger::toString<long>(const long & arg);
template <>
std::string BaseLogger::toString<unsigned long>(const unsigned long & arg);

template <>
std::string BaseLogger::toString<long long>(const long long & arg);
template <>
std::string BaseLogger::toString<unsigned long long>(const unsigned long long & arg);

template <>
std::string BaseLogger::toString<float>(const float & arg);

template <>
std::string BaseLogger::toString<double>(const double & arg);

template <>
std::string BaseLogger::toString<Point2i>(const Point2i & arg);

template <>
std::string BaseLogger::toString<Point2f>(const Point2f & arg);

template <>
std::string BaseLogger::toString<Point2d>(const Point2d & arg);

#if defined(EIGEN3_LIB)
template <>
std::string BaseLogger::toString<Eigen::Vector2i>(const Eigen::Vector2i & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector3i>(const Eigen::Vector3i & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector4i>(const Eigen::Vector4i & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector2f>(const Eigen::Vector2f & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector3f>(const Eigen::Vector3f & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector4f>(const Eigen::Vector4f & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector2d>(const Eigen::Vector2d & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector3d>(const Eigen::Vector3d & arg);

template <>
std::string BaseLogger::toString<Eigen::Vector4d>(const Eigen::Vector4d & arg);
#endif

#if defined(OPENCV_LIB)
template <>
std::string BaseLogger::toString<cv::Point2i>(const cv::Point2i & arg);

template <>
std::string BaseLogger::toString<cv::Point2f>(const cv::Point2f & arg);

template <>
std::string BaseLogger::toString<cv::Point2d>(const cv::Point2d & arg);
#endif

class Logger;

class WLogger: public BaseLogger
{
public:
    ~WLogger();

    template < typename Type >
    WLogger & operator << (Type message)
    {
#if defined(SONAR_LOG_ENABLED)
        m_message += " " + toString<Type>(message);
#else
        SONAR_UNUSED(message);
#endif
        return (*this);
    }

private:
    friend class Logger;

    //WLogger(const WLogger & ) = delete;
    //void operator = (const WLogger & ) = delete;

    WLogger();

    WLogger(Logger * logger, std::string && message);

    WLogger(Logger * logger,
            std::string && message,
            std::shared_ptr<std::ofstream> && filestream);

    Logger * m_logger;
    std::string m_message;
    std::shared_ptr<std::ofstream> m_filestream;
};

class Logger: public BaseLogger
{
public:
    Logger(LogLevel logLevel, const std::string & path);

    LogLevel logLevel() const;

    template < typename Type >
    WLogger operator << (Type message)
    {
#if defined(SONAR_LOG_ENABLED)
        m_mutex.lock();
        std::string m = toString<Type>(message);
#if defined(SONAR_LOG_FILE_ENABLED)
        std::shared_ptr<std::ofstream> filestream(new std::ofstream());
        filestream->open(m_path, std::ios_base::app);
        if (!filestream->is_open())
        {
#if defined(SONAR_LOG_CONSOLE_ENABLED)
#if defined(QT_CORE_LIB)
            qWarning().noquote() << QString::fromStdString(m_path + " - could not open file");
#else
            std::cerr << (m_path + " - could not open file");
#endif
#endif
            return WLogger(this, move(m));
        }
        return WLogger(this, move(m), move(filestream));
#else
        return WLogger(this, move(m));
#endif
#else
        SONAR_UNUSED(message);
        return WLogger();
#endif
    }

private:
    friend class WLogger;

    std::mutex m_mutex;
    LogLevel m_logLevel;
    std::string m_path;
};

static Logger info(LogLevel::Info, SONAR_LOG_PATH);
static Logger warning(LogLevel::Warning, SONAR_LOG_PATH);
static Logger error(LogLevel::Error, SONAR_LOG_PATH);

} // namespace sonar

#endif // SONAR_LOGGER_H
