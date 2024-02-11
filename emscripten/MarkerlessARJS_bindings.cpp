#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <ARPipeline.hpp>
#include <CameraCalibration.hpp>
#include <opencv2/core.hpp>
//#include "MarkerlessARJS.h"

using namespace emscripten;

class ARPipeline_em {
public:
  ARPipeline_em() {
    m_pipeline = ARPipeline();
  }

  ARPipeline_em(size_t width, size_t height, const emscripten::val patternImage, const CameraCalibration& calibration) {
    auto u8 = emscripten::vecFromJSArray<uint8_t>(patternImage);
    cv::Mat patternImageMat(width, height, CV_8UC1, u8.data());
    m_pipeline = ARPipeline(patternImageMat, calibration);
  }

  bool processFrame(const emscripten::val inputFrame, size_t width, size_t height) {
     auto u8 = emscripten::vecFromJSArray<uint8_t>(inputFrame);
    cv::Mat inputFrameMat(width, height, CV_8UC1, u8.data());
    return m_pipeline.processFrame(inputFrameMat);
  }

  const Transformation& getPatternLocation() const {
    return m_pipeline.getPatternLocation();
  }

  ARPipeline m_pipeline;
};


EMSCRIPTEN_BINDINGS(constant_bindings) {

  /*class_<MarkerlessARJS>("MarkerlessARJS")
  .constructor()
  .function("setupCamera", &MarkerlessARJS::setupCamera)
  .function("initGL", &MarkerlessARJS::initGL)
  .function("processPatternImage", &MarkerlessARJS::processPatternImage)
  .function("processVideoFrame", &MarkerlessARJS::processVideoFrame)
  .function("getPatternPose", &MarkerlessARJS::getPatternPose);*/

  class_<CameraCalibration>("CameraCalibration")
  .constructor<float, float, float, float>()
  .constructor<float, float, float, float, float*>();

  class_<ARPipeline_em>("ARPipeline")
 .constructor<size_t, size_t, const emscripten::val, CameraCalibration>()
 .function("processFrame", &ARPipeline_em::processFrame)
 .function("getPatternLocation", &ARPipeline_em::getPatternLocation);

}
