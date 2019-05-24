cmake_minimum_required(VERSION 3.12)
project(visual)

set(CMAKE_CXX_STANDARD 14)

add_executable(visual main.cpp head.h function.cpp number.h)
find_package(opencv REQUIRED)
target_link_libraries(visual ${OpenCV_LIBS})