include (eigen3.pri)
#include (opengv.pri)

TEST_DEBUG_TOOLS_ENABLED = $$(DEBUG_TOOLS_ENABLED)
!isEmpty(TEST_DEBUG_TOOLS_ENABLED) {
    DEFINES += DEBUG_TOOLS_ENABLED
    include (opencv.pri)
}
