# CMake version
cmake_minimum_required (VERSION 2.8)

# project
project (DTHLS)
MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir "${PROJECT_SOURCE_DIR})

#target-utils
add_definitions(-DENABLE_LINUX)
INCLUDE_DIRECTORIES(src/dtutils)
AUX_SOURCE_DIRECTORY(src/dtutils SRC_UTILS)
ADD_LIBRARY(dtutils STATIC ${SRC_UTILS})


# target - lib
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/dtutils)
AUX_SOURCE_DIRECTORY(src SRC_HLS)
ADD_LIBRARY(dthls STATIC ${SRC_HLS})
target_link_libraries(dthls  dtutils)

# target - test
SET(SRC_TEST test/hls_test.c )
ADD_EXECUTABLE(test test/hls_test.c)
TARGET_LINK_LIBRARIES(test dthls dtutils)
