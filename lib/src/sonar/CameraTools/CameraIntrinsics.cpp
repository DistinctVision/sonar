#include "sonar/CameraTools/CameraIntrinsics.h"

#include "sonar/General/cast.h"

using namespace std;
using namespace Eigen;

namespace sonar {

CameraIntrinsics::CameraIntrinsics(const Size2i & resolution):
    m_resolution(resolution)
{}

CameraIntrinsics::~CameraIntrinsics()
{}

CameraType CameraIntrinsics::type() const
{
    return CameraType::Undefined;
}

Size2i CameraIntrinsics::resolution() const
{
    return m_resolution;
}

vector<Point2f> CameraIntrinsics::projectPoints(const vector<Point2f> & points) const
{
    vector<Point2f> p_points(points.size());
    for (size_t i = 0; i < points.size(); ++i)
        p_points[i] = this->projectPoint(p_points[i]);
    return p_points;
}

vector<Point2f> CameraIntrinsics::unprojectPoints(const vector<Point2f> & points) const
{
    vector<Point2f> u_points(points.size());
    for (size_t i = 0; i < points.size(); ++i)
        u_points[i] = this->unprojectPoint(points[i]);
    return u_points;
}

} // namespace sonar
