option(OPENCV_ENABLE "Include opencv in project" ON)
if (OPENCV_ENABLE)
  find_package(OpenCV REQUIRED)
  include_directories(${OpenCV_INCLUDE_DIRS})
  target_link_libraries(sonar ${OpenCV_LIBS})
endif()