import * as THREE from "./three.module.js";

import * as GRID from "./methods.js"

let socket = io("http://" + window.location.hostname + ":" + window.location.port);



function httpGet(Url) {
    let xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", Url, false); // false for synchronous request
    xmlHttp.send(null);
    return JSON.parse(xmlHttp.responseText);
}


window.onload = function() {
    let tmp = GRID.init();
    let scene = tmp[0];
    let renderer = tmp[1];
    let camera = tmp[2];
    let controls = tmp[3];
    let A = 100, B = 1, OLDX = 0, OLDY = 0, OLDZ = 0;

    let stats = GRID.initStats(Stats);


    socket.on("update", function(msg) {
        OLDX = (A * OLDX + B * msg[0]) / (A + B);
        OLDY = (A * OLDY + B * msg[1]) / (A + B);
        OLDZ = (A * OLDZ + B * msg[2]) / (A + B);
        // window.pen.rotation.set(OLDX * Math.PI / 180, OLDY * Math.PI / 180, OLDZ * Math.PI / 180);
    });


    // Main loop
    let GameLoop = function() {
        requestAnimationFrame(GameLoop);
        stats.begin();
        controls.update();


        renderer.render(scene, camera);
        stats.end();
    }
    GameLoop()
};
