#include "MarkerlessARJS.h"

MarkerlessARJS::MarkerlessARJS(){};

void MarkerlessARJS::setupCamera(float _fx, float _fy, float _cx, float _cy) {
  m_calibration.fx() = _fx;
  m_calibration.fy() = _fy;
  m_calibration.cx() = _cx;
  m_calibration.cy() = _cy;
};

void MarkerlessARJS::processPatternImage(emscripten::val patternImage, size_t width, size_t height) {
  auto u8 = emscripten::convertJSArrayToNumberVector<uint8_t>(patternImage);
  cv::Mat grayImage(height, width, CV_8UC1);
  cv::Mat patternImageMat = cv::Mat(height, width, CV_8UC4, u8.data());
  cv::cvtColor(patternImageMat, grayImage, cv::COLOR_RGBA2GRAY);
  ARPipeline pipeline(grayImage, m_calibration);
  m_pipeline = pipeline;
};

void MarkerlessARJS::processVideoFrame(emscripten::val frameImage, size_t width, size_t height) {
  auto u8 = emscripten::convertJSArrayToNumberVector<uint8_t>(frameImage);
  cv::Mat grayImage(height, width, CV_8UC1);
  cv::Mat frameImageMat = cv::Mat(height, width, CV_8UC4, u8.data());
  cv::cvtColor(frameImageMat, grayImage, cv::COLOR_RGBA2GRAY);
  m_pipeline.processFrame(frameImageMat);

};

#include "MarkerlessARJS_bindings.cpp"