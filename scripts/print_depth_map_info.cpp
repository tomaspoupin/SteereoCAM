#include <iostream>
#include <commons.hpp>
#include <vector>

using namespace TaraXLSDK;

const ACCURACY_MODE accuracy_mode[3] = {ACCURACY_MODE::LOW,
                                        ACCURACY_MODE::HIGH,
                                        ACCURACY_MODE::ULTRA};

int main(int argc, char **argv)
{
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

    for (int i = 0; i < 3; ++i) {
        taraxl_depth->setAccuracy(accuracy_mode[i]);

        taraxl_depth->getMap(left, right, disparityMap, true, depthMap, true);
        taraxl_depth->getMinDisparity(minDisparity);
        taraxl_depth->getMaxDisparity(maxDisparity);

        if (i == 0) {
            std::cout << "Tamaño imagen: " << left.cols << "x" << left.rows << std::endl;
            std::cout << "Canales imagen: "  << left.channels() << std::endl;
        }
        std::cout << std::endl << "Modo de presición: " << ac_string[i] << std::endl;
        std::cout << "Disparidad mínima: " << minDisparity << std::endl;
        std::cout << "Disparidad máxima: " << maxDisparity << std::endl;
        std::cout << "Canales mapa disparidad: " << disparityMap.channels() << std::endl;
        std::cout << "Tipo mapa de disparidad: " << type2str(disparityMap.type()) << std::endl;
        std::cout << "Canales mapa profundidad: " << depthMap.channels() << std::endl;
        std::cout << "Tipo mapa de profundidad-: " << type2str(depthMap.type()) << std::endl;
    }
    selected_cam.disconnect();
}