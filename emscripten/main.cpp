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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::function<void()> loop;
void main_loop() { loop(); }

//! Load an image from file
uint8_t* loadImage(std::string filename, int width, int height) {
int comp;

const uint8_t* img = stbi_load( filename.c_str(), &width, &height, &comp, 3);
return (uint8_t*)img;
}

int main(void) {
  // Change this calibration to yours:
  CameraCalibration calibration(526.58037684199849f, 524.65577209994706f, 318.41744018680112f, 202.96659047014398f);
  int width = 640;
  int height = 480;
  cv::Size frameSize(width, height);
  ARDrawingContext context("context", frameSize, calibration);
  printf("ARDrawingContext created\n");
  //emscripten::val patternImage;

  auto u8 = loadImage("data/pinball.jpg", 640, 480);

  //auto u8 = emscripten::convertJSArrayToNumberVector<uint8_t>(patternImage);
  printf("patternImage converted\n");
  cv::Mat grayImage(height, width, CV_8UC1);
  cv::Mat patternImageMat = cv::Mat(height, width, CV_8UC4, u8);
  cv::cvtColor(patternImageMat, grayImage, cv::COLOR_RGBA2GRAY);
  ARPipeline pipeline(grayImage, calibration);

  printf("pipeline ok\n");

  loop = [&] { context.updateWindow(); };

  emscripten_set_main_loop(main_loop, 0, true);
}