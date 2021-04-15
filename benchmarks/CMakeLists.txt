cmake_minimum_required(VERSION 2.4)
project(E_CAM_VIDEO_CAPTURE)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#find required Packages
find_package( OpenCV REQUIRED HINTS /usr/local/taraxl-opencv/ )
find_package( Threads REQUIRED )
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(PCL 1.8 REQUIRED HINTS "/usr/local/taraxl-pcl")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")

set(TARA_HEADER_DIR "/usr/local/taraxl-sdk/include/")
set(TARA_LIB_DIR "/usr/local/taraxl-sdk/lib/")

include_directories(include)
include_directories (${OpenCV_INCLUDE_DIRS})
include_directories (${TARA_HEADER_DIR})
include_directories (${PCL_INCLUDE_DIRS})
link_directories (${PCL_LIBRARY_DIRS})

link_directories (${TARA_LIB_DIR})

add_definitions(${PCL_DEFINITIONS})
set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "" )

add_executable(benchmark_fps fps.cpp commons.cpp)
add_executable(benchmark_depth_sdk depth_map_sdk.cpp commons.cpp)
add_executable(benchmark_disparity_opencv disparity_map_opencv.cpp commons.cpp)

set_target_properties(
        benchmark_fps benchmark_depth_sdk benchmark_disparity_opencv
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        )

target_link_libraries(benchmark_fps
        ${OpenCV_LIBS}
        libecon_taraxl.so
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})
target_link_libraries(benchmark_depth_sdk
        ${OpenCV_LIBS}
        libecon_taraxl.so
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})
target_link_libraries(benchmark_disparity_opencv
        ${OpenCV_LIBS}
        libecon_taraxl.so
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})