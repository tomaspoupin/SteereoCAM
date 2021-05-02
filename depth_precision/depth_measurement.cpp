#include <iostream>
#include <vector>
#include <commons.hpp>

cv::Mat generateInfMask(cv::Mat src);

const ACCURACY_MODE accuracy_mode[3] = {ACCURACY_MODE::LOW,
                                        ACCURACY_MODE::HIGH,
                                        ACCURACY_MODE::ULTRA};

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

    auto taraxl_depth = std::unique_ptr<TaraXLDepth>(new TaraXLDepth(selected_cam));
    int minDisparity, maxDisparity;
    cv::Mat left, right, disparityMap, depthMap;

    cv::Rect ROI(700, 550, 200, 200);
    
    taraxl_depth->setAccuracy(accuracy_mode[1]);

    for (int i = 0; i < 10; ++i)
        taraxl_depth->getMap(left, right, disparityMap, true, depthMap, true);

    cv::Mat area(depthMap, ROI);

    cv::Mat mask = generateInfMask(area);

    cv::Scalar mean = cv::mean(area, mask);

    std::cout << "Valor promedio area: " << mean.val[0] << std::endl;
    
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