cmake_minimum_required(VERSION 3.3)
project(leader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

aux_source_directory(. SRCS)

add_subdirectory(src)

find_package(Threads)
add_executable(leader main.cpp)
target_link_libraries(leader LeaderLib ${CMAKE_THREAD_LIBS_INIT})