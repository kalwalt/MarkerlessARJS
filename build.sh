#! /bin/bash

# Get our location.
OURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

EM_FLAGS="-O3 --llvm-lto 1 --bind -s ASSERTIONS=0 --memory-init-file 0 -s INVOKE_RUN=0 -s NO_EXIT_RUNTIME=1"
EM_TOOLCHAIN="${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake"
OPENCV_INTRINSICS="-DCV_ENABLE_INTRINSICS=OFF -DCPU_BASELINE="" -DCPU_DISPATCH="""
OPENCV_MODULES_EXCLUDE="-DBUILD_opencv_dnn=OFF -DBUILD_opencv_ml=OFF -DBUILD_opencv_objdetect=OFF -DBUILD_opencv_photo=OFF -DWITH_IPP=OFF -DBUILD_IPP_IW=OFF -DBUILD_ITT=OFF -DBUILD_opencv_stitching=OFF -DWITH_ITT=OFF -DBUILD_IPP_IW=OFF -DWITH_IPP=OFF -DBUILD_opencv_video=ON -DBUILD_opencv_videoio=ON -DWITH_ITT=OFF -DWITH_TIFF=OFF -DWITH_JASPER=ON"
OPENCV_CONF="${OPENCV_MODULES_EXCLUDE} -DBUILD_PERF_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_opencv_apps=OFF -DBUILD_JPEG=ON -DBUILD_PNG=ON -DBUILD_TESTS=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_opencv_apps=OFF -DWITH_PTHREADS_PF=OFF -DWITH_PNG=ON -DWITH_WEBP=ON -DWITH_JPEG=ON -DCMAKE_BUILD_TYPE=Release"

echo "Building MarkerlessARJS for the web with Emscripten"
echo "Building dependencies"
EM_MRKARJS_FLAGS="-msse -msse2 -msse3 -mssse3 -I$OURDIR/emscripten/depends/opencv-em -I$OURDIR/emscripten/depends/opencv-em/include -I$OURDIR/emscripten/depends/opencv-em/modules/core/include -I$OURDIR/emscripten/depends/opencv-em/modules/highgui/include -I$OURDIR/emscripten/depends/opencv-em/modules/imgcodecs/include -I$OURDIR/emscripten/depends/opencv-em/modules/videoio/include -I$OURDIR/emscripten/depends/opencv-em/modules/imgproc/include -I$OURDIR/emscripten/depends/opencv-em/modules/calib3d/include -I$OURDIR/emscripten/depends/opencv-em/modules/features2d/include -I$OURDIR/emscripten/depends/opencv-em/modules/flann/include -I$OURDIR/emscripten/depends/opencv-em/modules/video/include -I$OURDIR/emscripten -I$OURDIR/emscripten/include -I$OURDIR/emscripten/depends/opencv-em/build_opencv_em_ui"
cd emscripten/depends/
if [ ! -d "build_opencv-em" ] ; then
  mkdir build_opencv-em
fi

cd $OURDIR
echo "Building MarkerlessARJS"
if [ ! -d "build-em" ] ; then
    mkdir build-em
fi
cd build-em
rm -f CMakeCache.txt
rm -rf ./MarkerlessAR.js
emconfigure cmake .. -DCMAKE_BUILD_TYPE=${DEBUG+Debug}${DEBUG-Release} -DCMAKE_CXX_FLAGS="$EM_FLAGS $EM_MRKARJS_FLAGS" -DCMAKE_C_FLAGS="$EM_FLAGS $EM_MRKARJS_FLAGS" -DCMAKE_C_FLAGS_RELEASE="-DNDEBUG -O3" -DCMAKE_CXX_FLAGS_RELEASE="-DNDEBUG -O3" -DCMAKE_EXE_LINKER_FLAGS_RELEASE="-O3"

if [ "${DEBUG+Debug}${DEBUG-Release}" = "Debug" ]; then
    emmake make VERBOSE=1
else
    emmake make VERBOSE=1
fi
cd $OURDIR; cp build-em/MarkerlessARJS.js build/lib/MarkerlessARJS.js; cp build-em/MarkerlessARJS.wasm build/lib/MarkerlessARJS.wasm
