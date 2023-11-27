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

class MarkerlessARJS {
public:
  MarkerlessARJS();
  void setupCamera(float _fx, float _fy, float _cx, float _cy);
  void processPatternImage(emscripten::val patternImage, size_t width, size_t height);
  void processVideoFrame(emscripten::val frameImage, size_t width, size_t height);
  emscripten::val getPatternPose();

private:
  ARPipeline m_pipeline;
  CameraCalibration m_calibration;
};