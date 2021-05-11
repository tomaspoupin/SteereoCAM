#include <iostream>
#include <vector>
#include <commons.hpp>
#include <CSVWriter.h>
#include <sstream>

cv::Mat generateInfMask(cv::Mat src);

const ACCURACY_MODE accuracy_mode[3] = {ACCURACY_MODE::LOW,
                                        ACCURACY_MODE::HIGH,
                                        ACCURACY_MODE::ULTRA};

double time_ms = 0;
double print_interval = 1000; //ms

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cerr << "Uso: benchmark_depth_sdk <tiempo en segundos>"
                     " <depth accuracy>" << std::endl;
        return 1;
    }

    int accuracy;
    std::stringstream buffer;
    buffer << argv[1];
    buffer >> time_ms;
    time_ms *= 1000.0;

    buffer.str("");
    buffer.clear();
    buffer << argv[2];
    buffer >> accuracy;

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

    cv::Rect ROI(700, 1, 200, 800);
    
    taraxl_depth->setAccuracy(accuracy_mode[accuracy]);

    for (int i = 0; i < 10; ++i)
        taraxl_depth->getMap(left, right, disparity_map, true, depth_map, true);

    cv::Mat leftROI(left, ROI);

    cv::imwrite("left.jpg", left);
    cv::imwrite("right.jpg", left);
    cv::imwrite("ROI.jpg", leftROI);
    cv::imwrite("depth_map.jpg", depth_map);

    std::cout << "Modo de medicion: " << ac_string[accuracy] << std::endl;
    std::cout << "Comenzando medicion, tiempo: " << (int) time_ms / 1000 << " segundos." << std::endl;

    auto init_time = std::chrono::high_resolution_clock::now();
    auto print_time = std::chrono::high_resolution_clock::now();
    while(true){
        auto checkpoint = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_checkpoint = checkpoint - init_time;
        if (ms_checkpoint.count() >= time_ms)
            break;

        taraxl_depth->getMap(left, right, disparity_map, true, depth_map, true);
        cv::Mat measurement_area(depth_map, ROI);
        cv::Mat mask = generateInfMask(measurement_area);
        cv::Scalar mean = cv::mean(measurement_area, mask);


        std::chrono::duration<double, std::milli> print_checkpoint = checkpoint - print_time;
        if (print_checkpoint.count() >= print_interval) {
            auto print_time = std::chrono::high_resolution_clock::now();
            std::cout << "Distancia: " << mean.val[0] << std::endl;
        }
        measurements.newRow() << mean.val[0];
    }

    std::cout << "Medición finalizada, exportando archivo." << std::endl;
    
    measurements.writeToFile("distancias.csv");

    std::cout << "Archivo exportado." << std::endl;
    selected_cam.disconnect();
}

cv::Mat generateInfMask(cv::Mat src) {
    cv::Mat dst(src.size(), CV_8U);

    for (int i = 0; i < src.cols; ++i) {
        for (int j = 0; j < src.rows; ++j) {
            if (!cvIsInf(src.at<float>(j, i)) && src.at<float>(j, i) < 200) {
                dst.at<unsigned char>(j, i) = 255;
            }
            else {
                dst.at<unsigned char>(j, i) = 0;
            }
        }
    }
    return dst;
}