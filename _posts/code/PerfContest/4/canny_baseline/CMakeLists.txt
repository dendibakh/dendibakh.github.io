cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-O3 -ffast-math -march=core-avx2 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-O3 -ffast-math -march=core-avx2 ${CMAKE_C_FLAGS}")

add_executable(canny canny_source.c)
target_link_libraries(canny m)

add_executable(validate validate.cpp)
