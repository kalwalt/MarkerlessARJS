#include "MarkerlessARJS.h"
#include "emscripten.h"
#include <opencv2/opencv.hpp>
#include <string>

void processVideo(const cv::Mat& patternImage, CameraCalibration& calibration, cv::VideoCapture& capture)
{

    // Grab first frame to get the frame dimensions
    cv::Mat currentFrame;
    capture >> currentFrame;

    // Check the capture succeeded:
    if (currentFrame.empty())
    {
        std::cout << "Cannot open video capture device" << std::endl;
        return;
    }

    cv::Size frameSize(currentFrame.cols, currentFrame.rows);

    ARPipeline pipeline(patternImage, calibration);
    //ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

    bool shouldQuit = false;
    do
    {
        capture >> currentFrame;
        if (currentFrame.empty())
        {
            shouldQuit = true;
            continue;
        }

        //shouldQuit = processFrame(id, currentFrame);
    } while (!shouldQuit);
}

void processSingleImage(const cv::Mat& patternImage, CameraCalibration& calibration, const cv::Mat& image)
{
    cv::Size frameSize(image.cols, image.rows);
    ARPipeline pipeline(patternImage, calibration);
    //ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

    /*bool shouldQuit = false;
    do
    {
        shouldQuit = processFrame(id, image);
    } while (!shouldQuit);*/
};

/*bool getEnableHomographyRefinement(){
  return mkarc->pipeline->m_patternDetector.enableHomographyRefinement;
}*/

void setHomographyReprojectionThreshold(ARPipeline &pipeline, float value) {
  pipeline.m_patternDetector.homographyReprojectionThreshold = value;
};

void buildProjectionMatrix(const CameraCalibration& calibration, int screen_width, int screen_height, Matrix44& projectionMatrix)
{
  float nearPlane = 0.01f;  // Near clipping distance
  float farPlane  = 100.0f;  // Far clipping distance

  // Camera parameters
  float f_x = calibration.fx(); // Focal length in x axis
  float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
  float c_x = calibration.cx(); // Camera primary point x
  float c_y = calibration.cy(); // Camera primary point y

  projectionMatrix.data[0] = -2.0f * f_x / screen_width;
  projectionMatrix.data[1] = 0.0f;
  projectionMatrix.data[2] = 0.0f;
  projectionMatrix.data[3] = 0.0f;

  projectionMatrix.data[4] = 0.0f;
  projectionMatrix.data[5] = 2.0f * f_y / screen_height;
  projectionMatrix.data[6] = 0.0f;
  projectionMatrix.data[7] = 0.0f;

  projectionMatrix.data[8] = 2.0f * c_x / screen_width - 1.0f;
  projectionMatrix.data[9] = 2.0f * c_y / screen_height - 1.0f;
  projectionMatrix.data[10] = -( farPlane + nearPlane) / ( farPlane - nearPlane );
  projectionMatrix.data[11] = -1.0f;

  projectionMatrix.data[12] = 0.0f;
  projectionMatrix.data[13] = 0.0f;
  projectionMatrix.data[14] = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );
  projectionMatrix.data[15] = 0.0f;
}

/*
bool isPatternPresent(const cv::Mat& cameraFrame, ARPipeline& pipeline) {
  // Find a pattern and update it's detection status:
  return pipeline.processFrame(cameraFrame);

}

bool patternPose(ARPipeline& pipeline) {
  // Update a pattern pose:
  return pipeline.getPatternLocation();
}
*/
