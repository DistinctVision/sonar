/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_PINHOLECAMERAINTRINSICS_H
#define SONAR_PINHOLECAMERAINTRINSICS_H

#include "CameraIntrinsics.h"

namespace sonar {

class PinholeCameraIntrinsics:
        public CameraIntrinsics
{
public:
    PinholeCameraIntrinsics(const Size2i & resolution,
                            const Eigen::Vector2f & focalLength,
                            const Eigen::Vector2f & opticalCenter);

    CameraType type() const override;

    Eigen::Vector2f focalLength() const;
    Eigen::Vector2f opticalCenter() const;

    Point2f projectPoint(const Point2f & point) const override;
    std::vector<Point2f> projectPoints(const std::vector<Point2f> & points) const override;

    Point2f unprojectPoint(const Point2f & point) const override;
    std::vector<Point2f> unprojectPoints(const std::vector<Point2f> & points) const override;

    Eigen::Matrix4f glProjectionMatrix(float depthNear, float depthFar) const;

private:
    const Eigen::Vector2f m_focalLength;
    const Eigen::Vector2f m_opticalCenter;
};

} // namespace sonar

#endif // SONAR_PINHOLECAMERAINTRINSICS_H
