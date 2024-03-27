# MarkerlessARJS
Markerless AR based on https://github.com/ahmetozlu/augmented_reality and https://github.com/MasteringOpenCV/code
This is a project to port the original code to javascript using emscripten.
These are the classes ported to JavaScript:
- CameraCalibration
- ARPipeline
- Transformation
- and other useful functions...

## Example

Two examples are provided in the *examples* folder. You need to run the examples with an http server. The `MarkerlessARJS_basic_example.html` will display  in the browser a sphere and a blue rectangle around it; look also in the console with some useful information.
Instead the `MarkerlessARJS_webcam_example.html` will display a sphere onto the pinball image (included in the project). 
Note that you need to rebuild the whole project with Emscripten only if you modify the C++ code and after build the dist library with the command:
`npm run build-es6`

## Dependencies

- OpenCV 4.7.0
- OpenCV_contrib 4.7.0
- Emscripten emsdk 3.1.26

## Next features

- [x] js library wih all the classes and functions from the project as a separate bundle.
- [x] ES6 version of the library.
- [x] 3d rendering example with three.js library. (partial working example...)
