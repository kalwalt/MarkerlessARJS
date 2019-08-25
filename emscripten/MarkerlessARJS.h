#include "ARPipeline.hpp"
#include "CameraCalibration.hpp"
#include "Pattern.hpp"
#include "PatternDetector.hpp"
#include <opencv2/videoio.hpp>

/**
 * Processes a recorded video or live view from web-camera and allows you to adjust homography refinement and
 * reprojection threshold in runtime.
 */
void processVideo(const cv::Mat& patternImage, CameraCalibration& calibration, cv::VideoCapture& capture);

/**
 * Processes single image. The processing goes in a loop.
 * It allows you to control the detection process by adjusting homography refinement switch and
 * reprojection threshold in runtime.
 */
void processSingleImage(const cv::Mat& patternImage, CameraCalibration& calibration, const cv::Mat& image);

/**
 * Performs full detection routine on camera frame and draws the scene using drawing context.
 * In addition, this function draw overlay with debug information on top of the AR window.
 * Returns true if processing loop should be stopped; otherwise - false.
 */
bool processFrame(const cv::Mat& cameraFrame, ARPipeline& pipeline);

bool isPatternPresent(ARPipeline& pipeline);

bool patternPose(ARPipeline& pipeline);
