#include "MarkerTrackingSystem.h"

#if defined(OPENCV_LIB)

#include "sonar/CameraTools/CameraIntrinsics.h"

#include "MarkerFinder.h"

using namespace std;
using namespace Eigen;

namespace sonar {

MarkerTrackingSystem::MarkerTrackingSystem(const shared_ptr<CameraIntrinsics> & cameraIntrinsics):
    AbstractTrackingSystem(cameraIntrinsics)
{
    m_markerFinder = make_shared<MarkerFinder>(MarkerFinder::MarkersDictionaryType::DICT_5x5_50);
}

TrackingState MarkerTrackingSystem::process(const ImageRef<uchar> & grayImage)
{
    auto [horizontalFlipping, verticalFlipping] = _checkFlippings();
    vector<Point2f> markerCorners = m_markerFinder->findMarker(grayImage, horizontalFlipping, verticalFlipping);
    // Unproject marker image points to common plane for universality.

    vector<Point2f> focalMarkerCorners = m_cameraIntrinsics->unprojectPoints(markerCorners);
    if (markerCorners.empty())
    {
        m_trackingState = TrackingState::LostTracking;
        return m_trackingState;
    }
    // matrix K is identity if coordinates projected to common plane
    m_lastPose = m_markerFinder->getPose(focalMarkerCorners, Matrix3f::Identity()).cast<double>();
    m_trackingState = TrackingState::Tracking;
    return m_trackingState;
}

tuple<bool, bool> MarkerTrackingSystem::_checkFlippings() const
{
    // Project image corner to projection plane
    Point2f cornerPlanePoint = m_cameraIntrinsics->unprojectPoint(Point2f(0.0f, 0.0f));
    Point2f centerPlanePoint = m_cameraIntrinsics->unprojectPoint(cast<float>(m_cameraIntrinsics->resolution()) * 0.5f);
    return { (cornerPlanePoint.x > centerPlanePoint.x), !(cornerPlanePoint.y > centerPlanePoint.y) };
}

} // namespace sonar

#endif // OPENCV_LIB
