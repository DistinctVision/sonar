add_definitions(-DMODULE_DEBUG_TOOLS)

set(SONAR_SOURCES_FILES
    ${SONAR_SOURCES_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/debug_tools.cpp)

set(SONAR_HEADER_FILES
    ${SONAR_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/debug_tools.h)
