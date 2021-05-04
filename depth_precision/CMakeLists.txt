add_executable(depth_measurements depth_measurement.cpp)

target_link_libraries(depth_measurements 
    ${OpenCV_LIBS}
    libecon_taraxl.so
    stereo_commons
    )