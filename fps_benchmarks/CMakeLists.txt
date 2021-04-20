add_executable(benchmark_fps fps.cpp)
add_executable(benchmark_depth_sdk depth_map_sdk.cpp)
add_executable(benchmark_disparity_opencv disparity_map_opencv.cpp)


target_link_libraries(benchmark_fps
        stereo_commons
        ${OpenCV_LIBS}
        libecon_taraxl.so
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})
target_link_libraries(benchmark_depth_sdk
        stereo_commons
        ${OpenCV_LIBS}
        libecon_taraxl.so
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})
target_link_libraries(benchmark_disparity_opencv
        stereo_commons
        ${OpenCV_LIBS}
        libecon_taraxl.so
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})