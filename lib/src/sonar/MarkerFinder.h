/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_MARKERFINDER_H
#define SONAR_MARKERFINDER_H

#if defined(OPENCV_LIB)

#include <vector>
#include <tuple>

#include <Eigen/Eigen>

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>

#include "global_types.h"
#include "sonar/General/Point2.h"
#include "sonar/General/Image.h"

namespace sonar {

/// Class for search marker and get info about it.
/// This class find only one marker and remember id of it. On next frame will use marker with last marker or else use new marker.
/// For marker corners use this uv coordinates: [(0, 0), (0, 1), (1, 1), (1, 0)] - normalized image corners coordinates.
class MarkerFinder
{
public:
    enum class MarkersDictionaryType
    {
        DICT_5x5_50 = cv::aruco::DICT_5X5_50
    };

    MarkerFinder(MarkersDictionaryType markersType);

    /// Get target marker id
    /// @return -1 for any markers and value >= 0 if filter by id is used
    int targetMarkerId() const;

    /// Set target marker id, and we will find marker with only this id
    /// @param targetMarkerId - -1 if we can find any markers or id >= 0 for finding marker only with this id
    void setTargetMarkerId(int targetMarkerId);

    /// Get last marker id
    /// @return -1 - if marker is not found else value >= 0
    int lastMarkerId() const;

    /// Reset last markerid (this is for stable finding marker if we found many markers)
    void reset();

    /// Do finding of marker
    /// @param grayImage - input image for search
    /// @return coordinates of marker corners or empty vector if marker is not found
    std::vector<Point2f> findMarker(const ImageRef<uchar> & grayImage);

    /// Get affine transform of marker (just test fnction).
    /// @param markerCorners - image coordinates of marker corners.
    /// @return aproximated affine matrix
    [[deprecated]]
    Eigen::Matrix3f computeAffineTransformOfMarker(const std::vector<Point2f> & imageMarkerCorners) const;

    /// Do finding of marker and get affine transform (just test fnction).
    /// @param grayImage - input image for search
    /// @return tuple - aproximated affine matrix and True if marker is founded else identity matrix and False
    /// @return aproximated affine matrix
    [[deprecated]]
    std::tuple<Eigen::Matrix3f, bool> findAffineTransformOfMarker(const ImageRef<uchar> & grayImage);

    /// Get homography transform of marker.
    /// @param markerCorners - image coordinates of marker corners.
    /// @return homography matrix
    /// @return aproximated affine matrix
    [[deprecated]]
    Eigen::Matrix3f computeHomographyTransformOfMarker(const std::vector<Point2f> & imageMarkerCorners) const;

    /// Get pose from pixel coordinates of marker corners
    Pose_f getPose(const std::vector<Point2f> & imageMarkerCorners, const Eigen::Matrix3f & K);

private:
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> m_detectorParameters;

    int m_targetMarkerId;
    int m_lastMarkerId;
};

} // namespace sonar

#endif // OPENCV_LIB

#endif // SONAR_MARKERFINDER_H
