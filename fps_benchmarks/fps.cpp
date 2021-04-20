#include <iostream>
#include <sstream>
#include "commons.hpp"
#include "CSVWriter.h"

int main(int argc, char **argv) {
    CSVWriter csv;
    std::stringstream buffer;
    double time_in_seconds;
    TaraXL taraxl_cam;
    TaraXLCamList taraxl_cam_list;
    TARAXL_STATUS_CODE status;
    auto selected_cam = std::make_shared<TaraXLCam>();

    if (argc != 2) {
        std::cerr << "Uso: benchmark_fps <tiempo en segundos>" << std::endl;
        return 1;
    }
    buffer << argv[1];
    buffer >> time_in_seconds;

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

    std::cout << "Realizando medición de fps durante " << time_in_seconds << " segundos." << std::endl;
    FramerateBenchmark fps_benchmark(selected_cam);
    test_performance(fps_benchmark, time_in_seconds*1000);
    selected_cam->disconnect();

    std::cout << "FPS Promedio: " << 1000.0 / promedio(fps_benchmark.measurements) << std::endl;
    std::cout << "Escribiendo resultados." << std::endl;

    for (double unit : fps_benchmark.measurements) {
        double fps = 1000.0 / unit;
        csv.newRow() << fps;
    }

    fs::path curr_dir(fs::current_path());
    fs::path full_path = curr_dir / "report" / "benchmark_fps";
    if (!fs::exists(full_path))
        fs::create_directories("report/benchmark_fps");

    fs::path filename = full_path / "fps.csv";
    csv.writeToFile(filename.string());
    std::cout << "Resultados escritos satisfactoriamente." << std::endl;

}