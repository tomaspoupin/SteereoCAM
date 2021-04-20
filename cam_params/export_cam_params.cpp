#include <iostream>
#include <opencv2/core/persistence.hpp>
#include <opencv2/imgproc.hpp>
#include <commons.hpp>

int main(int argc, char** argv)
{
    if (argc != 1)
    {
        std::cerr << "El progama no requiere argumentos." << std::endl;
        return 1;
    }

    StereoParams cam_params;
    TaraXL taraxl_cam;
    TaraXLCam selected_cam;
    TaraXLCamList taraxl_cam_list;
    TARAXL_STATUS_CODE status;

    bool init = init_camera(taraxl_cam,
                        selected_cam,
                        taraxl_cam_list,
                        status);

    if (init)
        std::cout << "Cámara iniciada correctamente." << std::endl;
    else {
        std::cerr << "Error inicializando cámara." << std::endl;
        return 1;
    }

    get_stereo_params(selected_cam, cam_params);

    cv::FileStorage left_calib_storage("stereo_params_left.yml", 
        cv::FileStorage::Mode::WRITE);
    left_calib_storage.write("apertureHeight",
        cam_params.LEFT_CALIB.apertureHeight);
    left_calib_storage.write("apertureWidth",
        cam_params.LEFT_CALIB.apertureWidth);
    left_calib_storage.write("focalLength",
        cam_params.LEFT_CALIB.focalLength);
    left_calib_storage.write("fovX",
        cam_params.LEFT_CALIB.fovX);
    left_calib_storage.write("fovY",
        cam_params.LEFT_CALIB.fovY);
    left_calib_storage.write("cameraMatrix",
        cam_params.LEFT_CALIB.cameraMatrix);
    left_calib_storage.write("rectifiedCameraMatrix",
        cam_params.LEFT_CALIB.rectifiedCameraMatrix);
    left_calib_storage.write("distortionMatrix",
        cam_params.LEFT_CALIB.distortionMatrix);

    cv::FileStorage right_calib_storage("stereo_params_right.yml", 
        cv::FileStorage::Mode::WRITE);
    right_calib_storage.write("apertureHeight",
        cam_params.RIGHT_CALIB.apertureHeight);
    right_calib_storage.write("apertureWidth",
        cam_params.RIGHT_CALIB.apertureWidth);
    right_calib_storage.write("focalLength",
        cam_params.RIGHT_CALIB.focalLength);
    right_calib_storage.write("fovX",
        cam_params.RIGHT_CALIB.fovX);
    right_calib_storage.write("fovY",
        cam_params.RIGHT_CALIB.fovY);
    right_calib_storage.write("cameraMatrix",
        cam_params.RIGHT_CALIB.cameraMatrix);
    right_calib_storage.write("rectifiedCameraMatrix",
        cam_params.RIGHT_CALIB.rectifiedCameraMatrix);
    right_calib_storage.write("distortionMatrix",
        cam_params.RIGHT_CALIB.distortionMatrix);

    cv::FileStorage right_rect_calib("stereo_params_right_rect.yml", 
        cv::FileStorage::Mode::WRITE);
    right_rect_calib.write("apertureHeight",
        cam_params.RIGHT_CALIB_RECTIFIED.apertureHeight);
    right_rect_calib.write("apertureWidth",
        cam_params.RIGHT_CALIB_RECTIFIED.apertureWidth);
    right_rect_calib.write("focalLength",
        cam_params.RIGHT_CALIB_RECTIFIED.focalLength);
    right_rect_calib.write("fovX",
        cam_params.RIGHT_CALIB_RECTIFIED.fovX);
    right_rect_calib.write("fovY",
        cam_params.RIGHT_CALIB_RECTIFIED.fovY);
    right_rect_calib.write("cameraMatrix",
        cam_params.RIGHT_CALIB_RECTIFIED.cameraMatrix);
    right_rect_calib.write("rectifiedCameraMatrix",
        cam_params.RIGHT_CALIB_RECTIFIED.rectifiedCameraMatrix);
    right_rect_calib.write("distortionMatrix",
        cam_params.RIGHT_CALIB_RECTIFIED.distortionMatrix);

    cv::FileStorage left_rect_calib("stereo_params_left_rect.yml", 
        cv::FileStorage::Mode::WRITE);
    left_rect_calib.write("apertureHeight",
        cam_params.LEFT_CALIB_RECTIFIED.apertureHeight);
    left_rect_calib.write("apertureWidth",
        cam_params.LEFT_CALIB_RECTIFIED.apertureWidth);
    left_rect_calib.write("focalLength",
        cam_params.LEFT_CALIB_RECTIFIED.focalLength);
    left_rect_calib.write("fovX",
        cam_params.LEFT_CALIB_RECTIFIED.fovX);
    left_rect_calib.write("fovY",
        cam_params.LEFT_CALIB_RECTIFIED.fovY);
    left_rect_calib.write("cameraMatrix",
        cam_params.LEFT_CALIB_RECTIFIED.cameraMatrix);
    left_rect_calib.write("rectifiedCameraMatrix",
        cam_params.LEFT_CALIB_RECTIFIED.rectifiedCameraMatrix);
    left_rect_calib.write("distortionMatrix",
        cam_params.LEFT_CALIB_RECTIFIED.distortionMatrix);

    cv::FileStorage transformations("stereo_params_transform.yml", 
        cv::FileStorage::Mode::WRITE);
    transformations.write("rotMatrix", cam_params.ROT_MATRIX);
    transformations.write("transMatrix", cam_params.TRANS_MATRIX);
    transformations.write("QMatrix", cam_params.Q_MATRIX);

    left_calib_storage.release();
    right_calib_storage.release();
    left_rect_calib.release();
    right_rect_calib.release();
    transformations.release();
    std::cout << "Archivos escritos satisfactoriamente." << std::endl;
}