#
# MarkerlessARJS CMake configuration file.
#
# To invoke CMake on this file, from the chosen build directory, invoke:
#     cmake <path to directory containing this file>
#
# Copyright 2019, @kalwalt.
# Author(s): Walter Perdan <info@kalwaltart.it>
#

cmake_minimum_required( VERSION 3.4 )

# Any directives that must go before the project command.

#if(DEFINED CMAKE_BUILD_TYPE)
#  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
#endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Define project.
project(MarkerlessARJS
        VERSION ${ARX_VERSION_STRING}
        LANGUAGES CXX C
)

set(JPEG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/depends/emscripten/libjpeg/includes)

set(JPEG_LIBRARIES ${PROJECT_SOURCE_DIR}/depends/emscripten/libjpeg/)
message("JPEG_INCLUDE_DIR ${JPEG_LIBRARIES}")
set(HAVE_INTEL_SIMD 0)

# Installation settings.
# If the user didn't define CMAKE_INSTALL_PREFIX, set a local install destination.
# When building packages, this will typically be overridden with DESTDIR (e.g. "make install DESTDIR=/tmp").
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../SDK" CACHE PATH "default install path" FORCE )
endif()

add_subdirectory(emscripten/Markerless)
set(MRK_LIBRARY_STYLE "STATIC")
set(PUBLIC_HEADERS
    emscripten/Markerless/include/ARPipeline.hpp
    emscripten/Markerless/include/CameraCalibration.hpp
    emscripten/Markerless/include/DebugHelpers.hpp
    emscripten/Markerless/include/GeometryTypes.hpp
    emscripten/Markerless/include/Pattern.hpp
    emscripten/Markerless/include/PatternDetector.hpp
)

set(SOURCE
    emscripten/Markerless/src/ARPipeline.cpp
    emscripten/Markerless/src/CameraCalibration.cpp
    emscripten/Markerless/src/GeometryTypes.cpp
    emscripten/Markerless/src/Pattern.cpp
    emscripten/Markerless/src/PatternDetector.cpp
)
add_library(MRK ${MRK_LIBRARY_STYLE}
    ${PUBLIC_HEADERS} ${SOURCE}
)
target_include_directories(MARKERLESS
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/Markerless/include
    PRIVATE emscripten/Markerless/include
    #PRIVATE ${JPEG_INCLUDE_DIR}
    PRIVATE ${OpenCV_INCLUDE_DIR}
)

target_link_libraries(MARKERLESS
    INTERFACE ${LIBS}
  #  PRIVATE ${JPEG_LIBRARIES}
)

include_directories(${PROJECT_SOURCE_DIR}/emscripten/Markerless/include)
#include_directories(${PROJECT_SOURCE_DIR}/emscripten/depends)
include_directories(${PROJECT_SOURCE_DIR}/emscripten/include)
#include_directories(${PROJECT_SOURCE_DIR}/emscripten)
include_directories(${PROJECT_SOURCE_DIR}/emscripten/depends/build_opencv-em)
include_directories(${PROJECT_SOURCE_DIR}/emscripten/depends/build_opencv_em/include)
include_directories(${PROJECT_SOURCE_DIR}/emscripten/depends/build_opencv_em/modules/core/include)
include_directories(${PROJECT_SOURCE_DIR}/emscripten/depends/opencv_contrib/modules/xfeatures2d/include)

add_definitions("-std=c++11")
set(OPENCV_EM "${PROJECT_SOURCE_DIR}/emscripten/depends/build_opencv-em")

link_directories(${PROJECT_SOURCE_DIR}/emscripten/src ${PROJECT_SOURCE_DIR}/build-em ${OPENCV_EM}/lib ${OPENCV_EM}/3rdparty/lib)
link_libraries(libopencv_videoio libopencv_video libopencv_features2d libopencv_flann libopencv_imgproc libopencv_core libopencv_imgcodecs libopencv_calib3d libopencv_highgui libMRK libprotobuf libpng libjpeg libwebp IlmImf zlib)
add_executable(MarkerlessARJS ${PROJECT_SOURCE_DIR}/emscripten/MarkerlessARJS_bindings.cpp ${PROJECT_SOURCE_DIR}/emscripten/MarkerlessARJS.cpp)

set(EXPORT_FUNCTIONS "'_fromVideo', '_processVideo', '_processSingleImage', '_setHomographyReprojectionThreshold', '_buildProjectionMatrix'")
set(EXPORTED_RUNTIME_FUNCTIONS "'ccall', 'cwrap', 'FS', 'setValue'")
set(MODULE_OPTIONS "-s MODULARIZE_INSTANCE=1 -s EXPORT_NAME='MarkerlessARJS' -s EXPORT_ES6=1" )

set(MEM 16777216) #This is default value
#message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
set(MEMORY "-s TOTAL_MEMORY=${MEM} -s ALLOW_MEMORY_GROWTH=1")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    # https://github.com/kripken/emscripten/blob/master/src/settings.js#L85
    # set(TOTAL_STACK "-s TOTAL_STACK=5*1024*1024)"
    set(DEBUG_EMCC "-g4 -s SAFE_HEAP=1 -s ASSERTIONS=1 --js-opts 0 -s DEMANGLE_SUPPORT=0" )
    # set(NO_WASM "-s WASM=0")
    set(WASM_LOADER "--pre-js ${PROJECT_SOURCE_DIR}/emscripten/Wasm_Loader.js")
else()
    # set(OPTIMIZE_EMCC "--memory-init-file 0 -s INVOKE_RUN=0 --llvm-lto 1 -s ASSERTIONS=0 -s PRECISE_I64_MATH=0 -s PRECISE_F32=2")
    # set(NO_WASM "-s WASM=0")
    # If WASM add a pre-js to register the wasm loader
    set(WASM_LOADER "--pre-js ${PROJECT_SOURCE_DIR}/emscripten/Wasm_Loader.js")
endif()

# set(OPTIMIZE_OPTIONS "-s NO_EXIT_RUNTIME=1 -s INVOKE_RUN=0")
set_target_properties(MarkerlessARJS PROPERTIES LINK_FLAGS "${OPTIMIZE_OPTIONS} ${MODULE_OPTIONS} ${NO_WASM} ${WASM_LOADER} ${MEMORY} ${DEBUG_EMCC} ${OPTIMIZE_EMCC} --bind -s EXPORTED_FUNCTIONS='[${EXPORT_FUNCTIONS}]' -s EXTRA_EXPORTED_RUNTIME_METHODS='[${EXPORTED_RUNTIME_FUNCTIONS}]'")
