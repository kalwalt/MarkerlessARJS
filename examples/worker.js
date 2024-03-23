importScripts("../dist/MarkerlessAR.js");

var markerless;
var cc, pipeline, projMat;
var pinball_buff;
var stream = null;
var loc, corners, videoWidth, videoHeight;

self.onmessage = function (e) {
    var msg = e.data;
    switch (msg.type) {
      case "pinball": {
        pinball_buff = msg.buff;
        initTracker(pinball_buff);
        return;
      }
      case "video_data": {
        stream = msg.data;
        videoWidth = msg.videoWidth;
        videoHeight = msg.videoHeight;
        update();
        return;
      }
      case "test": {
        console.log("test");
    }
  };
}

self.addEventListener('message', function(e) {
    self.postMessage(e.data);
    }, false);

function initTracker(buff) {
    function onLoad(m) {
        markerless = m;
        console.log('Starting the pipeline setup...');
        cc = new m.CameraCalibration(640, 480, 320, 240);
        console.log(cc);
        projMat = m.buildProjectionMatrix(cc, 0.01, 1000.0, 640, 480);
        console.log(projMat);
        if(projMat) {
            self.postMessage({type: "projection_matrix", projMat: JSON.stringify(projMat)});
        }
        console.log('loading the pinball image...');
        if(buff) {
            pipeline = new m.ARPipeline(1637, 2048, buff, cc);
            console.log(pipeline);
        }
    }
    
    MarkerlessAR.default.MarkerlessAR.init().then(onLoad).catch((error) => {
        console.error(error);
    })
}

function update() {
    var ok;
    if (pipeline && pipeline.processFrame) {

        ok = pipeline.processFrame(stream.data, 320, 240);

    }
    console.log("marker found: ", ok);

    if (ok) {
        loc = pipeline.getPatternMat44();
        corners = pipeline.getCorners();
        self.postMessage({type: "pattern_found", matrix: JSON.stringify(loc), corners: JSON.stringify(corners)});
    } else {
        self.postMessage({type: "pattern_lost"});
    }
    stream = null;
}