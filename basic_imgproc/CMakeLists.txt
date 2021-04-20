add_executable(basic_stereo basic_stereo.cpp)

target_link_libraries(basic_stereo 
    ${OpenCV_LIBS}
    libecon_taraxl.so
    )