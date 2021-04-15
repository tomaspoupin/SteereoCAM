#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "commons.hpp"
#include "CSVWriter.h"

namespace fs = boost::filesystem;

int main(int argc, char **argv) {
    std::stringstream buffer;

    double time_in_seconds;
    int num_disparities;
    int block_size;

    TaraXL taraxl_cam;
    TaraXLCamList taraxl_cam_list;
    TARAXL_STATUS_CODE status;
    auto selected_cam = std::make_shared<TaraXLCam>();

    if (argc != 4) {
        std::cerr << "Uso: benchmark_disparity_opencv <tiempo en segundos>"
                     " <numero disparidad>"
                     " <tamaño bloque>" << std::endl;
        return 1;
    }

    buffer << argv[1];
    buffer >> time_in_seconds;
    buffer.str("");
    buffer.clear();

    buffer << argv[2];
    buffer >> num_disparities;
    buffer.str("");
    buffer.clear();

    buffer << argv[3];
    buffer >> block_size;
    buffer.str("");
    buffer.clear();

    bool init = init_camera(taraxl_cam,
                            *selected_cam,
                            taraxl_cam_list,
                            status);
    if (init)
        std::cout << "Cámara iniciada correctamente." << std::endl;
    else {
        std::cout << "Error inicializando cámara." << std::endl;
        return 1;
    }

    std::cout << "Inicializando parámetros." << std::endl;

    StereoProcParams proc_params = {
            .NUM_DISPARITIES = num_disparities,
            .BLOCK_SIZE = block_size,
            .DEPTH_ACCURACY = ACCURACY_MODE::LOW,
            .PCL_QUALITY = TARAXL_POINTCLOUD_QUALITY::STANDARD,
            .PCL_FORMAT = TARAXL_POINTCLOUD_FORMAT::TARAXL_PCD_CLOUD,
    };

    DepthOpenCVBenchmark disparity_opencv_benchmark(selected_cam, proc_params);

    std::cout << "Ejecutando benchmark." << std::endl <<
        "Numero de disparidades: " << num_disparities << std::endl <<
        "Tamaño de bloque: " << block_size << std::endl;

    test_performance(disparity_opencv_benchmark, time_in_seconds*1000);
    selected_cam->disconnect();

    std::cout << "Exportando material e informe:" << std::endl;
    fs::path curr_dir(fs::current_path());
    fs::path full_path = curr_dir / "report" / "benchmark_disparity_opencv";
    if (!fs::exists(full_path))
        fs::create_directories("report/benchmark_disparity_opencv");

    fs::path left_filename = full_path / "left.mkv";
    fs::path right_filename = full_path / "right.mkv";
    fs::path disparity_filename = full_path / "disparity.mkv";

    double fps = 1000.0 / promedio(disparity_opencv_benchmark.measurements);
    record_video(disparity_opencv_benchmark.payload.left, fps, left_filename.string());
    record_video(disparity_opencv_benchmark.payload.right, fps, right_filename.string());
    record_video(disparity_opencv_benchmark.payload.disparity, fps, disparity_filename.string());

    std::cout << "Material exportado!" << std::endl;

    fs::path fps_filename = full_path / "fps.csv";
    CSVWriter csv;
    for (double unit : disparity_opencv_benchmark.measurements) {
        csv.newRow() << 1000.0 / unit;
    }
    csv.writeToFile(fps_filename.string());

    std::cout << "Informe exportado!" << std::endl;
    std::cout << "Ubicación: " << full_path.string() << std::endl;
}