#! /bin/bash

# Get our location.
OURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

EM_FLAGS="-Oz --llvm-lto 1 --bind -s ASSERTIONS=0 --memory-init-file 0 -s INVOKE_RUN=0 -s NO_EXIT_RUNTIME=1"
EM_TOOLCHAIN="${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake"
OPENCV_INTRINSICS="-DCV_ENABLE_INTRINSICS:BOOL=0 -DCPU_BASELINE="" -DCPU_DISPATCH="""
OPENCV_MODULES_EXCLUDE="-DBUILD_opencv_dnn=0 -DBUILD_opencv_ml=0 -DBUILD_opencv_objdetect=0 -DBUILD_opencv_photo=0 -DWITH_IPP:BOOL=0 -DBUILD_IPP_IW:BOOL=0 -DBUILD_ITT:BOOL=0 -DBUILD_opencv_stitching=0 -DWITH_ITT:BOOL=0 -DBUILD_IPP_IW:BOOL=0 -DWITH_IPP:BOOL=0 -DBUILD_opencv_video:BOOL=1 -DBUILD_opencv_videoio:BOOL=1 -DWITH_ITT:BOOL=0 -DWITH_TIFF=0 -DWITH_JASPER=0"
OPENCV_CONF="${OPENCV_MODULES_EXCLUDE} -DBUILD_PERF_TESTS:BOOL=0 -DBUILD_SHARED_LIBS:BOOL=0 -DBUILD_opencv_apps:BOOL=0 -DBUILD_JPEG:BOOL=1 -DBUILD_PNG=1 -DBUILD_TESTS:BOOL=0 -DBUILD_opencv_python_tests:BOOL=0 -DBUILD_opencv_apps:BOOL=0 -DWITH_PTHREADS_PF=0 -DWITH_PNG=1 -DWITH_WEBP=1 -DWITH_JPEG:BOOL=1 -DCMAKE_BUILD_TYPE=Release"

echo "Building MarkerlessARJS for the web with Emscripten"
echo "Building dependencies"
EM_MRKARJS_FLAGS="-msse -msse2 -msse3 -mssse3 -I$OURDIR/emscripten/depends/opencv-em -I$OURDIR/emscripten/depends/opencv-em/include -I$OURDIR/emscripten/depends/opencv-em/modules/core/include -I$OURDIR/emscripten/depends/opencv-em/modules/highgui/include -I$OURDIR/emscripten/depends/opencv-em/modules/imgcodecs/include -I$OURDIR/emscripten/depends/opencv-em/modules/videoio/include -I$OURDIR/emscripten/depends/opencv-em/modules/imgproc/include -I$OURDIR/emscripten/depends/opencv-em/modules/calib3d/include -I$OURDIR/emscripten/depends/opencv-em/modules/features2d/include -I$OURDIR/emscripten/depends/opencv-em/modules/flann/include -I$OURDIR/emscripten/depends/opencv-em/modules/video/include -I$OURDIR/emscripten -I$OURDIR/emscripten/include -I$OURDIR/emscripten/depends/opencv-em/build_opencv_em_ui"
cd emscripten/depends/
if [ ! -d "build_opencv-em" ] ; then
  mkdir build_opencv-em
fi
cd build_opencv-em
cmake ../opencv-em -GNinja -DCMAKE_TOOLCHAIN_FILE=$EM_TOOLCHAIN $OPENCV_CONF $OPENCV_INTRINSICS -DCMAKE_CXX_FLAGS="$EM_FLAGS" -DCMAKE_C_FLAGS="$EM_FLAGS" -DCMAKE_C_FLAGS_RELEASE="-DNDEBUG -O3" -DCMAKE_CXX_FLAGS_RELEASE="-DNDEBUG -O3"
ninja -j 1 -v
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
cd $OURDIR; make install
