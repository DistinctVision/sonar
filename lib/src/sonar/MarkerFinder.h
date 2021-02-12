#ifndef SONAR_MARKERFINDER_H
#define SONAR_MARKERFINDER_H

#if defined(OPENCV_LIB)

#include <vector>
#include <tuple>

#include <Eigen/Core>

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>

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

    /// Reset last markerid (this is for stable finding marker if we found many markers)
    void reset();

    /// Do finding of marker
    /// @param grayImage - input image for search
    /// @param markerId - find marker only with this id
    /// @return coordinates of marker corners or empty vector if marker is not found
    std::vector<Point2f> findMarker(const ImageRef<uchar> & grayImage, int markerId = -1);

    /// Get affine transform of marker (just test fnction).
    /// @param markerCorners - image coordinates of marker corners.
    /// @return aproximated affine matrix
    Eigen::Matrix3f computeAffineTransformOfMarker(const std::vector<Point2f> & imageMarkerCorners) const;

    /// Do finding of marker and get affine transform (just test fnction).
    /// @param grayImage - input image for search
    /// @param markerId - find marker only with this id
    /// @return tuple - aproximated affine matrix and True if marker is founded else identity matrix and False
    std::tuple<Eigen::Matrix3f, bool> findAffineTransformOfMarker(const ImageRef<uchar> & grayImage, int markerId = -1);

    /// Get homography transform of marker.
    /// @param markerCorners - image coordinates of marker corners.
    /// @return homography matrix
    Eigen::Matrix3f computeHomographyTransformOfMarker(const std::vector<Point2f> & imageMarkerCorners) const;

private:
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> m_detectorParameters;

    int m_lastMarkerId;

};

} // namespace sonar

#endif // OPENCV_LIB

#endif // SONAR_MARKERFINDER_H
