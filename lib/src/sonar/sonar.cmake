INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/General/General.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ImageTools/ImageTools.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/CameraTools/CameraTools.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ThreadsTools/ThreadsTools.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/DebugTools/DebugTools.cmake)

set(SONAR_SOURCES_FILES
    ${SONAR_SOURCES_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/MarkerFinder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Sonar_c.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AbstractTrackingSystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MarkerTrackingSystem.cpp)

set(SONAR_HEADER_FILES
    ${SONAR_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/MarkerFinder.h
    ${CMAKE_CURRENT_LIST_DIR}/Sonar_c.h
    ${CMAKE_CURRENT_LIST_DIR}/global_types.h
    ${CMAKE_CURRENT_LIST_DIR}/AbstractTrackingSystem.h
    ${CMAKE_CURRENT_LIST_DIR}/MarkerTrackingSystem.h)
