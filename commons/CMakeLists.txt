add_library(stereo_commons commons.cpp)

target_include_directories(stereo_commons PUBLIC include)

target_link_libraries(stereo_commons
${OpenCV_LIBS}
libecon_taraxl.so
${Boost_FILESYSTEM_LIBRARY}
${Boost_SYSTEM_LIBRARY}
${PCL_LIBRARIES})