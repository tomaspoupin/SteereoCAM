cmake_minimum_required(VERSION 2.4)
project(E_CAM_BASIC_IMGPROC)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#find required Packages
find_package( OpenCV REQUIRED HINTS /usr/local/taraxl-opencv/ )

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")

set(TARA_HEADER_DIR "/usr/local/taraxl-sdk/include/")
set(TARA_LIB_DIR "/usr/local/taraxl-sdk/lib/")

include_directories (${OpenCV_INCLUDE_DIRS})
include_directories (${TARA_HEADER_DIR})

link_directories (${TARA_LIB_DIR})

add_executable(basic_stereo basic_stereo.cpp)

target_link_libraries(basic_stereo 
    ${OpenCV_LIBS}
    libecon_taraxl.so
    )