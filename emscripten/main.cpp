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

    EM_ASM(
            var video = document.getElementById("video");
            console.warn(video);
    if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
        var hint = {
        };
        hint['audio'] = false;
        hint['video'] = true;
        console.log(hint);
        if( window.innerWidth < 800 ) {
            var width = ( window.innerWidth < window.innerHeight ) ? 240 : 360;
            var height = ( window.innerWidth < window.innerHeight ) ? 360 : 240;

            var aspectRatio = window.innerWidth / window.innerHeight;

            console.log( width, height );

            //hint['video']['facingMode'] = 'environment';
            hint['video'] = { facingMode: 'environment'};
            /*hint['video'] = {
                    width: {
                min: width,
                max: width
            }
            };*/

            console.log( hint );
        }

        navigator.mediaDevices.getUserMedia( hint ).then( function( stream ) {
            video.srcObject = stream;
            video.addEventListener( 'loadedmetadata', function() {
                video.play();

                console.log( 'video', video, video.videoWidth, video.videoHeight );

            } );
        } );
    }
    );

    // Main loop

    loop = [&] {
        context.emscriptenMainLoopCallback();
    };

    emscripten_set_main_loop(main_loop, 0, 1);

    context.destroyWindow();

    return 0;
}