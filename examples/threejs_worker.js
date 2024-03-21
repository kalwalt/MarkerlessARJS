function isMobile () {
  return /Android|mobile|iPad|iPhone/i.test(navigator.userAgent);
}

var setMatrix = function (matrix, value) {
  var array = [];
  for (var key in value) {
    array[key] = value[key];
  }
  if (typeof matrix.elements.set === "function") {
    matrix.elements.set(array);
  } else {
    matrix.elements = [].slice.call(array);
  }
};

function start(video, input_width, input_height, render_update, track_update) {
  var vw, vh;
  var sw, sh;
  var pscale, sscale;
  var w, h;
  var pw, ph;
  var ox, oy;
  var worker;
  var camera_para = './../examples/Data/camera_para.dat'

  var canvas_process = document.createElement('canvas');
  var context_process = canvas_process.getContext('2d', { willReadFrequently: true });
  var targetCanvas = document.querySelector("#canvas");

  var renderer = new THREE.WebGLRenderer({ canvas: targetCanvas, alpha: true, antialias: true });
  renderer.setPixelRatio(window.devicePixelRatio);

  var scene = new THREE.Scene();

  var camera = new THREE.Camera();
  camera.matrixAutoUpdate = false;

  scene.add(camera);

  var sphere = new THREE.Mesh(
    new THREE.SphereGeometry(0.5, 8, 8),
    new THREE.MeshNormalMaterial()
  );

  var root = new THREE.Object3D();
  scene.add(root);

  var marker;

  sphere.material.flatShading;
  sphere.scale.set(200, 200, 200);

  root.matrixAutoUpdate = false;
  root.add(sphere);

  var load = function () {
    vw = input_width;
    vh = input_height;

    pscale = 320 / Math.max(vw, vh / 3 * 4);
    sscale = isMobile() ? window.outerWidth / input_width : 1;

    sw = vw * sscale;
    sh = vh * sscale;

    w = vw * pscale;
    h = vh * pscale;
    pw = Math.max(w, h / 3 * 4);
    ph = Math.max(h, w / 4 * 3);
    ox = (pw - w) / 2;
    oy = (ph - h) / 2;
    canvas_process.style.clientWidth = pw + "px";
    canvas_process.style.clientHeight = ph + "px";
    canvas_process.width = pw;
    canvas_process.height = ph;

    renderer.setSize(sw, sh);

    worker = new Worker('./worker.js', {type: 'module'})

    var pinball_buff = imread('pinball');

    if (pinball_buff) {
      console.log(pinball_buff)
      worker.postMessage({ type: "pinball", buff: pinball_buff, width: 1637, height: 2048 });
    }

    //worker.postMessage({ type: "pinball", buff: pinball_buff, width: 1637, height: 2048 });

    worker.onmessage = function (ev) {
      var msg = ev.data;
      switch (msg.type) {
        case "projection_matrix": {
          var proj = JSON.parse(msg.projMat);
          var ratioW = pw / w;
          var ratioH = ph / h;
          proj[0] *= ratioW;
          proj[4] *= ratioW;
          proj[8] *= ratioW;
          proj[12] *= ratioW;
          proj[1] *= ratioH;
          proj[5] *= ratioH;
          proj[9] *= ratioH;
          proj[13] *= ratioH;
          setMatrix(camera.projectionMatrix, proj);
          break;
        }
        case "endLoading": {
          if (msg.end == true) {
            // removing loader page if present
            var loader = document.getElementById('loading');
            if (loader) {
              loader.querySelector('.loading-text').innerText = 'Start the tracking!';
              setTimeout(function(){
                loader.parentElement.removeChild(loader);
              }, 2000);
            }
          }
          break;
        }
        case 'pattern_found': {
          found(msg);
          break;
        }
        case 'pattern_lost': {
          found(null);
          break;
        }
        case 'markerInfos': {
          marker = msg.marker;
        }
      }
      track_update();
     // process();
    };
  };

  var world;

  var found = function (msg) {
    if (!msg) {
      world = null;
    } else {
      world = JSON.parse(msg.matrixGL_RH);
    }
  };

  var lasttime = Date.now();
  var time = 0;

  var draw = function () {
    render_update();
    var now = Date.now();
    var dt = now - lasttime;
    time += dt;
    lasttime = now;

    if (!world) {
      sphere.visible = false;
    } else {
      sphere.visible = true;
      //sphere.position.y = ((marker.height / marker.dpi) * 2.54 * 10) / 2.0;
      //sphere.position.x = ((marker.width / marker.dpi) * 2.54 * 10) / 2.0;
      // set matrix of 'root' by detected 'world' matrix
      setMatrix(root.matrix, world);
    }
    renderer.render(scene, camera);
  };

  var process = function () {
    context_process.fillStyle = 'black';
    context_process.fillRect(0, 0, pw, ph);
    context_process.drawImage(video, 0, 0, vw, vh, ox, oy, w, h);

    var imageData = context_process.getImageData(0, 0, pw, ph);
    //worker.postMessage({ type: 'process', imagedata: imageData }, [imageData.data.buffer]);
    worker.postMessage({ type: "video_data", data: imageData, videoWidth: input_width, videoHeight: input_height }, [imageData.data.buffer]);
    requestAnimationFrame(process);
  }
  var tick = function () {
    draw();
    requestAnimationFrame(tick);
  };

  var imread = function (imageSource) {
    var img = null;
    if (typeof imageSource === 'string') {
        img = document.getElementById(imageSource);
    } else {
        img = imageSource;
    }
    var canvas = null;
    var ctx = null;
    if (img instanceof HTMLImageElement) {
        canvas = document.createElement('canvas');
        canvas.width = img.width;
        canvas.height = img.height;
        ctx = canvas.getContext('2d', { willReadFrequently: true });
        ctx.drawImage(img, 0, 0, img.width, img.height);
    } else if (img instanceof HTMLCanvasElement || img instanceof OffscreenCanvas) {
        canvas = img;
        ctx = canvas.getContext('2d');
    } else {
        throw new Error('Please input the valid canvas or img id.');
        return;
    }

    var imgData = ctx.getImageData(0, 0, canvas.width, canvas.height);
    return imgData.data;
};

  load();
  tick();
  process();
}
