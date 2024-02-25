cd build_em
del MarkerlessARJS.html

docker exec emscripten-markerlessarjs emcmake cmake -B build_em -S ./ -DEMSCRIPTEN_COMP=1 -DHTML_LIB=0 -DCMAKE_BUILD_TYPE=Release ..
docker exec emscripten-markerlessarjs emmake make -C build_em

copy MarkerlessARJS.html ..\build\lib