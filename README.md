# MarkerlessARJS
Markerless AR based on https://github.com/ahmetozlu/augmented_reality and https://github.com/MasteringOpenCV/code
This is a project to port the original code to javascript using emscripten.

## Example

A simple example is provided in the *build/lib* folder. Running the example in a http server, you will see in the browser a an image of the pinball image with a blue rectangle around it; look also in the console with some useful information.
This example bundle in the .html file all the wasm code, if you modify the C++ code or the shell.html file you need to rebuild the whole project with Emscripten.

## Dependencies

- OpenCV 4.7.0
- OpenCV_contrib 4.7.0
- Emscripten emsdk 3.1.26

## Next features

- [ ] js library wih all the classes and functions from the project as a separate bundle.
- [ ] ES6 version of the library.
- [ ] 3d rendering example witth three.js library.
