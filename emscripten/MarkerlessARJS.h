#include "ARPipeline.hpp"
#include "CameraCalibration.hpp"
#include "Pattern.hpp"
#include "PatternDetector.hpp"
#include <opencv2/videoio.hpp>

/**
 * Processes a recorded video or live view from web-camera and allows you to adjust homography refinement and
 * reprojection threshold in runtime.
 */
void processVideo(const cv::Mat& patternImage, cv::VideoCapture& capture);

/**
 * Processes single image. The processing goes in a loop.
 * It allows you to control the detection process by adjusting homography refinement switch and
 * reprojection threshold in runtime.
 */
void processSingleImage(const cv::Mat& patternImage, const cv::Mat& image);

//bool getEnableHomographyRefinement(int id);

void setHomographyReprojectionThreshold(float value);
/*
bool isPatternPresent(ARPipeline& pipeline);

bool patternPose(ARPipeline& pipeline);
*/
