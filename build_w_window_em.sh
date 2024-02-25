#! /bin/bash

#docker run -dit --name emscripten-markerlessarjs -v $(pwd):/src emscripten/emsdk:3.1.26 bash

cd build_em

emcmake cmake -G "Ninja" -DEMSCRIPTEN_COMP=1 -DHTML_LIB=0 -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

copy MarkerlessARJS.html ../build/lib