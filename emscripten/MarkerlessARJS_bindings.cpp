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
    cv::Mat patternImageMat(height, width, CV_8UC4, u8.data());
    m_pipeline = ARPipeline(patternImageMat, calibration);
  }

  bool processFrame(const emscripten::val inputFrame, size_t width, size_t height) {
     auto u8 = emscripten::vecFromJSArray<uint8_t>(inputFrame);
    cv::Mat inputFrameMat(height, width, CV_8UC4, u8.data());
    return m_pipeline.processFrame(inputFrameMat);
  }

  emscripten::val getPatternMat44() const {
    Transformation transformation = m_pipeline.getPatternLocation();
    Matrix44 mat44 = transformation.getMat44();
    return emscripten::val{emscripten::typed_memory_view(16, &mat44.data[0])};
  }

  emscripten::val getHomography() const {
    cv::Mat h = m_pipeline.getHomography();
    emscripten::val homography = emscripten::val::array();
    for (auto i = 0; i < 9; i++) {
        homography.call<void>("push", h.data[i]);
    }
    return homography;
  }

  emscripten::val getCorners() const {
    std::vector<cv::Point2f> corners = m_pipeline.getCorners();
    emscripten::val cornersArray = emscripten::val::array();
    for (auto i = 0; i < corners.size(); i++) {
        emscripten::val point = emscripten::val::array();
        point.call<void>("push", corners[i].x);
        point.call<void>("push", corners[i].y);
        cornersArray.call<void>("push", point);
    }
    return cornersArray;
  }

  ARPipeline m_pipeline;
};

emscripten::val buildProjectionMatrix(const CameraCalibration& calibration, float nearPlane, float farPlane, int screen_width, int screen_height)
{
  Matrix44 projectionMatrix;

  // Camera parameters
  float f_x = calibration.fx(); // Focal length in x axis
  float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
  float c_x = calibration.cx(); // Camera primary point x
  float c_y = calibration.cy(); // Camera primary point y

  projectionMatrix.data[0] = -2.0f * f_x / screen_width;
  projectionMatrix.data[1] = 0.0f;
  projectionMatrix.data[2] = 0.0f;
  projectionMatrix.data[3] = 0.0f;

  projectionMatrix.data[4] = 0.0f;
  projectionMatrix.data[5] = 2.0f * f_y / screen_height;
  projectionMatrix.data[6] = 0.0f;
  projectionMatrix.data[7] = 0.0f;

  projectionMatrix.data[8] = 2.0f * c_x / screen_width - 1.0f;
  projectionMatrix.data[9] = 2.0f * c_y / screen_height - 1.0f;    
  projectionMatrix.data[10] = -( farPlane + nearPlane) / ( farPlane - nearPlane );
  projectionMatrix.data[11] = -1.0f;

  projectionMatrix.data[12] = 0.0f;
  projectionMatrix.data[13] = 0.0f;
  projectionMatrix.data[14] = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );        
  projectionMatrix.data[15] = 0.0f;

  emscripten::val arr = emscripten::val::array();
  for (auto i = 0; i < 16; i++) {
     arr.call<void>("push",  projectionMatrix.data[i]);
  }
  return arr;
}



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
 .function("getPatternMat44", &ARPipeline_em::getPatternMat44)
 //.function("getHomography", &ARPipeline_em::getHomography)
 .function("getCorners", &ARPipeline_em::getCorners);

  class_<Transformation_em>("Transformation")
  .constructor<>()
  .constructor<const Matrix33&, const Vector3&>()
  .function("getMat44", &Transformation_em::getMat44);
  //.function("getInverted", &Transformation_em::getInverted);

  function("buildProjectionMatrix", &buildProjectionMatrix);
}
