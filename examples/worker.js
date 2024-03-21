//importScripts("../build/lib/MarkerlessARJS.js");
import Module from "../build/lib/MarkerlessARJS_ES6.js"

const markerless = await Module();

var pinball_buff;
var stream, loc, corners, videoWidth, videoHeight;

self.onmessage = function (ev) {
    var msg = ev.data;
    if (msg.type === "pinball") {
        pinball_buff = msg.buff;
    }
    if (msg.type === "video_data") {
        stream = msg.data;
        videoWidth = msg.videoWidth;
        videoHeight = msg.videoHeight;
    }
}

// Create a new instance of the MarkerlessARJS class
//Module.onRuntimeInitialized = async function () {
    var cc = new markerless.CameraCalibration(1280, 720, 640, 360);
    console.log(cc);
    var projMat = markerless.buildProjectionMatrix(cc, 0.01, 1000.0, 640, 480);
    console.log(projMat);
    if(projMat) {
        self.postMessage({type: "projection_matrix", projMat: JSON.stringify(projMat)});
    }
    var pipeline;
    if(pinball_buff) {
        pipeline = new markerless.ARPipeline(1637, 2048, pinball_buff, cc);
        console.log(pipeline);
        update()
    }

    function update() {
        var ok;
        if (pipeline && pipeline.processFrame) {    
            console.log(stream);
            ok = pipeline.processFrame(stream, 320, 240);
            console.log(ok);
        }
        //var ok = pipeline.processFrame(stream, videoWidth, videoHeight);
        //console.log(ok);
        loc = pipeline.getPatternMat44();
        corners = pipeline.getCorners();
        if(ok) {
            self.postMessage({ type: "pattern_found", matrix: JSON.stringify(loc), corners: JSON.stringify(corners) });
        } else {
            self.postMessage({ type: "pattern_lost" });
        }
       // self.requestAnimationFrame(update);

    }
   //update();
//}// onRuntimeinit

/*self.onmessage = function (ev) {
    var msg = ev.data;
    if (msg.type === "pinball") {
        pinball_buff = msg.buff;
    }
    if (msg.type === "video_data") {
        stream = msg.data;
        videoWidth = msg.videoWidth;
        videoHeight = msg.videoHeight;
    }
}*/