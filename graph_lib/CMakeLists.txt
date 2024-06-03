#参考
#https://blog.csdn.net/ox0080/article/details/96453985
#https://www.cnblogs.com/binbinjx/p/5626916.html
#https://www.it1352.com/1984337.html

cmake_minimum_required (VERSION 2.6)
project(graph_lib)
aux_source_directory(. DIR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -fPIC")

#find_package(Boost REQUIRED COMPONENTS
#    graph
#)
#include_directories(${Boost_INCLUDE_DIRS})

add_library(mygraph STATIC ${DIR})

#target_link_libraries(mygraph ${Boost_LIBRARIES})
