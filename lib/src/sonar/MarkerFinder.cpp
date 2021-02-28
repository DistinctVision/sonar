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
    m_targetMarkerId(-1),
    m_lastMarkerId(-1)
{
    m_dictionary = cv::aruco::getPredefinedDictionary(static_cast<int>(markersType));
    m_detectorParameters = cv::aruco::DetectorParameters::create();
}

int MarkerFinder::lastMarkerId() const
{
    return m_lastMarkerId;
}

int MarkerFinder::targetMarkerId() const
{
    return m_targetMarkerId;
}

void MarkerFinder::setTargetMarkerId(int targetMarkerId)
{
    m_targetMarkerId = targetMarkerId;
}

void MarkerFinder::reset()
{
    m_lastMarkerId = -1;
}

vector<Point2f> MarkerFinder::findMarker(const ImageRef<uchar> & grayImage, bool horizontalFlipping, bool verticalFlipping)
{
    vector<int> markersIds;
    vector<vector<cv::Point2f>> markersCorners;

    cv::Mat cvGrayImage = _prepeareFrameForMarkerDetection(grayImage, horizontalFlipping, verticalFlipping);
    cv::aruco::detectMarkers(cvGrayImage, m_dictionary, markersCorners, markersIds, m_detectorParameters);

    if (markersIds.empty())
        return {};

    int currentMarkerIndex = -1;
    if (m_targetMarkerId >= 0)
    {
        auto itMarker = find(markersIds.begin(), markersIds.end(), m_targetMarkerId);
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

    vector<Point2f> currentMarkerCorners = cv_cast<float>(markersCorners[cast<size_t>(currentMarkerIndex)]);
    return _unwarpPoints(currentMarkerCorners, cast<float>(grayImage.size()), horizontalFlipping, verticalFlipping);
}

tuple<Matrix3f, bool> MarkerFinder::findAffineTransformOfMarker(const ImageRef<uchar> & grayImage)
{
    vector<Point2f> markerCorners = findMarker(grayImage);
    if (markerCorners.empty())
        return make_tuple(Matrix3f::Identity(), false);

    return make_tuple(computeAffineTransformOfMarker(markerCorners), true);
}

Matrix3f MarkerFinder::computeAffineTransformOfMarker(const vector<Point2f> & imageMarkerCorners) const
{
    assert(imageMarkerCorners.size() == 4);

    vector<Vector2d, Eigen::aligned_allocator<Vector2d>> markerPoints(4);
    markerPoints[0] << 0.0, 0.0;
    markerPoints[1] << 1.0, 0.0;
    markerPoints[2] << 1.0, 1.0;
    markerPoints[3] << 0.0, 1.0;

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
    markerPoints[1] << 1.0, 0.0;
    markerPoints[2] << 1.0, 1.0;
    markerPoints[3] << 0.0, 1.0;

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

Pose_f MarkerFinder::getPose(const vector<Point2f> & imageMarkerCorners, const Eigen::Matrix3f & K)
{
    assert(imageMarkerCorners.size() == 4);
    Matrix3f H = computeHomographyTransformOfMarker(imageMarkerCorners);
    Matrix3f Rt = K.inverse() * H;
    float scale = (Rt.col(0).norm() + Rt.col(1).norm()) * 0.5f;
    Vector3f r0 = Rt.col(0).normalized();
    Vector3f r1 = Rt.col(1).normalized();
    Vector3f r2 = r0.cross(r1);
    Pose_f pose;
    pose.R.col(0) = r0;
    pose.R.col(1) = r1;
    pose.R.col(2) = r2;
    pose.t = Rt.col(2) / scale;
    Vector3f worldPosition = - pose.R.transpose() * pose.t;
    if (worldPosition.z() < 0.0f)
    {
        pose.R.col(0) = (- pose.R.col(0)).eval();
        pose.R.col(1) = (- pose.R.col(1)).eval();
        pose.t = - pose.t;
    }
    // convert to quaternion and back for fix rotation matrix
    pose.R = Quaternionf(pose.R).toRotationMatrix();
    return pose;
}


cv::Mat MarkerFinder::_prepeareFrameForMarkerDetection(const ImageRef<uchar> & frame, bool horizontalFlipping, bool verticalFlipping) const
{
    cv::Mat cvFrame = image_utils::convertToCvMat(frame);
    if (horizontalFlipping)
    {
        if (verticalFlipping)
        {
            cv::flip(cvFrame, cvFrame, -1);
        }
        else
        {
            cv::flip(cvFrame, cvFrame, 1);
        }
    }
    else
    {
        if (verticalFlipping)
        {
            cv::flip(cvFrame, cvFrame, 0);
        }
    }
    return cvFrame;
}

vector<Point2f> MarkerFinder::_unwarpPoints(const vector<Point2f> & points, 
                                            const Size2f & imageSize, bool horizontalFlipping, bool verticalFlipping) const
{
    vector<Point2f> outPoints(points.size());
    if (horizontalFlipping)
    {
        if (verticalFlipping)
        {
            for (size_t i = 0; i < points.size(); ++i)
            {
                const Point2f & p = points[i];
                outPoints[i].set(imageSize.x - 1.0f - p.x, imageSize.y - 1.0f - p.y);
            }
        }
        else
        {
            for (size_t i = 0; i < points.size(); ++i)
            {
                const Point2f& p = points[i];
                outPoints[i].set(imageSize.x - 1.0f - p.x, p.y);
            }
        }
    }
    else
    {
        if (verticalFlipping)
        {
            for (size_t i = 0; i < points.size(); ++i)
            {
                const Point2f& p = points[i];
                outPoints[i].set(p.x, imageSize.y - 1.0f - p.y);
            }
        }
        else
        {
            for (size_t i = 0; i < points.size(); ++i)
                outPoints[i] = points[i];
        }
    }
    return outPoints;
}

} // namespace sonar

#endif // OPENCV_LIB
