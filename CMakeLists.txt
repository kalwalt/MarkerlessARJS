#
# MarkerlessARJS CMake configuration file.
#
# To invoke CMake on this file, from the chosen build directory, invoke: cmake
# <path to directory containing this file>
#
# Copyright 2019, @kalwalt. Author(s): Walter Perdan <info@kalwaltart.it>
#

cmake_minimum_required(VERSION 3.15)

# Define project.
project(
  MarkerlessARJS
  VERSION 0.2.0
  LANGUAGES CXX C)

include(FetchContent)

if(${EMSCRIPTEN_COMP} EQUAL 1)
  message(
    "Fetching opencv for emscripten compilation from webarkit/opencv-em ...")
  FetchContent_Declare(
    build_opencv
    URL https://github.com/webarkit/opencv-em/releases/download/0.1.5/opencv-js-4.7.0-emcc-3.1.26-simd.zip
  )
else()
  message("Fetching opencv from webarkit/opencv-em ...")
  FetchContent_Declare(
    build_opencv
    URL https://github.com/webarkit/opencv-em/releases/download/0.1.5/opencv-4.7.0.zip
  )
endif()

FetchContent_MakeAvailable(build_opencv)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_SOURCE_DIR}/build"
      CACHE PATH "default install path" FORCE)
endif()

add_subdirectory(emscripten/Markerless)

add_definitions("-std=c++17")

set(HTML_LIB true)

macro(ADD_LIB app ES6)

  add_executable(${app}
                 ${PROJECT_SOURCE_DIR}/emscripten/MarkerlessARJS_bindings.cpp)

  target_include_directories(
    ${app}
    PRIVATE
      "${PROJECT_SOURCE_DIR}/emscripten/Markerless/include"
      # "${PROJECT_SOURCE_DIR}/emscripten/Markerless/stb_image"
      "${build_opencv_SOURCE_DIR}"
      "${build_opencv_SOURCE_DIR}/3rdparty/include/libjpeg"
      "${build_opencv_SOURCE_DIR}/libs/opencv/include"
      "${build_opencv_SOURCE_DIR}/libs/opencv/modules/calib3d/include"
      "${build_opencv_SOURCE_DIR}/libs/opencv/modules/core/include"
      "${build_opencv_SOURCE_DIR}/libs/opencv/modules/features2d/include"
      "${build_opencv_SOURCE_DIR}/libs/opencv/modules/flann/include"
      # "${build_opencv_SOURCE_DIR}/libs/opencv/modules/imgcodecs/include"
      "${build_opencv_SOURCE_DIR}/libs/opencv/modules/imgproc/include"
      "${build_opencv_SOURCE_DIR}/libs/opencv/modules/video/include"
      "${build_opencv_SOURCE_DIR}/libs/opencv_contrib/modules/xfeatures2d/include"
  )

  target_link_libraries(
    ${app}
    MARKERLESS
    "${build_opencv_SOURCE_DIR}/lib/libopencv_calib3d.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_features2d.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_flann.a"
    # "${build_opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_video.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_xfeatures2d.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_imgproc.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_core.a"
    # "${build_opencv_SOURCE_DIR}/3rdparty/lib/liblibopenjp2.a"
    "${build_opencv_SOURCE_DIR}/3rdparty/lib/libzlib.a")

  # set(EXPORTED_RUNTIME_FUNCTIONS "'ccall', 'cwrap', 'FS', 'setValue'")
  set(MEM 16777216) # This is default value
  set(MEMORY "-s TOTAL_MEMORY=${MEM} -s ALLOW_MEMORY_GROWTH=1 ")
  set(SIMD " -msimd128 ")

  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    # https://github.com/kripken/emscripten/blob/master/src/settings.js#L85
    # set(TOTAL_STACK "-s TOTAL_STACK=5*1024*1024)"
    set(DEBUG_EMCC
        " -g4 -s ASSERTIONS=2 --js-opts 0 -s DEMANGLE_SUPPORT=0 -O0 ")
  else()
    set(OPTIMIZE_EMCC " -Oz ")
    # set(ES6_FLAGS  " -s EXPORT_ES6=1 -s USE_ES6_IMPORT_META=0 -s MODULARIZE=1
    # ")
  endif()

  if(${ES6} STREQUAL 0)
    set(ES6_FLAG " ")
  else()
    set(ES6_FLAG " -s EXPORT_ES6=1 -s USE_ES6_IMPORT_META=0 -s MODULARIZE=1 ")
  endif()

  set(WASM "-s WASM=1")
  set(SINGLE_FILE " -s SINGLE_FILE=1 ")
  set(CMAKE_EXECUTABLE_SUFFIX ".js")

  set_target_properties(
    ${app}
    PROPERTIES
      LINK_FLAGS
      "${OPTIMIZE_OPTIONS} ${MODULE_OPTIONS} ${WASM} ${SIMD} ${ES6_FLAG} ${SINGLE_FILE} ${USE_GLFW} ${MEMORY} ${OPTIMIZE_EMCC} ${EMBED_FILE} --bind "
  )

endmacro()

add_lib(MarkerlessARJS 0)
add_lib(MarkerlessARJS_ES6 1)
