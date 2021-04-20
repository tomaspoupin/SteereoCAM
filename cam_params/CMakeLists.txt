add_executable(export_cam_params export_cam_params.cpp)

target_link_libraries(export_cam_params
    stereo_commons
    ${OpenCV_LIBS})