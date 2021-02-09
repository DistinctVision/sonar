/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_OPTICALFLOWCALCULATOR
#define SONAR_OPTICALFLOWCALCULATOR

#include "sonar/General/Image.h"

namespace sonar {

enum class TrackingResult
{
    Fail,
    Completed,
    Uncompleted
};

class OpticalFlowCalculator
{
public:
    OpticalFlowCalculator();
    ~OpticalFlowCalculator();

    Point2i cursorSize() const;
    int cursorWidth() const;
    int cursorHeight() const;
    void setCursorSize(const Point2i & cursorSize);

    Image<uchar> patch();
    ConstImage<uchar> patch() const;

    void setFirstImage(const ImageRef<uchar> & image);
    ConstImage<uchar> firstImage() const;

    void setSecondImage(const ImageRef<uchar> & image);
    ConstImage<uchar> secondImage() const;

    int numberIterations() const;
    void setNumberIterations(int numberIterations);

    float pixelEps() const;
    void setPixelEps(float pixelEps);

    float detThreshold() const;
    void setDetThreshold(float detThreshold);

    float lastDet() const;

    static void getSubPixelImageF(Image<float> & outImage, const ImageRef<uchar> & image,
                                 const Point2f & beginPoint);
    static Image<float> getSubPixelImageF(const ImageRef<uchar> & image, const Point2f & beginPoint,
                                         const Point2i & size);
    static void getSubPixelImage(Image<uchar> & outImage, const ImageRef<uchar> & image,
                                 const Point2f & beginPoint);
    static Image<uchar> getSubPixelImage(const ImageRef<uchar> & image, const Point2f & beginPoint,
                                         const Point2i & size);

    TrackingResult tracking2d(Point2f & second, const Point2f & first);
    TrackingResult tracking2d(Point2f & second, const Point2i & first);
    TrackingResult tracking2dLK(Point2f & second, const Point2f & first);
    TrackingResult tracking2dLK(Point2f & second, const Point2i & first);

    TrackingResult tracking2d_patch(Point2f & second);
    TrackingResult tracking2dLK_patch(Point2f & position);

    TrackingResult horizontalTracking(Point2f & second, const Point2i & first);
    TrackingResult horizontalTrackingLK(Point2f & second, const Point2i & first);

    TrackingResult horizontalTracking(Point2f & second, const Point2f & first);
    TrackingResult horizontalTrackingLK(Point2f & second, const Point2f & first);

private:
    ConstImage<uchar> m_firstImage;
    ConstImage<uchar> m_secondImage;

    Point2i m_cursorSize;
    Point2i m_pathSize;
    float m_invMaxErrorSquared;

    Point2i m_begin_second;
    Point2i m_end_second;

    Point2i m_begin_first;
    Point2i m_end_first;

    ConstImage<float> m_gaussian;
    float m_sigmaGaussian;

    Image<float> m_derivatives1d;
    Image<uchar> m_path;
    Image<Point2f> m_derivatives2d;

    int m_numberIterations;
    float m_pixelEps;
    float m_detThreshold;

    float m_lastDet;

    void _solveGaussian();
    void _setSigmaGaussian(float sigma);

    TrackingResult _tracking2dOnSecondImage(Point2f & position, const uchar * imageRef, int imageStride);
    TrackingResult _tracking2dOnSecondImageLK(Point2f & position, const uchar * imageRef, int imageStride);
    TrackingResult _horizontalTrackingOnSecondImage(Point2f & position,
                                                    const uchar * imageRef, int imageStride);
    TrackingResult _horizontalTrackingOnSecondImageLK(Point2f & position,
                                                      const uchar * imageRef, int imageStride);
};

} // namespace sonar

#endif // SONAR_OPTICALFLOWCALCULATOR
