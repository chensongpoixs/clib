project(clib)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")

SET(CMAKE_BUILD_TYPE Release)
ADD_DEFINITIONS(-std=c++11 -g -Wall )



INCLUDE_DIRECTORIES(
${PROJECT_SOURCE_DIR}/cnet
${PROJECT_SOURCE_DIR}/ctime
${PROJECT_SOURCE_DIR}/clog
${PROJECT_SOURCE_DIR}/cutil
)

file(GLOB_RECURSE ALL_CXX_SRCS
./cnet/*.cpp
./ctime/*.cpp 
./clog/*.cpp 
./cutil/*.cpp 
)

add_library(${PROJECT_NAME} STATIC ${ALL_CXX_SRCS} )
target_link_libraries(${PROJECT_NAME})

