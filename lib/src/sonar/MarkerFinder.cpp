#include "MarkerFinder.h"

#if defined(OPENCV_LIB)

#include <algorithm>

#include <Eigen/SVD>

#include "sonar/General/cast.h"
#include "sonar/General/ImageUtils.h"

using namespace std;
using namespace Eigen;

namespace sonar {

MarkerFinder::MarkerFinder(MarkerFinder::MarkersDictionaryType markersType):
    m_lastMarkerId(-1)
{
    m_dictionary = cv::aruco::getPredefinedDictionary(static_cast<int>(markersType));
    m_detectorParameters = cv::aruco::DetectorParameters::create();
}

void MarkerFinder::reset()
{
    m_lastMarkerId = -1;
}

vector<Point2f> MarkerFinder::findMarker(const ImageRef<uchar> & grayImage, int markerId)
{
    vector<int> markersIds;
    vector<vector<cv::Point2f>> markerCorners;

    cv::Mat cvGrayImage = image_utils::convertToCvMat(grayImage);
    cv::aruco::detectMarkers(cvGrayImage, m_dictionary, markerCorners, markersIds, m_detectorParameters);

    if (markersIds.empty())
        return {};

    int currentMarkerIndex = -1;
    if (markerId >= 0)
    {
        auto itMarker = find(markersIds.begin(), markersIds.end(), markerId);
        if (itMarker != markersIds.end())
            currentMarkerIndex = cast<int>(itMarker - markersIds.begin());
    }

    if (currentMarkerIndex < 0)
    {
        if (m_lastMarkerId >= 0)
        {
            auto itMarker = find(markersIds.begin(), markersIds.end(), m_lastMarkerId);
            if (itMarker != markersIds.end())
                currentMarkerIndex = cast<int>(itMarker - markersIds.begin());
        }
    }

    if (currentMarkerIndex < 0)
        currentMarkerIndex = 0;

    m_lastMarkerId = markersIds[cast<size_t>(currentMarkerIndex)];

    return cv_cast<float>(markerCorners[cast<size_t>(currentMarkerIndex)]);
}

tuple<Matrix3f, bool> MarkerFinder::findAffineTransformOfMarker(const ImageRef<uchar> & grayImage, int markerId)
{
    vector<Point2f> markerCorners = findMarker(grayImage, markerId);
    if (markerCorners.empty())
        return make_tuple(Matrix3f::Identity(), false);

    return make_tuple(computeAffineTransformOfMarker(markerCorners), true);
}

Matrix3f MarkerFinder::computeAffineTransformOfMarker(const vector<Point2f> & imageMarkerCorners) const
{
    assert(imageMarkerCorners.size() == 4);

    vector<Vector2d, Eigen::aligned_allocator<Vector2d>> markerPoints(4);
    markerPoints[0] << 0.0, 0.0;
    markerPoints[1] << 0.0, 1.0;
    markerPoints[2] << 1.0, 1.0;
    markerPoints[3] << 1.0, 0.0;

    MatrixXd A(8, 6);
    VectorXd b(8);
    int offset = 0;
    for (size_t i = 0; i < 4; ++i)
    {
        const Vector2d & planePoint = markerPoints[i];
        const Point2f & imagePoint = imageMarkerCorners[i];

        A.row(offset) << planePoint.x(), planePoint.y(), 1.0, 0.0, 0.0, 0.0;
        b(offset) = static_cast<double>(imagePoint.x);
        ++offset;

        A.row(offset) << 0.0, 0.0, 0.0, planePoint.x(), planePoint.y(), 1.0;
        b(offset) = static_cast<double>(imagePoint.y);
        ++offset;
    }

    VectorXd x = (A.transpose() * A).inverse() * A.transpose() * b;

    Matrix3d H;
    H << x(0), x(1), x(2), x(3), x(4), x(5), 0.0, 0.0, 1.0;

    return H.cast<float>();
}

Matrix3f MarkerFinder::computeHomographyTransformOfMarker(const vector<Point2f> & imageMarkerCorners) const
{
    assert(imageMarkerCorners.size() == 4);

    vector<Vector2d, Eigen::aligned_allocator<Vector2d>> markerPoints(4);
    markerPoints[0] << 0.0, 0.0;
    markerPoints[1] << 0.0, 1.0;
    markerPoints[2] << 1.0, 1.0;
    markerPoints[3] << 1.0, 0.0;

    MatrixXd A(9, 9);
    int offset = 0;
    for (size_t i = 0; i < 4; ++i)
    {
        const Vector2d & planePoint = markerPoints[i];
        const Point2f & imagePoint = imageMarkerCorners[i];

        A.row(offset) << planePoint.x(), planePoint.y(), 1.0,
                         0.0, 0.0, 0.0,
                         - planePoint.x() * imagePoint.x, - planePoint.y() * imagePoint.x, - imagePoint.x;
        ++offset;

        A.row(offset) << 0.0, 0.0, 0.0,
                         planePoint.x(), planePoint.y(), 1.0,
                         - planePoint.x() * imagePoint.y, - planePoint.y() * imagePoint.y, - imagePoint.y;
        ++offset;
    }
    A.row(8).setZero();

    BDCSVD<MatrixXd> svd(A, Eigen::ComputeThinV);
    Matrix<double, 9, 1> h = svd.matrixV().col(8);

    Matrix3d H;
    H << h(0), h(1), h(2),
         h(3), h(4), h(5),
         h(6), h(7), h(8);

    return H.cast<float>();
}

} // namespace sonar

#endif // OPENCV_LIB
