#include "sonar/CameraTools/CameraIntrinsics.h"

#include "sonar/General/cast.h"

using namespace std;
using namespace Eigen;

namespace sonar {

CameraIntrinsics::CameraIntrinsics(const Point2i & resolution):
    m_resolution(resolution)
{}

CameraIntrinsics::~CameraIntrinsics()
{}

CameraType::Enum CameraIntrinsics::type() const
{
    return CameraType::Undefined;
}

Point2i CameraIntrinsics::resolution() const
{
    return m_resolution;
}

Point2f CameraIntrinsics::projectPoint(const Point2f & point) const
{
    SONAR_UNUSED(point);
    throw std::runtime_error("Not implemented");
}

vector<Point2f> CameraIntrinsics::projectPoints(const vector<Point2f> & points) const
{
    SONAR_UNUSED(points);
    throw std::runtime_error("Not implemented");
}

Point2f CameraIntrinsics::unprojectPoint(const Point2f & point) const
{
    SONAR_UNUSED(point);
    throw std::runtime_error("Not implemented");
}

vector<Point2f> CameraIntrinsics::unprojectPoints(const vector<Point2f> & points) const
{
    SONAR_UNUSED(points);
    throw std::runtime_error("Not implemented");
}

} // namespace sonar
