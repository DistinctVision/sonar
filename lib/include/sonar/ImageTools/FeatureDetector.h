/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_FEATUREDETECTOR_H
#define SONAR_FEATUREDETECTOR_H

#include <vector>

#include "sonar/General/Image.h"
#include "sonar/General/ImagePyramid.h"
#include "sonar/ImageTools/FastCorner.h"

namespace sonar {

class FeatureDetector
{
public:
    struct FeatureCorner
    {
        Point2i pos;
        int level;

        FeatureCorner() {}
        FeatureCorner(const Point2i & pos, int level)
        {
            this->pos = pos;
            this->level = level;
        }
    };

    FeatureDetector();

    Point2i gridSize() const;
    void setGridSize(const Point2i & gridSize);

    int barrier() const;
    void setBarrier(int barrier);

    float detectionThreshold() const;
    void setDetectionThreshold(float threshold);

    int minLevelForFeature() const;
    int maxLevelForFeature() const;
    void setLevelForFeature(int minLevel, int maxLevel);

    Point2i borderSize() const;
    void setBorderSize(const Point2i & borderSize);

    std::vector<FeatureCorner> detectCorners(const ImagePyramid_u & imagePyramid) const;
    std::vector<FeatureCorner> detectCorners(const ImagePyramid_u & imagePyramid,
                                             const std::vector<Point2i> & existCorners) const;

    int indexOfCell(const Point2i & point, const Point2f & cellSize) const;
    int indexOfCell(const Point2f & point, const Point2f & cellSize) const;

    int maxCountFeatures() const;
    void setMaxCountFeatures(int maxCountFeatures);

protected:
    struct Cell
    {
        Point2i pos;
        float score;
        int level;
    };

    Point2i m_borderSize;
    int m_barrier;
    float m_detectionThreshold;
    int m_minLevelForFeature;
    int m_maxLevelForFeature;
    int m_maxCountFeatures;

    Point2i m_gridSize;
    mutable std::vector<Cell> m_cells;
};

} // namespace sonar

#endif // SONAR_FEATUREDETECTOR_H
