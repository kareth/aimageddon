cmake_minimum_required (VERSION 2.8.12.2)

project (SnakeServer)

option(SET_WARNINGS "Compiler warning flags" OFF)
option(SET_DEBUG "Compiler debugging flags" OFF)
option(SET_CPPLINT "Style Check" ON)

# Allow absolute paths
SET(BASEPATH "${CMAKE_SOURCE_DIR}")

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
SET(gtest_SOURCE_DIR "${BASEPATH}/gtest")

# FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(Boost 1.54.0 COMPONENTS system filesystem thread REQUIRED)

enable_testing()

INCLUDE_DIRECTORIES("${BASEPATH}")
INCLUDE_DIRECTORIES("${BASEPATH}/jsoncons/src")
INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

include(EnableCxx11)
include(BuildType)
include(StyleCheck)

add_subdirectory(gtest)
add_subdirectory(gflags)

add_executable(server main.cpp)
target_link_libraries(server
  ${Boost_LIBRARIES}
  gflags
)

add_style_check_target(server main.cpp SnakeServer)
