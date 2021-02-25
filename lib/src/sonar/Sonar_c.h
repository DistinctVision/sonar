/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_C_H
#define SONAR_C_H

#if defined(_WIN32) || defined(_WIN64)
#if defined(SONAR_SET_EXPORT)
#define SONAR_EXPORT __declspec(dllexport)
#else
#define SONAR_EXPORT __declspec(dllimport)
#endif
#else
#define SONAR_EXPORT
#endif

extern "C" {

/// Initalize tracking system. If tracking was existing before then it will recreated.
/// @param trackingSystemType - accoring with enum sonar::TrackingSystemType:
///     1 - Marker searching system
/// @param imageWidth - width of input image
/// @param imageHeight - height of input image
/// @param fx, fy, fx, cy - pinhole camera intrinsics
SONAR_EXPORT bool sonar_initialize_tracking_system_for_pinhole(int trackingSystemType,
                                                               int imageWidth, int imageHeight,
                                                               float fx, float fy, float cx, float cy);

/// Send frame to process
/// @param grayFrameData - buffer of frame. It must have one channel
/// @param frameWidth - width of frame
/// @param frameHeight - height of frame
SONAR_EXPORT void sonar_process_frame(void * grayFrameData, int frameWidth, int frameHeight);

/// Get world coordinates of camera
/// @param worldCameraRotationMatrixData - output buffer for rotation matrix of camera. Buffer must have size for 9 elements.
/// @param worldCameraPostionData - output buffer for position verctor of camera. Buffer must have size for 3 elements.
/// @return true if camera position is founded on last frame and else - false
SONAR_EXPORT bool sonar_get_camera_world_pose(float * worldCameraRotationMatrixData, float * worldCameraPostionData);

} // extern "C"

#endif // SONAR_C_H
