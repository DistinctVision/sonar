#include "Logger.h"

#include <cstdlib>

using namespace std;
#if defined(EIGEN3_LIB)
using namespace Eigen;
#endif

namespace sonar {

template <>
string BaseLogger::toString<const char *>(const char * const & arg)
{
    return string(arg);
}

template <>
string BaseLogger::toString<std::string>(const string & arg)
{
    return arg;
}

template <>
string BaseLogger::toString<bool>(const bool & arg)
{
    return arg ? "true" : "false";
}

template <>
string BaseLogger::toString<char>(const char & arg)
{
    return to_string(static_cast<int>(arg));
}

template <>
string BaseLogger::toString<unsigned char>(const unsigned char & arg)
{
    return to_string(static_cast<int>(arg));
}

template <>
string BaseLogger::toString<short>(const short & arg)
{
    return to_string(static_cast<int>(arg));
}

template <>
string BaseLogger::toString<unsigned short>(const unsigned short & arg)
{
    return to_string(static_cast<int>(arg));
}

template <>
string BaseLogger::toString<int>(const int & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<unsigned int>(const unsigned int & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<long>(const long & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<unsigned long>(const unsigned long & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<long long>(const long long & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<unsigned long long>(const unsigned long long & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<float>(const float & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<double>(const double & arg)
{
    return to_string(arg);
}

template <>
string BaseLogger::toString<Point2i>(const Point2i & arg)
{
    return string("Point2i(") + to_string(arg.x) + ", " +
                                to_string(arg.y) + ")";
}

template <>
string BaseLogger::toString<Point2f>(const Point2f & arg)
{
    return string("Point2f(") + to_string(arg.x) + ", " +
                                to_string(arg.y) + ")";
}

template <>
string BaseLogger::toString<Point2d>(const Point2d & arg)
{
    return string("Point2d(") + to_string(arg.x) + ", " +
                                to_string(arg.y) + ")";
}

#if defined(EIGEN3_LIB)

template <>
string BaseLogger::toString<Vector2i>(const Vector2i & arg)
{
    return string("Eigen::Vector2i(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ")";
}

template <>
string BaseLogger::toString<Vector3i>(const Vector3i & arg)
{
    return string("Eigen::Vector3i(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ", " +
                                        to_string(arg.z()) + ")";
}

template <>
string BaseLogger::toString<Vector4i>(const Vector4i & arg)
{
    return string("Eigen::Vector4i(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ", " +
                                        to_string(arg.z()) + ", " +
                                        to_string(arg.w()) + ")";
}

template <>
string BaseLogger::toString<Vector2f>(const Vector2f & arg)
{
    return string("Eigen::Vector2f(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ")";
}

template <>
string BaseLogger::toString<Vector3f>(const Vector3f & arg)
{
    return string("Eigen::Vector3f(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ", " +
                                        to_string(arg.z()) + ")";
}

template <>
string BaseLogger::toString<Vector4f>(const Vector4f & arg)
{
    return string("Eigen::Vector4f(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ", " +
                                        to_string(arg.z()) + ", " +
                                        to_string(arg.w()) + ")";
}

template <>
string BaseLogger::toString<Vector2d>(const Vector2d & arg)
{
    return string("Eigen::Vector2d(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ")";
}

template <>
string BaseLogger::toString<Vector3d>(const Vector3d & arg)
{
    return string("Eigen::Vector3d(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ", " +
                                        to_string(arg.z()) + ")";
}

template <>
string BaseLogger::toString<Vector4d>(const Vector4d & arg)
{
    return string("Eigen::Vector4d(") + to_string(arg.x()) + ", " +
                                        to_string(arg.y()) + ", " +
                                        to_string(arg.z()) + ", " +
                                        to_string(arg.w()) + ")";
}

#endif

#if defined(OPENCV_LIB)

template <>
string BaseLogger::toString<cv::Point2i>(const cv::Point2i & arg)
{
    return string("cv::Point2i(") + to_string(arg.x) + ", " +
                                    to_string(arg.y) + ")";
}

template <>
string BaseLogger::toString<cv::Point2f>(const cv::Point2f & arg)
{
    return string("cv::Point2f(") + to_string(arg.x) + ", " +
                                    to_string(arg.y) + ")";
}

template <>
string BaseLogger::toString<cv::Point2d>(const cv::Point2d & arg)
{
    return string("cv::Point2d(") + to_string(arg.x) + ", " + to_string(arg.y) + ")";
}

#endif

Logger::Logger(LogLevel logLevel, const string & path)
{
    m_logLevel = logLevel;
    m_path = path;
}

LogLevel Logger::logLevel() const
{
    return m_logLevel;
}

WLogger::WLogger()
{
#if defined(SONAR_LOG_ENABLED)
    m_logger = nullptr;
#endif
}

WLogger::WLogger(Logger * logger, string && message)
{
#if defined(SONAR_LOG_ENABLED)
    m_logger = logger;
    m_message = move(message);
#else
    SONAR_UNUSED(logger);
    SONAR_UNUSED(message);
#endif
}

WLogger::WLogger(Logger * logger, string && message, shared_ptr<ofstream> && filestream)
{
#if defined(SONAR_LOG_ENABLED)
    m_logger = logger;
#if defined(SONAR_LOG_FILE_ENABLED)
    m_filestream = move(filestream);
#else
    SONAR_UNUSED(filestream);
#endif
    m_message = move(message);
#else
    SONAR_UNUSED(logger);
    SONAR_UNUSED(message);
    SONAR_UNUSED(filestream);
#endif
}

WLogger::~WLogger()
{
#if defined(SONAR_LOG_ENABLED)
#if defined(SONAR_LOG_CONSOLE_ENABLED)
    if (!m_message.empty())
    {
        switch (m_logger->m_logLevel) {
        case LogLevel::Error:
        {
#if defined (QT_CORE_LIB)
            qCritical().noquote() << QString::fromStdString(m_message);
#else
            cerr << "error: " << m_message << endl;
#endif
        } break;
        case LogLevel::Warning:
        {
#if defined (QT_CORE_LIB)
            qWarning().noquote() << QString::fromStdString(m_message);
#else
            cerr << "warning: " << m_message << endl;
#endif
        } break;
        default:
        {
#if defined (QT_CORE_LIB)
            qDebug().noquote() << QString::fromStdString(m_message);
#else
            cout << m_message << endl;
#endif
        }
        }
    }
#endif
#if defined(SONAR_LOG_FILE_ENABLED)
    if (m_filestream)
    {
        string prefix;
        switch (m_logger->logLevel()) {
        case LogLevel::Error:
            prefix = "error: ";
            break;
        case LogLevel::Warning:
            prefix = "warning: ";
            break;
        default:
            break;
        }
        *m_filestream << prefix << m_message << endl;
        m_filestream->close();
    }
#endif
    m_logger->m_mutex.unlock();
#endif
}

} // namespace sonar
