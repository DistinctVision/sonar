#include "sonar/ImageTools/OpticalFlow.h"

#include <cassert>

using namespace std;

namespace sonar {

OpticalFlow::OpticalFlow()
{
    setNumberLevels(3);
    setCursorSize(Point2i(20, 20));
}

Point2i OpticalFlow::cursorSize() const
{
    return m_opticalFlowCalculator.cursorSize();
}

void OpticalFlow::setCursorSize(const Point2i & size)
{
    m_opticalFlowCalculator.setCursorSize(size);
    m_maxVelocitySquared = cast<float>(min(size.x, size.y));
    m_maxVelocitySquared *= m_maxVelocitySquared;
}

float OpticalFlow::pixelEps() const
{
    return m_opticalFlowCalculator.pixelEps();
}

void OpticalFlow::setPixelEps(float pixelEps)
{
    m_opticalFlowCalculator.setPixelEps(pixelEps);
}

int OpticalFlow::maxNumberIterations() const
{
    return m_opticalFlowCalculator.numberIterations();
}

void OpticalFlow::setMaxNumberIterations(int count)
{
    m_opticalFlowCalculator.setNumberIterations(count);
}

int OpticalFlow::numberLevels() const
{
    return m_numberLevels;
}

void OpticalFlow::setNumberLevels(int numberLevels)
{
    assert(numberLevels > 0);

    m_numberLevels = numberLevels;
}

float OpticalFlow::detThreshold() const
{
    return m_opticalFlowCalculator.detThreshold();
}

void OpticalFlow::setDetThreashold(float value)
{
    m_opticalFlowCalculator.setDetThreshold(value);
}

ImagePyramid_u OpticalFlow::firstPyramid() const
{
    return m_firstPyramid;
}

ImagePyramid_u OpticalFlow::secondPyramid() const
{
    return m_secondPyramid;
}

void OpticalFlow::setFirstPyramid(const ImagePyramid_u & firstPyramid)
{
    assert(!firstPyramid.isNull());

    m_firstPyramid = ImagePyramid_u(firstPyramid, m_numberLevels);
}

void OpticalFlow::setSecondPyramid(const ImagePyramid_u & secondPyramid)
{
    assert(!secondPyramid.isNull());

    m_secondPyramid = ImagePyramid_u(secondPyramid, m_numberLevels);
}

void OpticalFlow::setFirstImage(const ImageRef<uchar> & image)
{
    setFirstPyramid(ImagePyramid_u(image, m_numberLevels));
}

void OpticalFlow::setSecondImage(const ImageRef<uchar> &image)
{
    setSecondPyramid(ImagePyramid_u(image, m_numberLevels));
}

void OpticalFlow::reset()
{
    m_firstPyramid.clear();
    m_secondPyramid.clear();
}

TrackingResult OpticalFlow::tracking2d(Point2f & secondPosition, const Point2f & firstPosition)
{
    Point2f p, prev;
    for (int level = m_numberLevels - 1; level >= 0; --level)
    {
        m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(level));
        m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(level));
        float scale = cast<float>(1 << level);
        p = secondPosition / scale;
        prev = p;
        if (m_opticalFlowCalculator.tracking2d(p, firstPosition / scale) != TrackingResult::Fail)
        {
            if ((p - prev).lengthSquared() < m_maxVelocitySquared)
            {
                secondPosition = p * scale;
            }
            else
            {
                return TrackingResult::Fail;
            }
        }
    }

    m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(0));
    m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(0));
    p = secondPosition;
    prev = p;
    TrackingResult result = m_opticalFlowCalculator.tracking2d(p, firstPosition);
    if (result != TrackingResult::Fail)
    {
        if ((p - prev).lengthSquared() < m_maxVelocitySquared)
        {
            secondPosition = p;
        }
        else
        {
            return TrackingResult::Fail;
        }
    }
    return result;
}

