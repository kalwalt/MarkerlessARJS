cmake_minimum_required(VERSION 3.22)

project(MARKERLESS)

include(FetchContent)

if(${EMSCRIPTEN_COMP} EQUAL 1)
  message("Fetching opencv for emscripten compilation from webarkit/opencv-em ...")
  FetchContent_Declare(
    build_opencv
    URL https://github.com/webarkit/opencv-em/releases/download/0.1.3/opencv-js-4.7.0-emcc-3.1.26.zip
  )
else()
  message("Fetching opencv from webarkit/opencv-em ...")
  FetchContent_Declare(
    build_opencv
    URL https://github.com/webarkit/opencv-em/releases/download/0.1.3/opencv-4.7.0.zip
  )
endif()

FetchContent_MakeAvailable(build_opencv)


get_filename_component(PARENT_DIR ./ ABSOLUTE)

set(PUBLIC_HEADERS
  include/ARPipeline.hpp
  include/CameraCalibration.hpp
  include/GeometryTypes.hpp
  include/Pattern.hpp
  include/PatternDetector.hpp
)

set(SOURCE
  src/ARPipeline.cpp
  src/CameraCalibration.cpp
  src/GeometryTypes.cpp
  src/Pattern.cpp
  src/PatternDetector.cpp
)

add_library(MARKERLESS STATIC
  ${PUBLIC_HEADERS} ${SOURCE}
)

target_include_directories(
  MARKERLESS
  PRIVATE
  "${PARENT_DIR}/include"
)

target_include_directories(
  MARKERLESS
  PRIVATE
  "${build_opencv_SOURCE_DIR}"
  "${build_opencv_SOURCE_DIR}/3rdparty/include/libjpeg"
  "${build_opencv_SOURCE_DIR}/libs/opencv/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv/modules/calib3d/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv/modules/core/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv/modules/features2d/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv/modules/flann/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv/modules/imgcodecs/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv/modules/imgproc/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv/modules/video/include"
  "${build_opencv_SOURCE_DIR}/libs/opencv_contrib/modules/xfeatures2d/include"
)

target_link_libraries(
  MARKERLESS
  "${build_opencv_SOURCE_DIR}/lib/libopencv_calib3d.a"
  "${build_opencv_SOURCE_DIR}/lib/libopencv_features2d.a"
  "${build_opencv_SOURCE_DIR}/lib/libopencv_flann.a"
  "${build_opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a"
  "${build_opencv_SOURCE_DIR}/lib/libopencv_video.a"
  "${build_opencv_SOURCE_DIR}/lib/libopencv_xfeatures2d.a"
  "${build_opencv_SOURCE_DIR}/lib/libopencv_imgproc.a"
  "${build_opencv_SOURCE_DIR}/lib/libopencv_core.a"
  "${build_opencv_SOURCE_DIR}/3rdparty/lib/liblibopenjp2.a"
  "${build_opencv_SOURCE_DIR}/3rdparty/lib/libzlib.a"
)

target_include_directories(MARKERLESS
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${OpenCV_INCLUDE_DIR}
)

target_link_libraries(MARKERLESS
  INTERFACE ${LIBS}
)

if(NOT ARX_TARGET_PLATFORM_WINDOWS)
  target_link_libraries(MARKERLESS
    PRIVATE ${OpenCV_LIBS}
  )
else()
  set(LINK_LIST "")

  foreach(x ${OpenCV_LIBS})
    string(REPLACE ".lib" "341d.lib" x_debug ${x})
    list(APPEND LINK_LIST debug ${x_debug} optimized ${x})
  endforeach()

  target_link_libraries(MARKERLESS
    PUBLIC ${LINK_LIST}
  )
endif()
