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
}

/*bool getEnableHomographyRefinement(){
  return mkarc->pipeline->m_patternDetector.enableHomographyRefinement;
}*/

void setHomographyReprojectionThreshold(ARPipeline &pipeline, float value) {
  pipeline.m_patternDetector.homographyReprojectionThreshold = value;
};


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
