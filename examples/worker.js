importScripts("../dist/MarkerlessAR.js");

console.log(MarkerlessAR);
const m = new MarkerlessAR.default.MarkerlessAR();
console.log(m);
//const markerless = await MarkerlessAR.default.MarkerlessAR.init();
//console.log(markerless);
/*MarkerlessAR.default.MarkerlessAR.init().then((markerless) => {
    console.log(markerless);
})*/
var markerless;
var cc, pipeline, projMat;
var pinball_buff;
var stream = null;
var loc, corners, videoWidth, videoHeight;

/*self.onmessage = function (ev) {
    var msg = ev.data;
    if (msg.type === "pinball") {
        pinball_buff = msg.buff;
        load(pinball_buff);
    }
    if (msg.type === "video_data") {
        stream = msg.data;
        videoWidth = msg.videoWidth;
        videoHeight = msg.videoHeight;
        update();
    }
    if (msg.type === "test") {
        console.log("test");
    }
}*/

/*MarkerlessAR.default.MarkerlessAR.init().then((markerless) => {
    console.log(markerless);
    cc = new markerless.CameraCalibration(1280, 720, 640, 360);
    console.log(cc);
})
*/
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
        cc = new m.CameraCalibration(1280, 720, 640, 360);
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