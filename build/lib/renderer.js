var oWidth = 4000;
var oHeight = 3000;
var arElem, overlayCanvas, displayImageCanvas;

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

function isMobile() {
    return /Android|mobile|iPad|iPhone/i.test(navigator.userAgent);
}

var buildCameraProj = function (input_width, input_height, camera, proj) {
    var vw, vh, sw, sh, pw, ph, pscale, sscale;
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
}
var renderThreeJS = function (detected, cameraMatrix, matrix, homo, corners) {
    createOverlayCanvas();

    /*arElem = document.getElementById("arElem");
    arElem.style["transform-origin"] = "top left"; // default is center
    arElem.style.zIndex = 2;*/
    var renderer = new THREE.WebGLRenderer({ canvas: canvasElement, alpha: true, antialias: true });
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

    sphere.material.flatShading;
    sphere.visible = false;

    root.matrixAutoUpdate = false;
    root.add(sphere);
    buildCameraProj(4000, 3000, camera, cameraMatrix);
    // clearOverlayCtx();
    // arElem.style.display = "none";
    //clearOverlayCtx();

    if (detected == true) {
        sphere.visible = true;
        setMatrix(root.matrix, matrix);
        //arElem.style.display = "block";
        drawImage();
        // transformElem(homo, arElem);
        drawCorners(corners)
    }

    renderer.setSize(4000, 3000);
    renderer.render(scene, camera);
    console.log("scene rendered");
}

function drawImage() {
    displayImageCanvas = document.createElement("canvas");
    setVideoStyle(displayImageCanvas);
    displayImageCanvas.id = "displayImage";
    displayImageCanvas.width = oWidth;
    displayImageCanvas.height = oHeight;
    displayImageCanvas.zIndex = 90;
    displayImageCanvas.style.width = "1280px";
    let img = document.getElementById("pinball-test");
    let ctx = displayImageCanvas.getContext('2d', { willReadFrequently: true });
    ctx.drawImage(img, 0, 0, img.width, img.height);
    document.body.appendChild(displayImageCanvas);
}

function setVideoStyle(elem) {
    elem.style.position = "absolute";
    elem.style.top = 0;
    elem.style.left = 0;
}

function createOverlayCanvas() {
    overlayCanvas = document.createElement("canvas");
    setVideoStyle(overlayCanvas);
    overlayCanvas.id = "overlay";
    overlayCanvas.width = oWidth;
    overlayCanvas.height = oHeight;
    overlayCanvas.style.zIndex = 100;
    overlayCanvas.style.width = "1280px";
    document.body.appendChild(overlayCanvas);
}

function transformElem(h, elem) {
    // column major order
    let transform = [
        h[0],
        h[3],
        0,
        h[6],
        h[1],
        h[4],
        0,
        h[7],
        0,
        0,
        1,
        0,
        h[2],
        h[5],
        0,
        h[8],
    ];
    transform = "matrix3d(" + transform.join(",") + ")";
    elem.style["-ms-transform"] = transform;
    elem.style["-webkit-transform"] = transform;
    elem.style["-moz-transform"] = transform;
    elem.style["-o-transform"] = transform;
    elem.style.transform = transform;
    elem.style.display = "block";
}

function clearOverlayCtx() {
    const overlayCtx = overlayCanvas.getContext("2d");
    overlayCtx.clearRect(0, 0, oWidth, oHeight);
}

function drawCorners(corners) {
    const overlayCtx = overlayCanvas.getContext("2d");
    clearOverlayCtx();

    overlayCtx.beginPath();
    overlayCtx.strokeStyle = "blue";
    overlayCtx.lineWidth = 3;

    // [x1,y1,x2,y2,x3,y3,x4,y4]
    overlayCtx.moveTo(corners[0][0], corners[0][1]);
    overlayCtx.lineTo(corners[1][0], corners[1][1]);
    overlayCtx.lineTo(corners[2][0], corners[2][1]);
    overlayCtx.lineTo(corners[3][0], corners[3][1]);
    overlayCtx.lineTo(corners[0][0], corners[0][1]);

    overlayCtx.stroke();
}
