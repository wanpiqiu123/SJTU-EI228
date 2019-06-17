cmake_minimum_required(VERSION 3.12)
project(visual)

set(CMAKE_CXX_STANDARD 14)

add_executable(visual main.cpp head.h function.cpp number.h bluetooth.cpp)
find_package(OpenCV REQUIRED)
target_link_libraries(visual ${OpenCV_LIBS})