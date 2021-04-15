#ifndef __ECAMCOMMON__
#define __ECAMCOMMON__
#include <chrono>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include "TaraXL.h"
#include "TaraXLCam.h"
#include "TaraXLDepth.h"
#include "TaraXLPointcloud.h"
#include "pcl/common/common_headers.h"
#include "pcl/visualization/pcl_visualizer.h"
#include "pcl/visualization/cloud_viewer.h"
#include <pcl/visualization/common/common.h>
#include "pcl/common/transforms.h"

namespace fs = boost::filesystem;

using namespace TaraXLSDK;

/**
 * Los resultados de cada benchmark serán guardados en esta estructura
 */
struct StereoPayload {
    std::vector<cv::Mat> left;
    std::vector<cv::Mat> right;
    std::vector<cv::Mat> disparity;
    std::vector<cv::Mat> depth;
    std::vector<Points::Ptr> pointcloud;
};

/**
 * Los parámetros de la cámara quedan almacenados en esta estructura.
 */
struct StereoParams {
    cv::Mat Q_MATRIX;
    cv::Mat ROT_MATRIX;
    cv::Mat TRANS_MATRIX;
    CalibrationParams LEFT_CALIB;
    CalibrationParams RIGHT_CALIB;
    CalibrationParams LEFT_CALIB_RECTIFIED;
    CalibrationParams RIGHT_CALIB_RECTIFIED;
};

/**
 * Parámetros que definen el flujo de procesamiento dentro de un benchmark.
 */
struct StereoProcParams {
    int NUM_DISPARITIES;
    int BLOCK_SIZE;
    ACCURACY_MODE DEPTH_ACCURACY;
    TARAXL_POINTCLOUD_QUALITY PCL_QUALITY;
    TARAXL_POINTCLOUD_FORMAT PCL_FORMAT;

    void operator()(const StereoProcParams& other_) {
        NUM_DISPARITIES = other_.NUM_DISPARITIES;
        BLOCK_SIZE = other_.BLOCK_SIZE;
        DEPTH_ACCURACY = other_.DEPTH_ACCURACY;
        PCL_QUALITY = other_.PCL_QUALITY;
        PCL_FORMAT = other_.PCL_FORMAT;
    }
};

/**
 * Clase interfaz que define el comportamiento de un benchmark.
 */
class Benchmark {
public:
    virtual void add_measurement(double measure) = 0;
    virtual void preprocess() = 0;
    virtual void postprocess() = 0;
    virtual void perform_operation() = 0;
    virtual ~Benchmark() = default;
};

/**
 * Benchmark que mide cantidad de fps que puede medir la camara.
 */
class FramerateBenchmark : public Benchmark {
public:
    explicit FramerateBenchmark(std::shared_ptr<TaraXLCam> stereo_cam_);
    void add_measurement(double measure) override;
    void preprocess() override {}
    void postprocess() override;
    void perform_operation() override;

    StereoPayload payload;
    std::vector<double> measurements;
private:
    std::shared_ptr<TaraXLCam> stereo_cam;
    cv::Mat left, right;
};

/**
 * Benchmark que mide la velocidad con la cual se puede generar mapas de
 * profundidad y disparidad utilizando el SDK de TaraXL
 */
class DepthSDKBenchmark : public Benchmark {
public:
    explicit DepthSDKBenchmark(std::shared_ptr<TaraXLDepth> taraxl_depth_,
                               const StereoProcParams& proc_params_);
    void add_measurement(double measure) override;
    void preprocess() override {}
    void postprocess() override;
    void perform_operation() override;

    StereoPayload payload;
    std::vector<double> measurements;
    StereoProcParams proc_params;
private:
    std::shared_ptr<TaraXLDepth> taraxl_depth;
    cv::Mat left, right, disparity, depth;
};

/**
 * Benchmark que mide la velocidad con la cual es posible obtener mapas de disparidad
 * utilizando OpenCV.
 */
class DepthOpenCVBenchmark : public Benchmark {
public:
    explicit DepthOpenCVBenchmark(std::shared_ptr<TaraXLCam> stereo_cam_,
                                  const StereoProcParams& proc_params_);
    void add_measurement(double measure) override;
    void preprocess() override;
    void postprocess() override;
    void perform_operation() override;

    StereoPayload payload;
    std::vector<double> measurements;
    StereoProcParams proc_params;
private:
    std::shared_ptr<TaraXLCam> stereo_cam;
    cv::Ptr<cv::StereoBM> stereo_bm;
    cv::Mat left, right, disparity;
};

/**
 * Benchmark que mide velocidad con la cual se pueden obtener nube de puntos
 */
class PCLSDKBenchmark : public Benchmark {
public:
    explicit PCLSDKBenchmark(std::shared_ptr<TaraXLPointcloud> taraxl_pcl_,
                             const StereoProcParams& proc_params_);
    void add_measurement(double measure) override;
    void preprocess() override {}
    void postprocess() override;
    void perform_operation() override;

    StereoPayload payload;
    std::vector<double> measurements;
    StereoProcParams proc_params;
private:
    std::shared_ptr<TaraXLPointcloud> taraxl_pcl;
    Points::Ptr currentCloud;
};

/**
 * Inicializa la cámara estéreo
 * @param taraxl_cam Objeto principal del SDK de la cámara
 * @param selected_cam Cámara estéreo seleccionada
 * @param taraxl_cam_list lista de dispositivos disponibles
 * @param status código de resultado
 * @return verdadero si la cámara se inicializó correctamente
 */
bool init_camera(TaraXL& taraxl_cam,
                 TaraXLCam& selected_cam,
                 TaraXLCamList& taraxl_cam_list,
                 TARAXL_STATUS_CODE& status);

/**
 * Ejecuta operación de benchmark de acuerdo al benchmark escogido
 * @param benchmark benchmark a ejecutar
 * @param time_ms
 */
void test_performance(Benchmark& benchmark, double time_ms);

/**
 * Calcula el promedio de un arreglo
 * @param measurements arreglo de mediciones
 * @return Promedio de mediciones
 */
double promedio(const std::vector<double>& measurements);

/**
 * Graba un video en formato mkv a partir de un arreglo de imágenes
 * @param footage Arreglo de imágenes a grabar
 * @param fps FPS con la cual será grabado el video
 * @param filename nombre del archivo de destino
 */
void record_video(std::vector<cv::Mat>& footage, double fps, const std::string& filename);
#endif