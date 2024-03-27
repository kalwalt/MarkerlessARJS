#! /bin/bash

#docker run -dit --name emscripten-markerlessarjs -v $(pwd):/src emscripten/emsdk:3.1.26 bash

cd build_em

emcmake cmake -G "Ninja" -DEMSCRIPTEN_COMP=1 -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

copy MarkerlessARJS.js ..\build\lib
copy MarkerlessARJS_ES6.js ..\build\lib