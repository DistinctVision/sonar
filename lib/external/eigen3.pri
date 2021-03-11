DEFINES += EIGEN3_LIB

windows {
    TEST_EIGEN_DIR = $$(EIGEN_DIR)
    isEmpty(TEST_EIGEN_DIR) {
        error("Variable \"EIGEN_DIR\" is not set")
    } else {
        INCLUDEPATH += $$(EIGEN_DIR)
        DEPENDPATH += $$(EIGEN_DIR)
    }
} else: linux {
    INCLUDEPATH += /usr/include/eigen3
    DEPENDPATH += /usr/include/eigen3
} else {
    error("Eigen not included")
}
