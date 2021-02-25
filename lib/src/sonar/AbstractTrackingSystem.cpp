#include "AbstractTrackingSystem.h"

#include <cassert>
#include <exception>

#include "sonar/CameraTools/CameraIntrinsics.h"
#include "MarkerTrackingSystem.h"

using namespace std;
using namespace Eigen;

namespace sonar {

shared_ptr<AbstractTrackingSystem> createTrackingSystem(TrackingSystemType trackingSystemType, const shared_ptr<CameraIntrinsics> & cameraIntrinsics)
{
    switch (trackingSystemType) {
    case TrackingSystemType::MarkerSearching:
#if defined(OPENCV_LIB)
        return make_shared<MarkerTrackingSystem>(cameraIntrinsics);
#else
        throw runtime_error("It needs opencv for creating tracking sytem of type 'MarkerSearching'");
#endif
    default:
        break;
    }
    return {};
}

AbstractTrackingSystem::AbstractTrackingSystem(const shared_ptr<CameraIntrinsics> & cameraIntrinsics):
    m_cameraIntrinsics(cameraIntrinsics),
    m_trackingState(TrackingState::Undefining)
{
    assert(m_cameraIntrinsics);
}

shared_ptr<const CameraIntrinsics> AbstractTrackingSystem::cameraIntrinsics() const
{
    return m_cameraIntrinsics;
}

TrackingState AbstractTrackingSystem::trackingState() const
{
    return m_trackingState;
}

Pose_d AbstractTrackingSystem::lastPose() const
{
    return m_lastPose;
}

tuple<bool, Matrix3d, Vector3d> AbstractTrackingSystem::getWorldCameraPose() const
{
    bool successFlag = (m_trackingState == TrackingState::Tracking);
    Matrix3d worldCameraRotation = m_lastPose.R.inverse();
    Vector3d worldCameraPosition = - worldCameraRotation * m_lastPose.t;
    return { successFlag, worldCameraRotation, worldCameraPosition };
}

} // namespace sonar
