/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_CAMERAINTRINSICS_H
#define SONAR_CAMERAINTRINSICS_H

#include <vector>

#include <Eigen/Eigen>

#include "sonar/General/Point2.h"

#include "sonar/global_types.h"

namespace sonar {

class CameraIntrinsics
{
public:
    CameraIntrinsics(const Size2i & resolution);

    virtual ~CameraIntrinsics();

    virtual CameraType type() const;

    Size2i resolution() const;

    virtual Point2f projectPoint(const Point2f & point) const = 0;
    virtual std::vector<Point2f> projectPoints(const std::vector<Point2f> & points) const;

    virtual Point2f unprojectPoint(const Point2f & point) const = 0;
    virtual std::vector<Point2f> unprojectPoints(const std::vector<Point2f> & points) const;

private:
    const Size2i m_resolution;
};

} // namespace sonar

#endif // SONAR_CAMERAINTRINSICS_H
