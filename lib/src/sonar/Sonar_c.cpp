/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#include "sonar/Sonar_c.h"

#include <vector>
#include <memory>
#include <cstring>

#include <fstream>

#include <Eigen/Eigen>

#include "sonar/General/Logger.h"
#include "sonar/General/cast.h"
#include "sonar/General/Point2.h"
#include "sonar/General/Image.h"

#include "sonar/CameraTools/PinholeCameraIntrinsics.h"
#include "AbstractTrackingSystem.h"

using namespace std;
using namespace Eigen;

namespace sonar {

class SystemContext
{
public:
    static SystemContext & instance()
    {
        static SystemContext context;
        return context;
    }

    SystemContext()
    {}

    bool createTrackingSystem(TrackingSystemType trackingSystemType, const shared_ptr<CameraIntrinsics> & cameraIntrinsics)
    {
        m_trackingSystem = sonar::createTrackingSystem(trackingSystemType, cameraIntrinsics);
        return (m_trackingSystem.get() != nullptr);
    }

    TrackingState process_frame(const ImageRef<uchar> & frame)
    {
        if (!m_trackingSystem)
        {
            return TrackingState::Undefining;
        }
        return m_trackingSystem->process(frame);
    }

    tuple<bool, Matrix3d, Vector3d> getLocalCameraPose() const
    {
        if (!m_trackingSystem)
        {
            return { false, Matrix3d::Identity(), Vector3d::Zero() };
        }
        Pose_d pose =  m_trackingSystem->lastPose();
        return { true, pose.R, pose.t };
    }

    tuple<bool, Matrix3d, Vector3d> getWorldCameraPose() const
    {
        if (!m_trackingSystem)
        {
            return { false, Matrix3d::Identity(), Vector3d::Zero() };
        }
        return m_trackingSystem->getWorldCameraPose();
    }

private:
    shared_ptr<AbstractTrackingSystem> m_trackingSystem;
};

} // namespace sonar

using namespace sonar;

extern "C" {

bool sonar_initialize_tracking_system_for_pinhole(int trackingSystemType, int imageWidth, int imageHeight,
                                                  float fx, float fy, float cx, float cy)
{
    info << "sonar_initialize_tracking_system_for_pinhole(" << 1 << imageWidth << imageHeight << fx << fy << cx << cy << ")";
    auto cameraIntrinsics = make_shared<PinholeCameraIntrinsics>(Size2i(imageWidth, imageHeight),
                                                                 Vector2f(fx, fy), Vector2f(cx, cy));
    return SystemContext().instance().createTrackingSystem(static_cast<TrackingSystemType>(trackingSystemType), cameraIntrinsics);
}

int sonar_process_frame(const void * grayFrameData, int frameWidth, int frameHeight)
{
    info << "sonar_process_frame(" << SONAR_PTR2STR(grayFrameData) << frameWidth << frameHeight << ")";
    ConstImage<uchar> frame(frameWidth, frameHeight, reinterpret_cast<const unsigned char*>(grayFrameData), false);
    TrackingState trackingState = SystemContext().instance().process_frame(frame);
    return static_cast<int>(trackingState);
}

bool sonar_get_camera_local_pose(float * localCameraRotationMatrixData, float * localCameraTranslationData)
{
    info << "sonar_get_camera_world_pose(" << SONAR_PTR2STR(localCameraRotationMatrixData) << SONAR_PTR2STR(localCameraTranslationData);
    bool successFlag;
    Matrix3d rotationMatrix;
    Vector3d translation;
    tie(successFlag, rotationMatrix, translation) = SystemContext::instance().getLocalCameraPose();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            localCameraRotationMatrixData[i * 3 + j] = cast<float>(rotationMatrix(i, j));
        localCameraTranslationData[i] = cast<float>(translation(i));
    }
    return successFlag;
}

bool sonar_get_camera_world_pose(float * worldCameraRotationMatrixData, float * worldCameraPostionData)
{
    info << "sonar_get_camera_world_pose(" << SONAR_PTR2STR(worldCameraRotationMatrixData) << SONAR_PTR2STR(worldCameraPostionData);
    bool successFlag;
    Matrix3d rotationMatrix;
    Vector3d postion;
    tie(successFlag, rotationMatrix, postion) = SystemContext::instance().getWorldCameraPose();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            worldCameraRotationMatrixData[i * 3 + j] = cast<float>(rotationMatrix(i, j));
        worldCameraPostionData[i] = cast<float>(postion(i));
    }
    return successFlag;
}

} // extern "C"
