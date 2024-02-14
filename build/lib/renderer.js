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

var buildCameraProj = function (w, h, camera, proj) {
    var pw = Math.max(w, h / 3 * 4);
    var ph = Math.max(h, w / 4 * 3);
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
    buildCameraProj(640, 480, camera, cameraMatrix);

    scene.add(camera);

    var sphere = new THREE.Mesh(
        new THREE.SphereGeometry(0.5, 8, 8),
        new THREE.MeshNormalMaterial()
    );
    var root = new THREE.Object3D();
    scene.add(root);

    sphere.material.flatShading;
    sphere.visible = false;
    //sphere.scale.set(200, 200, 200);
    if (detected == true) {
        sphere.visible = true;
        setMatrix(sphere.matrix, matrix);
    }

    root.matrixAutoUpdate = false;
    root.add(sphere);

    renderer.setSize(640, 480);
    renderer.render(scene, camera);
    console.log("scene rendered");
}