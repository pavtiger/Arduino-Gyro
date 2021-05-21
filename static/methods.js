import * as THREE from "./three.module.js";

import { OrbitControls } from "./OrbitControls.js";
import { OBJLoader } from "./OBJLoader.js";
import { MTLLoader } from "./MTLLoader.js";
import { DDSLoader } from "./DDSLoader.js";


function httpGet(Url) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", Url, false); // false for synchronous request
    xmlHttp.send(null);
    return JSON.parse(xmlHttp.responseText);
}


function sleep(ms) {
    return new Promise((resolve) => setTimeout(resolve, ms));
}


function initStats(Stats) {
    var stats = new Stats();
    stats.setMode(0); // 0: fps, 1: ms

    // Align top-left
    stats.domElement.style.position = "absolute";
    stats.domElement.style.left = "0px";
    stats.domElement.style.top = "0px";
    document.body.appendChild( stats.dom );
    return stats;
}


function init() {
    var scene = new THREE.Scene();
    var camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

    var renderer = new THREE.WebGLRenderer({antialias: true});
    renderer.setSize(window.innerWidth, window.innerHeight);

    document.body.appendChild(renderer.domElement);
    var controls = new OrbitControls(camera, renderer.domElement);
    controls.enableKeys = false;

    window.addEventListener("resize", function () {
        var width = window.innerWidth;
        var height = window.innerHeight;
        renderer.setSize(width, height);
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
    });

    const onProgress = function ( xhr ) {
        if ( xhr.lengthComputable ) {
            const percentComplete = xhr.loaded / xhr.total * 100;
            // TODO: create a loading screen
            console.log( Math.round(percentComplete) + "% downloaded" );
        }
    };
    const onError = function () { };

    const manager = new THREE.LoadingManager();
    manager.addHandler(/\.dds$/i, new DDSLoader());

    new MTLLoader(manager)
        .setPath("models/")
        .load( "pen.mtl", function (materials) {

            materials.preload();

            new OBJLoader(manager)
                .setMaterials( materials )
                .setPath("models/")
                .load( "pen.obj", function (object) {
                    let pivotPoint = new THREE.Object3D();
                    pivotPoint.add(object);
                    object.position.set(0, 1, 0);
                    object.rotation.set(0, 0, 3.14);

                    window.pen = pivotPoint;
                    scene.add(window.pen);

                }, onProgress, onError );

        });

    // light
    var directionalLight = new THREE.DirectionalLight(0xFFFFFF, 1);
    directionalLight.position.set(6, 8, 8);
    scene.add(directionalLight);

    directionalLight = new THREE.DirectionalLight(0xFFFFFF, 1);
    directionalLight.position.set(-6, -8, -8);
    scene.add(directionalLight);

    var ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.7);
    scene.add(ambientLight);

    camera.position.set(-1.83222123889, 1.83199683912, -2.66435763809);

    camera.rotation.y = 3.14;
    camera.rotation.x = 0.6;

    return [scene, renderer, camera, controls];
}


export { initStats, sleep, init };
