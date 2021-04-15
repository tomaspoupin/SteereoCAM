#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "TaraXL.h"
#include "TaraXLCam.h"
#include "TaraXLDepth.h"

using namespace TaraXLSDK;

int main(int argc, char **argv)
{
    if (argc != 1)
    {
        std::cerr << "El progama no requiere argumentos." << std::endl;
        return 1;
    }

    TARAXL_STATUS_CODE status;
    TaraXL taraxlCam;
    TaraXLCam selectedCam;
    TaraXLCamList taraxlCamList;

    status = taraxlCam.enumerateDevices(taraxlCamList);
    if (status != TARAXL_SUCCESS)
    {
        std::cout << "No se pudo obtener lista de dispositivos." << std::endl;
        return 1;
    }

    if (taraxlCamList.size() == 0)
    {
        std::cout << "Camara no detectada." << std::endl;
        return 1;
    }
    if (taraxlCamList.size() > 1)
    {
        std::cout << "Se detecto más de 1 camara conectada." << std::endl;
        return 1;
    }

    selectedCam = taraxlCamList.at(0);
    status = selectedCam.connect();
    if (status != TARAXL_SUCCESS)
    {
        std::cout << "No se pudo conectar a la camara." << std::endl;
        return 1;
    }

    cv::Mat left, right;
    int brightness, gain, exposure;

    std::cout << "Ingrese apertura (1 a 7500): ";
    std::cin >> exposure;
    std::cout << "Ingrese ganancia (1 a 240): ";
    std::cin >> gain;
    std::cout << "Ingrese brillo (1 a 10): ";
    std::cin >> brightness;

    selectedCam.setExposure(exposure);
    selectedCam.setBrightness(brightness);
    selectedCam.setGain(gain);

    selectedCam.getExposure(exposure);
    selectedCam.getBrightness(brightness);
    selectedCam.getGain(gain);

    status = selectedCam.grabFrame(left, right);
    if (status != TARAXL_SUCCESS)
    {
        std::cout << "No se pudo capturar imágen." << std::endl;
        return 1;
    }

    std::cout << std::endl << "Ganancia: " << gain << std::endl;
    std::cout << "Apertura: " << exposure << std::endl;
    std::cout << "Brillo: " << brightness << std::endl;

    std::cout << std::endl << "Ancho imagen izquierda: " << left.cols << std::endl;
    std::cout << "Alto imagen izquierda: " << left.rows << std::endl;
    std::cout << "Canales imagen izquierda: " << left.channels() << std::endl;

    std::cout << std::endl << "Ancho imagen derecha: " << right.cols << std::endl;
    std::cout << "Alto imagen derecha: " << right.rows << std::endl;
    std::cout << "Canales imagen derecha: " << right.channels() << std::endl;

    std::cout << std::endl << "Realizando procesamiento básico..." << std::endl;

    cv::Mat boxFilter, gaussianFilter, medianFilter;

    cv::boxFilter(left, boxFilter, -1, cv::Size(15, 15));
    cv::GaussianBlur(left, gaussianFilter, cv::Size(15, 15), 8.0);
    cv::medianBlur(left, medianFilter, 15);

    std::cout << std::endl << "Guardando imágenes." << std::endl;

    cv::imwrite("stereo_cam_left_img.jpg", left);
    cv::imwrite("stereo_cam_right_img.jpg", right);
    cv::imwrite("stereo_cam_left_box.jpg", boxFilter);
    cv::imwrite("stereo_cam_left_gaussian.jpg", gaussianFilter);
    cv::imwrite("stereo_cam_left_median.jpg", medianFilter);

    selectedCam.disconnect();
}