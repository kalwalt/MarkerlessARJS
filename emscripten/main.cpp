//
// Created by perdan on 06/12/2023.
//


#include <ARDrawingContext.hpp>
#include <ARPipeline.hpp>
#include <CameraCalibration.hpp>
#include <GeometryTypes.hpp>
#include <Pattern.hpp>
#include <PatternDetector.hpp>

#include <emscripten.h>
#include <emscripten/val.h>
#include <opencv2/core.hpp>
//#include <opencv2/videoio.hpp>
#include <string>

std::function<void()> loop;
void main_loop() { loop(); }

int main(void) {
  // Change this calibration to yours:
  CameraCalibration calibration(526.58037684199849f, 524.65577209994706f, 318.41744018680112f, 202.96659047014398f);
  int width = 640;
  int height = 480;
  cv::Size frameSize(width, height);
  ARDrawingContext context("context", frameSize, calibration);
  printf("context created\n");
  emscripten::val patternImage;

  auto u8 = emscripten::convertJSArrayToNumberVector<uint8_t>(patternImage);
  cv::Mat grayImage(height, width, CV_8UC1);
  cv::Mat patternImageMat = cv::Mat(height, width, CV_8UC4, u8.data());
  cv::cvtColor(patternImageMat, grayImage, cv::COLOR_RGBA2GRAY);
  ARPipeline pipeline(grayImage, calibration);

  loop = [&] { context.updateWindow(); };

  emscripten_set_main_loop(main_loop, 0, true);
}