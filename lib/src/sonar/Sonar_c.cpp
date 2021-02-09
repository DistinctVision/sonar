/**
* This file is part of sonar library
* Copyright (C) 2019 Vlasov Aleksey ijonsilent53@gmail.com
* For more information see <https://github.com/DistinctVision/sonar>
**/

#include "sonar/Sonar_c.h"

#include <vector>
#include <memory>
#include <cstring>

#include <fstream>

#include <Eigen/Eigen>

#include "sonar/General/cast.h"
#include "sonar/General/Point2.h"
#include "sonar/General/Image.h"

namespace sonar {

class SystemContext
{
public:
    static SystemContext & instance()
    {
        static SystemContext context;
        return context;
    }

    SystemContext()
    {
    }

private:
};

} // namespace sonar

extern "C" {

} // extern "C"
