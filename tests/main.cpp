#include "sonar/General/macros.h"

#include "test_marker_transform.h"
#include "test_marker_pose_tracking.h"

int main(int argc, char ** argv)
{
    SONAR_UNUSED(argc);
    SONAR_UNUSED(argv);

    //test_marker_transform(true);
    //test_marker_transform(false);
    test_marker_pose_tracking();

    return 0;
}
