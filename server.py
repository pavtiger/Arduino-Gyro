from dataclasses import dataclass
from threading import Thread
import unicodedata as ud
import json
import math
import time
from random import randint

import eventlet
from flask_socketio import SocketIO
from flask import Flask, send_from_directory, render_template

from ip import ip_address, port

async_mode = None
app = Flask(__name__, static_url_path='')
socketio = SocketIO(app, async_mode=async_mode)



@dataclass
class Point3d:
    x: float
    y: float
    z: float


# Vector structure for collisions
class Point:
    def __init__(self, a, b):
        if type(a) == Point and type(b) == Point:
            self.x = b.x - a.x
            self.y = b.y - a.y
        else:
            self.x = a
            self.y = b

    def dp(self, other):
        return self.x * other.x + other.y * self.y

    def cp(self, other):
        return self.x * other.y - other.x * self.y



# Main page
@app.route('/')
def root():
    return render_template('main.html')


# Get files from server (etc. libs)
@app.route('/js/<path:path>')
def send_js(path):
    return send_from_directory('js', path)


@socketio.on('add')
def up(data):
    data = list(map(float, data.split('; ')))

    socketio.emit('update', data)



# def game_loop(name):
#     while True:
#         time.sleep(0.01)



if __name__ == "__main__":
    eventlet.monkey_patch()

    # x = Thread(target=game_loop, args=(1,))
    # x.start()

    socketio.run(app, host=ip_address, port=port)
