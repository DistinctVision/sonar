add_definitions(-DMODULE_GENERAL)

set(SONAR_SOURCES_FILES
    ${SONAR_SOURCES_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/Logger.cpp)

set(SONAR_HEADER_FILES
    ${SONAR_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/macros.h
    ${CMAKE_CURRENT_LIST_DIR}/Logger.h
    ${CMAKE_CURRENT_LIST_DIR}/Image.h
    ${CMAKE_CURRENT_LIST_DIR}/ImagePyramid.h
    ${CMAKE_CURRENT_LIST_DIR}/ImageUtils.h
    ${CMAKE_CURRENT_LIST_DIR}/Point2.h
    ${CMAKE_CURRENT_LIST_DIR}/WLS.h
    ${CMAKE_CURRENT_LIST_DIR}/cast.h
    ${CMAKE_CURRENT_LIST_DIR}/Paint.h
    ${CMAKE_CURRENT_LIST_DIR}/MathUtils.h)
