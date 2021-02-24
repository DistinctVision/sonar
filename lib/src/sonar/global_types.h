/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_GLOBAL_TYPES_H
#define SONAR_GLOBAL_TYPES_H

#include <vector>

#include <Eigen/Core>

#include "sonar/General/Point2.h"

#define SONAR_UNUSED(x) static_cast<void>(x)

#if defined(OPENGV_LIB)

#include <opengv/types.hpp>

#endif

namespace sonar {

class Uncopyable
{
public:
    Uncopyable() = default;

private:
    Uncopyable(const Uncopyable &) = delete ;
    Uncopyable & operator = (const Uncopyable &) = delete;
};

#if defined(OPENGV_LIB)
using point_t = opengv::point_t;
using points_t = opengv::points_t;
using bearingVector_t = opengv::bearingVector_t;
using bearingVectors_t = opengv::bearingVectors_t;
using translation_t = opengv::translation_t;
using essential_t = opengv::essential_t;
using essentials_t = opengv::essentials_t;
using transformation_t = opengv::transformation_t;
using transformations_t = opengv::transformations_t;
#else
using point_t = Eigen::Vector3d;
using points_t = std::vector<point_t, Eigen::aligned_allocator<point_t>>;
using bearingVector_t = Eigen::Vector3d;
using bearingVectors_t = std::vector<bearingVector_t, Eigen::aligned_allocator<bearingVector_t>>;
using translation_t = Eigen::Vector3d;
using essential_t = Eigen::Matrix3d;
using essentials_t = std::vector<essential_t, Eigen::aligned_allocator<essential_t>>;
using transformation_t = Eigen::Matrix<double, 3, 4>;
using transformations_t = std::vector<transformation_t, Eigen::aligned_allocator<transformation_t>>;
#endif

using Vectors2f = std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>>;
using Vectors3f = std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>>;

using Vectors2d = std::vector<Eigen::Vector2d, Eigen::aligned_allocator<Eigen::Vector2d>>;
using Vectors3d = std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>>;

using Matrices3f = std::vector<Eigen::Matrix3f, Eigen::aligned_allocator<Eigen::Matrix3f>>;
using Matrices3d = std::vector<Eigen::Matrix3d, Eigen::aligned_allocator<Eigen::Matrix3d>>;

template <typename Type>
struct Pose
{
    Eigen::Matrix<Type, 3, 3> R = Eigen::Matrix<Type, 3, 3>::Identity();
    Eigen::Matrix<Type, 3, 1> t = Eigen::Matrix<Type, 3, 1>::Zero();

    Eigen::Matrix<Type, 3, 1> worldPosition() const
    {
        return - this->R.inverse() * this->t;
    }

    void setWorldPosition(const Eigen::Matrix<Type, 3, 1> & worldPosition)
    {
        this->t = - this->R * worldPosition;
    }

    Eigen::Matrix<Type, 3, 3> worldRotation() const
    {
        return this->R.inverse();
    }

    void setWorldRotation(const Eigen::Matrix<Type, 3, 3> & worldRotation)
    {
        this->R = worldRotation.inverse();
    }

    Eigen::Matrix<Type, 3, 1> transform(const Eigen::Matrix<Type, 3, 1> & v) const
    {
        return this->R * v + this->t;
    }

    Pose<Type> transform(const Pose<Type> & pose) const
    {
        Pose<Type> localPose;
        localPose.R = this->R * pose.R;
        localPose.t = this->R * pose.t + this->t;
        return localPose;
    }

    Pose<Type> inverse() const
    {
        Pose<Type> invPose;
        invPose.R = this->R.inverse();
        invPose.t = - invPose.R * this->t;
        return invPose;
    }

    template <typename CastType>
    Pose<CastType> cast() const
    {
        Pose<CastType> castPose;
        castPose.R = this->R.template cast<CastType>();
        castPose.t = this->t.template cast<CastType>();
        return castPose;
    }
};

using Pose_f = Pose<float>;
using Pose_d = Pose<double>;

} // namespace sonar

#endif // SONAR_GLOBAL_TYPES_H
