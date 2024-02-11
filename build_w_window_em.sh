#! /bin/bash

#docker run -dit --name emscripten-markerlessarjs -v $(pwd):/src emscripten/emsdk:3.1.26 bash

cd build_em
mkdir data
cd ..
cp data/igl.html build_em/data
cp data/pinball.jpg build_em/data

emcmake cmake -G "Ninja" -DEMSCRIPTEN_COMP=1 -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

copy MarkerlessARJS.html ../build/lib