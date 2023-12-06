#! /bin/bash

#docker run -dit --name emscripten-markerlessarjs -v $(pwd):/src emscripten/emsdk:3.1.26 bash

docker exec emscripten-markerlessarjs emcmake cmake -B build_em -S ./ -DEMSCRIPTEN_COMP=1 -DCMAKE_BUILD_TYPE=Release ..
docker exec emscripten-markerlessarjs emmake make -C build_em

cp build_em/MarkerlessARJS.html build/lib