#include "commons.hpp"

bool init_camera(TaraXL& taraxl_cam,
                 TaraXLCam& selected_cam,
                 TaraXLCamList& taraxl_cam_list,
                 TARAXL_STATUS_CODE& status) {
    status = taraxl_cam.enumerateDevices(taraxl_cam_list);
    if (status != TARAXL_SUCCESS) {
        std::cout << "No se pudo obtener lista de dispositivos." << std::endl;
        return false;
    }

    if (taraxl_cam_list.empty()) {
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

void get_stereo_params(TaraXLCam& selected_cam, StereoParams& params) {
    selected_cam.getCalibrationParameters(
            params.ROT_MATRIX, params.TRANS_MATRIX,
            params.LEFT_CALIB, params.RIGHT_CALIB,
            params.LEFT_CALIB_RECTIFIED, params.RIGHT_CALIB_RECTIFIED
            );
    selected_cam.getQMatrix(params.Q_MATRIX);
}

void test_performance(Benchmark& benchmark, double time_ms) {

    auto init = std::chrono::high_resolution_clock::now();
    while (true) {
        auto checkpoint = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_checkpoint = checkpoint - init;
        if (ms_checkpoint.count() >= time_ms)
            break;

        benchmark.preprocess();
        auto start = std::chrono::high_resolution_clock::now();
        benchmark.perform_operation();
        auto end = std::chrono::high_resolution_clock::now();
        benchmark.postprocess();

        std::chrono::duration<double, std::milli> ms_double = end - start;
        benchmark.add_measurement(ms_double.count());
    }
}

FramerateBenchmark::FramerateBenchmark(std::shared_ptr<TaraXLCam> stereo_cam_)
: stereo_cam(std::move(stereo_cam_))
{}
void FramerateBenchmark::add_measurement(double measure) {
    measurements.push_back(measure);
}
void FramerateBenchmark::perform_operation() {
    stereo_cam->grabFrame(left, right);
}
void FramerateBenchmark::postprocess() {
    payload.left.push_back(left.clone());
    payload.right.push_back(right.clone());
}

DepthSDKBenchmark::DepthSDKBenchmark(std::shared_ptr<TaraXLDepth> taraxl_depth_,
                                     const StereoProcParams& proc_params_)
: taraxl_depth(std::move(taraxl_depth_)), proc_params(proc_params_)
{
    taraxl_depth->setAccuracy(proc_params.DEPTH_ACCURACY);
}
void DepthSDKBenchmark::add_measurement(double measure) {
    measurements.push_back(measure);
}
void DepthSDKBenchmark::perform_operation() {
    taraxl_depth->getMap(left, right,
                         disparity, true,
                         depth, true);
}
void DepthSDKBenchmark::postprocess() {
    disparity.convertTo(disparity, CV_8U);
    depth.convertTo(depth, CV_8U);
    payload.left.push_back(left.clone());
    payload.right.push_back(right.clone());
    payload.disparity.push_back(disparity);
    payload.depth.push_back(depth);
}

DepthOpenCVBenchmark::DepthOpenCVBenchmark(std::shared_ptr<TaraXLCam> stereo_cam_,
                                           const StereoProcParams &proc_params_)
: stereo_cam(std::move(stereo_cam_)), proc_params(proc_params_),
stereo_bm(cv::StereoBM::create(proc_params_.NUM_DISPARITIES, proc_params_.BLOCK_SIZE))
{}
void DepthOpenCVBenchmark::add_measurement(double measure) {
    measurements.push_back(measure);
}
void DepthOpenCVBenchmark::preprocess() {
    stereo_cam->grabFrame(left, right);
}
void DepthOpenCVBenchmark::perform_operation() {
    stereo_bm->compute(left, right, disparity);
}
void DepthOpenCVBenchmark::postprocess() {
    cv::normalize(disparity, disparity, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    payload.left.push_back(left.clone());
    payload.right.push_back(right.clone());
    payload.disparity.push_back(disparity);
}

PCLSDKBenchmark::PCLSDKBenchmark(std::shared_ptr<TaraXLPointcloud> taraxl_pcl_,
                                 const StereoProcParams &proc_params_)
: taraxl_pcl(std::move(taraxl_pcl_)), proc_params(proc_params_)
{
    taraxl_pcl->setPointcloudQuality(proc_params.PCL_QUALITY);
}
void PCLSDKBenchmark::add_measurement(double measure) {
    measurements.push_back(measure);
}
void PCLSDKBenchmark::perform_operation() {
    taraxl_pcl->getPoints(currentCloud);
}
void PCLSDKBenchmark::postprocess() {
    payload.pointcloud.push_back(currentCloud);
}

double promedio(const std::vector<double>& measurements) {
    double cumsum = 0;
    for (double unit : measurements) {
        cumsum += unit;
    }
    return cumsum / (double)measurements.size();
}

void record_video(std::vector<cv::Mat>& footage, double fps, const std::string& filename) {
    cv::VideoWriter record(
            filename,
            cv::VideoWriter::fourcc('X', '2', '6', '4'),
            fps,
            cv::Size(1280, 720),
            false
    );
    for (cv::Mat& frame : footage) {
        cv::Mat output_frame;
        cv::resize(frame, output_frame, cv::Size(1280, 720));
        record.write(output_frame);
    }
}