#include <iostream>
#include <vector>
#include <commons.hpp>
#include <CSVWriter.h>

cv::Mat generateInfMask(cv::Mat src);

const ACCURACY_MODE accuracy_mode[3] = {ACCURACY_MODE::LOW,
                                        ACCURACY_MODE::HIGH,
                                        ACCURACY_MODE::ULTRA};

double time_ms = 60000;

int main(int argc, char** argv) {
    TaraXL taraxl_cam;
    TaraXLCam selected_cam;
    TaraXLCamList taraxl_cam_list;
    TARAXL_STATUS_CODE status;

    const std::string ac_string[3] = {"LOW", "HIGH", "ULTRA"};

    bool init = init_camera(taraxl_cam,
                            selected_cam,
                            taraxl_cam_list,
                            status);
    if (init)
        std::cout << "Cámara iniciada correctamente." << std::endl;
    else {
        std::cout << "Error inicializando cámara." << std::endl;
        return 1;
    }

    CSVWriter measurements;
    measurements.newRow() << "Distancia";

    auto taraxl_depth = std::unique_ptr<TaraXLDepth>(new TaraXLDepth(selected_cam));
    int minDisparity, maxDisparity;
    cv::Mat left, right, disparity_map, depth_map;

    cv::Rect ROI(700, 550, 200, 200);
    
    taraxl_depth->setAccuracy(accuracy_mode[1]);

    for (int i = 0; i < 10; ++i)
        taraxl_depth->getMap(left, right, disparity_map, true, depth_map, true);

    auto init = std::chrono::high_resolution_clock::now();
    while(true){
        auto checkpoint = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_checkpoint = checkpoint - init;
        if (ms_checkpoint.count() >= time_ms)
            break;

        taraxl_depth->getMap(left, right, disparity_map, true, depth_map, true);
        cv::Mat measurement_area(depth_map, ROI);
        cv::Mat mask = generateInfMask(measurement_area);
        cv::Scalar mean = cv::mean(area, mask);
        measurements.newRow() << mean.val[0];
    }
    
    measurements.writeToFile("distancias.csv");

    selected_cam.disconnect();
}

cv::Mat generateInfMask(cv::Mat src) {
    cv::Mat dst(src.size(), CV_8U);

    for (int i = 0; i < src.cols; ++i) {
        for (int j = 0; j < src.rows; ++j) {
            if (!cvIsInf(src.at<float>(j, i))) {
                dst.at<unsigned char>(j, i) = 255;
            }
            else {
                dst.at<unsigned char>(j, i) = 0;
            }
        }
    }
    return dst;
}