void OpticalFlow::tracking2d(vector<TrackingResult> & status,
                             vector<Point2f> & secondPoints,
                             const vector<Point2f> & firstPoints)
{
    assert(secondPoints.size() == firstPoints.size());

    if (status.size() != secondPoints.size())
    {
        status.resize(secondPoints.size());
    }
    fill(status.begin(), status.end(), TrackingResult::Completed);
    Point2f p, prev;
    for (int level = m_numberLevels - 1; level >= 0; --level)
    {

        m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(level));
        m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(level));
        float scale = cast<float>(1 << level);
        vector<Point2f>::iterator itSecond = secondPoints.begin();
        vector<Point2f>::const_iterator itFirst = firstPoints.cbegin();
        vector<TrackingResult>::iterator itSuccess = status.begin();
        for ( ; itSecond != secondPoints.end(); ++itSecond, ++itFirst, ++itSuccess)
        {
            if (*itSuccess == TrackingResult::Fail)
                continue;
            p = *itSecond / scale;
            prev = p;
            if (m_opticalFlowCalculator.tracking2d(p, *itFirst / scale) != TrackingResult::Fail)
            {
                if ((p - prev).lengthSquared() < m_maxVelocitySquared)
                {
                    *itSecond = p * scale;
                }
                else
                {
                    *itSuccess = TrackingResult::Fail;
                }
            }
        }
    }

    m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(0));
    m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(0));
    vector<Point2f>::iterator itSecond = secondPoints.begin();
    vector<Point2f>::const_iterator itFirst = firstPoints.cbegin();
    vector<TrackingResult>::iterator itSuccess = status.begin();
    for ( ; itSecond != secondPoints.end(); ++itSecond, ++itFirst, ++itSuccess)
    {
        if (*itSuccess == TrackingResult::Fail)
            continue;
        p = *itSecond;
        prev = p;
        *itSuccess = m_opticalFlowCalculator.tracking2d(p, *itFirst);
        if (*itSuccess != TrackingResult::Fail)
        {
            if ((p - prev).lengthSquared() < m_maxVelocitySquared)
            {
                *itSecond = p;
            }
            else
            {
                *itSuccess = TrackingResult::Fail;
            }
        }
    }
}

TrackingResult OpticalFlow::tracking2dLK(Point2f & secondPosition, const Point2f & firstPosition)
{
    Point2f p, prev;
    for (int level = m_numberLevels - 1; level >= 0; --level)
    {

        m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(level));
        m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(level));
        float scale = cast<float>(1 << level);
        p = secondPosition / scale;
        prev = p;
        if (m_opticalFlowCalculator.tracking2dLK(p, firstPosition / scale) != TrackingResult::Fail)
        {
            if ((p - prev).lengthSquared() < m_maxVelocitySquared)
            {
                secondPosition = p * scale;
            }
            else
            {
                return TrackingResult::Fail;
            }
        }
    }
    m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(0));
    m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(0));
    p = secondPosition;
    prev = p;
    TrackingResult result = m_opticalFlowCalculator.tracking2dLK(p, firstPosition);
    if (result != TrackingResult::Fail)
    {
        if ((p - prev).lengthSquared() < m_maxVelocitySquared)
        {
            secondPosition = p;
        }
        else
        {
            return TrackingResult::Fail;
        }
    }
    return result;
}

void OpticalFlow::tracking2dLK(vector<TrackingResult> & status,
                               vector<Point2f> & secondPoints,
                               const vector<Point2f> & firstPoints)
{
    assert(secondPoints.size() == firstPoints.size());

    if (status.size() != secondPoints.size())
    {
        status.resize(secondPoints.size(), TrackingResult::Completed);
    }
    fill(status.begin(), status.end(), TrackingResult::Completed);

    Point2f p, prev;
    for (int level = m_numberLevels - 1; level >= 0; --level)
    {

        m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(level));
        m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(level));
        float scale = cast<float>(1 << level);
        vector<Point2f>::iterator itSecond = secondPoints.begin();
        vector<Point2f>::const_iterator itFirst = firstPoints.cbegin();
        vector<TrackingResult>::iterator itSuccess = status.begin();
        for ( ; itSecond != secondPoints.end(); ++itSecond, ++itFirst, ++itSuccess)
        {
            if (*itSuccess == TrackingResult::Fail)
                continue;
            p = *itSecond / scale;
            prev = p;
            if (m_opticalFlowCalculator.tracking2dLK(p, *itFirst / scale) != TrackingResult::Fail)
            {
                if ((p - prev).lengthSquared() < m_maxVelocitySquared)
                {
                    *itSecond = p * scale;
                }
                else
                {
                    *itSuccess = TrackingResult::Fail;
                }
            }
        }
    }

    m_opticalFlowCalculator.setFirstImage(m_firstPyramid.get(0));
    m_opticalFlowCalculator.setSecondImage(m_secondPyramid.get(0));
    vector<Point2f>::iterator itSecond = secondPoints.begin();
    vector<Point2f>::const_iterator itFirst = firstPoints.cbegin();
    vector<TrackingResult>::iterator itSuccess = status.begin();
    for ( ; itSecond != secondPoints.end(); ++itSecond, ++itFirst, ++itSuccess)
    {
        if (*itSuccess == TrackingResult::Fail)
            continue;
        p = *itSecond;
        prev = p;
        *itSuccess = m_opticalFlowCalculator.tracking2dLK(p, *itFirst);
        if (*itSuccess != TrackingResult::Fail)
        {
            if ((p - prev).lengthSquared() < m_maxVelocitySquared)
            {
                *itSecond = p;
            }
            else
            {
                *itSuccess = TrackingResult::Fail;
            }
        }
    }
}

void OpticalFlow::swapFirstSecond()
{
    ImagePyramid_u temp = m_firstPyramid;
    m_firstPyramid = m_secondPyramid;
    m_secondPyramid = temp;
}

} // namespace sonar
