add_executable(print_depth_info print_depth_map_info.cpp)

add_executable(export_cam_params export_cam_params.cpp)

target_link_libraries(print_depth_info 
    ${OpenCV_LIBS}
    libecon_taraxl.so
    stereo_commons
    )

target_link_libraries(export_cam_params
    libecon_taraxl.so
    stereo_commons
    ${OpenCV_LIBS})