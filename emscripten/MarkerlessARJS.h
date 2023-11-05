#include <ARPipeline.hpp>
#include <CameraCalibration.hpp>
#include <GeometryTypes.hpp>
#include <Pattern.hpp>
#include <PatternDetector.hpp>

#include <emscripten.h>
#include <opencv2/core.hpp>
//#include <opencv2/videoio.hpp>
#include <string>

class MarkerlessARJS {
public:
  MarkerlessARJS();
  void setupCamera(float _fx, float _fy, float _cx, float _cy);
  void processPatternImage(unsigned char *patternImage);
  void processVideoFrame(unsigned char *frameImage);

private:
  ARPipeline m_pipeline;
  CameraCalibration m_calibration;
};