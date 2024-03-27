importScripts("../dist/MarkerlessAR.js");

var markerless;
var cc, pipeline, projMat;
var pinball_buff;
var stream = null;
var loc, corners, videoWidth, videoHeight;
var vWidth, vHeight;

self.onmessage = function (e) {
    var msg = e.data;
    switch (msg.type) {
      case "pinball": {
        pinball_buff = msg.buff;
        initTracker(pinball_buff, msg);
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

function initTracker(buff, msg) {
    function onLoad(m) {
        markerless = m;
        console.log('Starting the pipeline setup...');
        vWidth = msg.vSizeWidth;
        vHeight = msg.vSizeHeight;
        let patternWidth = msg.width;
        let patternHeight = msg.height;
        cc = new m.CameraCalibration(vWidth, vHeight, vWidth/2, vHeight/2);
        console.log(cc);
        projMat = m.buildProjectionMatrix(cc, 0.01, 1000.0, vWidth/2, vHeight/2);
        console.log(projMat);
        if(projMat) {
            self.postMessage({type: "projection_matrix", projMat: JSON.stringify(projMat)});
        }
        console.log('loading the pinball image...');
        if(buff) {
            pipeline = new m.ARPipeline(patternWidth, patternHeight, buff, cc);
            pipeline.refineHomography(true);
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

        ok = pipeline.processFrame(stream.data, vWidth, vHeight);

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