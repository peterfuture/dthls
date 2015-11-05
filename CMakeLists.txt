# CMake version
cmake_minimum_required (VERSION 2.8)

# project
project (DTHLS)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# Library dependence
FIND_PACKAGE(DTCURL  REQUIRED)
INCLUDE_DIRECTORIES(${DTCURL_INCLUDE_DIR})

#target-utils
add_definitions(-DENABLE_LINUX)
INCLUDE_DIRECTORIES(src/dtutils)
AUX_SOURCE_DIRECTORY(src/dtutils SRC_UTILS)
ADD_LIBRARY(dtutils STATIC ${SRC_UTILS})


# target - lib
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/dtutils)
AUX_SOURCE_DIRECTORY(src SRC_HLS)
ADD_LIBRARY(dthls STATIC ${SRC_HLS})
TARGET_LINK_LIBRARIES(dthls dtutils)
TARGET_LINK_LIBRARIES(dthls dtcurl)

# target - test
SET(SRC_TEST test/hls_test.c )
ADD_EXECUTABLE(test test/hls_test.c)
TARGET_LINK_LIBRARIES(test dthls dtutils)
