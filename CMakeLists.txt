cmake_minimum_required(VERSION 2.4)
project(STEEREO_CAM)

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
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories (${OpenCV_INCLUDE_DIRS})
include_directories (${TARA_HEADER_DIR})
include_directories (${PCL_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories (${PCL_LIBRARY_DIRS})
link_directories (${TARA_LIB_DIR})
add_definitions(${PCL_DEFINITIONS})
set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "" )

add_subdirectory(commons)

option(BASIC_IMGPROC "Procesamiento básico con cámara estéreo" OFF)
option(VIDEO_CAPTURE "Módulo para grabar video" OFF)
option(FPS_BENCHMARKS "Benchmarks de FPS" OFF)
option(STEREO_PARAMS "Exportar parametros de camara" OFF)
option(SCRIPTS "Compilar Scripts del repositorio" ON)

if(BASIC_IMGPROC)
    add_subdirectory(basic_imgproc)
endif(BASIC_IMGPROC)

if(VIDEO_CAPTURE)
    add_subdirectory(video_capture)
endif(VIDEO_CAPTURE)

if(FPS_BENCHMARKS)
    add_subdirectory(fps_benchmarks)
endif(FPS_BENCHMARKS)

if(STEREO_PARAMS)
    add_subdirectory(cam_params)
endif(STEREO_PARAMS)

if(SCRIPTS)
    add_subdirectory(scripts)
endif(SCRIPTS)