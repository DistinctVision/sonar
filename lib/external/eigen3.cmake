if (LINUX)
  find_package(Eigen3 REQUIRED)
  include_directories(${EIGEN3_INCLUDE_DIRS})
  add_definitions(-DEIGEN3_LIB)
else()	
  set(EIGEN3_DIR "" CACHE FILEPATH "Directory of eigen 3 library")
  if (${EIGEN3_DIR} STREQUAL "")
    message(FATAL_ERROR "Eigen3 directory is not set")
  elseif (NOT (EXISTS "${EXTERNAL_LIBRARIES_PATH}" AND IS_DIRECTORY "${EXTERNAL_LIBRARIES_PATH}"))
    message(FATAL_ERROR "Invalid Eigen3 directory")
  endif()
endif()
