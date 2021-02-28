#include "test_marker_pose_tracking.h"

#if defined(OPENCV_LIB)

#include <iostream>

#include <Eigen/Core>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "sonar/global_types.h"
#include "sonar/General/MathUtils.h"
#include "sonar/General/Image.h"
#include "sonar/General/ImageUtils.h"
#include "sonar/General/Paint.h"

#include "sonar/MarkerTrackingSystem.h"
#include "sonar/CameraTools/PinholeCameraIntrinsics.h"

using namespace std;
using namespace Eigen;
using namespace sonar;
using namespace sonar::math_utils;

namespace test_marker_pose_tracking_internal {

void drawCube(const cv::Mat & cvImage, const cv::Scalar & color, const shared_ptr<CameraIntrinsics> & camera, const Pose_f & pose)
{
    Vectors3f cubeVertices({
        Vector3f(0.0f, 0.0f, 0.0f),
        Vector3f(1.0f, 0.0f, 0.0f),
        Vector3f(1.0f, 1.0f, 0.0f),
        Vector3f(0.0f, 1.0f, 0.0f),
        Vector3f(0.0f, 0.0f, 0.5f),
        Vector3f(1.0f, 0.0f, 0.5f),
        Vector3f(1.0f, 1.0f, 0.5f),
        Vector3f(0.0f, 1.0f, 0.5f),
    });

    vector<pair<size_t, size_t>> cubeEdges({ {0, 1}, {1, 2}, {2, 3}, {3, 0},
                                             {4, 5}, {5, 6}, {6, 7}, {7, 4},
                                             {0, 4}, {1, 5}, {2, 6}, {3, 7} });
    vector<Point2f> projectedPoints(cubeVertices.size());
    for (size_t i = 0; i < cubeVertices.size(); ++i)
    {
        cubeVertices[i] = (pose.R * cubeVertices[i] + pose.t).eval();
        projectedPoints[i] = camera->projectPoint(projectToPoint(cubeVertices[i]));
    }
    for (const pair<size_t, size_t> & edge : cubeEdges)
    {
        if ((cubeVertices[edge.first].z() <= 0.0f) || (cubeVertices[edge.second].z() <= 0.0f))
            continue;
        cv::line(cvImage, cv_cast<float>(projectedPoints[edge.first]),
                cv_cast<float>(projectedPoints[edge.second]), color, 3);
    }
}

} // test_marker_pose_tracking_internal

bool test_marker_pose_tracking()
{
    using namespace test_marker_pose_tracking_internal;
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
    cv::Mat cvFrameImage;
    if (!capture.read(cvFrameImage))
    {
        cerr << "image is not received" << endl;
        return false;
    }

    float focalLength = cvFrameImage.cols * 1.25f;
    auto camera = make_shared<PinholeCameraIntrinsics>(Size2i(cvFrameImage.cols, cvFrameImage.rows),
                                                       Vector2f(focalLength, - focalLength),
                                                       Vector2f(cvFrameImage.cols * 0.5f, cvFrameImage.rows * 0.5f));
    auto trackingSystem = make_shared<MarkerTrackingSystem>(camera);

    Image<Rgb_u> frame_bgr;
    Image<uchar> frame_bw;

    while (capture.isOpened())
    {
        if (!capture.read(cvFrameImage))
        {
            break;
        }
        frame_bgr = image_utils::convertCvMat_rgb_u(cvFrameImage);
        frame_bw = image_utils::convertToGrayscale(frame_bgr);

        TrackingState trackingState = trackingSystem->process(frame_bw);

        if (trackingState == TrackingState::Tracking)
        {
            drawCube(cvFrameImage, cv::Scalar(0, 255, 0), camera, trackingSystem->lastPose().cast<float>());
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
