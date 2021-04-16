#include <iostream>
#include <iomanip>
#include <deque>
#include <ctime>
#include <thread>
#include <mutex>
#include <chrono>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "TaraXL.h"
#include "TaraXLCam.h"
#include "TaraXLDepth.h"

using namespace TaraXLSDK;
using StereoPair = std::pair<cv::Mat, cv::Mat>;

bool KEEP_RECORDING = true;
bool VERBOSE = false;
long int frame_count = 0;

std::mutex stereo_mutex;

struct cam_params {
    double fps;
    Resolution res;
};

bool init_camera(TaraXL& taraxl_cam,
                 TaraXLCam& selected_cam,
                 TaraXLCamList& taraxl_cam_list,
                 TARAXL_STATUS_CODE& status);
cam_params get_params();
cv::VideoWriter get_videowriter(cam_params& params);
void wait_for_key();
double get_framerate(TaraXLCam& selected_cam);
void record_video(TaraXLCam& selected_cam, cam_params& params);
void process_stereo_pair(cv::VideoWriter& stereo_cam,
                         std::deque<StereoPair>& stereo_queue,
                         cam_params& params);
void show_status(std::deque<StereoPair>& stereo_queue);

int main(int argc, char **argv) {

    // Inicialización de cámara
    if (argc != 1) {
        const char* verbose_option_1 = "-v";
        const char* verbose_option_2 = "--verbose";
        bool set_verbose =
                (std::strcmp(argv[1], verbose_option_1) == 0) ||
                (std::strcmp(argv[1], verbose_option_2) == 0);
        if (set_verbose) {
            VERBOSE = true;
            std::cout << "Verbose activado." << std::endl;
        }
        else {
            std::cerr << "Uso: video_capture [OPTION]" << std::endl;
            std::cerr << "OPTION: -v, --verbose" << std::endl;
            return 1;
        }
    }

    TaraXL taraxl_cam;
    TaraXLCam selected_cam;
    TaraXLCamList taraxl_cam_list;
    TARAXL_STATUS_CODE status;

    std::cout << "Inicializando cámara." << std::endl;
    bool init = init_camera(taraxl_cam, selected_cam, taraxl_cam_list, status);
    if (init)
        std::cout << "Cámara inicializada correctamente." << std::endl;
    else {
        std::cout << "No se pudo iniciar la cámara correctamente." << std::endl;
        return 1;
    }
    cam_params params = get_params();
    std::cout << "Detectando fps." << std::endl;
    params.fps = get_framerate(selected_cam);

    std::cout << "Iniciando grabación en resolución " <<
        params.res.width << "x" << params.res.height << " @ "
        << std::fixed << std::setprecision(2) << params.fps << " fps" << std::endl;

    std::thread io(wait_for_key);
    std::thread record(record_video, std::ref(selected_cam), std::ref(params));

    io.join();
    record.join();
}

void record_video(TaraXLCam& selected_cam, cam_params& params)
{
    cv:: VideoWriter stereo_cam = get_videowriter(params);
    std::deque<StereoPair> stereo_queue;

    if (VERBOSE)
        std::cout << std::endl << "Creando hilos de procesamiento." << std::endl;
    std::thread stereo_processor(process_stereo_pair,
                                 std::ref(stereo_cam),
                               std::ref(stereo_queue),
                               std::ref(params));
    std::thread status(show_status, std::ref(stereo_queue));

    while (KEEP_RECORDING) {
        cv::Mat left, right;
        selected_cam.grabFrame(left, right);

        stereo_mutex.lock();
        stereo_queue.push_back(std::make_pair(left, right));
        stereo_mutex.unlock();
    }
    if (VERBOSE)
        std::cout << "Esperando término de hilos de procesamiento." << std::endl;
    status.join();
    if (VERBOSE)
        std::cout << "{Status} Finalizado." << std::endl;
    stereo_processor.join();
    if (VERBOSE)
        std::cout << "{Hilo Estéreo} Finalizado." << std::endl;

    stereo_cam.release();
    selected_cam.disconnect();

    std::cout << "Grabación finalizada con exito!" << std::endl;
}

void process_stereo_pair(cv::VideoWriter& stereo_cam,
                         std::deque<StereoPair>& stereo_queue,
                         cam_params& params) {
    bool queue_not_empty = false;
    while (KEEP_RECORDING || queue_not_empty > 0) {
        stereo_mutex.lock();

        size_t stereo_queue_size = stereo_queue.size();
        if (stereo_queue_size <= 0) {
            queue_not_empty = false;
            stereo_mutex.unlock();
            continue;
        }
        queue_not_empty = true;
        cv::Mat frame;
        StereoPair& stereo_frame = stereo_queue.front();

        cv::hconcat(stereo_frame.first, stereo_frame.second, frame);
        cv::resize(frame,
                   frame,
                   cv::Size(params.res.width,params.res.height),
                   0, 0,
                   cv::INTER_LINEAR);
        stereo_cam.write(frame);

        frame_count++;
        stereo_queue.pop_front();
        stereo_mutex.unlock();
    }
}

