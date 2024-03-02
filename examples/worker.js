importScripts("../build/lib/MarkerlessARJS.js");

var pinball_buff;
var stream, loc, corners, videoWidth, videoHeight;

// Create a new instance of the MarkerlessARJS class
Module.onRuntimeInitialized = async function () {
    var cc = new Module.CameraCalibration(1280, 720, 640, 360);
    console.log(cc);
    var projMat = Module.buildProjectionMatrix(cc, 0.01, 1000.0, 640, 480);
    console.log(projMat);
    var pipeline = new Module.ARPipeline(1637, 2048, pinball_buff, cc);
    console.log(pipeline);
    function update() {
        var ok = pipeline.processFrame(stream, videoWidth, videoHeight);
        console.log(ok);
        loc = pipeline.getPatternMat44();
        corners = pipeline.getCorners();
        if(ok) {
            self.postMessage({ type: "pattern_found", matrix: JSON.stringify(loc), corners: JSON.stringify(corners) });
        }
        self.requestAnimationFrame(update);

    }
    update();
}

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