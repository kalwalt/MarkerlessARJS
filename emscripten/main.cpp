//
// Created by @kalwalt on 06/12/2023.
//

#include "ARDrawingContextIGL.hpp"

#include <ARPipeline.hpp>
#include <CameraCalibration.hpp>
#include <GeometryTypes.hpp>
#include <Pattern.hpp>
#include <PatternDetector.hpp>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <opencv2/core.hpp>

#include <string>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

std::function<void()> loop;

void main_loop() { loop(); }

//! Load an image from file
uint8_t *loadImage(std::string filename, int width, int height) {
    int comp;

    const uint8_t *img = stbi_load(filename.c_str(), &width, &height, &comp, 3);
    return (uint8_t *) img;
}

EM_JS(int, get_video_stream, (), {
var video = document.getElementById('video');
var canvas_process = document.createElement('canvas');
var context_process = canvas_process.getContext('2d');
context_process.fillStyle = 'black';
(context_process.fillRect(0, 0, 640, 480));
(context_process.drawImage(video, 0, 0, 640, 480));
var imageData = (context_process.getImageData(0, 0, 640, 480));
var pointer = Module._malloc(640 * 480 * 4);

function update() {
    (Module.HEAPU8.set(imageData.data, pointer));
};

window.requestAnimationFrame(update);
return pointer;
})

EM_JS(emscripten::EM_VAL, get_video_stream_js, (), {
var video = document.getElementById('video');
var canvas_process = document.createElement('canvas');
canvas_process.id = 'canvas_process';
var context_process = canvas_process.getContext('2d');
context_process.fillStyle = 'black';
(context_process.fillRect(0, 0, 640, 480));
(context_process.drawImage(video, 0, 0, 640, 480));
var imageData = (context_process.getImageData(0, 0, 640, 480));
return Emval.toHandle(imageData.data);
})

cv::Mat convertVideo2Gray(emscripten::val data_buffer, size_t width, size_t height) {
    auto u8 = emscripten::convertJSArrayToNumberVector<uint8_t>(data_buffer);
    cv::Mat grayVideo(height, width, CV_8UC1);
    cv::Mat videoMat = cv::Mat(height, width, CV_8UC4, u8.data());
    cv::cvtColor(videoMat, grayVideo, cv::COLOR_RGBA2GRAY);
    return videoMat;
}

EM_JS(void, displayGrayVideo, (emscripten::EM_VAL video, size_t width, size_t height), {
    var canvas = document.getElementById('canvasGray');
    var context = canvas.getContext('2d');
    var imageData = (new ImageData(new Uint8ClampedArray(Emval.toValue(video)), width, height));
    (context.putImageData(imageData, 0, 0));
})

int main(int argc, char *argv[]) {
    // Change this calibration to yours:
    CameraCalibration calibration(526.58037684199849f, 524.65577209994706f, 318.41744018680112f, 202.96659047014398f);
    int width = 640;
    int height = 480;
    cv::Size frameSize(width, height);

    ARDrawingContextIGL context("example", frameSize, calibration);

    context.initialize();

    printf("webGL initialized\n");

    auto u8 = loadImage("data/pinball.jpg", 640, 480);

    printf("patternImage converted\n");

    cv::Mat grayImage(height, width, CV_8UC1);
    cv::Mat patternImageMat = cv::Mat(height, width, CV_8UC4, u8);
    cv::cvtColor(patternImageMat, grayImage, cv::COLOR_RGBA2GRAY);
    ARPipeline pipeline(grayImage, calibration);

    printf("pipeline ok\n");

    context.initVideoStream();

    auto pointer = get_video_stream();
    printf("pointer: %d\n", pointer);

    // Main loop

    loop = [&] {
        context.emscriptenMainLoopCallback();

        emscripten::val video = emscripten::val::take_ownership(get_video_stream_js());

        displayGrayVideo(video.as_handle(), 640, 480);

        cv::Mat cameraFrame = convertVideo2Gray(video, 640, 480);

        // Find a pattern and update it's detection status:
        context.isPatternPresent = pipeline.processFrame(cameraFrame);

        if(context.isPatternPresent) {
            printf("pattern detected!\n");
        } else {
            printf("pattern not detected...\n");
        };

        // Update a pattern pose:
        context.patternPose = pipeline.getPatternLocation();
    };

    emscripten_set_main_loop(main_loop, 0, 1);

    context.destroyWindow();

    return 0;
}