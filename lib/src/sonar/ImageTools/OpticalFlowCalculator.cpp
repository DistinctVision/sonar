#include "sonar/ImageTools/OpticalFlowCalculator.h"

#include <cmath>
#include <limits>
#include <climits>
#include <cassert>

#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;

namespace sonar {

OpticalFlowCalculator::OpticalFlowCalculator()
{
    m_invMaxErrorSquared = 0.0f;
    m_cursorSize.set(4, 4);
    setCursorSize(m_cursorSize);
    m_pixelEps = 1e-2f;
    m_detThreshold = 1e-2f;
    m_numberIterations = 8;
    m_lastDet = 0.0f;
}

OpticalFlowCalculator::~OpticalFlowCalculator()
{
}

Point2i OpticalFlowCalculator::cursorSize() const
{
    return m_cursorSize;
}

int OpticalFlowCalculator::cursorWidth() const
{
    return m_cursorSize.x;
}

int OpticalFlowCalculator::cursorHeight() const
{
    return m_cursorSize.y;
}

void OpticalFlowCalculator::setCursorSize(const Point2i & cursorSize)
{
    assert((cursorSize.x > 0) && (cursorSize.y > 0));

    m_cursorSize = cursorSize;
    m_pathSize = m_cursorSize * 2 + Point2i(1, 1);
    m_sigmaGaussian = std::max(m_cursorSize.x, m_cursorSize.y) / 2.5f;
    m_derivatives1d = Image<float>(m_pathSize);
    m_derivatives2d = Image<Point2f>(m_pathSize);
    _solveGaussian();
    if  (!m_secondImage.isNull())
    {
        m_begin_first = m_cursorSize + Point2i(2, 2);
        m_end_first = m_firstImage.size() - (m_cursorSize + Point2i(3, 3));
        m_begin_second = m_cursorSize + Point2i(2, 2);
        m_end_second = m_secondImage.size() - (m_cursorSize + Point2i(3, 3));
    }
    m_path = Image<uchar>(m_pathSize + Point2i(2, 2));
}

Image<uchar> OpticalFlowCalculator::patch()
{
    return m_path;
}

ConstImage<uchar> OpticalFlowCalculator::patch() const
{
    return m_path;
}

void OpticalFlowCalculator::_setSigmaGaussian(float sigma)
{
    m_sigmaGaussian = sigma;
    _solveGaussian();
}

void OpticalFlowCalculator::_solveGaussian()
{
    double maxErrorSquared = 0.0;
    float sigmaSquare = m_sigmaGaussian * m_sigmaGaussian;
    float k1 = cast<float>(1.0 / (2.0 * M_PI * sigmaSquare));
    float invK2 = 1.0f / (2.0f * sigmaSquare);
    float sumK = 0.0f;
    Point2i p;
    Image<float> gaussian(m_pathSize);
    float * gaussian_data = gaussian.data();
    int gaussian_size = gaussian.area();
    m_gaussian = gaussian;
    int k = 0;
    for (p.y = - m_cursorSize.y; p.y <= m_cursorSize.y; ++p.y)
    {
        for (p.x = - m_cursorSize.x; p.x <= m_cursorSize.x; ++p.x, ++k)
        {
            gaussian_data[k] = k1 * exp(- p.lengthSquared() * invK2);
            sumK += gaussian_data[k];
            maxErrorSquared += (gaussian_data[k] * gaussian_data[k]) * (255.0 * 255.0);
        }
    }
    for (k = 0; k < gaussian_size; ++k)
        gaussian_data[k] /= sumK;
    maxErrorSquared /= sumK;
    if (maxErrorSquared > numeric_limits<float>::epsilon())
        m_invMaxErrorSquared = cast<float>(1.0 / maxErrorSquared);
    else
        m_invMaxErrorSquared = 0.0f;
}

int OpticalFlowCalculator::numberIterations() const
{
    return m_numberIterations;
}

void OpticalFlowCalculator::setNumberIterations(int numberIterations)
{
    m_numberIterations = numberIterations;
}

float OpticalFlowCalculator::pixelEps() const
{
    return m_pixelEps;
}

void OpticalFlowCalculator::setPixelEps(float eps)
{
    m_pixelEps = fabs(eps);
}

float OpticalFlowCalculator::detThreshold() const
{
    return m_detThreshold;
}

void OpticalFlowCalculator::setDetThreshold(float detThreshold)
{
    m_detThreshold = detThreshold;
}

float OpticalFlowCalculator::lastDet() const
{
    return m_lastDet;
}

void OpticalFlowCalculator::setFirstImage(const ImageRef<uchar> & image)
{
    m_firstImage = image;
    m_begin_first = m_cursorSize + Point2i(2, 2);
    m_end_first = m_firstImage.size() - (m_cursorSize + Point2i(3, 3));
}

ConstImage<uchar> OpticalFlowCalculator::firstImage() const
{
    return m_firstImage;
}

void OpticalFlowCalculator::setSecondImage(const ImageRef<uchar> & image)
{
    m_secondImage = image;
    m_begin_second = m_cursorSize + Point2i(2, 2);
    m_end_second = m_secondImage.size() - (m_cursorSize + Point2i(3, 3));
}

ConstImage<uchar> OpticalFlowCalculator::secondImage() const
{
    return m_secondImage;
}

void OpticalFlowCalculator::getSubPixelImageF(Image<float> & outImage,
                                              const ImageRef<uchar> & image,
                                              const Point2f & beginPoint)
{
    Point2i beginPoint_i(cast<int>(floor(beginPoint.x)), cast<int>(floor(beginPoint.y)));
    assert((beginPoint_i.x >= 0) && (beginPoint_i.y >= 0));
    assert(((beginPoint_i.x + outImage.width() + 1) <= image.width()) &&
            ((beginPoint_i.y + outImage.height() + 1) <= image.height()));
    const uchar * imageStr = image.pointer(beginPoint_i);
    const uchar * imageStrNext = &imageStr[image.widthStep()];

    float * outStr = outImage.data();

    Point2f sub_pix(beginPoint.x - beginPoint_i.x, beginPoint.y - beginPoint_i.y);
    // interpolation weights
    float wTL, wTR, wBL, wBR;
    wTL = (1.0f - sub_pix.x) * (1.0f - sub_pix.y);
    wTR = sub_pix.x * (1.0f - sub_pix.y);
    wBL = (1.0f - sub_pix.x) * sub_pix.y;
    wBR = sub_pix.x * sub_pix.y;

    Point2i p;
    for (p.y = 0; p.y < outImage.height(); ++p.y)
    {
        for (p.x = 0; p.x < outImage.width(); ++p.x)
        {
            outStr[p.x] = wTL * imageStr[p.x] + wTR * imageStr[p.x + 1] +
                          wBL * imageStrNext[p.x] + wBR * imageStrNext[p.x + 1];
        }
        imageStr = imageStrNext;
        imageStrNext = &imageStrNext[image.widthStep()];
        outStr = &outStr[outImage.widthStep()];
    }
}

Image<float> OpticalFlowCalculator::getSubPixelImageF(const ImageRef<uchar> & image,
                                                      const Point2f & beginPoint,
                                                      const Point2i & size)
{
    Image<float> outImage(size);
    getSubPixelImageF(outImage, image, beginPoint);
    return outImage;
}

void OpticalFlowCalculator::getSubPixelImage(Image<uchar> & outImage,
                                             const ImageRef<uchar> & image,
                                             const Point2f & beginPoint)
{
    Point2i beginPoint_i(cast<int>(floor(beginPoint.x)), cast<int>(floor(beginPoint.y)));
    assert((beginPoint_i.x >= 0) && (beginPoint_i.y >= 0));
    assert(((beginPoint_i.x + outImage.width() + 1) <= image.width()) &&
           ((beginPoint_i.y + outImage.height() + 1) <= image.height()));
    const uchar * imageStr = image.pointer(beginPoint_i);
    const uchar * imageStrNext = &imageStr[image.widthStep()];

    uchar * outStr = outImage.data();

    Point2f sub_pix(beginPoint.x - beginPoint_i.x, beginPoint.y - beginPoint_i.y);
    // interpolation weights
    float wTL, wTR, wBL, wBR;
    wTL = (1.0f - sub_pix.x) * (1.0f - sub_pix.y);
    wTR = sub_pix.x * (1.0f - sub_pix.y);
    wBL = (1.0f - sub_pix.x) * sub_pix.y;
    wBR = sub_pix.x * sub_pix.y;

    Point2i p;
    for (p.y = 0; p.y < outImage.height(); ++p.y)
    {
        for (p.x = 0; p.x < outImage.width(); ++p.x)
        {
            outStr[p.x] = cast<uchar>(wTL * imageStr[p.x] + wTR * imageStr[p.x + 1] +
                                      wBL * imageStrNext[p.x] + wBR * imageStrNext[p.x + 1]);
        }
        imageStr = imageStrNext;
        imageStrNext = &imageStrNext[image.widthStep()];
        outStr = &outStr[outImage.widthStep()];
    }
}

Image<uchar> OpticalFlowCalculator::getSubPixelImage(const ImageRef<uchar> & image,
                                                     const Point2f & beginPoint,
                                                     const Point2i & size)
{
    Image<uchar> outImage(size);
    getSubPixelImage(outImage, image, beginPoint);
    return outImage;
}

TrackingResult OpticalFlowCalculator::tracking2d(Point2f & second, const Point2f & first)
{
    if ((first.x < m_begin_first.x) || (first.y < m_begin_first.y) ||
        (first.x >= m_end_first.x) || (first.y >= m_end_first.y))
        return TrackingResult::Fail;
    getSubPixelImage(m_path, m_firstImage, Point2f(first.x - (m_cursorSize.x + 1.5f),
                                                   first.y - (m_cursorSize.y + 1.5f)));
    return _tracking2dOnSecondImage(second, m_path.data(), m_path.widthStep());
}

TrackingResult OpticalFlowCalculator::tracking2dLK(Point2f & second, const Point2f & first)
{
    if ((first.x < m_begin_first.x) || (first.y < m_begin_first.y) ||
        (first.x >= m_end_first.x) || (first.y >= m_end_first.y))
        return TrackingResult::Fail;
    getSubPixelImage(m_path, m_firstImage, Point2f(first.x - (m_cursorSize.x + 1.5f),
                                                     first.y - (m_cursorSize.y + 1.5f)));
    return _tracking2dOnSecondImageLK(second, m_path.data(), m_path.widthStep());
}

TrackingResult OpticalFlowCalculator::tracking2dLK(Point2f & second, const Point2i & first)
{
    if ((first.x < m_begin_first.x) || (first.y < m_begin_first.y) ||
        (first.x >= m_end_first.x) || (first.y >= m_end_first.y))
        return TrackingResult::Fail;
    return _tracking2dOnSecondImageLK(second, m_firstImage.pointer(first), m_firstImage.widthStep());
}

TrackingResult OpticalFlowCalculator::tracking2d_patch(Point2f & second)
{
    return _tracking2dOnSecondImage(second, m_path.data(), m_path.widthStep());
}

TrackingResult OpticalFlowCalculator::tracking2dLK_patch(Point2f & position)
{
    return _tracking2dOnSecondImageLK(position, m_path.data(), m_path.widthStep());
}

TrackingResult OpticalFlowCalculator::horizontalTracking(Point2f & second, const Point2i & first)
{
    if ((first.x < m_begin_first.x) || (first.y < m_begin_first.y) ||
        (first.x >= m_end_first.x) || (first.y >= m_end_first.y))
        return TrackingResult::Fail;
    return _horizontalTrackingOnSecondImage(second, m_firstImage.pointer(first), m_firstImage.widthStep());
}

TrackingResult OpticalFlowCalculator::horizontalTrackingLK(Point2f & second, const Point2i & first)
{
    if ((first.x < m_begin_first.x) || (first.y < m_begin_first.y) ||
        (first.x >= m_end_first.x) || (first.y >= m_end_first.y))
        return TrackingResult::Fail;
    return _horizontalTrackingOnSecondImageLK(second, m_firstImage.pointer(first), m_firstImage.widthStep());
}

TrackingResult OpticalFlowCalculator::horizontalTracking(Point2f & second, const Point2f & first)
{
    if ((first.x < m_begin_first.x) || (first.y < m_begin_first.y) ||
        (first.x >= m_end_first.x) || (first.y >= m_end_first.y))
        return TrackingResult::Fail;
    getSubPixelImage(m_path, m_firstImage, Point2f(first.x - (m_cursorSize.x + 1.5f),
                                                   first.y - (m_cursorSize.y + 1.5f)));
    return _horizontalTrackingOnSecondImage(second, m_path.data(), m_path.widthStep());
}

TrackingResult OpticalFlowCalculator::horizontalTrackingLK(Point2f & second, const Point2f & first)
{
    if ((first.x < m_begin_first.x) || (first.y < m_begin_first.y) ||
        (first.x >= m_end_first.x) || (first.y >= m_end_first.y))
        return TrackingResult::Fail;
    getSubPixelImage(m_path, m_firstImage, Point2f(first.x - (m_cursorSize.x + 1.5f),
                                                   first.y - (m_cursorSize.y + 1.5f)));
    return _horizontalTrackingOnSecondImageLK(second, m_path.data(), m_path.widthStep());
}

TrackingResult OpticalFlowCalculator::_tracking2dOnSecondImage(Point2f & position, const uchar * imageRef, int imageStride)
{
    const uchar * basePathRef = &imageRef[imageStride + 1];
    const uchar * pathStr = basePathRef;
    const uchar * pathStrPrev = &pathStr[-imageStride];
    const uchar * pathStrNext = &pathStr[imageStride];
    Point2f wd;
    Point2i p;
    int k = 0;
    Matrix3f H;
    H.setZero();
    const float * gaussian_data = m_gaussian.data();
    Point2f * derivatives = m_derivatives2d.data();
    for (p.y = 0; p.y < m_pathSize.y; ++p.y)
    {
        for (p.x = 0; p.x < m_pathSize.x; ++p.x)
        {
            Point2f & d = derivatives[k];
            d.x = (pathStr[p.x + 1] - pathStr[p.x - 1]) * 0.5f;
            d.y = (pathStrNext[p.x] - pathStrPrev[p.x]) * 0.5f;
            wd = d * gaussian_data[k];

            // m_H - symmetric matrix
            H(0, 0) += d.x * wd.x;
            H(0, 1) += d.x * wd.y;
            H(0, 2) += wd.x;
            H(1, 1) += d.y * wd.y;
            H(1, 2) += wd.y;
            H(2, 2) += gaussian_data[k];

            ++k;
        }
        pathStrPrev = pathStr;
        pathStr = pathStrNext;
        pathStrNext = &pathStrNext[imageStride];
    }

    //m_lastDet = (m_H(0, 0) * m_H(1, 1) * m_H(2, 2) + m_H(0, 1) * m_H(1, 2) * m_H(2, 0) + m_H(0, 2) * m_H(2, 1) * m_H(1, 0)) -
    //            (m_H(0, 0) * m_H(1, 2) * m_H(2, 1) + m_H(0, 1) * m_H(1, 0) * m_H(2, 2) + m_H(0, 2) * m_H(1, 1) * m_H(2, 0));
    // determinant for symmetric matrix
    m_lastDet = (H(0, 0) * H(1, 1) * H(2, 2) + H(0, 1) * H(1, 2) * H(0, 2) + H(0, 2) * H(1, 2) * H(0, 1)) -
                (H(0, 0) * H(1, 2) * H(1, 2) + H(0, 1) * H(0, 1) * H(2, 2) + H(0, 2) * H(1, 1) * H(0, 2));

    if (fabs(m_lastDet) < m_detThreshold)
        return TrackingResult::Fail;

    H(1, 0) = H(0, 1);
    H(2, 0) = H(0, 2);
    H(2, 1) = H(1, 2);
    H /= m_lastDet;

    Point2i position_i;
    Point2f sub_pix;

    // compute interpolation weights
    float wTL, wTR, wBL, wBR;

    float wdt, pixelValue;
    float mean_diff = 0.0f;

    Vector3f Jres, delta;
    Point2f prevDelta;

    for (int iteration = 0; iteration < m_numberIterations; ++iteration)
    {
        position_i.x = cast<int>(floor(position.x));
        position_i.y = cast<int>(floor(position.y));

        if ((position_i.x < m_begin_second.x) || (position_i.y < m_begin_second.y) ||
            (position_i.x >= m_end_second.x) || (position_i.y >= m_end_second.y))
            return TrackingResult::Fail;

        // compute interpolation weights
        sub_pix.x = position.x - position_i.x;
        sub_pix.y = position.y - position_i.y;

        wTL = (1.0f - sub_pix.x) * (1.0f - sub_pix.y);
        wTR = sub_pix.x * (1.0f - sub_pix.y);
        wBL = (1.0f - sub_pix.x) * sub_pix.y;
        wBR = sub_pix.x * sub_pix.y;

        pathStr = basePathRef;

        const uchar * imageStr = m_secondImage.pointer(position_i - m_cursorSize);
        const uchar * imageStrNext = &imageStr[m_secondImage.widthStep()];

        Jres.setZero();

        k = 0;
        for (p.y = 0; p.y < m_pathSize.y; ++p.y)
        {
            for (p.x = 0; p.x < m_pathSize.x; ++p.x)
            {
                pixelValue = wTL * imageStr[p.x] + wTR * imageStr[p.x + 1] +
                             wBL * imageStrNext[p.x] + wBR * imageStrNext[p.x + 1]
                        + mean_diff;
                wdt = (pixelValue - pathStr[p.x]) * gaussian_data[k];

                Point2f & d = derivatives[k];

                Jres(0) += wdt * d.x;
                Jres(1) += wdt * d.y;
                Jres(2) += wdt;

                ++k;
            }
            pathStr = &pathStr[imageStride];
            imageStr = imageStrNext;
            imageStrNext = &imageStrNext[m_secondImage.widthStep()];
        }

        delta = H * Jres;
        position.x -= delta(0);
        position.y -= delta(1);
        mean_diff += delta(2);

        if (iteration > 0)
        {
            if ((fabs(delta(0) + prevDelta.x) < m_pixelEps) && (fabs(delta(1) + prevDelta.y) < m_pixelEps))
            {
                return TrackingResult::Completed;
            }
        }
        prevDelta.set(delta(0), delta(1));
    }
    return TrackingResult::Uncompleted;
}

TrackingResult OpticalFlowCalculator::_tracking2dOnSecondImageLK(Point2f & position, const uchar * imageRef, int imageStride)
{
    const uchar * basePathRef = &imageRef[imageStride + 1];

    const uchar * pathStr = basePathRef;
    const uchar * pathStrPrev = &pathStr[-imageStride];
    const uchar * pathStrNext = &pathStr[imageStride];
    Point2f wd;
    Point2i p;
    float Dxx = 0.0f, Dxy = 0.0f, Dyy = 0.0f;
    int k = 0;
    const float * gaussian_data = m_gaussian.data();
    Point2f * derivatives = m_derivatives2d.data();
    for (p.y = 0; p.y < m_pathSize.y; ++p.y)
    {
        for (p.x = 0; p.x < m_pathSize.x; ++p.x)
        {
            Point2f & d = derivatives[k];
            d.x = (pathStr[p.x + 1] - pathStr[p.x - 1]) * 0.5f;
            d.y = (pathStrNext[p.x] - pathStrPrev[p.x]) * 0.5f;
            wd = d * gaussian_data[k];

            Dxx += d.x * wd.x;
            Dxy += d.x * wd.y;
            Dyy += d.y * wd.y;

            ++k;
        }
        pathStrPrev = pathStr;
        pathStr = pathStrNext;
        pathStrNext = &pathStrNext[imageStride];
    }
    m_lastDet = Dxx * Dyy - Dxy * Dxy;
    if (fabs(m_lastDet) < m_detThreshold)
        return TrackingResult::Fail;

    wd.x = Dxx;
    Dxx = Dyy / m_lastDet;
    Dyy = wd.x / m_lastDet;
    Dxy = - Dxy / m_lastDet;

    Point2i position_i;
    Point2f sub_pix;

    // interpolation weights
    float wTL, wTR, wBL, wBR;

    float wdt, pixelValue;

    Point2f b, delta;
    Point2f prevDelta;

    for (int iteration = 0; iteration < m_numberIterations; ++iteration)
    {
        position_i.x = cast<int>(floor(position.x));
        position_i.y = cast<int>(floor(position.y));

        if ((position_i.x < m_begin_second.x) || (position_i.y < m_begin_second.y) ||
            (position_i.x >= m_end_second.x) || (position_i.y >= m_end_second.y))
            return TrackingResult::Fail;

        // compute interpolation weights
        sub_pix.x = position.x - position_i.x;
        sub_pix.y = position.y - position_i.y;

        wTL = (1.0f - sub_pix.x) * (1.0f - sub_pix.y);
        wTR = sub_pix.x * (1.0f - sub_pix.y);
        wBL = (1.0f - sub_pix.x) * sub_pix.y;
        wBR = sub_pix.x * sub_pix.y;

        pathStr = basePathRef;

        const uchar * imageStr = m_secondImage.pointer(position_i - m_cursorSize);
        const uchar * imageStrNext = &imageStr[m_secondImage.widthStep()];

        b.set(0.0f, 0.0f);

        k = 0;
        for (p.y = 0; p.y < m_pathSize.y; ++p.y)
        {
            for (p.x = 0; p.x < m_pathSize.x; ++p.x)
            {
                pixelValue = wTL * imageStr[p.x] + wTR * imageStr[p.x + 1] +
                             wBL * imageStrNext[p.x] + wBR * imageStrNext[p.x + 1];
                wdt = (pixelValue - pathStr[p.x]) * gaussian_data[k];

                Point2f & d = derivatives[k];

                b += d * wdt;

                ++k;
            }
            pathStr = &pathStr[imageStride];
            imageStr = imageStrNext;
            imageStrNext = &imageStrNext[m_secondImage.widthStep()];
        }

        delta.set(Dxx * b.x + Dxy * b.y, Dxy * b.x + Dyy * b.y);
        position -= delta;

        if (iteration > 0)
        {
            if ((fabs(delta.x + prevDelta.x) < m_pixelEps) && (fabs(delta.y + prevDelta.y) < m_pixelEps))
            {
                return TrackingResult::Completed;
            }
        }
        prevDelta = delta;
    }

    return TrackingResult::Uncompleted;
}

TrackingResult OpticalFlowCalculator::_horizontalTrackingOnSecondImage(Point2f & position,
                                                                       const uchar * imageRef, int imageStride)
{
    //TODO check

    Point2i position_i;
    Point2f sub_pix;

    position_i.y = cast<int>(floor(position.y));
    sub_pix.y = position.y - position_i.y;

    if ((position_i.y < m_begin_second.y) || (position_i.y >= m_end_second.y))
        return TrackingResult::Fail;

    const uchar * basePathRef = &imageRef[imageStride + 1];
    const uchar * pathStr = basePathRef;
    Point2i p;
    int k = 0;
    float wd;
    Matrix2f H;
    H.setZero();
    const float * gaussian_data = m_gaussian.data();
    float * derivatives = m_derivatives1d.data();
    for (p.y = 0; p.y < m_pathSize.y; ++p.y)
    {
        for (p.x = 0; p.x < m_pathSize.x; ++p.x)
        {
            float & d = derivatives[k];
            d = (pathStr[p.x + 1] - pathStr[p.x - 1]) * 0.5f;
            wd = d * gaussian_data[k];

            H(0, 0) += d * wd;
            H(0, 1) += wd;
            H(1, 1) += gaussian_data[k];
            ++k;
        }
        pathStr = &pathStr[imageStride];
    }

    H(0, 1) = - H(0, 1);
    H(1, 0) = H(0, 1);
    m_lastDet = H(0, 0) * H(1, 1) - H(1, 0) * H(0, 1);
    if (fabs(m_lastDet) < m_detThreshold)
        return TrackingResult::Fail;

    H /= m_lastDet;

    // compute interpolation weights
    float wTL, wTR, wBL, wBR;

    float wdt, pixelValue;
    float mean_diff = 0.0f;

    Vector2f Jres, delta;
    float prevDelta;

    for (int iteration = 0; iteration < m_numberIterations; ++iteration)
    {
        position_i.x = cast<int>(floor(position.x));

        if ((position_i.x < m_begin_second.x) || (position_i.x >= m_end_second.x))
            return TrackingResult::Fail;

        // compute interpolation weights
        sub_pix.x = position.x - position_i.x;

        wTL = (1.0f - sub_pix.x) * (1.0f - sub_pix.y);
        wTR = sub_pix.x * (1.0f - sub_pix.y);
        wBL = (1.0f - sub_pix.x) * sub_pix.y;
        wBR = sub_pix.x * sub_pix.y;

        pathStr = basePathRef;

        const uchar * imageStr = m_secondImage.pointer(position_i - m_cursorSize);
        const uchar * imageStrNext = &imageStr[m_secondImage.widthStep()];

        Jres.setZero();

        k = 0;
        for (p.y = 0; p.y < m_pathSize.y; ++p.y)
        {
            for (p.x = 0; p.x < m_pathSize.x; ++p.x)
            {
                pixelValue = wTL * imageStr[p.x] + wTR * imageStr[p.x + 1] +
                             wBL * imageStrNext[p.x] + wBR * imageStrNext[p.x + 1]
                        + mean_diff;
                wdt = (pixelValue - pathStr[p.x]) * gaussian_data[k];

                float & d = derivatives[k];

                Jres(0) += wdt * d;
                Jres(1) += wdt;

                ++k;
            }
            pathStr = &pathStr[imageStride];
            imageStr = imageStrNext;
            imageStrNext = &imageStrNext[m_secondImage.widthStep()];
        }

        delta = H * Jres;
        position.x += delta(0);
        mean_diff -= delta(1);

        if (iteration > 0)
        {
            if (fabs(delta(0) + prevDelta) < m_pixelEps)
            {
                return TrackingResult::Completed;
            }
        }
        prevDelta = delta(0);
    }
    return TrackingResult::Uncompleted;
}

TrackingResult OpticalFlowCalculator::_horizontalTrackingOnSecondImageLK(Point2f & position,
                                                                         const uchar * imageRef, int imageStride)
{
    Point2i position_i;
    Point2f sub_pix;

    position_i.y = cast<int>(floor(position.y));
    sub_pix.y = position.y - position_i.y;

    if ((position_i.y < m_begin_second.y) || (position_i.y >= m_end_second.y))
        return TrackingResult::Fail;

    const uchar * basePathRef = &imageRef[imageStride + 1];
    const uchar * pathStr = basePathRef;
    Point2i p;
    int k = 0;
    const float * gaussian_data = m_gaussian.data();
    float * derivatives = m_derivatives1d.data();
    float H = 0.0f;
    for (p.y = 0; p.y < m_pathSize.y; ++p.y)
    {
        for (p.x = 0; p.x < m_pathSize.x; ++p.x)
        {
            float & d = derivatives[k];
            d = (pathStr[p.x + 1] - pathStr[p.x - 1]) * 0.5f;

            H += d * gaussian_data[k];
            ++k;
        }
        pathStr = &pathStr[imageStride];
    }

    m_lastDet = H;
    if (fabs(m_lastDet) < m_detThreshold)
        return TrackingResult::Fail;

    H = 1.0f / m_lastDet;

    // compute interpolation weights
    float wTL, wTR, wBL, wBR;

    float wdt, pixelValue;
    float mean_diff = 0.0f;

    float Jres, delta;
    float prevDelta;

    for (int iteration = 0; iteration < m_numberIterations; ++iteration)
    {
        position_i.x = cast<int>(floor(position.x));

        if ((position_i.x < m_begin_second.x) || (position_i.x >= m_end_second.x))
            return TrackingResult::Fail;

        // compute interpolation weights
        sub_pix.x = position.x - position_i.x;

        wTL = (1.0f - sub_pix.x) * (1.0f - sub_pix.y);
        wTR = sub_pix.x * (1.0f - sub_pix.y);
        wBL = (1.0f - sub_pix.x) * sub_pix.y;
        wBR = sub_pix.x * sub_pix.y;

        pathStr = basePathRef;

        const uchar * imageStr = m_secondImage.pointer(position_i - m_cursorSize);
        const uchar * imageStrNext = &imageStr[m_secondImage.widthStep()];

        Jres = 0.0f;

        k = 0;
        for (p.y = 0; p.y < m_pathSize.y; ++p.y)
        {
            for (p.x = 0; p.x < m_pathSize.x; ++p.x)
            {
                pixelValue = wTL * imageStr[p.x] + wTR * imageStr[p.x + 1] +
                             wBL * imageStrNext[p.x] + wBR * imageStrNext[p.x + 1]
                        + mean_diff;
                wdt = (pixelValue - pathStr[p.x]) * gaussian_data[k];

                float & d = derivatives[k];

                Jres += wdt * d;

                ++k;
            }
            pathStr = &pathStr[imageStride];
            imageStr = imageStrNext;
            imageStrNext = &imageStrNext[m_secondImage.widthStep()];
        }

        delta = H * Jres;
        position.x += delta;

        if (iteration > 0)
        {
            if (fabs(delta + prevDelta) < m_pixelEps)
            {
                return TrackingResult::Completed;
            }
        }
        prevDelta = delta;
    }
    return TrackingResult::Uncompleted;
}

} // namespace sonar
