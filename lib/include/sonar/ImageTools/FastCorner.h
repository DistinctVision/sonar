/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#ifndef SONAR_FASTCORNER_H
#define SONAR_FASTCORNER_H

#include <vector>

#include "sonar/General/Point2.h"
#include "sonar/General/Image.h"

namespace sonar {

// Быстрый поиск углов на изображении
class FastCorner
{
public:
    struct Corner
    {
        Point2i pos;
        int score;
        int level;
    };

    /// Perform tree based 10 point FAST feature detection
    static void fast_corner_detect_10(const ImageRef<unsigned char> & im,
                                      const Point2i & begin, const Point2i & end,
                                      std::vector<Corner> & corners, int barrier);

    // Небольшая фильрация на результата
    static void fastNonmaxSuppression(const std::vector<Corner> & corners, std::vector<Corner> & ret_nonmax);

    static bool comp(const FastCorner::Corner & a, const FastCorner::Corner & b)
    {
        return a.score > b.score;
    }

    static int fast_corner_score_10(const unsigned char * p, int barrier);

    // Более умный отклик на углы
    static float shiTomasiScore_10(const ImageRef<unsigned char> & im, const Point2i & pos);

protected:
    static int _fast_pixel_ring[16];

    static void _make_fast_pixel_offset(int row_stride);
};

} // namespace sonar

#endif // SONAR_FASTCORNER_H
