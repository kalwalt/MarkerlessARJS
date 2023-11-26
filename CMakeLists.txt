#
# MarkerlessARJS CMake configuration file.
#
# To invoke CMake on this file, from the chosen build directory, invoke:
# cmake <path to directory containing this file>
#
# Copyright 2019, @kalwalt.
# Author(s): Walter Perdan <info@kalwaltart.it>
#

cmake_minimum_required(VERSION 3.15)

# Define project.
project(MarkerlessARJS
    VERSION ${ARX_VERSION_STRING}
    LANGUAGES CXX C
)

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

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build" CACHE PATH "default install path" FORCE)
endif()

add_subdirectory(emscripten/Markerless)

add_definitions("-std=c++11")
set(OPENCV_EM "${PROJECT_SOURCE_DIR}/emscripten/depends/build_opencv-em")

add_executable(MarkerlessARJS   ${PROJECT_SOURCE_DIR}/emscripten/MarkerlessARJS.cpp)

get_filename_component(DIR ./ ABSOLUTE)

set(MarkerlessARJS
emscripten/MarkerlessARJS.h
emscripten/Markerless/ARPipeline.hpp
emscripten/Markerless/CameraCalibration.hpp
emscripten/Markerless/GeometryTypes.hpp
emscripten/Markerless/Pattern.hpp
emscripten/Markerless/PatternDetector.hpp
)

set(MarkerlessARJS
emscripten/MarkerlessARJS.cpp
emscripten/Markerless/ARPipeline.cpp
emscripten/Markerless/CameraCalibration.cpp
emscripten/Markerless/GeometryTypes.cpp
emscripten/Markerless/Pattern.cpp
emscripten/Markerless/PatternDetector.cpp
)

target_include_directories(
    MarkerlessARJS
    PRIVATE
    "${PROJECT_SOURCE_DIR}/emscripten/Markerless/include"
    "${build_opencv_SOURCE_DIR}"
    "${build_opencv_SOURCE_DIR}/3rdparty/include/libjpeg"
    "${build_opencv_SOURCE_DIR}/libs/opencv/include"
    "${build_opencv_SOURCE_DIR}/libs/opencv/modules/calib3d/include"
    "${build_opencv_SOURCE_DIR}/libs/opencv/modules/core/include"
    "${build_opencv_SOURCE_DIR}/libs/opencv/modules/features2d/include"
    "${build_opencv_SOURCE_DIR}/libs/opencv/modules/flann/include"
    #"${build_opencv_SOURCE_DIR}/libs/opencv/modules/imgcodecs/include"
    "${build_opencv_SOURCE_DIR}/libs/opencv/modules/imgproc/include"
    "${build_opencv_SOURCE_DIR}/libs/opencv/modules/video/include"
    "${build_opencv_SOURCE_DIR}/libs/opencv_contrib/modules/xfeatures2d/include"
)

target_link_libraries(
    MarkerlessARJS
    MARKERLESS
    "${build_opencv_SOURCE_DIR}/lib/libopencv_calib3d.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_features2d.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_flann.a"
    #"${build_opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_video.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_xfeatures2d.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_imgproc.a"
    "${build_opencv_SOURCE_DIR}/lib/libopencv_core.a"
    #"${build_opencv_SOURCE_DIR}/3rdparty/lib/liblibopenjp2.a"
    "${build_opencv_SOURCE_DIR}/3rdparty/lib/libzlib.a"
)

# set(EXPORT_FUNCTIONS "'_fromVideo', '_processVideo', '_processSingleImage', '_setHomographyReprojectionThreshold', '_buildProjectionMatrix'")
# set(EXPORTED_RUNTIME_FUNCTIONS "'ccall', 'cwrap', 'FS', 'setValue'")
set(MEM 16777216) # This is default value
set(MEMORY "-s TOTAL_MEMORY=${MEM} -s ALLOW_MEMORY_GROWTH=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    # https://github.com/kripken/emscripten/blob/master/src/settings.js#L85
    # set(TOTAL_STACK "-s TOTAL_STACK=5*1024*1024)"
    # set(DEBUG_EMCC "-g4 -s SAFE_HEAP=1 -s ASSERTIONS=1 --js-opts 0 -s DEMANGLE_SUPPORT=0" )
    # set(NO_WASM "-s WASM=0")
    set(WASM_LOADER "--pre-js ${PROJECT_SOURCE_DIR}/emscripten/Wasm_Loader.js")
else()
    set(OPTIMIZE_EMCC "--memory-init-file 0 -s INVOKE_RUN=0  -s ASSERTIONS=0 -s PRECISE_F32=2")
    set(NO_WASM "-s WASM=0")

    # If WASM add a pre-js to register the wasm loader
    # set(WASM_LOADER "--pre-js ${PROJECT_SOURCE_DIR}/emscripten/Wasm_Loader.js")
endif()

# set(OPTIMIZE_OPTIONS "-s NO_EXIT_RUNTIME=1 -s INVOKE_RUN=0")
set_target_properties(MarkerlessARJS PROPERTIES LINK_FLAGS  "${OPTIMIZE_OPTIONS} ${MODULE_OPTIONS} ${NO_WASM} ${MEMORY} ${OPTIMIZE_EMCC} --bind -s EXPORTED_FUNCTIONS='[${EXPORT_FUNCTIONS}]' -s EXTRA_EXPORTED_RUNTIME_METHODS='[${EXPORTED_RUNTIME_FUNCTIONS}]'")
