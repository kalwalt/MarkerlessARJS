#! /bin/bash

#docker run -dit --name emscripten-markerlessarjs -v $(pwd):/src emscripten/emsdk:3.1.26 bash

docker exec emscripten-markerlessarjs emcmake cmake -B build_em -S ./ -DEMSCRIPTEN_COMP=1 -DHTML_LIB=0 -DCMAKE_BUILD_TYPE=Release ..
docker exec emscripten-markerlessarjs emmake make -C build_em

cp build_em/MarkerlessARJS.html build/lib
cp build_em/MarkerlessARJS.js build/lib
cp build_em/MarkerlessARJS_ES6.js build/lib