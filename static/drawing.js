import * as THREE from "./three.module.js";

import * as GRID from "./methods.js"

// let socket = io("http://" + window.location.hostname + ":" + window.location.port);



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

    let stats = GRID.initStats(Stats);

    // socket.on("connect", function() {
    //     socket.emit("message", "I am connected");
    // });
    //
    // socket.on("update", function(msg) {
    //     allPlayers = JSON.parse(msg);
    // });


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
