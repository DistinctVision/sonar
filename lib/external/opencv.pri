DEFINES += OPENCV_LIB
DEFINES += DEBUG_TOOLS_ENABLED

windows {
    TEST_OPENCV_INCLUDE_PATH = $$(OPENCV_INCLUDE_PATH)
    isEmpty(TEST_OPENCV_INCLUDE_PATH) {
        error("Variable \"OPENCV_INCLUDE_PATH\" is not set")
    } else {
        TEST_OPENCV_LIB_PATH = $$(OPENCV_LIB_PATH)
        isEmpty(TEST_OPENCV_LIB_PATH) {
            error("Variable \"OPENCV_LIB_PATH\" is not set")
        } else {
            INCLUDEPATH += $$(OPENCV_INCLUDE_PATH)
            DEPENDPATH += $$(OPENCV_INCLUDE_PATH)

            CONFIG(debug, debug|release) {
                OPENCV_VERSION=451d
            } else {
                OPENCV_VERSION=451
            }
            #LIBS += -L$$(OPENCV_LIB_PATH) -lopencv_world$$OPENCV_VERSION

            LIBS += -L$$(OPENCV_LIB_PATH) \
                -lopencv_core$$OPENCV_VERSION \
                -lopencv_imgproc$$OPENCV_VERSION \
                -lopencv_video$$OPENCV_VERSION \
                -lopencv_videoio$$OPENCV_VERSION \
                -lopencv_highgui$$OPENCV_VERSION \
                -lopencv_imgcodecs$$OPENCV_VERSION \
                #-lopencv_calib3d$$OPENCV_VERSION \
                -lopencv_flann$$OPENCV_VERSION \
                -lopencv_aruco$$OPENCV_VERSION
        }
    }
} else: unix {
    INCLUDEPATH += /usr/local/include/opencv4
    DEPENDPATH += /usr/local/include/opencv4

    LIBS += -L/usr/local/lib -lopencv_world
} else {
    error("OpenCV not included")
}
