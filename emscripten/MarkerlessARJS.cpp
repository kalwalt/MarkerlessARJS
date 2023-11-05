#include "MarkerlessARJS.h"

MarkerlessARJS::MarkerlessARJS(){};

void MarkerlessARJS::setupCamera(float _fx, float _fy, float _cx, float _cy) {
  CameraCalibration calibration(_fx, _fy, _cx, _cy);
  m_calibration = calibration;
};

void processPatternImage(unsigned char *patternImage) {
  ARPipeline pipeline(patternImage, m_calibration);
  m_pipeline = pipeline;
};

void MarkerlessARJS::processVideoFrame(unsigned char *frameImage) {
  ARPipeline pipeline(frameImage, m_calibration);
};

#include "MarkerlessARJS_bindings.cpp"