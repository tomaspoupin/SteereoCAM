add_executable(print_depth_info print_depth_map_info.cpp)

target_link_libraries(print_depth_info 
    ${OpenCV_LIBS}
    libecon_taraxl.so
    stereo_commons
    )