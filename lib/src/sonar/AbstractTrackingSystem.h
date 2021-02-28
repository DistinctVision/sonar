/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_ABSTRACTTRACKINGSYSTEM_H
#define SONAR_ABSTRACTTRACKINGSYSTEM_H

#include <memory>

#include <Eigen/Eigen>

#include "sonar/General/Image.h"

#include "sonar/global_types.h"

namespace sonar {

/// Type of tracking system for function createTrackingSystem.
enum class TrackingSystemType
{
    Unknown = 0,
    MarkerSearching = 1
};

/// Enum for getting information about active state of tracking system
enum class TrackingState
{
    Undefining = 0,
    Tracking,
    LostTracking
};

class CameraIntrinsics;
class AbstractTrackingSystem;

/// Create tracking system
/// @param trackingSystemType - type
/// @param cameraIntrinsics - camera intrinsics that are used from stream
std::shared_ptr<AbstractTrackingSystem> createTrackingSystem(TrackingSystemType trackingSystemType,
                                                             const std::shared_ptr<CameraIntrinsics> & cameraIntrinsics);

/// Base abstract class for tracking system
class AbstractTrackingSystem
{
public:
    /// @param cameraIntrinsics - we shoudn't change camera intrinsics after.
    AbstractTrackingSystem(const std::shared_ptr<CameraIntrinsics> & cameraIntrinsics);

    /// @return camera intrinsics - only for reading
    std::shared_ptr<const CameraIntrinsics> cameraIntrinsics() const;

    /// Get active state of system. Beginning state - Undefining.
    TrackingState trackingState() const;

    /// Get last pose. Pose is not world coordinates of camera. Use getWorldCameraPose for getting world coordinates of camera
    Pose_d lastPose() const;

    /// Get world coordinates of camera for active state of system.
    /// @return successFlag, worldCameraRotation, worldCameraPosition -
    ///     successFlag - true if system have information about actual coordinates of camera and else - false.
    ///     worldCameraRotation - world rotation matrix of camera
    ///     worldCameraPosition - world positon of camera
    std::tuple<bool, Eigen::Matrix3d, Eigen::Vector3d> getWorldCameraPose() const;

    /// Main method for processing image stream. Need call this method for every frame of stream.
    /// @param grayImage - input gray image of stream
    virtual TrackingState process(const ImageRef<uchar> & grayImage) = 0;

protected:
    std::shared_ptr<CameraIntrinsics> m_cameraIntrinsics;
    TrackingState m_trackingState;
    Pose_d m_lastPose;
};

} // namespace sonar

#endif // SONAR_ABSTRACTTRACKINGSYSTEM_H
