/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_OPTICALFLOW_H
#define SONAR_OPTICALFLOW_H

#include <vector>
#include <utility>

#include "sonar/General/Image.h"
#include "sonar/General/ImagePyramid.h"
#include "sonar/ImageTools/OpticalFlowCalculator.h"

namespace sonar {

class Frame;

class OpticalFlow
{
public:
    OpticalFlow();

    Point2i cursorSize() const;
    void setCursorSize(const Point2i & size);

    float pixelEps() const;
    void setPixelEps(float pixelEps);

    int maxNumberIterations() const;
    void setMaxNumberIterations(int count);

    int numberLevels() const;
    void setNumberLevels(int numberLevels);

    float detThreshold() const;
    void setDetThreashold(float value);

    ImagePyramid_u firstPyramid() const;
    ImagePyramid_u secondPyramid() const;

    void setFirstPyramid(const ImagePyramid_u & firstPyramid);
    void setSecondPyramid(const ImagePyramid_u & secondPyramid);

    void setFirstImage(const ImageRef<uchar> & image);
    void setSecondImage(const ImageRef<uchar> & image);

    void swapFirstSecond();

    TrackingResult tracking2d(Point2f & secondPosition, const Point2f & firstPosition);
    void tracking2d(std::vector<TrackingResult> & status,
                    std::vector<Point2f> & secondPoints,
                    const std::vector<Point2f> & firstPoints);
    TrackingResult tracking2dLK(Point2f & secondPosition, const Point2f & firstPosition);
    void tracking2dLK(std::vector<TrackingResult> & status,
                      std::vector<Point2f> & secondPoints,
                      const std::vector<Point2f> & firstPoints);

    void reset();

protected:
    ImagePyramid_u m_firstPyramid;
    ImagePyramid_u m_secondPyramid;
    OpticalFlowCalculator m_opticalFlowCalculator;

    float m_maxVelocitySquared;
    int m_numberLevels;
};

} // namespace sonar

#endif // SONAR_OPTICALFLOW_H
