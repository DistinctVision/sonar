cmake_minimum_required(VERSION 3.1.3)
project(paper_scanner VERSION 0.2 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)

if (MSVC)
  add_definitions(-D_USE_MATH_DEFINES)
endif()

set(CD ${CMAKE_CURRENT_SOURCE_DIR})

option(DEBUG_TOOLS_ENABLED "Debug tools enabled" OFF)
if (DEBUG_TOOLS_ENABLED)
    add_definitions(-DDEBUG_TOOLS_ENABLED)
endif()

include(${CD}/src/sonar/sonar.cmake)

add_library(sonar SHARED ${SONAR_SOURCES_FILES} ${SONAR_HEADER_FILES})

include(${CD}/external/eigen3.cmake)
include(${CD}/external/opencv.cmake)

set_target_properties(sonar PROPERTIES
                      SOVERSION ${PROJECT_VERSION}
                      VERSION ${PROJECT_VERSION}
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
                      DEBUG_POSTFIX d)

target_include_directories(sonar PUBLIC
                # only when building from the source tree
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
                # only when using the lib from the install path
                $<INSTALL_INTERFACE:include>
                ${ADDITIONAL_INCLUDE_DIRS} )

set(include_install_dir "include")

install(TARGETS sonar
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
        RUNTIME DESTINATION "bin"
        INCLUDES DESTINATION "${include_install_dir}")
install(DIRECTORY src/
        DESTINATION ${include_install_dir} 
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
