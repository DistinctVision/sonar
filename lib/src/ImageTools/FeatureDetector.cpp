#include "sonar/ImageTools/FeatureDetector.h"

#include <cassert>

#include "sonar/General/cast.h"

using namespace std;

namespace sonar {

FeatureDetector::FeatureDetector()
{
    m_barrier = 4;
    m_detectionThreshold = 80.0f;
    m_minLevelForFeature = 1;
    m_maxLevelForFeature = -1;
    setGridSize(Point2i(25, 25));
    m_maxCountFeatures = 1000;
    m_borderSize.set(12, 12);
}

Point2i FeatureDetector::gridSize() const
{
    return m_gridSize;
}

void FeatureDetector::setGridSize(const Point2i & gridSize)
{
    assert((gridSize.x > 0) && (gridSize.y > 0));

    m_gridSize = gridSize;
    Cell cell;
    cell.level = 0;
    cell.pos.setZero();
    cell.score = 0.0f;
    m_cells.resize(cast<size_t>(m_gridSize.y * m_gridSize.x), cell);
}

int FeatureDetector::indexOfCell(const Point2i & point, const Point2f & cellSize) const
{
    return cast<int>(floor(point.y / cellSize.y) * m_gridSize.y + floor(point.x / cellSize.x));
}

int FeatureDetector::indexOfCell(const Point2f & point, const Point2f & cellSize) const
{
    return cast<int>(floor(point.y / cellSize.y) * m_gridSize.y + floor(point.x / cellSize.x));
}

int FeatureDetector::maxCountFeatures() const
{
    return m_maxCountFeatures;
}

void FeatureDetector::setMaxCountFeatures(int maxCountFeatures)
{
    m_maxCountFeatures = maxCountFeatures;
}

Point2i FeatureDetector::borderSize() const
{
    return m_borderSize;
}

void FeatureDetector::setBorderSize(const Point2i & borderSize)
{
    assert((borderSize.x >= 0) && (borderSize.y >= 0));
    m_borderSize = borderSize;
}

int FeatureDetector::barrier() const
{
    return m_barrier;
}

void FeatureDetector::setBarrier(int barrier)
{
    m_barrier = barrier;
}

float FeatureDetector::detectionThreshold() const
{
    return m_detectionThreshold;
}

void FeatureDetector::setDetectionThreshold(float threshold)
{
    assert(threshold > 0.0f);

    m_detectionThreshold = threshold;
}

int FeatureDetector::minLevelForFeature() const
{
    return m_minLevelForFeature;
}

int FeatureDetector::maxLevelForFeature() const
{
    return m_maxLevelForFeature;
}

void FeatureDetector::setLevelForFeature(int minLevel, int maxLevel)
{
    m_minLevelForFeature = minLevel;
    m_maxLevelForFeature = maxLevel;
}

vector<FeatureDetector::FeatureCorner>
FeatureDetector::detectCorners(const ImagePyramid_u & imagePyramid) const
{
    assert((m_gridSize.x > 0) && (m_gridSize.y > 0));

    Point2i imageSize = imagePyramid.get(0).size();

    assert((imageSize.x != 0) && (imageSize.y != 0));

    Point2f cellSize(imageSize.x / cast<float>(m_gridSize.x),
                     imageSize.y / cast<float>(m_gridSize.y));

    vector<FastCorner::Corner> candidates;
    vector<FeatureDetector::FeatureCorner> corners;
    int prevSize = 0;

    int minLevel = (m_minLevelForFeature < 0) ? 0 : m_minLevelForFeature;
    int maxLevel = ((m_maxLevelForFeature < 0) ||
                    (m_maxLevelForFeature >= imagePyramid.numberLevels())) ?
                        (imagePyramid.numberLevels() - 1) :  m_maxLevelForFeature;

    for (int level = minLevel; level <= maxLevel; ++level)
    {
        ConstImage<uchar> image = imagePyramid.get(level);

        FastCorner::fast_corner_detect_10(image,
                                          m_borderSize,
                                          image.size() - (m_borderSize + Point2i(1, 1)),
                                          candidates,
                                          m_barrier);
        //FastCorner::fastNonmaxSuppression(m_candidates, m_finalCandidates);
        if (level > 0)
        {
            int scale = (1 << level);
            for (vector<FastCorner::Corner>::iterator it = candidates.begin() + prevSize;
                 it != candidates.end();
                 ++it)
            {
                it->pos *= scale;
                if ((it->pos.x < 0) || (it->pos.y < 0) ||
                        (it->pos.x >= imageSize.x) || (it->pos.y >= imageSize.y)) {
                    assert(false);
                }
                it->level = level;
            }
            break;
        }
        else
        {
            for (vector<FastCorner::Corner>::iterator it = candidates.begin() + prevSize;
                 it != candidates.end();
                 ++it)
            {
                it->level = 0;
            }
        }
        prevSize = cast<int>(candidates.size());
    }

    for (vector<Cell>::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
    {
        it->score = 0.0f;
    }

    ConstImage<uchar> firstImage = imagePyramid.get(0);
    size_t k;
    float score;
    for (vector<FastCorner::Corner>::iterator it = candidates.begin();
            it != candidates.end();
            ++it)
    {
        k = cast<size_t>(floor(it->pos.y / cellSize.y) * m_gridSize.x + floor(it->pos.x / cellSize.x));
        score = FastCorner::shiTomasiScore_10(firstImage, it->pos);
        if (score > m_cells[k].score)
        {
            m_cells[k].score = score;
            m_cells[k].pos = it->pos;
            m_cells[k].level = it->level;
        }
    }

    vector<int> cellOrders;
    cellOrders.resize(m_cells.size());

    for (size_t i = 0; i < cellOrders.size(); ++i)
        cellOrders[i] = cast<int>(i);
    random_shuffle(cellOrders.begin(), cellOrders.end());

    for (vector<int>::iterator it = cellOrders.begin(); it != cellOrders.end(); ++it)
    {
        const Cell & cell = m_cells[cast<size_t>(*it)];
        if (cell.score > m_detectionThreshold)
        {
            corners.push_back(FeatureCorner(cell.pos, cell.level));
            if (cast<int>(corners.size()) > m_maxCountFeatures)
                return corners;
        }
    }
    return corners;
}

vector<FeatureDetector::FeatureCorner>
FeatureDetector::detectCorners(const ImagePyramid_u & imagePyramid, const vector<Point2i> & existCorners) const
{
    assert((m_gridSize.x > 0) && (m_gridSize.y > 0));

    Point2i imageSize = imagePyramid.get(0).size();

    assert((imageSize.x != 0) && (imageSize.y != 0));

    Point2f cellSize(imageSize.x / cast<float>(m_gridSize.x),
                     imageSize.y / cast<float>(m_gridSize.y));

    vector<FastCorner::Corner> candidates;
    vector<FeatureDetector::FeatureCorner> corners;
    int prevSize = 0;

    int minLevel = (m_minLevelForFeature < 0) ? 0 : m_minLevelForFeature;
    int maxLevel = ((m_maxLevelForFeature < 0) ||
                    (m_maxLevelForFeature >= imagePyramid.numberLevels())) ?
                        (imagePyramid.numberLevels() - 1) :  m_maxLevelForFeature;

    for (int level = minLevel; level <= maxLevel; ++level)
    {
        ConstImage<uchar> image = imagePyramid.get(level);

        FastCorner::fast_corner_detect_10(image,
                                          m_borderSize,
                                          image.size() - (m_borderSize + Point2i(1, 1)),
                                          candidates,
                                          m_barrier);
        //FastCorner::fastNonmaxSuppression(m_candidates, m_finalCandidates);
        if (level > 0)
        {
            int scale = (1 << level);
            for (vector<FastCorner::Corner>::iterator it = candidates.begin() + prevSize;
                 it != candidates.end();
                 ++it)
            {
                it->pos *= scale;
                if ((it->pos.x < 0) || (it->pos.y < 0) ||
                        (it->pos.x >= imageSize.x) || (it->pos.y >= imageSize.y))
                {
                    assert(false);
                }
                it->level = level;
            }
            break;
        }
        else
        {
            for (vector<FastCorner::Corner>::iterator it = candidates.begin() + prevSize;
                 it != candidates.end();
                 ++it)
            {
                it->level = 0;
            }
        }
        prevSize = cast<int>(candidates.size());
    }

    vector<char> blockCells(m_cells.size(), 0);

    size_t k;
    for (auto it = existCorners.cbegin(); it != existCorners.cend(); ++it)
    {
        k = cast<size_t>(floor(it->y / cellSize.y) * m_gridSize.x + floor(it->x / cellSize.x));
        blockCells[k] = 1;
    }

    for (vector<Cell>::iterator it = m_cells.begin(); it != m_cells.end(); ++it)
    {
        it->score = m_detectionThreshold;
    }

    ConstImage<uchar> firstImage = imagePyramid.get(0);
    float score;
    for (vector<FastCorner::Corner>::iterator it = candidates.begin();
            it != candidates.end();
            ++it)
    {
        k = cast<size_t>(floor(it->pos.y / cellSize.y) * m_gridSize.x + floor(it->pos.x / cellSize.x));
        if (blockCells[k] > 0)
            continue;
        score = FastCorner::shiTomasiScore_10(firstImage, it->pos);
        if (score > m_cells[k].score)
        {
            m_cells[k].score = score;
            m_cells[k].pos = it->pos;
            m_cells[k].level = it->level;
        }
    }

    vector<int> cellOrders;
    cellOrders.resize(m_cells.size());

    for (size_t i = 0; i < cellOrders.size(); ++i)
        cellOrders[i] = cast<int>(i);
    random_shuffle(cellOrders.begin(), cellOrders.end());

    for (vector<int>::iterator it = cellOrders.begin(); it != cellOrders.end(); ++it)
    {
        const Cell & cell = m_cells[cast<size_t>(*it)];
        if (cell.score > m_detectionThreshold)
        {
            corners.push_back(FeatureCorner(cell.pos, cell.level));
            if (cast<int>(corners.size()) > m_maxCountFeatures)
                return corners;
        }
    }
    return corners;
}

} // namespace sonar

