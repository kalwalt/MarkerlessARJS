#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <ARPipeline.hpp>
#include <CameraCalibration.hpp>
#include <GeometryTypes.hpp>
#include <opencv2/core.hpp>
//#include "MarkerlessARJS.h"

using namespace emscripten;

class Matrix44_em {
public:
  emscripten::val getData() const {
   return emscripten::val{emscripten::typed_memory_view(16, &m_matrix.data[0])};
  };

  void setData(const emscripten::val data) {
    auto f = emscripten::vecFromJSArray<float>(data);
    for (int i = 0; i < 16; i++) {
      m_matrix.data[i] = f[i];
    };
  }

  Matrix44 getTransposed() const {
    return m_matrix.getTransposed();
  }

  Matrix44 getInvertedRT() const {
    return m_matrix.getInvertedRT();
  }

  static Matrix44 identity() {
    return Matrix44::identity();
  }

  Matrix44 m_matrix;
};

class Transformation_em {
public:
  Transformation_em() {
    m_transformation = Transformation();
  }

  Transformation_em(const Matrix33& rotation, const Vector3& translation) {
    m_transformation = Transformation(rotation, translation);
  }

  emscripten::val getMat44() const {
    Matrix44 mat44 = m_transformation.getMat44();
    return emscripten::val{emscripten::typed_memory_view(16, &mat44.data[0])};
  }

  Transformation m_transformation;
};

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

  class_<Matrix44_em>("Matrix44")
  .property("data", &Matrix44_em::getData, &Matrix44_em::setData)
  .function("getTransposed", &Matrix44_em::getTransposed)
  .function("getInvertedRT", &Matrix44_em::getInvertedRT)
  .class_function("identity", &Matrix44_em::identity);

  class_<CameraCalibration>("CameraCalibration")
  .constructor<float, float, float, float>()
  .constructor<float, float, float, float, float*>();

  class_<ARPipeline_em>("ARPipeline")
 .constructor<size_t, size_t, const emscripten::val, CameraCalibration>()
 .function("processFrame", &ARPipeline_em::processFrame)
 .function("getPatternLocation", &ARPipeline_em::getPatternLocation);

  class_<Transformation_em>("Transformation")
  .constructor<>()
  .constructor<const Matrix33&, const Vector3&>()
  .function("getMat44", &Transformation_em::getMat44);
  //.function("getInverted", &Transformation_em::getInverted);
}
