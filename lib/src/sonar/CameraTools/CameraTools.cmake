add_definitions(-DMODULE_CAMERA_TOOLS)

set(SONAR_SOURCES_FILES
    ${SONAR_SOURCES_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/CameraIntrinsics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PinholeCameraIntrinsics.cpp)

set(SONAR_HEADER_FILES
    ${SONAR_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/CameraIntrinsics.h
    ${CMAKE_CURRENT_LIST_DIR}/PinholeCameraIntrinsics.h)
