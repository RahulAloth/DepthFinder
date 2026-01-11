#include <vpi/OpenCVInterop.hpp>
#include <vpi/algo/StereoDisparity.h>
#include <vpi/VPI.h>

#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // Create VPI stream
    VPIStream stream;
    if (vpiStreamCreate(0, &stream) != VPI_SUCCESS) {
        std::cerr << "Failed to create VPI stream\n";
        return -1;
    }

    // Load stereo images using OpenCV (grayscale)
    cv::Mat leftCV  = cv::imread("left.png",  cv::IMREAD_GRAYSCALE);
    cv::Mat rightCV = cv::imread("right.png", cv::IMREAD_GRAYSCALE);

    if (leftCV.empty() || rightCV.empty()) {
        std::cerr << "Failed to load images\n";
        vpiStreamDestroy(stream);
        return -1;
    }

    std::cout << "Two images opened\n";

    int width  = leftCV.cols;
    int height = leftCV.rows;
    std::cout << "Left image size: " << width << "x" << height << "\n";

    width  = rightCV.cols;
    height = rightCV.rows;
    std::cout << "Right image size: " << width << "x" << height << "\n";

    // Wrap OpenCV images into VPIImage
    VPIImage leftVPI, rightVPI, disparityVPI;

    if (vpiImageCreateWrapperOpenCVMat(leftCV,  VPI_IMAGE_FORMAT_U8, 0, &leftVPI) != VPI_SUCCESS ||
        vpiImageCreateWrapperOpenCVMat(rightCV, VPI_IMAGE_FORMAT_U8, 0, &rightVPI) != VPI_SUCCESS) {
        std::cerr << "Failed to wrap OpenCV images into VPI\n";
        vpiStreamDestroy(stream);
        return -1;
    }

    int32_t vpi_width, vpi_height;
    vpiImageGetSize(rightVPI, &vpi_width, &vpi_height);
    std::cout << "Image Size right VPI - Width: " << vpi_width << ", Height: " << vpi_height << "\n";

    vpiImageGetSize(leftVPI, &vpi_width, &vpi_height);
    std::cout << "Image Size left VPI - Width: " << vpi_width << ", Height: " << vpi_height << "\n";

    std::cout << "Images wrapped into VPI\n";

    // Test: lock and save wrapped left image
    VPIImageData outData;
    VPIStatus st = vpiImageLockData(
        leftVPI,
        VPI_LOCK_READ,
        VPI_IMAGE_BUFFER_HOST_PITCH_LINEAR,
        &outData
    );
    if (st != VPI_SUCCESS) {
        std::cerr << "vpiImageLockData failed: " << st << "\n";
        vpiImageDestroy(leftVPI);
        vpiImageDestroy(rightVPI);
        vpiStreamDestroy(stream);
        return -1;
    }

    if (outData.bufferType != VPI_IMAGE_BUFFER_HOST_PITCH_LINEAR) {
        std::cerr << "Unexpected buffer type\n";
        vpiImageUnlock(leftVPI);
        vpiImageDestroy(leftVPI);
        vpiImageDestroy(rightVPI);
        vpiStreamDestroy(stream);
        return -1;
    }

    VPIImageBufferPitchLinear &pitch = outData.buffer.pitch;

    cv::Mat leftMat(
        pitch.planes[0].height,
        pitch.planes[0].width,
        CV_8UC1,
        pitch.planes[0].data,
        pitch.planes[0].pitchBytes
    );

    cv::imwrite("left_wrapped.png", leftMat);
    vpiImageUnlock(leftVPI);

    // Create disparity output image (S16, Q10.5)
    if (vpiImageCreate(width, height, VPI_IMAGE_FORMAT_S16, 0, &disparityVPI) != VPI_SUCCESS) {
        std::cerr << "Failed to create disparity VPI image\n";
        vpiImageDestroy(leftVPI);
        vpiImageDestroy(rightVPI);
        vpiStreamDestroy(stream);
        return -1;
    }

    // Disparity estimator payload creation
    VPIStereoDisparityEstimatorCreationParams createParams;
    vpiInitStereoDisparityEstimatorCreationParams(&createParams);

    createParams.maxDisparity     = 64;
    createParams.downscaleFactor  = 1;
    createParams.includeDiagonals = 1;

    VPIPayload payload;
    if (vpiCreateStereoDisparityEstimator(
            VPI_BACKEND_CUDA,
            width, height,
            VPI_IMAGE_FORMAT_U8,
            &createParams,
            &payload) != VPI_SUCCESS) {
        std::cerr << "Failed to create stereo disparity estimator payload\n";
        vpiImageDestroy(leftVPI);
        vpiImageDestroy(rightVPI);
        vpiImageDestroy(disparityVPI);
        vpiStreamDestroy(stream);
        return -1;
    }

    // Set runtime parameters
    VPIStereoDisparityEstimatorParams params;
    vpiInitStereoDisparityEstimatorParams(&params);

    params.maxDisparity        = 0;   // use payload value
    params.confidenceThreshold = 0;
    params.minDisparity        = 0;
    params.p1                  = 3;
    params.p2                  = 48;
    params.uniqueness          = -1;  // disable uniqueness

    if (vpiSubmitStereoDisparityEstimator(
            stream,
            VPI_BACKEND_CUDA,
            payload,
            leftVPI,
            rightVPI,
            disparityVPI,
            nullptr,     // no confidence map
            &params) != VPI_SUCCESS) {
        std::cerr << "Failed to submit stereo disparity estimator\n";
        vpiPayloadDestroy(payload);
        vpiImageDestroy(leftVPI);
        vpiImageDestroy(rightVPI);
        vpiImageDestroy(disparityVPI);
        vpiStreamDestroy(stream);
        return -1;
    }

    vpiStreamSync(stream);

    // Read back disparity result
    VPIImageData dispData;
    st = vpiImageLockData(
        disparityVPI,
        VPI_LOCK_READ,
        VPI_IMAGE_BUFFER_HOST_PITCH_LINEAR,
        &dispData
    );
    if (st != VPI_SUCCESS) {
        std::cerr << "Failed to lock disparity image\n";
        vpiPayloadDestroy(payload);
        vpiImageDestroy(leftVPI);
        vpiImageDestroy(rightVPI);
        vpiImageDestroy(disparityVPI);
        vpiStreamDestroy(stream);
        return -1;
    }

    auto &plane = dispData.buffer.pitch.planes[0];

    cv::Mat disp16(
        plane.height,
        plane.width,
        CV_16SC1,
        plane.data,
        plane.pitchBytes
    );

    // Convert Q10.5 → float
    cv::Mat dispFloat;
    disp16.convertTo(dispFloat, CV_32F, 1.0 / 32.0);

    cv::imwrite("disparity.png", dispFloat);
    vpiImageUnlock(disparityVPI);

    // Cleanup
    vpiPayloadDestroy(payload);
    vpiImageDestroy(leftVPI);
    vpiImageDestroy(rightVPI);
    vpiImageDestroy(disparityVPI);
    vpiStreamDestroy(stream);

    std::cout << "Disparity saved to disparity.png\n";
    return 0;
}