void show_status(std::deque<StereoPair>& stereo_queue) {
    if (!VERBOSE)
        return;

    size_t stereo_queue_size;
    long int n_frames;

    double verbose_time = 2000.0; // ms
    auto init = std::chrono::high_resolution_clock::now();
    while (KEEP_RECORDING) {
        auto checkpoint = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_checkpoint = checkpoint - init;
        if (ms_checkpoint.count() >= verbose_time) {
            stereo_mutex.lock();
            stereo_queue_size = stereo_queue.size();
            n_frames = frame_count;
            stereo_mutex.unlock();

            std::cout << std::endl << "Tamaño cola estéreo: " << stereo_queue_size << std::endl;
            std::cout << "Numero de frames escritos: " << n_frames << std::endl;

            init = std::chrono::high_resolution_clock::now();
        }
    }
}

cv::VideoWriter get_videowriter(cam_params& params) {
    time_t now = time(nullptr);
    char *date_time = ctime(&now);

    std::string video_name;
    video_name = std::string("SCAM-") + std::string(date_time) + std::string(".mkv");
    video_name.erase(
            std::remove(video_name.begin(), video_name.end(), '\n'),
            video_name.end()
    );

    cv::VideoWriter stereo_cam(
            video_name,
            cv::VideoWriter::fourcc('X', '2', '6', '4'),
            params.fps,
            cv::Size(params.res.width, params.res.height),
            false
    );
    return stereo_cam;
}

void wait_for_key() {
    while (KEEP_RECORDING) {
        std::cout << "Ingrese la tecla f para finalizar grabación: ";
        char c;
        std::cin >> c;

        if (c == 'f')
        {
            KEEP_RECORDING = false;
            return;
        }
    }
}

cam_params get_params() {
    std::cout << "Seleccione formato de grabación:" << std::endl;
    std::cout << "0: 1280x720 (HD) (por defecto)" << std::endl;
    std::cout << "1: 640x480 (SD)" << std::endl;
    std::cout << "Opción: ";
    cam_params selected_params = {0, {0, 0}};
    int option;
    std::cin >> option;

    switch (option) {
        case 0:
            selected_params.fps = 0.0;
            selected_params.res.width = 1280;
            selected_params.res.height = 720;
            break;

        case 1:
            selected_params.fps = 0.0;
            selected_params.res.width = 640;
            selected_params.res.height = 480;
            break;

        default:
            std::cout << "Opción invalida, seleccionado parámetros por defecto." << std::endl;
            selected_params.fps = 0.0;
            selected_params.res.width = 1280;
            selected_params.res.height = 720;
            break;
    }
    return selected_params;
}

bool init_camera(TaraXL& taraxl_cam,
                 TaraXLCam& selected_cam,
                 TaraXLCamList& taraxl_cam_list,
                 TARAXL_STATUS_CODE& status) {
    status = taraxl_cam.enumerateDevices(taraxl_cam_list);
    if (status != TARAXL_SUCCESS) {
        std::cout << "No se pudo obtener lista de dispositivos." << std::endl;
        return false;
    }

    if (taraxl_cam_list.size() == 0) {
        std::cout << "Camara no detectada." << std::endl;
        return false;
    }
    if (taraxl_cam_list.size() > 1) {
        std::cout << "Se detecto más de 1 camara conectada, "
                     "se utilizará el primer dispositivo." << std::endl;
    }

    selected_cam = taraxl_cam_list.at(0);
    status = selected_cam.connect();
    if (status != TARAXL_SUCCESS) {
        std::cout << "No se pudo conectar a la camara." << std::endl;
        return false;
    }
    return true;
}

double get_framerate(TaraXLCam& selected_cam) {
    const double STOP_TIME_MS = 10000.0;
    std::vector<double> measurements;
    auto init = std::chrono::high_resolution_clock::now();
    while (true) {
        auto checkpoint = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_checkpoint = checkpoint - init;
        if (ms_checkpoint.count() >= STOP_TIME_MS)
            break;

        cv::Mat left, right;
        auto start = std::chrono::high_resolution_clock::now();
        selected_cam.grabFrame(left, right);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_double = end - start;
        measurements.push_back(ms_double.count());
    }
    // borramos las primeras 15 entradas
    for (int i = 0; i < 15; ++i) {
        measurements.erase(measurements.begin());
    }

    double cumsum = 0.0;
    for (double time : measurements) {
        cumsum += time;
    }
    double mean = cumsum / (double)measurements.size();
    double fps = (1000.0 / mean) - 6;
    return fps;
}