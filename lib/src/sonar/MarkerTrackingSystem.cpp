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

void MarkerTrackingSystem::process(const ImageRef<uchar> & grayImage)
{
    vector<Point2f> markerCorners = m_markerFinder->findMarker(grayImage);
    // Unproject marker image points to common plane for universality.
    vector<Point2f> focalMarkerCorners = m_cameraIntrinsics->unprojectPoints(markerCorners);
    if (markerCorners.empty())
    {
        m_trackingState = TrackingState::LostTracking;
        return;
    }
    // matrix K is identity if coordinates projected to common plane
    m_lastPose = m_markerFinder->getPose(markerCorners, Matrix3f::Identity()).cast<double>();
    m_trackingState =TrackingState::Tracking;
}

} // namespace sonar

#endif // OPENCV_LIB
