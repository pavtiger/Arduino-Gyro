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
    let tmp = GRID.init(), scene = tmp[0], renderer = tmp[1], camera = tmp[2], controls = tmp[3];

    let A = 1, B = 1, OLDX = 0, OLDY = 0, OLDZ = 0, last = 0;

    let stats = GRID.initStats(Stats);


    socket.on("update", function(msg) {
        if (msg[0] != undefined && msg[1] != undefined && msg[2] != undefined) {
            OLDX = (A * OLDX + B * msg[0]) / (A + B);
            OLDY = (A * OLDY + B * msg[1]) / (A + B);
            // OLDZ = (A * OLDZ + B * msg[2]) / (A + B);

            // if (OLDX > 330 && msg[0] < 50) {
            //     OLDX = msg[0]
            // } else {
            //     OLDX = (A * OLDX + B * msg[0]) / (A + B);
            // }

            // if (OLDY > 330 && msg[1] < 50) {
            //     OLDY = msg[0]
            // } else {
            //     OLDY = (A * OLDY + B * msg[1]) / (A + B);
            // }

            // if (OLDZ > 330 && msg[2] < 50) {
            //     OLDZ = msg[2]
            // } else {
            //     OLDZ = (A * OLDZ + B * msg[2]) / (A + B);
            // }

            window.pen.rotation.set(OLDY * Math.PI + Math.PI / 2, OLDZ * Math.PI, OLDX * Math.PI);
            // window.pen.position.x += msg[3] / 1000;
            // window.pen.position.y += msg[4] / 1000;
            // window.pen.position.z += (msg[5] - 9.81) / 1000;

            if (Date.now() - last > 1000) {
                console.log(OLDX, OLDY, OLDZ);
                last = Date.now();
            }
        }
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
