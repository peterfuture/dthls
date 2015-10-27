# CMake version
cmake_minimum_required (VERSION 2.8)

# project
project (DTHLS)
MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir "${PROJECT_SOURCE_DIR})

# target - lib
INCLUDE_DIRECTORIES(src)
AUX_SOURCE_DIRECTORY(src SRC_HLS)
ADD_LIBRARY(dthls_shared SHARED ${SRC_HLS})
ADD_LIBRARY(dthls_static STATIC ${SRC_HLS})

# target - test
SET(SRC_TEST test/hls_test.c )
ADD_EXECUTABLE(test test/hls_test.c)
TARGET_LINK_LIBRARIES(test dthls_static)
