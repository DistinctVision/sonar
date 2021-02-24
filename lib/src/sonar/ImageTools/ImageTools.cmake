add_definitions(-DMODULE_IMAGE_TOOLS)

set(SONAR_SOURCES_FILES
    ${SONAR_SOURCES_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/FastCorner.cpp
    ${CMAKE_CURRENT_LIST_DIR}/faster_corner_10.cxx
    ${CMAKE_CURRENT_LIST_DIR}/FeatureDetector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/OpticalFlow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/OpticalFlowCalculator.cpp)

set(SONAR_HEADER_FILES
    ${SONAR_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/FastCorner.h
    ${CMAKE_CURRENT_LIST_DIR}/FeatureDetector.h
    ${CMAKE_CURRENT_LIST_DIR}/OpticalFlow.h
    ${CMAKE_CURRENT_LIST_DIR}/OpticalFlowCalculator.h)
