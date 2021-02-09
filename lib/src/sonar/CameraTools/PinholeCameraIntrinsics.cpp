#include "sonar/CameraTools/PinholeCameraIntrinsics.h"

#include "sonar/General/cast.h"

using namespace std;
using namespace Eigen;

namespace sonar {

PinholeCameraIntrinsics::PinholeCameraIntrinsics(const Point2i & resolution,
                                                 const Vector2f & focalLength,
                                                 const Vector2f & opticalCenter):
    CameraIntrinsics(resolution),
    m_focalLength(focalLength),
    m_opticalCenter(opticalCenter)
{}

CameraType::Enum PinholeCameraIntrinsics::type() const
{
    return CameraType::Pinhole;
}

Vector2f PinholeCameraIntrinsics::focalLength() const
{
    return m_focalLength;
}

Vector2f PinholeCameraIntrinsics::opticalCenter() const
{
    return m_opticalCenter;
}

Point2f PinholeCameraIntrinsics::projectPoint(const Point2f & point) const
{
    return Point2f(point.x * m_focalLength.x() + m_opticalCenter.x() - 0.5f,
                   point.y * m_focalLength.y() + m_opticalCenter.y() - 0.5f);
}

vector<Point2f> PinholeCameraIntrinsics::projectPoints(const vector<Point2f> & points) const
{
    vector<Point2f> p_points(points.size());
    for (size_t i = 0; i < points.size(); ++i)
    {
        const Point2f & p = points[i];
        p_points[i].set(p.x * m_focalLength.x() + m_opticalCenter.x() - 0.5f,
                        p.y * m_focalLength.y() + m_opticalCenter.y() - 0.5f);
    }
    return p_points;
}

Point2f PinholeCameraIntrinsics::unprojectPoint(const Point2f & point) const
{
    return Point2f((point.x + 0.5f - m_opticalCenter.x()) / m_focalLength.x(),
                   (point.y + 0.5f - m_opticalCenter.y()) / m_focalLength.y());
}

vector<Point2f> PinholeCameraIntrinsics::unprojectPoints(const vector<Point2f> & points) const
{
    vector<Point2f> u_points(points.size());
    for (size_t i = 0; i < points.size(); ++i)
    {
        const Point2f & p = points[i];
        u_points[i].set((p.x + 0.5f - m_opticalCenter.x()) / m_focalLength.x(),
                        (p.y + 0.5f - m_opticalCenter.y()) / m_focalLength.y());
    }
    return u_points;
}

Matrix4f PinholeCameraIntrinsics::glProjectionMatrix(float depthNear, float depthFar) const
{
    Matrix4f proj;
    proj << (m_focalLength.x() / m_opticalCenter.x()), 0.0f, 0.0f, 0.0f,
             0.0f, (m_focalLength.y() / m_opticalCenter.y()), 0.0f, 0.0f,
             0.0f, 0.0f, - depthFar / (depthNear - depthFar), depthNear * depthFar / (depthNear - depthFar),
             0.0f, 0.0f, 1.0f, 0.0f;
    return proj;
}

} // namespace sonar
