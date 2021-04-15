#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "commons.hpp"
#include "CSVWriter.h"

namespace fs = boost::filesystem;

const ACCURACY_MODE accuracy_mode[3] = {ACCURACY_MODE::LOW,
                                        ACCURACY_MODE::HIGH,
                                        ACCURACY_MODE::ULTRA};

int main(int argc, char **argv) {
    const std::string ac_string[3] = {"LOW", "HIGH", "ULTRA"};
    std::stringstream buffer;

    double time_in_seconds;
    int accuracy;

    TaraXL taraxl_cam;
    TaraXLCam selected_cam;
    TaraXLCamList taraxl_cam_list;
    TARAXL_STATUS_CODE status;

    if (argc != 3) {
        std::cerr << "Uso: benchmark_depth_sdk <tiempo en segundos>"
                     " <depth accuracy>" << std::endl;
        return 1;
    }
    buffer << argv[1];
    buffer >> time_in_seconds;
    buffer.str("");
    buffer.clear();
    buffer << argv[2];
    buffer >> accuracy;

    if (accuracy < 0 || accuracy > 2) {
        std::cout << "La presición debe estár entre 0 y 2." << std::endl;
        return 1;
    }

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

    std::cout << "Inicializando parámetros." << std::endl;

    auto taraxl_depth = std::make_shared<TaraXLDepth>(selected_cam);
    StereoProcParams proc_params = {
            .NUM_DISPARITIES = 0,
            .BLOCK_SIZE = 0,
            .DEPTH_ACCURACY = accuracy_mode[accuracy],
            .PCL_QUALITY = TARAXL_POINTCLOUD_QUALITY::STANDARD,
            .PCL_FORMAT = TARAXL_POINTCLOUD_FORMAT::TARAXL_PCD_CLOUD,
    };
    DepthSDKBenchmark depth_benchmark(taraxl_depth, proc_params);
    std::cout << "Ejecutando benchmark con presición: " <<
        ac_string[accuracy] << std::endl;
    std::cout << "Iniciando benchmark. Duración: " << time_in_seconds <<
       " segundos." <<std::endl;

    test_performance(depth_benchmark, time_in_seconds*1000);
    selected_cam.disconnect();

    std::cout << "Exportando material e informe:" << std::endl;
    fs::path curr_dir(fs::current_path());
    fs::path full_path = curr_dir / "report" / "benchmark_depth_sdk";
    if (!fs::exists(full_path))
        fs::create_directories("report/benchmark_depth_sdk");

    fs::path left_filename = full_path / "left.mkv";
    fs::path right_filename = full_path / "right.mkv";
    fs::path disparity_filename = full_path / "disparity.mkv";
    fs::path depth_filename = full_path / "depth.mkv";

    double fps = 1000.0 / promedio(depth_benchmark.measurements);
    record_video(depth_benchmark.payload.left, fps, left_filename.string());
    record_video(depth_benchmark.payload.right, fps, right_filename.string());
    record_video(depth_benchmark.payload.disparity, fps, disparity_filename.string());
    record_video(depth_benchmark.payload.depth, fps, depth_filename.string());

    std::cout << "Material exportado!" << std::endl;

    fs::path fps_filename = full_path / "fps.csv";
    CSVWriter csv;
    for (double unit : depth_benchmark.measurements) {
        csv.newRow() << 1000.0 / unit;
    }
    csv.writeToFile(fps_filename.string());

    std::cout << "Informe exportado!" << std::endl;
    std::cout << "Ubicación: " << full_path.string() << std::endl;
}