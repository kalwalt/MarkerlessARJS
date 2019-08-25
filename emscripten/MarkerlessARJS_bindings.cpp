#include <emscripten/bind.h>
#include "MarkerlessARJS.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(constant_bindings) {

  function("processVideo", &processVideo);
  function("processSingleImage", &processSingleImage);
  function("processFrame", processFrame);

}
