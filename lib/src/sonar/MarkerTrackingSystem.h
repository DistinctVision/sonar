/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_MARKERTRACKINGSYSTEM_H
#define SONAR_MARKERTRACKINGSYSTEM_H

#if defined(OPENCV_LIB)

#include <memory>

#include <opencv2/core.hpp>

#include "AbstractTrackingSystem.h"

namespace sonar {

class CameraIntrinsics;
class MarkerFinder;

/// This class use marker for tracking camera position. If marker is not found then system lost camera position.
class MarkerTrackingSystem: public AbstractTrackingSystem
{
public:
    MarkerTrackingSystem(const std::shared_ptr<CameraIntrinsics> & cameraIntrinsics);

    void process(const ImageRef<uchar> & grayImage) override;

private:
    std::shared_ptr<MarkerFinder> m_markerFinder;

    /// Check flip of image for current camera intrinsics 
    /// @return flags - (horizontalFlipping, verticalFlipping)
    std::tuple<bool, bool> _checkFlippings() const;
};

} // namespace sonar

#endif // OPENCV_LIB

#endif // SONAR_MARKERTRACKINGSYSTEM_H
