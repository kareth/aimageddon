###
# config
###
cmake_minimum_required(VERSION 2.8.1)
#cmake_policy(SET CMP0042 NEW)

###
# verbose make
###
# set(CMAKE_VERBOSE_MAKEFILE TRUE)


###
# project
###
set(PROJECT cpp_redis)
project(${PROJECT} CXX)


###
# compilation options
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra")


###
# variables
###
set(CPP_REDIS_INCLUDES ${PROJECT_SOURCE_DIR}/includes)


###
# includes
###
include_directories(${CPP_REDIS_INCLUDES})


###
# sources
###
set(DIRS "sources" "sources/network" "sources/builders" "sources/replies")
foreach(dir ${DIRS})
  # get directory sources
  file(GLOB s_${dir} "${dir}/*.cpp")
  # set sources
  set(SOURCES ${SOURCES} ${s_${dir}})
endforeach()


###
# executable
###
add_library(${PROJECT} SHARED ${SOURCES})
target_link_libraries(${PROJECT} boost_system)
#set_target_properties(${PROJECT}
#                      PROPERTIES
#                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
