#include "test_marker_transform.h"

#if defined(OPENCV_LIB)

#include <iostream>

#include <Eigen/Core>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "sonar/General/Image.h"
#include "sonar/General/ImageUtils.h"

#include "sonar/MarkerFinder.h"

using namespace std;
using namespace Eigen;
using namespace sonar;

bool test_marker_transform(bool useAffineFlag)
{
    auto convertMatrixToCv = [] (const Matrix3f & M) -> cv::Mat
    {
        cv::Mat cvM(3, 3, CV_32F);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                cvM.at<float>(i, j) = M(i, j);
        return cvM;
    };

    cv::VideoCapture capture;

    if (!capture.open(0))
    {
        cerr << "camera not found" << endl;
        return false;
    }

    if (!capture.isOpened())
    {
        cerr << "camera not open" << endl;
        return false;
    }

    cv::Mat cvTestImage = cv::imread("./../../sonar/tests/assets/test.jpg");
    if (cvTestImage.empty())
    {
        cerr << "test image is not founded" << endl;
        return false;
    }
    cv::Mat cvTestMask(cvTestImage.size(), CV_8UC1, cv::Scalar(255));
    Matrix3f uvTransform;
    uvTransform << 1.0f / cast<float>(cvTestImage.cols), 0.0f, 0.0f,
                   0.0f, 1.0f / cast<float>(cvTestImage.rows), 0.0f,
                   0.0f, 0.0f, 1.0f;

    cv::Mat cvFrameImage;
    Image<Rgb_u> frame_bgr;
    Image<uchar> frame_bw;

    MarkerFinder markersFinder(MarkerFinder::MarkersDictionaryType::DICT_5x5_50);

    while (capture.isOpened())
    {
        if (!capture.read(cvFrameImage))
        {
            break;
        }
        frame_bgr = image_utils::convertCvMat_rgb_u(cvFrameImage);
        frame_bw = image_utils::convertToGrayscale(frame_bgr);

        vector<Point2f> markerCorners = markersFinder.findMarker(frame_bw);
        if (!markerCorners.empty())
        {
            Matrix3f transform;
            if (useAffineFlag)
            {
                transform = markersFinder.computeAffineTransformOfMarker(markerCorners);
            }
            else
            {
                transform = markersFinder.computeHomographyTransformOfMarker(markerCorners);
            }
            transform = (transform * uvTransform).eval();

            cv::Mat cvWarpMask;
            cv::warpPerspective(cvTestMask, cvWarpMask, convertMatrixToCv(transform),
                                cvFrameImage.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
            cv::Mat cvWarpImage;
            cv::warpPerspective(cvTestImage, cvWarpImage, convertMatrixToCv(transform),
                                cvFrameImage.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
            cvWarpImage.copyTo(cvFrameImage, cvWarpMask);

            for (size_t i = 0; i < 4; ++i)
            {
                const Point2f & p1 = markerCorners[i];
                const Point2f & p2 = markerCorners[(i + 1) % 4];
                cv::line(cvFrameImage, cv_cast<int>(p1), cv_cast<int>(p2), cv::Scalar(0, 0, 255), 2);
            }
        }

        cv::imshow("frame", cvFrameImage);

        int key = cv::waitKey(33);
        if ((key == 27) || (key == 32))
        {
            break;
        }
    }
    return true;
}

#endif // OPENCV_LIB
