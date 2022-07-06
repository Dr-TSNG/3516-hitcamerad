cmake_minimum_required(VERSION 3.22)
project(3516_send)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        hitcamerad
        main.cpp udp.cpp mycamera.cpp
        circular_buffer.c udp-piece.c
)

target_include_directories(
        hitcamerad PRIVATE include
        ohos/camera ohos/surface
)
