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
var renderThreeJS = function (detected, cameraMatrix, matrix) {
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

    if (detected == true) {
        sphere.visible = true;
        setMatrix(root.matrix, matrix);
    }

    renderer.setSize(4000, 3000);
    renderer.render(scene, camera);
    console.log("scene rendered");
}