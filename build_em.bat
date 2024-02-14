docker exec emscripten-markerlessarjs emcmake cmake -B build_em -S ./ -DEMSCRIPTEN_COMP=1 -DCMAKE_BUILD_TYPE=Release ..
docker exec emscripten-markerlessarjs emmake make -C build_em

cd build_em
copy MarkerlessARJS.html ..\build\lib