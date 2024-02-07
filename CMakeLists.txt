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
        VERSION 1.0.0
        LANGUAGES CXX C
)

include(FetchContent)

if (${EMSCRIPTEN_COMP} EQUAL 1)
    message("Fetching opencv for emscripten compilation from webarkit/opencv-em ...")
    FetchContent_Declare(
            build_opencv
            URL https://github.com/webarkit/opencv-em/releases/download/0.1.3/opencv-js-4.7.0-emcc-3.1.26.zip
    )
else ()
    message("Fetching opencv from webarkit/opencv-em ...")
    FetchContent_Declare(
            build_opencv
            URL https://github.com/webarkit/opencv-em/releases/download/0.1.3/opencv-4.7.0.zip
    )
endif ()

FetchContent_MakeAvailable(build_opencv)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build" CACHE PATH "default install path" FORCE)
endif ()

add_subdirectory(emscripten/Markerless)

add_subdirectory(deps/igl)

add_executable(
        MarkerlessARJS
        ${PROJECT_SOURCE_DIR}/emscripten/Common.cpp
        ${PROJECT_SOURCE_DIR}/emscripten/main.cpp
)

igl_set_cxxstd(MarkerlessARJS 17)
igl_set_folder(MarkerlessARJS MarkerlessARJS)

target_include_directories(
        MarkerlessARJS
        PRIVATE
        "${PROJECT_SOURCE_DIR}/emscripten"
        "${PROJECT_SOURCE_DIR}/emscripten/Markerless/include"
        "${PROJECT_SOURCE_DIR}/emscripten/Markerless/stb_image"
        "${PROJECT_SOURCE_DIR}/deps/igl/src"
        "${build_opencv_SOURCE_DIR}"
        "${build_opencv_SOURCE_DIR}/3rdparty/include/libjpeg"
        "${build_opencv_SOURCE_DIR}/libs/opencv/include"
        "${build_opencv_SOURCE_DIR}/libs/opencv/modules/calib3d/include"
        "${build_opencv_SOURCE_DIR}/libs/opencv/modules/core/include"
        "${build_opencv_SOURCE_DIR}/libs/opencv/modules/features2d/include"
        "${build_opencv_SOURCE_DIR}/libs/opencv/modules/flann/include"
        "${build_opencv_SOURCE_DIR}/libs/opencv/modules/imgproc/include"
        "${build_opencv_SOURCE_DIR}/libs/opencv/modules/video/include"
        "${build_opencv_SOURCE_DIR}/libs/opencv_contrib/modules/xfeatures2d/include"
)

target_link_libraries(
        MarkerlessARJS
        PUBLIC
        MARKERLESS
        IGLLibrary
        "${build_opencv_SOURCE_DIR}/lib/libopencv_calib3d.a"
        "${build_opencv_SOURCE_DIR}/lib/libopencv_features2d.a"
        "${build_opencv_SOURCE_DIR}/lib/libopencv_flann.a"
        "${build_opencv_SOURCE_DIR}/lib/libopencv_video.a"
        "${build_opencv_SOURCE_DIR}/lib/libopencv_xfeatures2d.a"
        "${build_opencv_SOURCE_DIR}/lib/libopencv_imgproc.a"
        "${build_opencv_SOURCE_DIR}/lib/libopencv_core.a"
        "${build_opencv_SOURCE_DIR}/3rdparty/lib/libzlib.a"
)

set(MEMORY " -s ALLOW_MEMORY_GROWTH=1 ")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    # https://github.com/kripken/emscripten/blob/master/src/settings.js#L85
    # set(TOTAL_STACK "-s TOTAL_STACK=5*1024*1024)"
    # set(DEBUG_EMCC "-g4 -s SAFE_HEAP=1 -s ASSERTIONS=1 --js-opts 0 -s DEMANGLE_SUPPORT=0" )
    set(OPTIMIZE_EMCC " -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=0 -g3 -O0 ")
    # set(NO_WASM "-s WASM=0")
else ()
    set(OPTIMIZE_EMCC " -Oz ")
    set(NO_WASM " -s WASM=1 ")
    set(SINGLE_FILE " -s SINGLE_FILE=1 -s LLD_REPORT_UNDEFINED ")
    set(USE_GLFW " -s USE_WEBGL2=1 -s USE_GLFW=3 -s GL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=1 -s GL_EMULATE_GLES_VERSION_STRING_FORMAT=1 ")
    # data/pinball.jpg should be inside build_em folder...
    set(EMBED_FILE " --embed-file data/pinball.jpg")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

endif ()

set_target_properties(MarkerlessARJS PROPERTIES LINK_FLAGS "${OPTIMIZE_OPTIONS} ${MODULE_OPTIONS} ${NO_WASM} ${SINGLE_FILE} ${USE_GLFW} ${MEMORY} ${OPTIMIZE_EMCC} ${EMBED_FILE} --shell-file data/igl.html ")
