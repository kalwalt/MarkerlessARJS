#include <emscripten/bind.h>
//#include "MarkerlessARJS.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(constant_bindings) {

  class_<MarkerlessARJS>("MarkerlessARJS")
  .constructor()
  .function("setupCamera", &MarkerlessARJS::setupCamera)
  .function("initGL", &MarkerlessARJS::initGL)
  .function("processPatternImage", &MarkerlessARJS::processPatternImage)
  .function("processVideoFrame", &MarkerlessARJS::processVideoFrame)
  .function("getPatternPose", &MarkerlessARJS::getPatternPose);
}
